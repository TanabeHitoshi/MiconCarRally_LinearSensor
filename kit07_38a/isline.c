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
#include "isCheck.h"

int ImageData[16];		/* ���Z���T�[�̒l */
int pre_ImageData[16];	/* �ЂƂO�̃Z���T�[�l */
int val_ImageData[16];	/* �Z���^�[�̒l */
int ad_white[16] = {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024};		/* ���C���� */
int ad_black[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};		/* �� */
float sval[16];				/* ���K���Z���T�l */
int bi_sensor[16];			/* �Q�l�� */
int sensor8,sensor16;
int	White;					/* ���F�̐� */
float pos,pre_pos;			/* ���S�ʒu����̂��� */

/************************************************************************/
/* A/D�l�ǂݍ���(AN7)                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l A/D�l 0�`1023                                                 */
/************************************************************************/
void get_ad( void )
{
    /* A/D�R���o�[�^�̐ݒ� */
	/* AN7 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x07;                     /* ���͒[�qAN7(P0_0)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[7] = ad7;

	/* AN6 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x06;                     /* ���͒[�qAN7(P0_1)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[6] = ad6;
	
		/* AN5 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x05;                     /* ���͒[�qAN7(P0_2)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[5] = ad5;
	
	
		/* AN4 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x04;                     /* ���͒[�qAN7(P0_3)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[4] = ad4;
	
	
		/* AN3 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x03;                     /* ���͒[�qAN7(P0_4)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[3] = ad3;
	
	
		/* AN2 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x02;                     /* ���͒[�qAN7(P0_5)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[2] = ad2;
	
		/* AN1 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x01;                     /* ���͒[�qAN7(P0_6)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[1] = ad1;
	
	
		/* AN0 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x00;                     /* ���͒[�qAN7(P0_7)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[0] = ad0;
	
	
		/* AN19 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x87;                     /* ���͒[�qAN19(P7_7)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[8] = ad7;
	
	
			/* AN18 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x86;                     /* ���͒[�qAN18(P7_6)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[9] = ad6;
	
			/* AN17 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x85;                     /* ���͒[�qAN17(P7_5)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[10] = ad5;
	
			/* AN16 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x84;                     /* ���͒[�qAN16(P7_4)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[11] = ad4;
	
			/* AN15 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x83;                     /* ���͒[�qAN15(P7_3)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[12] = ad3;
	
			/* AN14 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x82;                     /* ���͒[�qAN14(P7_2)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[13] = ad2;
	
			/* AN13 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x81;                     /* ���͒[�qAN14(P7_1)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[14] = ad1;
	
			/* AN12 */
    admod   = 0x03;                     /* �P�����[�h�ɐݒ�             */
    adinsel = 0x80;                     /* ���͒[�qAN13(P7_0)��I��      */
    adcon1  = 0x30;                     /* A/D����\                  */
    asm(" nop ");                       /* ��AD��1�T�C�N���E�G�C�g�����*/
    adcon0  = 0x01;                     /* A/D�ϊ��X�^�[�g              */

    while( adcon0 & 0x01 );             /* A/D�ϊ��I���҂�              */

    ImageData[15] = ad0;
	
}
/************************************************************************/
/* ���[�p�X�t�B���^�[				                                     */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
void LPF(void)
{
	int i;
	for(i = 0; i < 16; i++) {
		val_ImageData[i] = (float)pre_ImageData[i] * 0.3 + (float)ImageData[i] *0.7;
		pre_ImageData[i] = val_ImageData[i];
	}
}
/************************************************************************/
/* ���K�������Z���T�l�̌v�Z(�W�{��)                                     */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
void Normanaization(void)
{
	int i;
	sensor8 = 0;
	get_ad();
	for(i = 0; i < 16; i++) {
		sval[i] = ((float)ImageData[i] - ad_black[i]) / (ad_white[i] - ad_black[i]);
	}
	/* 8bit */
	/*
	if(sval[1] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[3] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[5] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[7] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[8] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[10] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[12] > white)sensor8++;
		sensor8 = sensor8 << 1;
	if(sval[14] > white) sensor8++;
	*/
}
/************************************************************************/
/* �Q�l��                                                               */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
void binarization(void)
{
	int i;
	sensor16 = 0;
	White = 0;
	
	for(i = 0; i < 16; i++){
		sensor16 = sensor16 << 1;
//		if(sval[i] > white){
		if(ImageData[i] < ((ad_white[i]+ad_black[i])/2) ){
			bi_sensor[i] = 1;
			sensor16++;
			White++;
		}else{
			bi_sensor[i] = 0;
		}
	}

}
/************************************************************************/
/* �J�������d�S�ʒu�̌f�o                                             */
/* �����@ �Ȃ�                                                          */
/* �߂�l �d�S�l                                                        */
/************************************************************************/
void potition( void )
{
	static float line_pos[2] = {0,0};
	float  sum;
	int i,k;
	
    // pid line fllowing process
    line_pos[0] = line_pos[1]; // previous line position value
    sum = 0.0; line_pos[1] = 0.0;
    // calcuate line position    
	k = -6;                
    for(i = 2; i < 14; i++) {
      sum += sval[i];
      line_pos[1] += (sval[i] * k);
	  k++;
	  if(k == 0) k=1;
//	  printf("sval[%d] = ",i);float_printf(sval[i],3);printf("     ");	  
//	  printf("line_pos[1] = ");float_printf(line_pos[1],3);printf("\n");	  
    }
    
	line_pos[1] = line_pos[1] / sum; // line position
	
	pos = line_pos[1];
	pre_pos = line_pos[0];
}
/************************************************************************/
/* PID				                                                    */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
int PID(void)
{
	static float i_pos = 0.0;
	i_pos +=  pos - pre_pos;
	if(pos < 25 || pos > -25)
		return pos * (float)data_buff[DF_KP]/10.0 + i_pos * (float)data_buff[DF_KI]/10.0 + (pos - pre_pos) * (float)data_buff[DF_KD]/10.0;
	else
		return pos * (float)data_buff[DF_KP]*3.0/10.0 + i_pos * (float)data_buff[DF_KI]/10.0 + (pos - pre_pos) * (float)data_buff[DF_KD]/10.0;
	
}
/************************************************************************/
/* �L�����u���[�V����                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
void AutoCalibration(void)
{
	int i;
	
	get_ad();

	for(i = 0; i < 16; i++) {
		if( ad_black[i] < ImageData[i])	 ad_black[i] = ImageData[i];
		if( ImageData[i] < ad_white[i])	 ad_white[i] = ImageData[i];
	}
}

/************************************************************************/
/* �S�������o		                                                    */
/* �߂�l �Z���T�l ON(�S��):1 OFF(�Ȃ�):0  		                        */
/************************************************************************/
unsigned int allBlack(void)
{
	int i;
	int b = 0;
	for(i = 0; i < 16; i++){
		if(sval[i] < white)b++;
	}
//	printf("b = %d\n",b);
	if(b == 16){
		return 1;
	}else{
		return 0;
	}
}
/************************************************************************/
/* �X�^�[�g�o�[���o�Z���T�ǂݍ���                                       */
/* �߂�l �Z���T�l ON(�o�[����):1 OFF(�Ȃ�):0                           */
/************************************************************************/
unsigned int startbar_get( void )
{
	int i;
	int n = 0;

	for(i = 0; i < 16; i++){
		if(sval[i] > 0.8) n++;
	}
//	printf("n = %d\n",n);
	if( n == 16){
		return  1;
	}else{
		return 0;
	}
}
/************************************************************************/
/* ������\������B		                                                */
/* �����@ val->�\����������@n->����                                    */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
void float_printf(float val,int n)
{
	int i,i_val;
	if(val < 0){
		printf("-");
		val *= -1.0;
	}
	i_val = (int)val;
	printf("%d.",i_val);
	for(i=0;i<n;i++){
		val -= (float)i_val;
		val *= 10.0;
		i_val = (int)val;
		printf("%d",i_val);
	}
}
