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
#include "isline.h"
#include "drive.h"

/* ���݂̏�ԕۑ��p */
int             handleBuff;             /* ���݂̃n���h���p�x�L�^       */
int             leftMotorBuff;          /* ���݂̍����[�^PWM�l�L�^      */
int             rightMotorBuff;         /* ���݂̉E���[�^PWM�l�L�^      */
int             angle_buff;             /* ���݃n���h���p�x�ێ��p   */

int             servo_center;           /* �T�[�{�Z���^�l               */


/************************************************************************/
/* ���[�^���x����                                                       */
/* �����@ �����[�^:-100�`100�A�E���[�^:-100�`100                        */
/*        0�Œ�~�A100�Ő��]100%�A-100�ŋt�]100%                        */
/* �߂�l �Ȃ�                                                          */
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

    /* �����[�^���� */
    if( accele_l >= 0 ) {
        p2 &= 0xfd;
        trdgrd0 = (long)( PWM_CYCLE - 1 ) * accele_l / 100;
    } else {
        p2 |= 0x02;
        trdgrd0 = (long)( PWM_CYCLE - 1 ) * ( -accele_l ) / 100;
    }
	
	rightMotorBuff = accele_r;

    /* �E���[�^���� */
    if( accele_r >= 0 ) {
        p2 &= 0xf7;
        trdgrc1 = (long)( PWM_CYCLE - 1 ) * accele_r / 100;
    } else {
        p2 |= 0x08;
        trdgrc1 = (long)( PWM_CYCLE - 1 ) * ( -accele_r ) / 100;
    }
}

/************************************************************************/
/* �T�[�{�n���h������                                                   */
/* �����@ �T�[�{����p�x�F-40�`40                                       */
/*        40�ō���-40�x�A0�ł܂������A40�ŉE��40�x��]                  */
/************************************************************************/
void handle( int angle )
{
	if(angle >40)angle = 40;
	if(angle <-40)angle = -40;
	handleBuff = angle;
    angle_buff = angle;                 /* ���݂̊p�x�ۑ�               */

    /* �T�[�{�����E�t�ɓ����ꍇ�́A�u-�v���u+�v�ɑւ��Ă������� */
    trdgrd1 = servo_center - angle * HANDLE_STEP;
}
