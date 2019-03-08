/************************************************************************/
/* 対象マイコン R8C/38A                                                 */
/* ﾌｧｲﾙ内容     熱田の森								*/
/* Date         2019.2.17                                              */
/* Copyright    大阪府立城東工科高校									*/
/************************************************************************/
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

/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
void init( void );
void timer( unsigned long timer_set );
unsigned char dipsw_get( void );
unsigned char pushsw_get( void );
void led_out( unsigned char led );
void readDataFlashParameter( void );
void writeDataFlashParameter( void );
int lcdProcess( void );

/*======================================*/
/* グローバル変数の宣言                 */
/*======================================*/
const char *C_DATE = __DATE__;          /* コンパイルした日付           */
const char *C_TIME = __TIME__;          /* コンパイルした時間           */

/* DataFlash関係 */
//signed char     data_buff[ DF_PARA_SIZE ];

/************************************************************************/
/* メインプログラム                                                     */
/************************************************************************/
void main( void )
{
    int     i,j, ret, sensor ,t;
    char    fileName[ 8+1+3+1 ];        /* 名前＋'.'＋拡張子＋'\0'      */
//    unsigned char b;                    /* ！追加・変更！               */
	int CL_number,CL_BR[5],CL_SP[5],CL_BK_TM[5];
//	int	CL_BRAKE_TIME; 
//	int hd;
	int SPEED,ANGLE;

    /* マイコン機能の初期化 */
    init();                             /* 初期化                       */
    init_uart0_printf( SPEED_9600 );    /* UART0とprintf関連の初期化    */
    setMicroSDLedPort( &p6, &pd6, 0 );  /* microSD モニタLED設定        */
    asm(" fset I ");                    /* 全体の割り込み許可           */
    initLcd();                          /* LCD初期化                    */
	initSwitch();                       /* スイッチ初期化               */

	Light_OFF;
	Srevo_state = 0;
	
    readDataFlashParameter();           /* DataFlashパラメータ読み込み  */
    servo_center  = (unsigned char)data_buff[DF_SERVO1] * 0x100;
    servo_center |= (unsigned char)data_buff[DF_SERVO2];
	nowServo = servo_center;
	
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
	
	if(pushsw_get())	pattern = 5000;
	else				pattern = 1000;		/* モニター画面 */

    while( 1 ) {
    lcdProcess();		/* LCD表示、パラメータ設定処理*/
//	LPF();				/* ローパスフィルター */
	camera_Caputure();
//	printf("pattern = %d  stop_timer = %d\n",pattern,stop_timer);
	t = data_buff[DF_STOP]*1000;
	if( t != 0 && stop_timer >= ((unsigned long)t) && pattern > 100 && pattern < 1000){
		msdFlag = 0;
		motor( 0, 0 );
		pattern = 900;
	}
    switch( pattern ) {
    case 0:
		Light_OFF;
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
         pattern = 10;
         cnt1 = 0;
 		lcdPosition( 0, 0 );lcdPrintf( "Calibration");
		break;
    case 10:/* スタート待ち */
        if( pushsw_get() ) {
            /* スタート！！ */
            led_out( 0x0 );
			switch(data_buff[DF_RACE]){
				case 0:
					pattern = 200;
					set_Speed(MAX_SPEED);
					timer(2000);
				break;
				case 1:
					pattern = 300;
					set_Speed(Sprint_SPEED);
					handle(40);
					timer(2000);
				break;
				case 2:
					pattern = 400;
					set_Speed(Sprint_SPEED);
					handle(-30);
					timer(2000);
				break;
				case 3:
					pattern = 3000;
				break;
			    default:
					pattern = 200;
				break;
			}
//			pattern = 2000;
           if( msdError == 0 ) msdFlag = 1;    /* データ記録開始       */
            cnt1 = 0;
			timer( 100 );
			odometer_ini();		/* 走行距離をリセット */
	 		stop_timer = 0;
			Light_ON;
            break;
        }
		//mem_ad = 0;
    break;
/* Trace */
	case 200:
		Srevo_state = 1;
		if( pushsw_get() ) {
			pattern = 210;
		}
        if( cnt1 < 50 ) {               /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
	break;
	case 210:
		/* カーブによってPIDを変える */
		if(pid_angle > 10 || pid_angle < -10){
			set_PID(CurvePID);
		}else{
			set_PID(StrightPID);
		}
		SPEED = 100;
		/* カメラのずれによる減速 */
		if(pid_angle > 0){
			SPEED -= pid_angle;
		}else{
			 SPEED += pid_angle;
		}
		/* カメラ角度による減速 */
		if(servo_Gap > 0){
			SPEED -= servo_Gap/20;
		}else{
			SPEED += servo_Gap/20;
		}
		
		/* 姿勢制御 */
		ANGLE = 0;
		if(Center < 10 && Center > -10){
			ANGLE = pid_angle + servo_angle;
		}else{
			ANGLE = pid_angle;
		}

		run(SPEED,ANGLE);
	break;
/* Sprint Left */
	case 300:
		if( pushsw_get() ) {
			pattern = 310;
			handle(0);
		}
        if( cnt1 < 50 ) {               /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }	
	break;
	case 310:
		run(100,0);
		if(Center < -15 && Wide > 10){
			pattern = 320;
			set_PID(CurvePID);
		}
	break;
	case 320:
		run(50,50);
		if(Center > 0 && Wide > 10){
			pattern = 330;
			tripmeter_ini();
		}		
	break;
	case 330:
		run(75,pid_angle);
		if(tripmeter() > 300){
			set_Speed(MAX_SPEED);
			set_PID(StrightPID);
			pattern = 500;
		}
	break;

/* Sprint right */
	case 400:
		if( pushsw_get() ) {
			pattern = 410;
			handle(0);
		}
        if( cnt1 < 50 ) {               /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
	break;
	case 410:
		run(100,0);
		if(Center > 15 && Wide > 10){
			pattern = 420;
			set_PID(CurvePID);
		}		
	break;
	case 420:
		run(75,-50);
		if(Center < 0 && Wide > 10){
			pattern = 430;
			tripmeter_ini();
		}		
	break;
	case 430:
		run(75,pid_angle);
		if(tripmeter() > 150){
			set_Speed(MAX_SPEED);
			set_PID(StrightPID);
			pattern = 500;
		}
	break;
/* Sprint */
	case 500:/* 直線を猛スピードで爆走 */
		set_PID(SprintPID);
		SPEED = 100;
		if(Center > 20){
			pattern = 510;
		}
		if(Center < -20){
			pattern = 510;
		}
		/* カメラのずれによる減速 */
		if(pid_angle > 0){
			SPEED -= pid_angle*3;
		}else{
			 SPEED += pid_angle*3;
		}
		run(SPEED,pid_angle);
		
		if(tripmeter() > data_buff[DF_DISTANCE]*100){
			tripmeter_ini();
			pattern = 550;
		}
	break;
	case 510:
		set_PID(CurvePID);
		SPEED = 60;
		if(Center < 10 && White > 5){
			pattern = 500;
		}
		if(Center > -10 && White > 5){
			pattern = 500;
		}
		run(SPEED,pid_angle);
	break;
	case 550:/* ゴール手前で減速 */
		if(tripmeter() < 100) run(90,pid_angle);
		else if(tripmeter() < 200) run(80,pid_angle);
		else if(tripmeter() < 300) run(70,pid_angle);
//		else if(tripmeter() < 400) run(60,pid_angle);
//		else if(tripmeter() < 500) run(50,pid_angle);
		else run(60,pid_angle);
		if(White > 40){
			pattern = 560;
			tripmeter_ini();
			cnt1 = 0;
		}
		break;
	case 560:/* ゴールエリア内で停止 */
		if(tripmeter() < 300 || cnt1 > 3000){
			run(30,pid_angle);
		}else{
			run(0,pid_angle);
			if(cnt1 > 2000){
				pattern = 600;
			}
		}
		break;	
	case 600:
		handle(30);
		timer(1000);
		handle(-30);
		timer(1000);
		handle(0);
		timer(1000);
		pattern = 610;
	break;
	case 610:
		Light_OFF;
		run(0,0);
	break;
    case 101:
        /* microSDの停止処理 */
        /* 脱輪した際の自動停止処理後は、必ずこの処理を行ってください */
		handle( 0 );
        motor( 0, 0 );
        msdFlag = 0;
        pattern = 102;
 	break;
    case 102:
        /* 最後のデータが書き込まれるまで待つ*/
		handle( 0 );
        if( microSDProcessEnd() == 0 ) {
            pattern = 103;
        }
	break;
    case 103:/* 書き込み終了 */
		handle( 0 );
		pattern = 900;
	break;
	case 900: /* メモリの読み出し */
		handle( 0 );
        motor( 0, 0 );
		if( pushsw_get() ) {
			led_out( 0x3 );
			memPrint();
			led_out( 0x0 );
			pattern = 501;
		}
        if( cnt1 < 50 ) {               /* LED点滅処理                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
	break;
	case 1000:
		/* モニタ画面 */
		printf("Machine parameter\n");
		printf("   Stop Timer = %d s\n",data_buff[DF_STOP]);
		printf("   Max Speed = %d\n",data_buff[DF_PWM]);
		printf("   servo_center = %d\n",servo_center);
		printf("   servo   KSp %d.%d  KSi %d.%d  KSd  %d.%d \n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10,data_buff[DF_KSI]/10,data_buff[DF_KSI]%10,data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
		printf("   machine KMp %d.%d  KMi %d.%d  KMd  %d.%d \n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10,data_buff[DF_KMI]/10,data_buff[DF_KMI]%10,data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
		printf("\n");

		printf("Camera parameter \n");
		printf("   camera Stright Kp %d.%d  Ki %d.%d  Kd  %d.%d \n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10,data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10,data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
		printf("   camera Curve   Kp %d.%d  Ki %d.%d  Kd  %d.%d \n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10,data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10,data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
		printf("   Camera  LineStart %d   LineStop %d\n",data_buff[DF_LineStart],data_buff[DF_LineStop]);
		printf("\n");

		printf("Trace parameter\n");	
//		printf("   Speed  = %3d\n",data_buff[DF_start_motor]);
		printf("\n");

		printf("Sprint parameter\n");	
		printf("   Sprint Speed = %d\n",data_buff[DF_PWM_S]);
		printf("   Distance  = %2d00 mm\n",data_buff[DF_DISTANCE]);
		printf("   Sprint Stright Kp %d.%d%d  Ki %d.%d  Kd  %d.%d \n",data_buff[DF_KP_SP]/100,(data_buff[DF_KP_SP]/10)%10,(data_buff[DF_KP_SP]%100)%10,data_buff[DF_KI_SP]/10,data_buff[DF_KI_SP]%10,data_buff[DF_KD_SP]/10,data_buff[DF_KD_SP]%10);
		printf("\n");
		
		Light_ON;
		pattern = 0;
		cnt1 = 0;
	break;
	case 2000:
//		get_ad();
		printf("lEncoderTotal = %ld\n",lEncoderTotal);
		timer(1500);
		break;
	case 3000:
		if(cnt1 > 2000){
//			printf("Center = %d  nowServo = %d\n",Center,nowServo);

			bi_view();
			cnt1 = 0;
		}
	break;
	case 4000:
			printf("lEncoderTotal = %ld odometer = %ld\n",lEncoderTotal,odometer());
	break;
	case 5000:/* シリアル通信でパラメータを設定する */
		sciProcess();
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