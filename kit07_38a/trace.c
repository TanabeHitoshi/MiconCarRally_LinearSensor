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
#include "drive.h"
#include "trace.h"
#include "camera.h"

/* publicプロトタイプ宣言                     */
int		nowServo;
int		pre_center;
int		servo_angle;					/* サーボによる計算 */
int 		servo_Gap;						/* サーボの差 */

/************************************************************************/
/* サーボハンドル操作                                                   */
/* 引数　 サーボ操作角度：-40～40                                       */
/*        40で左へ-40度、0でまっすぐ、40で右へ40度回転                  */
/************************************************************************/
void servo_Trace( void )
{
	if( Wide > 0 && Wide < 50){
		nowServo +=  Center*data_buff[DF_KSP]/10 + (pre_center - Center) * data_buff[DF_KSD]/10;

		if(nowServo > servo_center + 30 * HANDLE_STEP)nowServo = servo_center + 40 * HANDLE_STEP;
		if(nowServo < servo_center - 30 * HANDLE_STEP)nowServo = servo_center - 40 * HANDLE_STEP;
		pre_center = Center;
	}else{
//		nowServo = SERVO_CENTER;
	}
    /* サーボが左右逆に動く場合は、「-」を「+」に替えてください */
    trdgrd1 = nowServo;	
}

/************************************************************************/
/* サーボ角度によるトレース				                                                    */
/* 引数　 なし                                                          */
/* 戻り値 ハンドル角			                                                */
/************************************************************************/
int motor_Trace(void)
{
	static int pre_servoGap;
	static int iGap;
	int			h;
	
	servo_Gap = servo_center - nowServo;
	iGap = servo_Gap - pre_servoGap;
	
	h = servo_Gap * data_buff[DF_KMP]/10 + iGap * data_buff[DF_KMI]/10 + (pre_servoGap - servo_Gap) * data_buff[DF_KMD]/10;
	pre_servoGap = servo_Gap;
	
	return h>>3;
}
/************************************************************************/
/* end of file                                                          */
/************************************************************************/