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
#include "isline.h"
#include "drive.h"

/* 現在の状態保存用 */
int             handleBuff;             /* 現在のハンドル角度記録       */
int             leftMotorBuff;          /* 現在の左モータPWM値記録      */
int             rightMotorBuff;         /* 現在の右モータPWM値記録      */
int             angle_buff;             /* 現在ハンドル角度保持用   */

int             servo_center;           /* サーボセンタ値               */

const int revolution_difference[] = {   /* 角度から内輪、外輪回転差計算 */
    100, 99, 97, 96, 95,
    93, 92, 91, 89, 88,
    87, 86, 84, 83, 82,
    81, 79, 78, 77, 76,
    75, 73, 72, 71, 70,
    69, 67, 66, 65, 64,
    62, 61, 60, 59, 58,
    56, 55, 54, 52, 51,
    50, 48, 47, 46, 44,
    43 };
/************************************************************************/
/* 走行関数                                              		        */
/* 引数　 モータ:-100～100                        						*/
/*        0で停止、100で正転100%、-100で逆転100%                        */
/* 戻り値 なし                                                          */
/************************************************************************/
void run(int speed)
{
		if( angle_buff > 0)	motor(speed,diff(speed));
		else			motor(diff(speed),speed);
}
/************************************************************************/
/* モータ速度制御                                                       */
/* 引数　 左モータ:-100～100、右モータ:-100～100                        */
/*        0で停止、100で正転100%、-100で逆転100%                        */
/* 戻り値 なし                                                          */
/************************************************************************/
void motor( int accele_l, int accele_r )
{
	if( accele_l >= 100) accele_l = 100;
	if( accele_r >= 100) accele_r = 100;
	if( accele_l <= -100) accele_l = -100;
	if( accele_r <= -100) accele_r = -100;
		

    accele_l = accele_l * data_buff[DF_PWM] / 100;
    accele_r = accele_r * data_buff[DF_PWM] / 100;

 	leftMotorBuff = accele_l;

    /* 左モータ制御 */
    if( accele_l >= 0 ) {
        p2 &= 0xfd;
        trdgrd0 = (long)( PWM_CYCLE - 1 ) * accele_l / 100;
    } else {
        p2 |= 0x02;
        trdgrd0 = (long)( PWM_CYCLE - 1 ) * ( -accele_l ) / 100;
    }
	
	rightMotorBuff = accele_r;

    /* 右モータ制御 */
    if( accele_r >= 0 ) {
        p2 &= 0xf7;
        trdgrc1 = (long)( PWM_CYCLE - 1 ) * accele_r / 100;
    } else {
        p2 |= 0x08;
        trdgrc1 = (long)( PWM_CYCLE - 1 ) * ( -accele_r ) / 100;
    }
}

/************************************************************************/
/* サーボハンドル操作                                                   */
/* 引数　 サーボ操作角度：-40～40                                       */
/*        40で左へ-40度、0でまっすぐ、40で右へ40度回転                  */
/************************************************************************/
void handle( int angle )
{
	if(angle >40)angle = 40;
	if(angle <-40)angle = -40;
	handleBuff = angle;
    angle_buff = angle;                 /* 現在の角度保存               */

    /* サーボが左右逆に動く場合は、「-」を「+」に替えてください */
    trdgrd1 = servo_center - angle * HANDLE_STEP;
}
/************************************************************************/
/* 外輪のPWMから、内輪のPWMを割り出す　ハンドル角度は現在の値を使用     */
/* 引数　 外輪PWM                                                       */
/* 戻り値 内輪PWM                                                       */
/************************************************************************/
int diff( int pwm )
{
    int ret;

    if( pwm >= 0 ) {
        /* PWM値が正の数なら */
        if( angle_buff < 0 ) {
            angle_buff = -angle_buff;
        }
		ret = revolution_difference[angle_buff] * pwm / 100;
    } else {
        /* PWM値が負の数なら */
        ret = pwm;                      /* そのまま返す             */
    }
    return ret;
}