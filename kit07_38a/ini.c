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
#include "camera.h"
#include "drive.h"
#include "ini.h"
#include "trace.h"
#include "isEncoder.h"

volatile unsigned long   cnt0;					/* timer�֐��p                  */
volatile unsigned long   cnt1;					/* main���Ŏg�p                 */
volatile unsigned long   cnt_lcd;				/* LCD�����Ŏg�p                */
volatile unsigned long   stop_timer;			/* ���s�^�C�}�[					*/
volatile unsigned int   cnt_Curve;				/* �J�[�u�Ŏg�p  */
volatile unsigned int   cnt_Stright;			/* ���i�Ŏg�p  */

int             pattern;                /* �p�^�[���ԍ�                 */
int				Srevo_state;			/* �T�[�{�̐��䂠�� -> 1�@�Ȃ� -> 0	*/
/* microSD�֘A�ϐ� */
signed char     msdBuff[ 512+32 ];      /* �ꎞ�ۑ��o�b�t�@(32�͗\��)   */
int             msdFlag;                /* 1:�f�[�^�L�^ 0:�L�^���Ȃ�    */
int             msdTimer;               /* �擾�Ԋu�v�Z�p               */
int             msdError;               /* �G���[�ԍ��L�^               */
/* �����L�^ */
struct MEM{
	unsigned int pattern;
	unsigned int distance;
	unsigned int center;
	int PID;
	char white;
	char speed;
};
int				memCnt = 0;					/* �����L�^�̔ԍ� */
struct MEM		mem[500];					/* �����L�^�̔z�� */

/************************************************************************/
/* R8C/38A �X�y�V�����t�@���N�V�������W�X�^(SFR)�̏�����                */
/************************************************************************/
void init( void )
{
    int i;

    /* �N���b�N��XIN�N���b�N(20MHz)�ɕύX */
    prc0  = 1;                          /* �v���e�N�g����               */
    cm13  = 1;                          /* P4_6,P4_7��XIN-XOUT�[�q�ɂ���*/
    cm05  = 0;                          /* XIN�N���b�N���U              */
    for(i=0; i<50; i++ );               /* ���肷��܂ŏ����҂�(��10ms) */
    ocd2  = 0;                          /* �V�X�e���N���b�N��XIN�ɂ���  */
    prc0  = 0;                          /* �v���e�N�gON                 */

    /* �|�[�g�̓��o�͐ݒ� */
    prc2 = 1;                           /* PD0�̃v���e�N�g����          */
//	pd0 = 0xfe;                         /* 0:AO�̱�۸ޓd��  1:SI 2:CLK  */
    pd0 = 0x0f;                         /* SI -> P0_1  CLK -> p0_2  AN0 -> P0_7  Light -> P0_3*/
    pd1 = 0xd0;                         /* 5:RXD0 4:TXD0 3-0:DIP SW     */
    p2  = 0xc0;
    pd2 = 0xfe;                         /* 7-0:���[�^�h���C�u���Ver.4  */
    pd3 = 0xfe;                         /* 0:���[�^���G���R�[�_         */
    p4  = 0x20;                         /* P4_5��LED:�����͓_��         */
    pd4 = 0xb8;                         /* 7:XOUT 6:XIN 5:LED 2:VREF    */
    pd5 = 0x7f;                         /* 7-0:LCD/microSD���          */
    pd6 = 0xef;                         /* 4-0:LCD/microSD���          */
    pd7 = 0x0f;                         /* A/D�ϊ�                      */
    pd8 = 0xff;                         /*                              */
    pd9 = 0x3f;                         /*                              */
    pur0 = 0x04;                        /* P1_3�`P1_0�̃v���A�b�vON     */

    /* �^�C�}RB�̐ݒ� */
    /* ���荞�ݎ��� = 1 / 20[MHz]   * (TRBPRE+1) * (TRBPR+1)
                    = 1 / (20*10^6) * 200        * 100
                    = 0.001[s] = 1[ms]
    */
    trbmr  = 0x00;                      /* ���샂�[�h�A������ݒ�       */
    trbpre = 200-1;                     /* �v���X�P�[�����W�X�^         */
    trbpr  = 100-1;                     /* �v���C�}�����W�X�^           */
    trbic  = 0x07;                      /* ���荞�ݗD�惌�x���ݒ�       */
    trbcr  = 0x01;                      /* �J�E���g�J�n                 */

    /* �^�C�}RD ���Z�b�g����PWM���[�h�̐ݒ�*/
    /* PWM���� = 1 / 20[MHz]   * �J�E���g�\�[�X * (TRDGRA0+1)
               = 1 / (20*10^6) * 8              * 40000
               = 0.016[s] = 16[ms]
    */
    trdpsr0 = 0x08;                     /* TRDIOB0,C0,D0�[�q�ݒ�        */
    trdpsr1 = 0x05;                     /* TRDIOA1,B1,C1,D1�[�q�ݒ�     */
    trdmr   = 0xf0;                     /* �o�b�t�@���W�X�^�ݒ�         */
    trdfcr  = 0x01;                     /* ���Z�b�g����PWM���[�h�ɐݒ�  */
    trdcr0  = 0x23;                     /* �\�[�X�J�E���g�̑I��:f8      */
    trdgra0 = trdgrc0 = PWM_CYCLE;      /* ����                         */
    trdgrb0 = trdgrd0 = 0;              /* P2_2�[�q��ON���ݒ�           */
    trdgra1 = trdgrc1 = 0;              /* P2_4�[�q��ON���ݒ�           */
    trdgrb1 = trdgrd1 = SERVO_CENTER;   /* P2_5�[�q��ON���ݒ�           */
    trdoer1 = 0xcd;                     /* �o�͒[�q�̑I��               */
    trdstr  = 0x0d;                     /* TRD0�J�E���g�J�n             */

    /* �^�C�}RG �^�C�}���[�h(���G�b�W�ŃJ�E���g)�̐ݒ� */
    timsr = 0x40;                       /* TRGCLKA�[�q P3_0�Ɋ��蓖�Ă� */
    trgcr = 0x15;                       /* TRGCLKA�[�q�̗��G�b�W�ŃJ�E���g*/
    trgmr = 0x80;                       /* TRG�̃J�E���g�J�n            */	
}

/************************************************************************/
/* �^�C�}RB ���荞�ݏ���                                                */
/************************************************************************/
#pragma interrupt intTRB(vect=24)
void intTRB( void )
{
    static signed char *p = msdBuff;    /* �L�^�ް��̏������ݐ�������߲��*/
    static int  line_no;                /* �s�ԍ�                       */
    int ret;
    unsigned int i;

    cnt0++;
    cnt1++;
    cnt_lcd++;
	cnt_Curve++;
	if(pattern > 10)stop_timer++;
					
	// �G���R�[�_���� + ���s�f�[�^����
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
    /* �g���X�C�b�`�p�֐�(1ms���ƂɎ��s)    */
    switchProcess();

    /* LCD�\�������p�֐�(1ms���ƂɎ��s)     */
    lcdShowProcess();

    /* microSD�Ԍ��������ݏ���(1ms���ƂɎ��s)   */
    microSDProcess();

    /* microSD�L�^���� */
    if( msdFlag == 1 ) {
        /* �L�^�Ԋu�̃`�F�b�N */
        msdTimer++;
        if( msdTimer == 9 ) {
            /* RAM�ɋL�^ ��������(�������۸��т̎�����150us) */
            /* �󂫂̓X�y�[�X�Ŗ��߂Ă�������                */
            // 0 �p�^�[��
            convertDecimalToStr( pattern, 3, p );
            p += 3;*p++ = ',';
            // 4 �Z���T
            convertDecimalToStr( Center, 4, p );
            p += 4;*p++ = ',';
            // 9 ��
            convertDecimalToStr( White, 2, p );
            p += 2;*p++ = ',';
            // 12 �ʒu
            convertDecimalToStr( (int)(0 * 10.0), 4, p );
            p += 4;*p++ = ',';
            // 17 ���x
            convertDecimalToStr( iEncoder, 2, p );
            p += 2;*p++ = ',';
            // 20 �����x
            convertDecimalToStr( Acceleration, 3, p );
            p += 3;*p++ = ',';
            // 24 �n���h��
            convertDecimalToStr( handleBuff, 3, p );
            p += 3;*p++ = ',';
            // 28 �����[�^PWM�l
            convertDecimalToStr( leftMotorBuff, 4, p );
            p += 4;*p++ = ',';
            // 33 �E���[�^PWM�l
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
            /* RAM�ɋL�^ �����܂� */
        } else if( msdTimer >= 10 ) {
            /* microSD�ɋL�^ */
            msdTimer = 0;
            if( p >= (msdBuff+512) ) {
                /* 512�ɂȂ�����AmicroSD�ɋL�^���� */
                p = msdBuff;
                ret = setMicroSDdata( msdBuff );
                if( ret == 1 ) {
                    /* �e�ʂ̏���Ȃǂŏ������݂��ł��Ȃ� */
                    msdFlag = 0;        /* �L�^�����I��                 */
                }
            }
        }
    }
}
/************************************************************************/
/* �����������̕\��			                                                */
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
/* �^�C�}�{��                                                           */
/* �����@ �^�C�}�l 1=1ms                                                */
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