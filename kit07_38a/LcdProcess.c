/*======================================*/
/* インクルード                         */
/*======================================*/
#include <stdio.h>
#include "lcd_lib.h"                    /* LCD表示用追加                */
#include "switch_lib.h"                 /* スイッチ追加                 */
#include "data_flash_lib.h"             /* データフラッシュライブラリ   */
#include "drive.h"
/* DataFlash関連 */

#define DF_ADDR_START   0x3000          /* 書き込み開始アドレス         */
#define DF_ADDR_END     0x33ff          /* 書き込み終了アドレス         */
#define DF_PARA_SIZE    64              /* DataFlashパラメータ数        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlashチェック            */

extern void handle( int angle );


signed char     data_buff[ DF_PARA_SIZE ];
extern unsigned long   cnt_lcd;                /* LCD処理で使用                */
extern int             pattern;                /* パターン番号                 */
extern int             servo_center;           /* サーボセンタ値               */
extern int sensor8;

/* LCD関連 */
int             lcd_pattern = 0;
char					MCR_program[15][3];				/* コースをプログラム 			*/

/************************************************************************/
/* DataFlashのパラメータ読み込み                                        */
/* 引数         なし                                                    */
/* 戻り値       なし                                                    */
/************************************************************************/
void readDataFlashParameter( void )
{
    int             i;
    unsigned int    st = DF_ADDR_END + 1 - DF_PARA_SIZE;
    signed char     c;

    while( 1 ) {
        // 読み込む番地を探す
        readDataFlash( st, &c, 1 );
        if( c == 0x11 ) {
            readDataFlash( st, data_buff, DF_PARA_SIZE );
            break;
        }
        st -= DF_PARA_SIZE;

        if( st < DF_ADDR_START ) {
            // 該当無し　初めて使用
            for( i=0; i<DF_PARA_SIZE; i++ ) data_buff[ i ] = 0;
            data_buff[DF_CHECK]     = 0x11;
            data_buff[DF_SERVO1]    = SERVO_CENTER >> 8;
            data_buff[DF_SERVO2]    = SERVO_CENTER & 0xff;
            data_buff[DF_PWM]       = 100;
//			data_buff[DF_crank_handlepwm] = 0;
            data_buff[DF_STOP] 	= 30;

            blockEraseDataFlash( DF_ADDR_START );
            writeDataFlash( DF_ADDR_START, data_buff, DF_PARA_SIZE );
            break;
        }
    }
}
/************************************************************************/
/* DataFlashへパラメータ書き込み                                        */
/* 引数         なし                                                    */
/* 戻り値       なし                                                    */
/************************************************************************/
void writeDataFlashParameter( void )
{
    unsigned int    st = DF_ADDR_START;
    signed char     c;

    while( 1 ) {
        // 書き込む番地を探す
        readDataFlash( st, &c, 1 );
        if( c == -1 ) {
            writeDataFlash( st, data_buff, DF_PARA_SIZE );
            break;
        }

        st += DF_PARA_SIZE;

        if( st > DF_ADDR_END ) {
            // すべて使用したら、イレーズして先頭に書き込み
            blockEraseDataFlash( DF_ADDR_START );
            writeDataFlash( DF_ADDR_START, data_buff, DF_PARA_SIZE );
            break;
        }
    }
}
/************************************************************************/
/* LCDとスイッチを使ったパラメータセット処理                            */
/* 引数         なし                                                    */
/* 戻り値       なし                                                    */
/************************************************************************/
int lcdProcess( void )
{
    int i,n;
	
    if( pattern > 10 ) {
        if( cnt_lcd >= 250 ) {
            cnt_lcd = 0;
            lcdPosition( 0, 0 );
                     /* 0123456789abcbef 1行16文字 */
            lcdPrintf( "pattern = %3d   ", pattern );
                 /* 01234567..89abcde.f 1行16文字 */
	        lcdPrintf( "                ");
        }
        return;
    }

    /* スイッチ4　設定値保存 */
    if( getSwFlag(SW_4) ) {
        // パラメータ保存
        writeDataFlashParameter();
    }

    /* スイッチ3　メニュー＋１ */
    if( getSwFlag(SW_3) ) {
        lcd_pattern++;
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );
        if( lcd_pattern == 19 ) lcd_pattern = 0;
    }

    /* スイッチ2　メニュー−１ */
    if( getSwFlag(SW_2) ) {
        lcd_pattern--;
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );
        if( lcd_pattern < 0 ) lcd_pattern = 18;
    }

    /* LCD、スイッチ処理 */
    switch( lcd_pattern ) {
	case 0:/* レース選択 */
        i = data_buff[DF_RACE];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 3 ) i = 0;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 3;
        }
        data_buff[DF_RACE] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                           /* 0123456789ab..f 1行16文字 */
		if(i == 0)lcdPrintf( "  Trace       " );
							   		 /* 0123456789abcdef0123456789abcdef 1行16文字 */
		else if(i == 1)lcdPrintf( "  Sprint Left" );
					  /* 0123456789abcdef0123456789abcdef 1行16文字 */
		else if(i == 2) lcdPrintf( "  Sprint Right" );
		else  lcdPrintf( "  Moniter       " );
                 /* 01234567..89abcde.f 1行16文字 */
        lcdPrintf( "  ");		
	break;
    case 1:/* サーボセンタ値調整 */
        if( getSwFlag(SW_1) ) {
            servo_center++;
            if( servo_center > 10000 ) servo_center = 10000;
        }
        if( getSwFlag(SW_0) ) {
            servo_center--;
            if( servo_center < 1000 ) servo_center = 1000;
        }
        data_buff[DF_SERVO1] = servo_center >> 8;
        data_buff[DF_SERVO2] = servo_center & 0xff;
        handle( 0 );

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789ab..f 1行16文字 */
        lcdPrintf( "01 servo = %05d", servo_center );
                 /* 01234567..89abcde.f 1行16文字 */
        lcdPrintf( "                ");
        break;
    case 2:
        /* 速度調整 */
        i = data_buff[DF_PWM];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_PWM] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1行16文字 */
        lcdPrintf( "02 Max SPEED         %03d        ", i );
                 /* 01234567..89abcde.f 1行16文字 */
        break;
    case 3:
        /* スプリント速度調整 */
        i = data_buff[DF_PWM_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_PWM_S] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1行16文字 */
        lcdPrintf( "03 Sprint SPEED      %03d        ", i );
                 /* 01234567..89abcde.f 1行16文字 */
        break;
    case 4:
        /* サーボ比例値 KSp */
        i = data_buff[DF_KSP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "04 Servo           KSp = %d.%d ", i,n );
        break;	

    case 5:
        /* サーボ積分値 KSi */
        i = data_buff[DF_KSI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "05 Servo           KSi = %d.%d ", i,n );
        break;			
	case 6:
        /* サーボ微分値 KSd */
        i = data_buff[DF_KSD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "06 Servo           KSd = %d.%d ", i,n );
        break;	
    case 7:/* マシン比例値 KMp */
        i = data_buff[DF_KMP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "07 Machine         KMp = %d.%d ", i,n );
        break;	
    case 8:/* サーボ積分値 KMi */
        i = data_buff[DF_KMI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "08 Machine         KMi = %d.%d ", i,n );
        break;			
	case 9:/* マシン微分値 KMd */
        i = data_buff[DF_KMD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "09 Machine         KMd = %d.%d ", i,n );
        break;	
    case 10:/* カメラ比例値（ストレート） Kp */
        i = data_buff[DF_KP_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "10 Camera         Kp_S = %d.%d ", i,n );
        break;	
    case 11:/* カメラ積分値（ストレート） Ki */
        i = data_buff[DF_KI_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "11 Camera          Ki_S = %d.%d ", i,n );
        break;			
	case 12:/* カメラ微分値（ストレート） KMd */
        i = data_buff[DF_KD_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "12 Camera          Kd_S = %d.%d ", i,n );
        break;	
    case 13:/* カメラ比例値（カーブ） Kp */
        i = data_buff[DF_KP_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "13 Camera         Kp_C = %d.%d ", i,n );
        break;	
    case 14:/* カメラ積分値（カーブ） Ki */
        i = data_buff[DF_KI_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "14 Camera          Ki_C = %d.%d ", i,n );
        break;			
	case 15:/* カメラ微分値（カーブ） Kd */
        i = data_buff[DF_KD_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "15 Camera          Kd_C = %d.%d ", i,n );
        break;	

	case 16:/* カメラスタート位置 */
			i = data_buff[DF_LineStart];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 127 ) i = 127;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
       	data_buff[DF_LineStart] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "16 LineStart       %03d", i );
        break;
	case 17:/* カメラストップ位置 */
			i = data_buff[DF_LineStop];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 127 ) i = 127;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
       	data_buff[DF_LineStop] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "17 LineStop       %03d", i );
        break;
	case 18:
        /* タイマー値調整 */
		i = data_buff[DF_STOP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 30 ) i = 30;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_STOP] = i;
		
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "18  stop time       %2d s", i );
        break;

	}
}
/************************************************************************/
/* シリアル通信を用いたパラメータセット処理                            */
/* 引数         なし                                                    */
/* 戻り値       なし                                                    */
/************************************************************************/
void sciProcess( void )
{
	int s,t;
	int i,n;
	
	/* モニタ画面 */
	printf("\n ---- Parameter change MEMU ---- \n");
	printf("1:Machine parameter \n");
	printf("   servo_center = %d\n",servo_center);
	printf("   Max Speed = %d\n",data_buff[DF_PWM]);
	printf("   Stop Timer = %dS\n",data_buff[DF_STOP]);
	printf("   Servo  parameter KSp %d.%d   KSi %d.%d   KSd %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10,data_buff[DF_KSI]/10,data_buff[DF_KSI]%10,data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
	printf("   Macine parameter KMp %d.%d   KMi %d.%d   KMd %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10,data_buff[DF_KMI]/10,data_buff[DF_KMI]%10,data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
//	printf("   Log time %dms\n",data_buff[DF_log_time]);
//	printf("   Log start Number %d\n",data_buff[DF_log_potion]);
//	printf("\n");

	printf("2:Camera parameter\n");	
	printf("   Camera  Stright parameter Kp %d.%d   Ki %d.%d   Kd %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10,data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10,data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
	printf("   Camera  Curve parameter   Kp %d.%d   Ki %d.%d   Kd %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10,data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10,data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
	printf("   Camera  LineStart %d   LineStop %d\n",data_buff[DF_LineStart],data_buff[DF_LineStop]);
//	printf("\n");

	printf("3:Trace parameter\n");	
//	printf("\n");

	printf("4:Sprint parameter\n");	
	printf("   Sprint Speed = %d\n",data_buff[DF_PWM_S]);
	printf("\n");
	;
	printf("change parameter number? >> ");
	scanf("%d",&s);
	switch(s){
		case 1:
			printf(" MiconCar parameter \n");
			printf("   1: servo_center = %d\n",servo_center);
			printf("   2: Max Speed = %d\n",data_buff[DF_PWM]);
			printf("   3: Stop Timer = %dS\n",data_buff[DF_STOP]);
			printf("   4: Servo parameter KSp %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10);
			printf("   5: Servo parameter KSi %d.%d\n",data_buff[DF_KSI]/10,data_buff[DF_KSI]%10);
			printf("   6: Servo parameter KSd %d.%d\n",data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
			printf("   7: Machine parameter KMp %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10);
			printf("   8: Machine parameter KMi %d.%d\n",data_buff[DF_KMI]/10,data_buff[DF_KMI]%10);
			printf("   9: Machine parameter KMd %d.%d\n",data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
//			printf("   7: Log time %dms\n",data_buff[DF_log_time]);
//			printf("   8: Log start Number %d\n",data_buff[DF_log_potion]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
				   servo_center  = (unsigned char)data_buff[DF_SERVO1] * 0x100;
 				   servo_center |= (unsigned char)data_buff[DF_SERVO2];
					printf("1: servo_center = %d\n",servo_center);
					printf("parameter value? >> ");
					scanf("%d",&s);
					if(s > 10000) s = 10000;
					if(s < 1000) s = 1000;
 					data_buff[DF_SERVO1] = s >> 8;
        			data_buff[DF_SERVO2] = s & 0xff;
					writeDataFlashParameter();
				break;
				case 2:
					printf("2: Max Speed = %d\n",data_buff[DF_PWM]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_PWM] = s;
					writeDataFlashParameter();
				break;
				case 3:
					printf("3: Stop Timer = %dS\n",data_buff[DF_STOP]);
					printf("parameter value? (0 <-> 30)>> ");
					scanf("%d",&s);
					if(s > 30) s = 30;
					if(s < 0) s = 0;
			        data_buff[DF_STOP] = s;
					writeDataFlashParameter();
				break;
				case 4:
					printf("4: Servo KSp %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSP] = s;
					writeDataFlashParameter();
				break;
				case 5:
					printf("5: Servo KSi %d.%d\n",data_buff[DF_KSI]/10,data_buff[DF_KSI]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSI] = s;
					writeDataFlashParameter();
				break;
				case 6:
					printf("6: Servo KSd %d.%d\n",data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSD] = s;
					writeDataFlashParameter();
				break;
				case 7:
					printf("7: Machine KMp %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMP] = s;
					writeDataFlashParameter();
				break;
				case 8:
					printf("8: Machine KMi %d.%d\n",data_buff[DF_KMI]/10,data_buff[DF_KMI]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMI] = s;
					writeDataFlashParameter();
				break;
				case 9:
					printf("9: Machine KMd %d.%d\n",data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMD] = s;
					writeDataFlashParameter();
				break;
				case 10:
//					printf("7: Log time %dms\n",data_buff[DF_log_time]);
					printf("parameter value? (1 <-> 10) >> ");
					scanf("%d",&s);
					if(s > 10) s = 10;
					if(s < 0) s = 0;
//			        data_buff[DF_log_time] = s;
					writeDataFlashParameter();
				break;
				case 11:
//					printf("8: Log start Number %d\n",data_buff[DF_log_potion]);
					printf("parameter value? (0 <-> 14) >> ");
					scanf("%d",&s);
					if(s > 14) s = 10;
					if(s < 0) s = 0;
//			        data_buff[DF_log_potion] = s;
					writeDataFlashParameter();
				break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 2:
			printf(" Camera parameter\n");	
			printf("   1: Camera Stright parameter Kp %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10);
			printf("   2: Camera Stright parameter Ki %d.%d\n",data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10);
			printf("   3: Camera Stright parameter Kd %d.%d\n",data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
			printf("   4: Camera Curve parameter Kp %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10);
			printf("   5: Camera Curve parameter Ki %d.%d\n",data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10);
			printf("   6: Camera Curve parameter Kd %d.%d\n",data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
			printf("   7: Camera LineStart %d\n",data_buff[DF_LineStart]);
			printf("   8: Camera LineStop  %d\n",data_buff[DF_LineStop]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
					printf("1: Camera Stright Kp %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_S] = s;
					writeDataFlashParameter();
				break;
				case 2:
					printf("2: Camera Stright Ki %d.%d\n",data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_S] = s;
					writeDataFlashParameter();
				break;
				case 3:
					printf("3: Camera Stright Kd %d.%d\n",data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_S] = s;
					writeDataFlashParameter();
				break;
				case 4:
					printf("4: Camera Curve Kp %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_C] = s;
					writeDataFlashParameter();
				break;
				case 5:
					printf("5: Camera Curve Ki %d.%d\n",data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_C] = s;
					writeDataFlashParameter();
				break;
				case 6:
					printf("6: Camera Curvet Kd %d.%d\n",data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_C] = s;
					writeDataFlashParameter();
				break;
				case 7:
					printf("7: Camera LineStart %d\n",data_buff[DF_LineStart]);
					printf("parameter value? (0 <-> 127) >> ");
					scanf("%d",&s);
					if(s > 127) s = 127;
					if(s < 0) s = 0;
			        data_buff[DF_LineStart] = s;
					writeDataFlashParameter();
				break;
				case 8:
					printf("8: Camera LineStop %d\n",data_buff[DF_LineStop]);
					printf("parameter value? (0 <-> 127) >> ");
					scanf("%d",&s);
					if(s > 127) s = 127;
					if(s < 0) s = 0;
			        data_buff[DF_LineStop] = s;
					writeDataFlashParameter();
				break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 3:
			printf("3:Trace parameter\n");	
//			printf("   5: R MOTOR = %3d \n",data_buff[DF_lane_motorR]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
//					printf("1: Speed  = %3d\n",data_buff[DF_lane_motorS]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
//			        data_buff[DF_lane_motorS] = s;
					writeDataFlashParameter();
				break;
				case 2:
//					printf("2: L handle = %d\n",data_buff[DF_laneL_PWM]);
					printf("parameter value? (-45 <-> 45) >> ");
					scanf("%d",&s);
					if(s > 45) s = 45;
					if(s < -45) s = -45;
//			        data_buff[DF_laneL_PWM] = s;
					writeDataFlashParameter();
				break;
				case 3:
//					printf("3: R handle = %d\n",data_buff[DF_laneR_PWM]);
					printf("parameter value? (-45 <-> 45) >> ");
					scanf("%d",&s);
					if(s > 45) s = 45;
					if(s < -45) s = -45;
//			        data_buff[DF_laneR_PWM] = s;
					writeDataFlashParameter();
				break;
				case 4:
//					printf("4: Changing Speed = %3d \n",data_buff[DF_lane_motor]);
					printf("parameter value? (-100 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < -100) s = -100;
//			        data_buff[DF_lane_motor] = s;
					writeDataFlashParameter();
					break;
				case 5:
//					printf("   5: R MOTOR = %3d \n",data_buff[DF_lane_motorR]);
					printf("parameter value? (-100 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < -100) s = -100;
//			        data_buff[DF_lane_motorR] = s;
					writeDataFlashParameter();
					break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 4:
			printf("4:Sprint parameter\n");	
			printf("   1: Sprint Speed = %d\n",data_buff[DF_PWM_S]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
					printf("1: Speed  = %3d\n",data_buff[DF_PWM_S]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_PWM_S] = s;
					writeDataFlashParameter();
				break;
				case 2:
//					printf("2: L handle = %d\n",data_buff[DF_laneL_PWM]);
					printf("parameter value? (-45 <-> 45) >> ");
					scanf("%d",&s);
					if(s > 45) s = 45;
					if(s < -45) s = -45;
//			        data_buff[DF_laneL_PWM] = s;
					writeDataFlashParameter();
				break;
				case 3:
//					printf("3: R handle = %d\n",data_buff[DF_laneR_PWM]);
					printf("parameter value? (-45 <-> 45) >> ");
					scanf("%d",&s);
					if(s > 45) s = 45;
					if(s < -45) s = -45;
//			        data_buff[DF_laneR_PWM] = s;
					writeDataFlashParameter();
				break;
				case 4:
//					printf("4: Changing Speed = %3d \n",data_buff[DF_lane_motor]);
					printf("parameter value? (-100 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < -100) s = -100;
//			        data_buff[DF_lane_motor] = s;
					writeDataFlashParameter();
					break;
				case 5:
//					printf("   5: R MOTOR = %3d \n",data_buff[DF_lane_motorR]);
					printf("parameter value? (-100 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < -100) s = -100;
//			        data_buff[DF_lane_motorR] = s;
					writeDataFlashParameter();
					break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 5:
			while(1){
			printf(" 99: Gate 1 --> 0  Gate 2 --> %d\n",data_buff[DF_MCR_P_N]);
			i = 0;
			while( i < 15){
				printf(" %2d: %c , ",i,MCR_program[i][0]);
				switch( MCR_program[i][0]){
					case 'c':
						printf("%d , %c \n",MCR_program[i][1],MCR_program[i][2]);
						break;
					case 's':
						printf("%d000 , %d \n",MCR_program[i][1],MCR_program[i][2]);
						break;
					case 'l':
						printf("%c \n",MCR_program[i][1]);
						break;
					default:
						printf("\n");
					break;	
				}
				i++;
			}
			printf("change program number? >> ");
			scanf("%d",&s);
			if(s == 99){					/* ２番ゲートstart position */
				printf("Gate 2 --> \n");
				scanf("%d",&t);
				data_buff[DF_MCR_P_N] = t;									
			}else{							/* MCR program */
				printf("1->saka  2->Clank  3->LenChange  4->Return  5->Clean  6->Insert  7->Delete\n");
				scanf("%d",&t);
				switch(t){
					case 1:/* 坂道 */
						data_buff[(DF_MCR_P00+3*s)] = 's';
						
						printf("distance (0 <-> 125)\n");
						scanf("%d",&t);
						data_buff[(DF_MCR_P00+3*s)+1] = t;
							
						printf("Speed value? (0 <-> 100) >> ");
						scanf("%d",&t);
						if(t > 100) t = 100;
						if(t < 0) t = 0;
				        data_buff[(DF_MCR_P00+3*s)+2] = t;
					break;	
					case 2:/* クランク */
						data_buff[(DF_MCR_P00+3*s)] = 'c';
							
						printf("Speed (0 <-> 100)\n");
						scanf("%d",&t);
						data_buff[(DF_MCR_P00+3*s)+1] = t;
							
						printf("Left->1 or Right-> 2\n");
						scanf("%d",&t);
						if(t == 1)data_buff[(DF_MCR_P00+3*s)+2] = 'L';
						else data_buff[(DF_MCR_P00+3*s)+2] = 'R';
					break;	
					case 3:/* レーンチェンジ */
						data_buff[(DF_MCR_P00+3*s)] = 'l';
							
						printf("Left->1 or Right-> 2\n");
						scanf("%d",&t);
						if(t == 1)data_buff[(DF_MCR_P00+3*s)+1] = 'L';
						else data_buff[(DF_MCR_P00+3*s)+1] = 'R';
					break;	
					case 4:/* リターン */
						data_buff[(DF_MCR_P00+3*s)] = 'r';
					break;
					case 5:/* クリア */
						data_buff[(DF_MCR_P00+3*s)] = ' ';;
						data_buff[(DF_MCR_P00+3*s)+1] = ' ';
						data_buff[(DF_MCR_P00+3*s)+2] = ' ';
					break;
					case 6:/* 挿入 */
						for(i = 14; i > s; i--){
							data_buff[(DF_MCR_P00+3*i)] = MCR_program[i-1][0];
							data_buff[(DF_MCR_P00+3*i)+1] = MCR_program[i-1][1];
							data_buff[(DF_MCR_P00+3*i)+2] = MCR_program[i-1][2];
							MCR_program[i][0] = data_buff[(DF_MCR_P00+3*i)];
							MCR_program[i][1] = data_buff[(DF_MCR_P00+3*i)+1];
							MCR_program[i][2] = data_buff[(DF_MCR_P00+3*i)+2];
							data_buff[(DF_MCR_P00+3*s)] = ' ';;
							data_buff[(DF_MCR_P00+3*s)+1] = ' ';
							data_buff[(DF_MCR_P00+3*s)+2] = ' ';
						}							
					break;
					case 7:/* 削除 */
						for(i = s; i < 15; i++){
							data_buff[(DF_MCR_P00+3*i)] = MCR_program[i+1][0];
							data_buff[(DF_MCR_P00+3*i)+1] = MCR_program[i+1][1];
							data_buff[(DF_MCR_P00+3*i)+2] = MCR_program[i+1][2];
							MCR_program[i][0] = data_buff[(DF_MCR_P00+3*i)];
							MCR_program[i][1] = data_buff[(DF_MCR_P00+3*i)+1];
							MCR_program[i][2] = data_buff[(DF_MCR_P00+3*i)+2];
							data_buff[(DF_MCR_P00+3*14)] = ' ';;
							data_buff[(DF_MCR_P00+3*14)+1] = ' ';
							data_buff[(DF_MCR_P00+3*14)+2] = ' ';
						}							
					break;		
				}
				MCR_program[s][0] = data_buff[(DF_MCR_P00+3*s)];
				MCR_program[s][1] = data_buff[(DF_MCR_P00+3*s)+1];
				MCR_program[s][2] = data_buff[(DF_MCR_P00+3*s)+2];
			}
			writeDataFlashParameter();
			}
		break;
		default:
			printf("NOT !!!\n");
		break;
	}
	printf("\n\n\n");
}

