/*======================================*/
/* インクルード                         */
/*======================================*/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFRの定義ファイル    */
#include "printf_lib.h"                 /* printf使用ライブラリ         */
#include "camera.h"
#include "lcd_lib.h"                    /* LCD表示用追加                */

/* TAOS TSL1401CL */
#define	TAOS_SI_HIGH	p0_addr.bit.b1 = 1	/* Port P1_7 bit */
#define	TAOS_SI_LOW		p0_addr.bit.b1 = 0	/* Port P1_7 bit */
#define	TAOS_CLK_HIGH	p0_addr.bit.b2 = 1	/* Port P7_0 bit */
#define	TAOS_CLK_LOW	p0_addr.bit.b2 = 0	/* Port P7_0 bit */

/* Capture */
#define		White_Max			120				/* ライン白色MAX値の設定 */
#define		White_min	5					/* 白色の最小値 			*/

/* public 変数	*/
int		White;									/* 白色の個数								*/
unsigned int 	Wide;							/* ラインの幅 */
int		Center;									/* ラインの中心 */
long		center_Weight;							/* ラインの重心 */
unsigned long pre_cycle;				/* １処理の時間	*/
/* private変数	*/
int		camera_Max,camera_Min,camera_Ave;		/* カメラ読み取り最大値、最小値、平均値		*/
int		real_Center;									/* ラインの中心 */
unsigned int 	Rsensor;						/* ラインの右端 */
unsigned int 	Lsensor;						/* ラインの左端 */
int   	EXPOSURE_timer = 1350;					/* 露光時間					*/
int		ImageData[130];							/* カメラの値								*/
int		BinarizationData[130];					/* ２値化									*/
int		LineStart;								/* カメラのスタート位置		*/
int		LineStop;								/* カメラのストップ位置		*/
unsigned int 	Rsensor01;				/* ラインの右端 */
unsigned int 	Lsensor01;				/* ラインの左端 */
unsigned int 	Wide01;					/* ラインの幅 */
unsigned int 	Rsensor02;				/* ラインの右端 */
unsigned int 	Lsensor02;				/* ラインの左端 */
unsigned int 	Wide02;					/* ラインの幅 */
unsigned int 	Rsensor;				/* ラインの右端 */
unsigned int 	Lsensor;				/* ラインの左端 */
unsigned int 	Wide;					/* ラインの幅 */
int			 	Center01;				/* ラインの重心 */
int			 	Center02;				/* ラインの重心 */
int				Center_ctrl;
int 			pre_Center;				/* 前回の重心値 */
int				pid_angle;					/* カメラによる計算 */
float	kp,ki,kd;

/* privateプロトタイプ宣言                     */
void	ImageCapture(void);						/* イメージキャプチャ						*/
void	binarization(void);						/* ２値化									*/
void	WhiteLineWide(void);					/* 白線の幅を測定							*/
void	WhiteLineCenter(void);					/* 白線の中心を測定							*/
void	WhiteLineWeight(void);					/* 白線の重心を測定							*/
int	PID(void);									/* ＰＩＤ値の計算 */
int	get_ad_camera( void );						/*	A/D値読み込み(AN7)							*/

/************************************************************************/
/* カメラからのデータ                                                   */
/* 引数　  なし                                      */
/* 戻り値 なし                                                          */
/************************************************************************/
void camera_Caputure(void){
	int i;
	
	ImageCapture();						/* イメージキャプチャ						*/
	for(i=0;i<EXPOSURE_timer;i++) asm("nop");
//	expose();
	ImageCapture();						/* イメージキャプチャ						*/
	binarization();						/* ２値化									*/
	WhiteLineWide();					/* 白線の幅を測定							*/
	WhiteLineCenter();					/* 白線の中心を測定							*/
	WhiteLineWeight();					/* 白線の重心を測定							*/
	pid_angle =PID();					/* ＰＩＤの値 */	
}

/************************************************************************/
/* イメージキャプチャ                                                   */
/* 引数　  なし                                      */
/* 戻り値 なし                                                          */
/************************************************************************/
void ImageCapture(void){	 
	
	unsigned char i;

	if(data_buff[DF_RACE] == 0){
		LineStart = data_buff[DF_LineStart];
		LineStop = data_buff[DF_LineStop];
	}else{
		LineStart = data_buff[DF_LineStart];
		LineStop = data_buff[DF_LineStop];		
	}
	camera_Max = 0,camera_Min = 1024;
	TAOS_SI_HIGH;
	TAOS_CLK_HIGH;
	TAOS_SI_LOW;
	ImageData[0] = 0;
	TAOS_CLK_LOW;
	for(i = 0; i < LineStart; i++) {		
		TAOS_CLK_HIGH;		
		TAOS_CLK_LOW;
	}
	for(i = LineStart; i < LineStop; i++) {					 
		TAOS_CLK_HIGH;
		ImageData[i] = get_ad_camera();								// inputs data from camera (one pixel each time through loop) 
		TAOS_CLK_LOW;
		
		if(camera_Max < ImageData[i]){
			camera_Max = ImageData[i];
		}			
		if(camera_Min > ImageData[i]){
			camera_Min = ImageData[i];
		}
		
	}
	for(i = LineStop; i < 128; i++) {		
		TAOS_CLK_HIGH;		
		TAOS_CLK_LOW;
	}
	
	TAOS_CLK_HIGH;
	TAOS_CLK_LOW;
}
/************************************************************************/
/* ２値化                                                               */
/* 引数		なし														*/
/* 戻り値	なし                                                        */
/************************************************************************/
void binarization(void)
{
	int i;
	/* 最高値と最低値から間の値を求める */
	camera_Ave = (camera_Max + camera_Min) * 2/3;

	/* 黒は０　白は１にする */
	White = 0;

	/* 前面白 */
	if( camera_Min > 50){
		for(i=1; i < 128; i++){
			White++;
			BinarizationData[i] = 1;
		}
	/* 通常モード */		
	}else if( camera_Max > 50 ){
		for(i = LineStart ; i < LineStop; i++) {
			if( ImageData[i] > camera_Ave ){	
				White++;			
				BinarizationData[i] = 1;
			}else{
				BinarizationData[i] = 0;
			}
		}
	/* 黒が一面のとき */
	}else{
		for(i=1; i < 128; i++){
			BinarizationData[i] = 0;
		}
	}

}
/************************************************************************/
/* 白線の幅を測定                                                       */
/* 引数		なし														*/
/* 戻り値	なし                                                        */
/************************************************************************/
void WhiteLineWide(void)
{
	int t=0,i;
		
	Lsensor01 = Lsensor02 = LineStart;
	Rsensor01 = Rsensor02 = LineStop;

	for(i = Lsensor01 ; i < LineStop; i++) {
		if(t==0){	/* １本目の白線 */
			if( BinarizationData[i] ){		/* 左から最初の白 */
				Lsensor01 = i;
				t = 1;
			}
		}else if(t==1){
			if( BinarizationData[i] ){		/* 左から最初の白 */
				Lsensor01 = i;
				t = 2;
			}
		}else if(t==2){	/* １本目の黒線 */
			if( !BinarizationData[i] ){		/* 左から最初の黒 */
				Rsensor01 = i;
				t = 3;
			}
		}else if(t==3){
			if( !BinarizationData[i] ){		/* 左から最初の黒 */
				Rsensor01 = i;
				t = 4;
			}
		}else if(t==4){	/* ２本目の白線 */
			if( BinarizationData[i] ){		/* 左から最初の白 */
				Lsensor02 = i;
				t = 5;
			}
		}else if(t==5){
			if( BinarizationData[i] ){		/* 左から最初の白 */
				Lsensor02 = i;
				t = 6;
			}
		}else if(t==6){	/* ２本目の黒線 */
			if( !BinarizationData[i] ){		/* 左から最初の黒 */
				Rsensor02 = i;
				t = 7;
			}
		}else if(t==7){	
			if( !BinarizationData[i] ){		/* 左から最初の黒 */
				Rsensor02 = i;
				break;
			}
		}
	}
	if(White > White_min){
		Wide01 = Rsensor01 - Lsensor01;		/* 1本目の白線幅を求める */
		if( Lsensor02 != LineStart){
			Wide02 = Rsensor02 - Lsensor02; /* 2本目の白線幅を求める */
		}else{
			Wide02 = 0;						/* 2本目白線なし		*/
		}
	}else{
		Wide01 = Wide02 = 0;				/* 黒一面 */
		Center = 0;
	}
}
/************************************************************************/
/* 白線の中心を測定                                                       */
/* 引数　 なし                                                          */
/* 戻り値 なし			                                                 */
/************************************************************************/
void WhiteLineCenter(void)
{
	int abs_Center01,abs_Center02;		/* センター値の絶対値 */

	if(Wide01 != 0){
		Center01 = (Lsensor01 + Rsensor01)/2;			/* 1本目の白線の重心を求める */
	}
	if(Wide02 != 0){
		Center02 = (Lsensor02 + Rsensor02)/2;			/* 2本目の白線の重心を求める */
	}
//	maker = 0;
	/* トレースラインの判定 */
	if(Wide01 != 0 && Wide02 == 0){						/* 1本目はあり、2本目がないとき */
		Center = Center01 - 64 - OFFSET;
		Wide = Wide01;
		Center02 = 0;
	}else if(Wide01 != 0 && Wide02 != 0){				/* 1本目はあり、2本目がありとき */
		abs_Center01 = pre_Center - Center01;
		if(abs_Center01 < 0)abs_Center01 *= -1;			/* －のときは＋にする　*/
		abs_Center02 = pre_Center - Center02;
		if(abs_Center02 < 0)abs_Center02 *= -1;			/* －のときは＋にする　*/
		if(abs_Center01 < abs_Center02){
			/* 前回の重心値が1本目のラインが近い */
			Center = Center01 - 64 - OFFSET;
			Wide = Wide01;
//			if(Wide02 > 8) maker = 1;
		}else{
			/* 前回の重心値が2本目のラインが近い */
			Center = Center02 - 64 - OFFSET;
			Wide = Wide02;
//			if(Wide01 > 8) maker = 2;
		}
	}else{

		Wide = 0;
		Center = 999;
	}
	/* 線幅が太いときはセンター値を計算しない */
	if(Wide > 50){
		Center = 999;
	}
	pre_Center = Center + 64;

}
/************************************************************************/
/* 白線の重心を測定                                                       */
/* 引数　 なし                                                          */
/* 戻り値 なし			                                                 */
/************************************************************************/
void WhiteLineWeight(void)
{
	int	i;
	int		value;
	int		weight;
	long	sum = 0,sum2 = 0;
	
	if((Wide > 5) && (Wide < 20)){
		center_Weight = 0;
		weight = Center - 10;
		for(i = real_Center-10 ; i <= real_Center+10;i++,weight++){
			value = ImageData[i];
			sum += value;
			sum2 += value*weight;
		}
		center_Weight = sum2*7 / sum;
		//平均値＝（階級値×度数）の合計/度数の合計
	}else{
		center_Weight = -999;
	}
//	saveData[4] = (char)center_Weight;				/* 重心のログ		*/
}
/************************************************************************/
/* PID				                                                    */
/* 引数　 なし                                                          */
/* 戻り値 なし			                                                */
/************************************************************************/
int PID(void)
{
	static float	iCenter = 0.0;
	static float	preCenter = 0.0;
	static float	h;
	if(Center != 999){
		iCenter +=  (float)Center - preCenter;
		h = (float)Center * kp + iCenter *ki + ((float)Center - preCenter) * kd;
		preCenter = (float)Center;
	}
	return -(int)h;
}
/************************************************************************/
/* PID設定				                                                    */
/* 引数　 なし                                                          */
/* 戻り値 ハンドル角			                                                */
/************************************************************************/
void set_PID(int s)
{
	switch(s){
		case StrightPID: /* ストレート */
			kp = (float)data_buff[DF_KP_S]/10.0;
			ki = (float)data_buff[DF_KI_S]/10.0;
			kd = (float)data_buff[DF_KD_S]/10.0;
		break;
		case CurvePID: /* カーブ */
			kp = (float)data_buff[DF_KP_C]/10.0;
			ki = (float)data_buff[DF_KI_C]/10.0;
			kd = (float)data_buff[DF_KD_C]/10.0;
		break;
		case SprintPID: /* カーブ */
			kp = (float)data_buff[DF_KP_SP]/100.0;
			ki = (float)data_buff[DF_KI_SP]/10.0;
			kd = (float)data_buff[DF_KD_SP]/10.0;
		break;
		case Sprint_2PID: /* カーブ */
			kp = (float)data_buff[DF_KP_SP2]/10.0;
			ki = (float)data_buff[DF_KI_SP2]/10.0;
			kd = (float)data_buff[DF_KD_SP2]/10.0;
		break;	}

}
/************************************************************************/
/* 露光時間調整                                                         */
/* 引数　 経過時間                                                          */
/* 戻り値 なし　　　　　                                                */
/************************************************************************/
void expose( void )
{
	int i;
	if( Wide != 0 && !(White >= 90)){//黒でなく白でもない
		if(camera_Max < White_Max){
			EXPOSURE_timer += (White_Max - camera_Max)*10;
		}else{
			EXPOSURE_timer -= (camera_Max - White_Max)*10;
		}
	}
/*
	if( EXPOSURE_timer > 500){
		EXPOSURE_timer = 250;
	}
*/
	if( EXPOSURE_timer < 0 ){
		EXPOSURE_timer = 0;
	}
//	printf("camera_Max %d White_Max %d  EXPOSURE_timer%d\n",camera_Max , White_Max,EXPOSURE_timer);
	
	for(i=0;i<EXPOSURE_timer;i++) asm("nop");
}
/************************************************************************/
/* RAWデータの表示	                                                   */
/* 引数　 なし                                                          */
/* 戻り値 なし			                                                 */
/************************************************************************/
void raw_view(void)
{
	int i;
	for(i = LineStart;i < LineStop; i++){
		printf("%d ",ImageData[i]);
	}
	printf("\n\n");

}
/************************************************************************/
/* ２値化データの表示	                                                   */
/* 引数　 なし                                                          */
/* 戻り値 なし			                                                 */
/************************************************************************/
void bi_view(void)
{
	int i;
	for(i = LineStart;i < LineStop; i++){
		if(BinarizationData[i] == 1){
			printf("*");
		}else{
			printf(" ");
		}
	}
	printf("time=%d Max=%d Min=%d Ave=%d ",EXPOSURE_timer,camera_Max,camera_Min,camera_Ave);
	printf("Wide=%d Center=%d Weight=%ld\n",Wide,Center,center_Weight);
//	printf("time=%ld EXPO=%d Max=%d Min=%d Ave=%d Wide=%d Center=%d Weight=%ld iServoPID=%d\n",pre_cycle/2,EXPOSURE_timer,camera_Max,camera_Min,camera_Ave,Wide,Center,center_Weight,iServoPID);
//	printf("Center=%d Weight=%ld iServoPID=%d\n",Center,center_Weight,iServoPID);
}

/************************************************************************/
/* A/D値読み込み(AN4)                                                   */
/* 引数　 なし                                                          */
/* 戻り値 A/D値 0～1023                                                 */
/************************************************************************/
int get_ad_camera( void )
{
    /* A/Dコンバータの設定 */
	admod   = 0x03;									/* 単発モードに設定						*/
    adinsel = 0x00;									/* 入力端子AN0(P0_7)を選択			*/
    adcon1  = 0x30;									/* A/D動作可能								*/
    asm(" nop ");									/* φADの1サイクルウエイト入れる	*/
    adcon0 = 0x01;									/* A/D変換スタート							*/
    while( adcon0 & 0x01 ) ;						/* A/D変換終了待ち							*/
    return ad0;
}