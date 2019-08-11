/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFR�̒�`�t�@�C��    */
#include "printf_lib.h"                 /* printf�g�p���C�u����         */
#include "microsd_lib.h"                /* microSD���䃉�C�u����        */
#include "lcd_lib.h"                    /* LCD�\���p�ǉ�                */
#include "switch_lib.h"                 /* �X�C�b�`�ǉ�                 */
#include "data_flash_lib.h"             /* �f�[�^�t���b�V�����C�u����   */
#include "drive.h"
#include "trace.h"
#include "camera.h"

/* public�v���g�^�C�v�錾                     */
int		nowServo;
int		pre_center;
int		servo_angle;					/* �T�[�{�ɂ��v�Z */
int 		servo_Gap;						/* �T�[�{�̍� */

/************************************************************************/
/* �T�[�{�n���h������                                                   */
/* �����@ �T�[�{����p�x�F-40�`40                                       */
/*        40�ō���-40�x�A0�ł܂������A40�ŉE��40�x��]                  */
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
    /* �T�[�{�����E�t�ɓ����ꍇ�́A�u-�v���u+�v�ɑւ��Ă������� */
    trdgrd1 = nowServo;	
}

/************************************************************************/
/* �T�[�{�p�x�ɂ��g���[�X				                                                    */
/* �����@ �Ȃ�                                                          */
/* �߂�l �n���h���p			                                                */
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