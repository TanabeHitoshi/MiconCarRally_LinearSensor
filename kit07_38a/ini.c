/*======================================*/
/* インクルード                         */
/*======================================*/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFRの定義ファイル    */
#include "printf_lib.h"                 /* printf使用ライブラリ         */
#include "microsd_lib.h"                /* microSD制御ライブラリ        */
#include "lcd_lib.h"                    /* LCD表示用追加                */
#include "switch_lib.h"                 /* スイッチ追加                 */
#include "data_flash_lib.h"             /* データフラッシュライブラリ   */
#include "camera.h"
#include "drive.h"
#include "ini.h"
#include "trace.h"
#include "isEncoder.h"

volatile unsigned long   cnt0;					/* timer関数用                  */
volatile unsigned long   cnt1;					/* main内で使用                 */
volatile unsigned long   cnt_lcd;				/* LCD処理で使用                */
volatile unsigned long   stop_timer;			/* 走行タイマー					*/
volatile unsigned int   cnt_Curve;				/* カーブで使用  */
volatile unsigned int   cnt_Stright;			/* 直進で使用  */

int             pattern;                /* パターン番号                 */
int				Srevo_state;			/* サーボの制御あり -> 1　なり -> 0	*/
/* microSD関連変数 */
signed char     msdBuff[ 512+32 ];      /* 一時保存バッファ(32は予備)   */
int             msdFlag;                /* 1:データ記録 0:記録しない    */
int             msdTimer;               /* 取得間隔計算用               */
int             msdError;               /* エラー番号記録               */
/* 内部記録 */
struct MEM{
	unsigned int pattern;
	unsigned int distance;
	unsigned int center;
	int PID;
	char white;
	char speed;
};
int				memCnt = 0;					/* 内部記録の番号 */
struct MEM		mem[500];					/* 内部記録の配列 */

/************************************************************************/
/* R8C/38A スペシャルファンクションレジスタ(SFR)の初期化                */
/************************************************************************/
void init( void )
{
    int i;

    /* クロックをXINクロック(20MHz)に変更 */
    prc0  = 1;                          /* プロテクト解除               */
    cm13  = 1;                          /* P4_6,P4_7をXIN-XOUT端子にする*/
    cm05  = 0;                          /* XINクロック発振              */
    for(i=0; i<50; i++ );               /* 安定するまで少し待つ(約10ms) */
    ocd2  = 0;                          /* システムクロックをXINにする  */
    prc0  = 0;                          /* プロテクトON                 */

    /* ポートの入出力設定 */
    prc2 = 1;                           /* PD0のプロテクト解除          */
//	pd0 = 0xfe;                         /* 0:AOのｱﾅﾛｸﾞ電圧  1:SI 2:CLK  */
    pd0 = 0x0f;                         /* SI -> P0_1  CLK -> p0_2  AN0 -> P0_7  Light -> P0_3*/
    pd1 = 0xd0;                         /* 5:RXD0 4:TXD0 3-0:DIP SW     */
    p2  = 0xc0;
    pd2 = 0xfe;                         /* 7-0:モータドライブ基板Ver.4  */
    pd3 = 0xfe;                         /* 0:ロータリエンコーダ         */
    p4  = 0x20;                         /* P4_5のLED:初期は点灯         */
    pd4 = 0xb8;                         /* 7:XOUT 6:XIN 5:LED 2:VREF    */
    pd5 = 0x7f;                         /* 7-0:LCD/microSD基板          */
    pd6 = 0xef;                         /* 4-0:LCD/microSD基板          */
    pd7 = 0x0f;                         /* A/D変換                      */
    pd8 = 0xff;                         /*                              */
    pd9 = 0x3f;                         /*                              */
    pur0 = 0x04;                        /* P1_3～P1_0のプルアップON     */

    /* タイマRBの設定 */
    /* 割り込み周期 = 1 / 20[MHz]   * (TRBPRE+1) * (TRBPR+1)
                    = 1 / (20*10^6) * 200        * 100
                    = 0.001[s] = 1[ms]
    */
    trbmr  = 0x00;                      /* 動作モード、分周比設定       */
    trbpre = 200-1;                     /* プリスケーラレジスタ         */
    trbpr  = 100-1;                     /* プライマリレジスタ           */
    trbic  = 0x07;                      /* 割り込み優先レベル設定       */
    trbcr  = 0x01;                      /* カウント開始                 */

    /* タイマRD リセット同期PWMモードの設定*/
    /* PWM周期 = 1 / 20[MHz]   * カウントソース * (TRDGRA0+1)
               = 1 / (20*10^6) * 8              * 40000
               = 0.016[s] = 16[ms]
    */
    trdpsr0 = 0x08;                     /* TRDIOB0,C0,D0端子設定        */
    trdpsr1 = 0x05;                     /* TRDIOA1,B1,C1,D1端子設定     */
    trdmr   = 0xf0;                     /* バッファレジスタ設定         */
    trdfcr  = 0x01;                     /* リセット同期PWMモードに設定  */
    trdcr0  = 0x23;                     /* ソースカウントの選択:f8      */
    trdgra0 = trdgrc0 = PWM_CYCLE;      /* 周期                         */
    trdgrb0 = trdgrd0 = 0;              /* P2_2端子のON幅設定           */
    trdgra1 = trdgrc1 = 0;              /* P2_4端子のON幅設定           */
    trdgrb1 = trdgrd1 = SERVO_CENTER;   /* P2_5端子のON幅設定           */
    trdoer1 = 0xcd;                     /* 出力端子の選択               */
    trdstr  = 0x0d;                     /* TRD0カウント開始             */

    /* タイマRG タイマモード(両エッジでカウント)の設定 */
    timsr = 0x40;                       /* TRGCLKA端子 P3_0に割り当てる */
    trgcr = 0x15;                       /* TRGCLKA端子の両エッジでカウント*/
    trgmr = 0x80;                       /* TRGのカウント開始            */	
}

/************************************************************************/
/* タイマRB 割り込み処理                                                */
/************************************************************************/
#pragma interrupt intTRB(vect=24)
void intTRB( void )
{
    static signed char *p = msdBuff;    /* 記録ﾃﾞｰﾀの書き込み先を示すﾎﾟｲﾝﾀ*/
    static int  line_no;                /* 行番号                       */
    int ret;
    unsigned int i;

    cnt0++;
    cnt1++;
    cnt_lcd++;
	cnt_Curve++;
	if(pattern > 10)stop_timer++;
					
	// エンコーダ制御 + 走行データ制御
	iTimer10++;

	if( iTimer10 >= 10 ){
		iTimer10 = 0;
		i = trg;
		iEncoder = i - uEncoderBuff;
		lEncoderTotal += iEncoder;
		Acceleration = pre_iEncoder - iEncoder;
		pre_iEncoder = iEncoder;
		uEncoderBuff = i;
		
		if(Srevo_state == 1) servo_Trace();
		servo_angle = motor_Trace();
	}	
    /* 拡張スイッチ用関数(1msごとに実行)    */
    switchProcess();

    /* LCD表示処理用関数(1msごとに実行)     */
    lcdShowProcess();

    /* microSD間欠書き込み処理(1msごとに実行)   */
    microSDProcess();

    /* microSD記録処理 */
    if( msdFlag == 1 ) {
        /* 記録間隔のチェック */
        msdTimer++;
        if( msdTimer == 9 ) {
            /* RAMに記録 ここから(ｻﾝﾌﾟﾙﾌﾟﾛｸﾞﾗﾑの実測で150us) */
            /* 空きはスペースで埋めてください                */
            // 0 パターン
            convertDecimalToStr( pattern, 3, p );
            p += 3;*p++ = ',';
            // 4 センサ
            convertDecimalToStr( Center, 4, p );
            p += 4;*p++ = ',';
            // 9 白
            convertDecimalToStr( White, 2, p );
            p += 2;*p++ = ',';
            // 12 位置
            convertDecimalToStr( (int)(0 * 10.0), 4, p );
            p += 4;*p++ = ',';
            // 17 速度
            convertDecimalToStr( iEncoder, 2, p );
            p += 2;*p++ = ',';
            // 20 加速度
            convertDecimalToStr( Acceleration, 3, p );
            p += 3;*p++ = ',';
            // 24 ハンドル
            convertDecimalToStr( handleBuff, 3, p );
            p += 3;*p++ = ',';
            // 28 左モータPWM値
            convertDecimalToStr( leftMotorBuff, 4, p );
            p += 4;*p++ = ',';
            // 33 右モータPWM値
            convertDecimalToStr( rightMotorBuff, 4, p );
            p += 4;*p++ = ',';
            // 38
            *p++ = ' ';
            *p++ = ' ';
            // 40
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            // 50
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            // 60
            *p++ = ' ';
            *p++ = ' ';
            *p++ = '\r'; // 62
            *p++ = '\n'; // 63
            /* RAMに記録 ここまで */
        } else if( msdTimer >= 10 ) {
            /* microSDに記録 */
            msdTimer = 0;
            if( p >= (msdBuff+512) ) {
                /* 512個になったら、microSDに記録する */
                p = msdBuff;
                ret = setMicroSDdata( msdBuff );
                if( ret == 1 ) {
                    /* 容量の上限などで書き込みができない */
                    msdFlag = 0;        /* 記録処理終了                 */
                }
            }
        }
    }
}
/************************************************************************/
/* 内部メモリの表示			                                                */
/************************************************************************/
void memPrint(void)
{
	int i,j;
	printf("pattern ,distance,sensor,pos,white,power,speed\n");

	for(i = 0; i < 500; i++){
		printf("%d,",mem[i].pattern);		
		printf("%d,",mem[i].distance);
		printf("0x%4x,",mem[i].center);
		for(j = 0; j < 16; j++){
			if(mem[i].center & 0x8000)printf("1");
			else					printf("0");
			mem[i].center = mem[i].center << 1;
		}
		printf(",");
//		float_printf(pos,3);printf(",");
		printf("%d,",mem[i].PID);
		printf("%d,",mem[i].white);
		printf("%d\n",mem[i].speed);
	}

}
/************************************************************************/
/* タイマ本体                                                           */
/* 引数　 タイマ値 1=1ms                                                */
/************************************************************************/
void timer( unsigned long timer_set )
{
	int i;
    cnt0 = 0;
    while( cnt0 < timer_set )i=0;
}
/************************************************************************/
/* end of file                                                          */
/************************************************************************/