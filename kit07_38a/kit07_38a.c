/************************************************************************/
/* 対象マイコン R8C/38A                                                 */
/* ﾌｧｲﾙ内容     ラインセンサーを用いる。								*/
/* バージョン   Ver.1.02                                                */
/* Date         2014.10.22                                              */
/* Copyright    大阪府立淀川工科高校									*/
/************************************************************************/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFRの定義ファイル    */
#include "printf_lib.h"                 /* printf使用ライブラリ         */
#include "microsd_lib.h"                /* microSD制御ライブラリ        */
#include "lcd_lib.h"                    /* LCD表示用追加                */
#include "switch_lib.h"                 /* スイッチ追加                 */
#include "data_flash_lib.h"             /* データフラッシュライブラリ   */
#include "isline.h"
#include "isCheck.h"
#include "drive.h"
#include "ini.h"

/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
void init( void );
void timer( unsigned long timer_set );
unsigned char dipsw_get( void );
unsigned char pushsw_get( void );
void led_out( unsigned char led );
int diff( int pwm );
void readDataFlashParameter( void );
void writeDataFlashParameter( void );
int lcdProcess( void );

/*======================================*/
/* グローバル変数の宣言                 */
/*======================================*/
const char *C_DATE = __DATE__;          /* コンパイルした日付           */
const char *C_TIME = __TIME__;          /* コンパイルした時間           */

/* DataFlash関係 */
signed char     data_buff[ DF_PARA_SIZE ];

/************************************************************************/
/* メインプログラム                                                     */
/************************************************************************/
void main( void )
{
    int     i, ret, sensor ,t;
    char    fileName[ 8+1+3+1 ];        /* 名前＋'.'＋拡張子＋'\0'      */
    unsigned char b;                    /* ！追加・変更！               */
	int CL_number,CL_BR[5],CL_SP[5],CL_BK_TM[5];
	int	CL_BRAKE_TIME; 
	unsigned int j;
	CL_number = 0;

    /* マイコン機能の初期化 */
    init();                             /* 初期化                       */
    init_uart0_printf( SPEED_9600 );    /* UART0とprintf関連の初期化    */
    setMicroSDLedPort( &p6, &pd6, 0 );  /* microSD モニタLED設定        */
    asm(" fset I ");                    /* 全体の割り込み許可           */
    initLcd();                          /* LCD初期化                    */
	initSwitch();                       /* スイッチ初期化               */

    readDataFlashParameter();           /* DataFlashパラメータ読み込み  */
    servo_center  = (unsigned char)data_buff[DF_SERVO1] * 0x100;
    servo_center |= (unsigned char)data_buff[DF_SERVO2];

    /* microSD初期化 */
    ret = initMicroSD();
    if( ret != 0x00 ) msdError = 1;

    /* FAT32でマウント */
    if( msdError == 0 ) {
        ret = mountMicroSD_FAT32();
        if( ret != 0x00 ) msdError = 2;
    }

    if( msdError != 0 ) {
        /* microSD処理にエラーがあれば3秒間、LEDの点灯方法を変える */
        cnt1 = 0;
        while( cnt1 < 3000 ) {
            if( cnt1 % 200 < 100 ) {
                led_out( 0x3 );
            } else {
                led_out( 0x0 );
            }
        }
    }
    /* マイコンカーの状態初期化 */
	printf("Hello!!\n");
    handle( 0 );
    motor( 0, 0 );
	pattern = 1000;		/* モニター画面 */

    while( 1 ) {
    lcdProcess();		/* LCD表示、パラメータ設定処理*/
	get_ad();			/* センサー値の読み込み */
	LPF();				/* ローパスフィルター */
	Normanaization();	/* 正規化 */
	binarization();		/* ２値化 */
	potition();			/* ライン位置の計算 */
//	printf("pattern = %d\n",pattern);
    switch( pattern ) {
    case 0:
		lcdPosition( 0, 0 );lcdPrintf( "init");
        led_out( 0x0 );
        // パラメータ保存
        writeDataFlashParameter();
        pattern = 1;
        cnt1 = 0;
        if( msdError == 0 ) {
           	/* microSDの空き領域から読み込み */
        	i = readMicroSDNumber();
            if( i == -1 ) {
            	msdError = 3;
            }
         }
         if( msdError == 0 ) {
             /* microSDの空き領域へ書き込み  */
             i++;
             if( i >= 10000 ) i = 1;
             ret = writeMicroSDNumber( i );
             if( ret == -1 ) {
                  msdError = 4;
             } else {
                /* ファイル名変換 */
                  sprintf( fileName, "log_%04d.csv", i );
             }
         }
         if( msdError == 0 ) {
             /* ファイルのタイムスタンプセット */
             setDateStamp( getCompileYear( C_DATE ),
             getCompileMonth( C_DATE ),  getCompileDay( C_DATE ) );
             setTimeStamp( getCompileHour( C_TIME ),
             getCompilerMinute( C_TIME), getCompilerSecond( C_TIME ) );

            /* 書き込みファイル名作成 */
            // 書き込みしたい時間[ms] : x = 10[ms] : 64バイト
            // 60000msなら、x = 60000 * 64 / 10 = 384000
            // 結果は512の倍数になるように繰り上げする。
             ret = writeFile( fileName, 384000 );
			 if( ret != 0x00 ) msdError = 11;
			/*
			 msdPrintf( "[Your Car Name] Log Data\n" );
			 while( checkMsdPrintf() ); // msdPrintf処理完了待ち
			*/

		}
         pattern = 1;
         cnt1 = 0;
 		lcdPosition( 0, 0 );lcdPrintf( "AutoCalibration");
		break;
	case 1:/* オートキャリブレーション */
		if( pushsw_get()){
			timer(1000);
			pattern = 2;
		}
		break;
	case 2: /* オートキャリブレーション */
		if( pushsw_get() ) {
			AutoCalibration();
/*			printf("black = ");
			for(i = 0; i < 16; i++){
				printf("%4d ",ad_black[i]);
			}
			printf("\n");
			printf("white = ");
			for(i = 0; i < 16; i++){
				printf("%4d ",ad_white[i]);
			}
			printf("\n");
*/
		}else{
			if(dipsw_get() == 0){
				pattern = 4;
			}else{
				pattern = 2000;
			}
		}			
        if( cnt1 < 100 ) {              /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 200 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
        break;
	case 4:
        /* スタートバーまで行く */
        if( startbar_get()) {
			timer(1000);
			pattern = 5;
		}
        led_out( 0x0 );
		break;
	case 5:
		if( pushsw_get()){
			timer(1000);
			pattern = 6;
		}
        if( !startbar_get()) {
			pattern = 4;
		}
        led_out( 0x3 );
		break;
    case 6:
        /* スタートバーが開いたかチェック */
        if( !startbar_get() || pushsw_get() ) {
            /* スタート！！ */
            led_out( 0x0 );
            pattern = 11;
            if( msdError == 0 ) msdFlag = 1;    /* データ記録開始       */
            cnt1 = 0;
			timer( 100 );
			lEncoderTotal	= 0;
			lEncoderCrank	= 0;
			lEncoderCrank2	= 0;			
			lEncoderHarf	= 0;
            break;
        }
        if( cnt1 < 50 ) {               /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
 		stop_timer = 0;
		//mem_ad = 0;
        break;
    case 11:
        /* 通常トレース */
		handle( PID());
		t = ((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2];
		if( stop_timer >= ((unsigned long)(t)*10) ){
			msdFlag = 0;
			motor( 0, 0 );
			pattern = 101;
			break;
		}
		if (stop_timer>1000) {//通常トレース後しばらくクロス、ハーフはチェックしない
	       	if( check_crossline() ) {       /* クロスラインチェック         */
				lEncoderCrank = lEncoderTotal;
            	pattern = 21;
            	break;
        	}
        	if( check_rightline()) {       /* 右ハーフラインチェック       */
            	lEncoderTotal = lEncoderHarf;
				pattern = 51;
            	break;
        	}
        	if( check_leftline() ) {        /* 左ハーフラインチェック       */
            	lEncoderTotal = lEncoderHarf;
            	pattern = 61;
            	break;
        	}
        	if( check_RightOutLine() ) {        /* 右アウトラインチェック       */
            	pattern = 15;
            	break;
        	}
        	if( check_LeftOutLine() ) {        /* 左アウトラインチェック       */
            	pattern = 12;
            	break;
        	}
		}
		if(pos > 0.0) run(100 - (2.0*pos + pre_pos)*8.0);
		else run(100 + (2.0*pos + pre_pos)*8.0);
        break;
	case 12:
		/* 右大カーブ */
		run(50);
		handle( 20 );
		if(!check_RightOutLine()){
			pattern = 13;			/* 通常トレース */
		}
		break;
	case 13:
		/* 右大カーブ */
		run(50);
		handle( PID()*2 );
		if(check_center()){
 			pattern = 11;			/* 通常トレース */
		}
		break;
	case 15:
		/* 左大カーブ */
		run(50);
		handle(-20);
		if(!check_LeftOutLine()){
			pattern = 16;			/* 通常トレース */
		}
		break;
	case 16:
		run(50);
		handle( PID()*2 );
		if(check_center()){
 			pattern = 11;			/* 通常トレース */
		}
		break;		
    case 21:
        /* １本目のクロスライン検出時の処理 */
        led_out( 0x3 );
        handle( 0 );
		motor((5 - iEncoder)*20,(5 - iEncoder)*20);
		pattern = 22;
        break;

    case 22:
        /* ２本目を読み飛ばす */
		motor((5 - iEncoder)*10,(5 - iEncoder)*10);
		if( lEncoderTotal-lEncoderCrank >= 100 ) {   /* 約10cmたったか？ */				
			pattern = 23;
		}
        break;

	case 23:
        /* クロスライン後のトレース、クランク検出 */
        if( check_leftline() ) {   /* ！追加・変更！ */
            /* 左クランクと判断→左クランククリア処理へ */
            led_out( 0x0 );
            handle( -data_buff[DF_crank_handlepwm] );
			motor( data_buff[DF_crank_motor2] ,data_buff[DF_crank_motor1] );
            pattern = 31;
            cnt1 = 0;
            break;
        }
        if( check_rightline() ) {   /* ！追加・変更！           */
            /* 右クランクと判断→右クランククリア処理へ */
            led_out( 0x3 );
            handle( data_buff[DF_crank_handlepwm] );
			motor( data_buff[DF_crank_motor1] ,data_buff[DF_crank_motor2] );
            pattern = 41;
            cnt1 = 0;
            break;
        }
		handle( PID());
		run( data_buff[DF_crank_motorS] );
        break;

    case 31:
        /* 左クランククリア処理　安定するまで少し待つ */
        if( check_black() ) {
            pattern = 34;
            cnt1 = 0;
        }
        break;
	
	case 32:
        /* 左クランククリア処理　外側の白線と見間違わないようにする */
        /* ！追加・変更！ ここから */
        if( check_leftline() ) {
            pattern = 34;
        }
        /* ！追加・変更！ ここまで */
        break;
		
    case 33:
        /* 左クランククリア処理　曲げ終わりのチェック */
/*        if( sensor_inp(MASK2_0) ) {
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここから */
/*        if( sensor_inp(MASK3_3) == 0x07 ) {
            pattern = 34;
            break;
        }
        /* ！追加・変更！ ここまで */
        break;

    case 34:
        /* 左クランククリア処理　外側の白線と見間違わないようにする */
        /* ！追加・変更！ ここから */
        if( check_center() ) {
			lEncoderCrank = lEncoderTotal;
            pattern = 35;
        }
        /* ！追加・変更！ ここまで */
        break;
	case 35:
		if( lEncoderTotal - lEncoderCrank > 300 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		run(50);
		break;

    case 41:
        /* 右クランククリア処理　安定するまで少し待つ */
//        if( cnt1 > 50 && check_black()) {
        if( check_black()) {
            pattern = 42;
            cnt1 = 0;
        }
        break;
    
	case 42:
        /* 右クランククリア処理　外側の白線と見間違わないようにする */
        /* ！追加・変更！ ここから */
        if( check(12,13) ) {
            pattern = 44;
        }
        /* ！追加・変更！ ここまで */
        break;
		
    case 43:
        /* 右クランククリア処理　曲げ終わりのチェック */
/*        if( sensor_inp(MASK0_2) ) {
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここから */
/*        if( sensor_inp(MASK3_3) == 0xe0 ) {
            pattern = 44;
            break;
        }
        /* ！追加・変更！ ここまで */
        break;

    case 44:
        /* 右クランククリア処理　外側の白線と見間違わないようにする */
        /* ！追加・変更！ ここから */
        if( check_center() ) {
			lEncoderCrank = lEncoderTotal;
            pattern = 45;
        }
        /* ！追加・変更！ ここまで */
        break;
	case 45:
		if( lEncoderTotal - lEncoderCrank > 500 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		run(50);
		break;

    case 51:
        /* １本目の右ハーフライン検出時の処理 */
        led_out( 0x2 );
        handle( 0 );
		motor((5 - iEncoder)*5,(5 - iEncoder)*5);
        pattern = 52;
        cnt1 = 0;
         /* ！追加・変更！ ここから */
        if( check(2,3) ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
       break;
    case 52:
        /* ２本目を読み飛ばす */
		motor((5 - iEncoder)*5,(5 - iEncoder)*5);
		if( lEncoderTotal - lEncoderHarf > 100 ){
            pattern = 53;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここから */
        if( check(2,3) ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
        /* ！追加・変更！ ここまで */
        break;

    case 53:
        /* 右ハーフライン後のトレース、レーンチェンジ */
        if( check_black() ) {
           	handle( data_buff[DF_laneR_PWM] );
//			motor(data_buff[DF_lane_motorL],data_buff[DF_lane_motorR]);
			run( data_buff[DF_lane_motorS] );
            pattern = 54;
            cnt1 = 0;
            break;
        }
		handle( PID() + 5);
//        motor( data_buff[DF_lane_motorS] ,data_buff[DF_lane_motorS] );
		run( data_buff[DF_lane_motorS] );
        break;

    case 54:
        /* 右レーンチェンジ終了のチェック */
        /* ！追加・変更！ ここから */
        if( check_center() ) {
            led_out( 0x0 );
			lEncoderTotal = lEncoderHarf;
            pattern = 55;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここまで */
        break;
	case 55:
		if( lEncoderTotal - lEncoderHarf > 500 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		run(50);
		break;

    case 61:
        /* １本目の左ハーフライン検出時の処理 */
        led_out( 0x1 );
        handle( 0 );
		motor((5 - iEncoder)*5,(5 - iEncoder)*5);
        pattern = 62;
        cnt1 = 0;
        /* ！追加・変更！ ここから */
        if( check(12,13) ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
		break;
    case 62:
        /* ２本目を読み飛ばす */
		motor((5 - iEncoder)*5,(5 - iEncoder)*5);
		if( lEncoderTotal - lEncoderHarf > 80 ){
            pattern = 63;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここから */
        if( check(12,13) ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
		/* ！追加・変更！ ここまで */
        break;

    case 63:
        /* 左ハーフライン後のトレース、レーンチェンジ */
        if( check_black() ) {
			handle( -data_buff[DF_laneL_PWM] );
//			motor(data_buff[DF_lane_motorR],data_buff[DF_lane_motorL]);
			run( data_buff[DF_lane_motorS] );
            pattern = 64;
            cnt1 = 0;
            break;
        }
		handle( PID() - 5);
//        motor( data_buff[DF_lane_motorS] ,data_buff[DF_lane_motorS] );
		run( data_buff[DF_lane_motorS] );
        break;

    case 64:
        /* 左レーンチェンジ終了のチェック */
        /* ！追加・変更！ ここから */
        if( check_center() ) {
            led_out( 0x0 );
            pattern = 65;
			lEncoderTotal = lEncoderHarf;
            cnt1 = 0;
        }
        /* ！追加・変更！ ここまで */
        break;
		
	case 65:
		if( lEncoderTotal - lEncoderHarf > 150 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		run(50);
		break;

    case 101:
        /* microSDの停止処理 */
        /* 脱輪した際の自動停止処理後は、必ずこの処理を行ってください */
		handle( PID());
        motor( 0, 0 );
        msdFlag = 0;
        pattern = 102;
        break;

    case 102:
        /* 最後のデータが書き込まれるまで待つ*/
		handle( PID());
        if( microSDProcessEnd() == 0 ) {
            pattern = 103;
        }
        break;

    case 103:
        /* 書き込み終了 */
 		handle( PID());
       led_out( 0x3 );
        break;
		
	case 500:
		/* メモリの読み出し */
		/*if( pushsw_get() ) {
			for(j = 0; j < 5000; j++){
				printf("%d\n",mem[j]);
			}
		}*/
		break;

	case 1000:
		/* モニタ画面 */
		printf("MiconCar parameter \n");
		printf("   Kp %d  Ki %d  Kd  %d \n",data_buff[DF_KP],data_buff[DF_KI],data_buff[DF_KD]);
		printf("   servo_center = %d\n",servo_center);
		printf("   Stop Timer = %d0\n",((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2]);
		printf("   Max Speed = %d\n",data_buff[DF_PWM]);
		printf("\n");

		printf("Crank parameter\n");	
		printf("   Speed  = %3d\n",data_buff[DF_crank_motorS]);
		printf("   handle = %d\n",data_buff[DF_crank_handlepwm]);
		printf("   MOTOR  OUT = %3d   IN = %3d \n",data_buff[DF_crank_motor1],data_buff[DF_crank_motor2]);
		printf("\n");

		printf("LaneChange parameter\n");	
		printf("   Speed  = %3d\n",data_buff[DF_lane_motorS]);
		printf("   R handle = %d  L handle = %d\n",data_buff[DF_laneR_PWM],data_buff[DF_laneL_PWM]);
		printf("   MOTOR  OUT = %3d   IN = %3d \n",data_buff[DF_lane_motorL],data_buff[DF_lane_motorR]);
		printf("\n");
	
		pattern = 0;
		
	case 2000:
		get_ad();
		printf("raw: ");
		for(i = 0; i < 16; i++){
			printf("%4d ",ImageData[i]);
		}
		printf("\n");

		printf("LPF: ");
		for(i = 0; i < 16; i++){
			printf("%4d ",val_ImageData[i]);
		}
		printf("\n");

		printf("Nor: ");
		for(i = 0; i < 16; i++){
			float_printf(sval[i],2);
			printf(" ");
		}
		printf("\n");

		printf("bi:  ");
		for(i = 0; i < 16; i++){
			printf("%4d ",bi_sensor[i]);
		}
		printf("\n");

		printf("pos = ");
		float_printf(pos,3);
		printf(" PID = %d  ",PID());
		printf("lEncoderTotal = %ld\n",lEncoderTotal);
		printf("startbar_get = %d  allBlack = %d\n\n",startbar_get(),allBlack());
		timer(1500);
		break;
		
    default:
        /* どれでもない場合は待機状態に戻す */
        pattern = 0;
        break;
    }
    }
}
/************************************************************************/
/* ディップスイッチ値読み込み                                           */
/* 戻り値 スイッチ値 0～15                                              */
/************************************************************************/
unsigned char dipsw_get( void )
{
    unsigned char sw;

    sw = p1 & 0x0f;                     /* P1_3～P1_0読み込み           */

    return  sw;
}

/************************************************************************/
/* プッシュスイッチ値読み込み                                           */
/* 戻り値 スイッチ値 ON:1 OFF:0                                         */
/************************************************************************/
unsigned char pushsw_get( void )
{
    unsigned char sw;

    sw  = ~p2;                          /* スイッチのあるポート読み込み */
    sw &= 0x01;

    return  sw;
}

/************************************************************************/
/* LED制御                                                              */
/* 引数　スイッチ値 LED0:bit0 LED1:bit1  "0":消灯 "1":点灯              */
/* 例)0x3→LED1:ON LED0:ON  0x2→LED1:ON LED0:OFF                       */
/************************************************************************/
void led_out( unsigned char led )
{
    unsigned char data;

    led = ~led;
    led <<= 6;
    data = p2 & 0x3f;
    p2 = data | led;
}
/************************************************************************/
/* end of file                                                          */
/************************************************************************/