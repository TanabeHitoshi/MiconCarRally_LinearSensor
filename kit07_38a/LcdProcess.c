/*======================================*/
/* インクルード                         */
/*======================================*/
#include <stdio.h>
#include "lcd_lib.h"                    /* LCD表示用追加                */
#include "switch_lib.h"                 /* スイッチ追加                 */
#include "data_flash_lib.h"             /* データフラッシュライブラリ   */

/* DataFlash関連 */

#define DF_ADDR_START   0x3000          /* 書き込み開始アドレス         */
#define DF_ADDR_END     0x33ff          /* 書き込み終了アドレス         */
#define DF_PARA_SIZE    64              /* DataFlashパラメータ数        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlashチェック            */

extern void handle( int angle );


extern signed char     data_buff[ DF_PARA_SIZE ];
extern unsigned long   cnt_lcd;                /* LCD処理で使用                */
extern int             pattern;                /* パターン番号                 */
extern int             servo_center;           /* サーボセンタ値               */
extern int sensor8;

/* LCD関連 */
int             lcd_pattern = 2;

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
            data_buff[DF_CRANK_PWM] = 40;
			data_buff[DF_crank_motor1] = 0;
			data_buff[DF_crank_motor2] = 0;
			data_buff[DF_laneL_PWM] = 0;
			data_buff[DF_laneR_PWM] = 0;
			data_buff[DF_lane_motorL] = 0;
			data_buff[DF_lane_motorR] = 0;
			data_buff[DF_lane_motorS] = 0;
			data_buff[DF_crank_handlepwm] = 0;
			i = 3000;
            data_buff[DF_STOP1] 	= i >> 8;
            data_buff[DF_STOP2] 	= i & 0xff;

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

    if( pattern != 1 ) {
        if( cnt_lcd >= 250 ) {
            cnt_lcd = 0;
            lcdPosition( 0, 0 );
                     /* 0123456789abcbef 1行16文字 */
            lcdPrintf( "pattern = %3d   ", pattern );
                     /* 01234567..89abcde.f 1行16文字 */
            lcdPrintf( "sensor=%02x bar=%d ",
                        sensor8, startbar_get() );
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
        if( lcd_pattern == 16 ) lcd_pattern = 1;
    }

    /* スイッチ2　メニュー－１ */
    if( getSwFlag(SW_2) ) {
        lcd_pattern--;
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );

        if( lcd_pattern == 0 ) lcd_pattern = 11;
    }

    /* LCD、スイッチ処理 */
    switch( lcd_pattern ) {
    case 1:
        /* サーボセンタ値調整 */
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
        lcdPrintf( "sensor=%02x bar=%d ",
                    sensor8, startbar_get() );
        break;

    case 2:
        /* PWM値調整 */
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
        /* 比例値 Kp */
        i = data_buff[DF_KP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1行16文字 */
        lcdPrintf( "03 Kp    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1行16文字 */
        break;	

    case 4:
        /* 比例値 Kp */
        i = data_buff[DF_KI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1行16文字 */
        lcdPrintf( "04 Ki    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1行16文字 */
        break;	
		
	case 5:
        /* 比例値 Kp */
        i = data_buff[DF_KD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1行16文字 */
        lcdPrintf( "05 Kd    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1行16文字 */
        break;	
		
    case 6:
        /* タイマー値調整 */
		i = ((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2];
        if( getSwFlag(SW_1) ) {
            i+=100;
            if( i > 9000 ) i = 9000;
        }
        if( getSwFlag(SW_0) ) {
            i-=100;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_STOP1] = i >> 8;
        data_buff[DF_STOP2] = i & 0xff;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "06  stop time       %04d mS", i );
        break;
	case 7://クランク時モーター速度
			i = data_buff[DF_crank_motorS];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_crank_motorS] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "07 Crank Speed      %03d", i );
        break;	
	case 8:
		i = data_buff[DF_crank_handlepwm];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 35 ) i = 35;
        }
        if( getSwFlag(SW_0) ) {
           	i--;
           	if( i < 0 ) i = 0;
       	}
        	data_buff[DF_crank_handlepwm] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "08 Crank handle     %03d", i );
        break;
	case 9://クランク時モーター　OUT
			i = data_buff[DF_crank_motor1];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_crank_motor1] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "09 Crank motor  OUT   %03d", i );
        break;
	case 10://クランク時モーター　IN
			i = data_buff[DF_crank_motor2];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < -100 ) i = -100;
        	}
        	data_buff[DF_crank_motor2] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "10 Crank motor  IN   %03d", i );
        break;

	case 11:
			i = data_buff[DF_lane_motorS];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorS] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "11 LaneChange   Speed    %3d", i );
        break;
	case 12:
			i = data_buff[DF_laneL_PWM];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 30 ) i = 30;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_laneL_PWM] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "12 LaneChange   L handle   %03d", i );
        break;
	case 13:
			i = data_buff[DF_laneR_PWM];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 30 ) i = 30;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_laneR_PWM] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "13 LaneChange   R handle   %03d", i );
        break;
	case 14:
			i = data_buff[DF_lane_motorL];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorL] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "14 LaneChange   Motor OUT %03d", i );
        break;
	case 15:
			i = data_buff[DF_lane_motorR];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorR] = i;

        /* LCD処理 */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1行16文字 */
        lcdPrintf( "15 LaneChange   Motor IN  %03d", i );
        break;

	}
}