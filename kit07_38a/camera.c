/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFR�̒�`�t�@�C��    */
#include "printf_lib.h"                 /* printf�g�p���C�u����         */
#include "camera.h"
#include "lcd_lib.h"                    /* LCD�\���p�ǉ�                */

/* TAOS TSL1401CL */
#define	TAOS_SI_HIGH	p0_addr.bit.b1 = 1	/* Port P1_7 bit */
#define	TAOS_SI_LOW		p0_addr.bit.b1 = 0	/* Port P1_7 bit */
#define	TAOS_CLK_HIGH	p0_addr.bit.b2 = 1	/* Port P7_0 bit */
#define	TAOS_CLK_LOW	p0_addr.bit.b2 = 0	/* Port P7_0 bit */

/* Capture */
#define		White_Max			120				/* ���C�����FMAX�l�̐ݒ� */
#define		White_min	5					/* ���F�̍ŏ��l 			*/

/* public �ϐ�	*/
int		White;									/* ���F�̌�								*/
unsigned int 	Wide;							/* ���C���̕� */
int		Center;									/* ���C���̒��S */
long		center_Weight;							/* ���C���̏d�S */
unsigned long pre_cycle;				/* �P�����̎���	*/
/* private�ϐ�	*/
int		camera_Max,camera_Min,camera_Ave;		/* �J�����ǂݎ��ő�l�A�ŏ��l�A���ϒl		*/
int		real_Center;									/* ���C���̒��S */
unsigned int 	Rsensor;						/* ���C���̉E�[ */
unsigned int 	Lsensor;						/* ���C���̍��[ */
int   	EXPOSURE_timer = 1350;					/* �I������					*/
int		ImageData[130];							/* �J�����̒l								*/
int		BinarizationData[130];					/* �Q�l��									*/
int		LineStart;								/* �J�����̃X�^�[�g�ʒu		*/
int		LineStop;								/* �J�����̃X�g�b�v�ʒu		*/
unsigned int 	Rsensor01;				/* ���C���̉E�[ */
unsigned int 	Lsensor01;				/* ���C���̍��[ */
unsigned int 	Wide01;					/* ���C���̕� */
unsigned int 	Rsensor02;				/* ���C���̉E�[ */
unsigned int 	Lsensor02;				/* ���C���̍��[ */
unsigned int 	Wide02;					/* ���C���̕� */
unsigned int 	Rsensor;				/* ���C���̉E�[ */
unsigned int 	Lsensor;				/* ���C���̍��[ */
unsigned int 	Wide;					/* ���C���̕� */
int			 	Center01;				/* ���C���̏d�S */
int			 	Center02;				/* ���C���̏d�S */
int				Center_ctrl;
int 			pre_Center;				/* �O��̏d�S�l */
int				pid_angle;					/* �J�����ɂ��v�Z */
float	kp,ki,kd;

/* private�v���g�^�C�v�錾                     */
void	ImageCapture(void);						/* �C���[�W�L���v�`��						*/
void	binarization(void);						/* �Q�l��									*/
void	WhiteLineWide(void);					/* �����̕��𑪒�							*/
void	WhiteLineCenter(void);					/* �����̒��S�𑪒�							*/
void	WhiteLineWeight(void);					/* �����̏d�S�𑪒�							*/
int	PID(void);									/* �o�h�c�l�̌v�Z */
int	get_ad_camera( void );						/*	A/D�l�ǂݍ���(AN7)							*/

/************************************************************************/
/* �J��������̃f�[�^                                                   */
/* �����@  �Ȃ�                                      */
/* �߂�l �Ȃ�                                                          */
/************************************************************************/
void camera_Caputure(void){
	int i;
	
	ImageCapture();						/* �C���[�W�L���v�`��						*/
	for(i=0;i<EXPOSURE_timer;i++) asm("nop");
//	expose();
	ImageCapture();						/* �C���[�W�L���v�`��						*/
	binarization();						/* �Q�l��									*/
	WhiteLineWide();					/* �����̕��𑪒�							*/
	WhiteLineCenter();					/* �����̒��S�𑪒�							*/
	WhiteLineWeight();					/* �����̏d�S�𑪒�							*/
	pid_angle =PID();					/* �o�h�c�̒l */	
}

/************************************************************************/
/* �C���[�W�L���v�`��                                                   */
/* �����@  �Ȃ�                                      */
/* �߂�l �Ȃ�                                                          */
/************************************************************************/
void ImageCapture(void){	 
	
	unsigned char i;

	if(data_buff[DF_RACE] == 0){
		LineStart = data_buff[DF_LineStart];
		LineStop = data_buff[DF_LineStop];
	}else{
		LineStart = data_buff[DF_LineStart];
		LineStop = data_buff[DF_LineStop];		
	}
	camera_Max = 0,camera_Min = 1024;
	TAOS_SI_HIGH;
	TAOS_CLK_HIGH;
	TAOS_SI_LOW;
	ImageData[0] = 0;
	TAOS_CLK_LOW;
	for(i = 0; i < LineStart; i++) {		
		TAOS_CLK_HIGH;		
		TAOS_CLK_LOW;
	}
	for(i = LineStart; i < LineStop; i++) {					 
		TAOS_CLK_HIGH;
		ImageData[i] = get_ad_camera();								// inputs data from camera (one pixel each time through loop) 
		TAOS_CLK_LOW;
		
		if(camera_Max < ImageData[i]){
			camera_Max = ImageData[i];
		}			
		if(camera_Min > ImageData[i]){
			camera_Min = ImageData[i];
		}
		
	}
	for(i = LineStop; i < 128; i++) {		
		TAOS_CLK_HIGH;		
		TAOS_CLK_LOW;
	}
	
	TAOS_CLK_HIGH;
	TAOS_CLK_LOW;
}
/************************************************************************/
/* �Q�l��                                                               */
/* ����		�Ȃ�														*/
/* �߂�l	�Ȃ�                                                        */
/************************************************************************/
void binarization(void)
{
	int i;
	/* �ō��l�ƍŒ�l����Ԃ̒l�����߂� */
	camera_Ave = (camera_Max + camera_Min) * 2/3;

	/* ���͂O�@���͂P�ɂ��� */
	White = 0;

	/* �O�ʔ� */
	if( camera_Min > 50){
		for(i=1; i < 128; i++){
			White++;
			BinarizationData[i] = 1;
		}
	/* �ʏ탂�[�h */		
	}else if( camera_Max > 50 ){
		for(i = LineStart ; i < LineStop; i++) {
			if( ImageData[i] > camera_Ave ){	
				White++;			
				BinarizationData[i] = 1;
			}else{
				BinarizationData[i] = 0;
			}
		}
	/* ������ʂ̂Ƃ� */
	}else{
		for(i=1; i < 128; i++){
			BinarizationData[i] = 0;
		}
	}

}
/************************************************************************/
/* �����̕��𑪒�                                                       */
/* ����		�Ȃ�														*/
/* �߂�l	�Ȃ�                                                        */
/************************************************************************/
void WhiteLineWide(void)
{
	int t=0,i;
		
	Lsensor01 = Lsensor02 = LineStart;
	Rsensor01 = Rsensor02 = LineStop;

	for(i = Lsensor01 ; i < LineStop; i++) {
		if(t==0){	/* �P�{�ڂ̔��� */
			if( BinarizationData[i] ){		/* ������ŏ��̔� */
				Lsensor01 = i;
				t = 1;
			}
		}else if(t==1){
			if( BinarizationData[i] ){		/* ������ŏ��̔� */
				Lsensor01 = i;
				t = 2;
			}
		}else if(t==2){	/* �P�{�ڂ̍��� */
			if( !BinarizationData[i] ){		/* ������ŏ��̍� */
				Rsensor01 = i;
				t = 3;
			}
		}else if(t==3){
			if( !BinarizationData[i] ){		/* ������ŏ��̍� */
				Rsensor01 = i;
				t = 4;
			}
		}else if(t==4){	/* �Q�{�ڂ̔��� */
			if( BinarizationData[i] ){		/* ������ŏ��̔� */
				Lsensor02 = i;
				t = 5;
			}
		}else if(t==5){
			if( BinarizationData[i] ){		/* ������ŏ��̔� */
				Lsensor02 = i;
				t = 6;
			}
		}else if(t==6){	/* �Q�{�ڂ̍��� */
			if( !BinarizationData[i] ){		/* ������ŏ��̍� */
				Rsensor02 = i;
				t = 7;
			}
		}else if(t==7){	
			if( !BinarizationData[i] ){		/* ������ŏ��̍� */
				Rsensor02 = i;
				break;
			}
		}
	}
	if(White > White_min){
		Wide01 = Rsensor01 - Lsensor01;		/* 1�{�ڂ̔����������߂� */
		if( Lsensor02 != LineStart){
			Wide02 = Rsensor02 - Lsensor02; /* 2�{�ڂ̔����������߂� */
		}else{
			Wide02 = 0;						/* 2�{�ڔ����Ȃ�		*/
		}
	}else{
		Wide01 = Wide02 = 0;				/* ����� */
		Center = 0;
	}
}
/************************************************************************/
/* �����̒��S�𑪒�                                                       */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                 */
/************************************************************************/
void WhiteLineCenter(void)
{
	int abs_Center01,abs_Center02;		/* �Z���^�[�l�̐�Βl */

	if(Wide01 != 0){
		Center01 = (Lsensor01 + Rsensor01)/2;			/* 1�{�ڂ̔����̏d�S�����߂� */
	}
	if(Wide02 != 0){
		Center02 = (Lsensor02 + Rsensor02)/2;			/* 2�{�ڂ̔����̏d�S�����߂� */
	}
//	maker = 0;
	/* �g���[�X���C���̔��� */
	if(Wide01 != 0 && Wide02 == 0){						/* 1�{�ڂ͂���A2�{�ڂ��Ȃ��Ƃ� */
		Center = Center01 - 64 - OFFSET;
		Wide = Wide01;
		Center02 = 0;
	}else if(Wide01 != 0 && Wide02 != 0){				/* 1�{�ڂ͂���A2�{�ڂ�����Ƃ� */
		abs_Center01 = pre_Center - Center01;
		if(abs_Center01 < 0)abs_Center01 *= -1;			/* �|�̂Ƃ��́{�ɂ���@*/
		abs_Center02 = pre_Center - Center02;
		if(abs_Center02 < 0)abs_Center02 *= -1;			/* �|�̂Ƃ��́{�ɂ���@*/
		if(abs_Center01 < abs_Center02){
			/* �O��̏d�S�l��1�{�ڂ̃��C�����߂� */
			Center = Center01 - 64 - OFFSET;
			Wide = Wide01;
//			if(Wide02 > 8) maker = 1;
		}else{
			/* �O��̏d�S�l��2�{�ڂ̃��C�����߂� */
			Center = Center02 - 64 - OFFSET;
			Wide = Wide02;
//			if(Wide01 > 8) maker = 2;
		}
	}else{

		Wide = 0;
		Center = 999;
	}
	/* �����������Ƃ��̓Z���^�[�l���v�Z���Ȃ� */
	if(Wide > 50){
		Center = 999;
	}
	pre_Center = Center + 64;

}
/************************************************************************/
/* �����̏d�S�𑪒�                                                       */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                 */
/************************************************************************/
void WhiteLineWeight(void)
{
	int	i;
	int		value;
	int		weight;
	long	sum = 0,sum2 = 0;
	
	if((Wide > 5) && (Wide < 20)){
		center_Weight = 0;
		weight = Center - 10;
		for(i = real_Center-10 ; i <= real_Center+10;i++,weight++){
			value = ImageData[i];
			sum += value;
			sum2 += value*weight;
		}
		center_Weight = sum2*7 / sum;
		//���ϒl���i�K���l�~�x���j�̍��v/�x���̍��v
	}else{
		center_Weight = -999;
	}
//	saveData[4] = (char)center_Weight;				/* �d�S�̃��O		*/
}
/************************************************************************/
/* PID				                                                    */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                */
/************************************************************************/
int PID(void)
{
	static float	iCenter = 0.0;
	static float	preCenter = 0.0;
	static float	h;
	if(Center != 999){
		iCenter +=  (float)Center - preCenter;
		h = (float)Center * kp + iCenter *ki + ((float)Center - preCenter) * kd;
		preCenter = (float)Center;
	}
	return -(int)h;
}
/************************************************************************/
/* PID�ݒ�				                                                    */
/* �����@ �Ȃ�                                                          */
/* �߂�l �n���h���p			                                                */
/************************************************************************/
void set_PID(int s)
{
	switch(s){
		case StrightPID: /* �X�g���[�g */
			kp = (float)data_buff[DF_KP_S]/10.0;
			ki = (float)data_buff[DF_KI_S]/10.0;
			kd = (float)data_buff[DF_KD_S]/10.0;
		break;
		case CurvePID: /* �J�[�u */
			kp = (float)data_buff[DF_KP_C]/10.0;
			ki = (float)data_buff[DF_KI_C]/10.0;
			kd = (float)data_buff[DF_KD_C]/10.0;
		break;
		case SprintPID: /* �J�[�u */
			kp = (float)data_buff[DF_KP_SP]/100.0;
			ki = (float)data_buff[DF_KI_SP]/10.0;
			kd = (float)data_buff[DF_KD_SP]/10.0;
		break;
		case Sprint_2PID: /* �J�[�u */
			kp = (float)data_buff[DF_KP_SP2]/10.0;
			ki = (float)data_buff[DF_KI_SP2]/10.0;
			kd = (float)data_buff[DF_KD_SP2]/10.0;
		break;	}

}
/************************************************************************/
/* �I�����Ԓ���                                                         */
/* �����@ �o�ߎ���                                                          */
/* �߂�l �Ȃ��@�@�@�@�@                                                */
/************************************************************************/
void expose( void )
{
	int i;
	if( Wide != 0 && !(White >= 90)){//���łȂ����ł��Ȃ�
		if(camera_Max < White_Max){
			EXPOSURE_timer += (White_Max - camera_Max)*10;
		}else{
			EXPOSURE_timer -= (camera_Max - White_Max)*10;
		}
	}
/*
	if( EXPOSURE_timer > 500){
		EXPOSURE_timer = 250;
	}
*/
	if( EXPOSURE_timer < 0 ){
		EXPOSURE_timer = 0;
	}
//	printf("camera_Max %d White_Max %d  EXPOSURE_timer%d\n",camera_Max , White_Max,EXPOSURE_timer);
	
	for(i=0;i<EXPOSURE_timer;i++) asm("nop");
}
/************************************************************************/
/* RAW�f�[�^�̕\��	                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                 */
/************************************************************************/
void raw_view(void)
{
	int i;
	for(i = LineStart;i < LineStop; i++){
		printf("%d ",ImageData[i]);
	}
	printf("\n\n");

}
/************************************************************************/
/* �Q�l���f�[�^�̕\��	                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�			                                                 */
/************************************************************************/
void bi_view(void)
{
	int i;
	for(i = LineStart;i < LineStop; i++){
		if(BinarizationData[i] == 1){
			printf("*");
		}else{
			printf(" ");
		}
	}
	printf("time=%d Max=%d Min=%d Ave=%d ",EXPOSURE_timer,camera_Max,camera_Min,camera_Ave);
	printf("Wide=%d Center=%d Weight=%ld\n",Wide,Center,center_Weight);
//	printf("time=%ld EXPO=%d Max=%d Min=%d Ave=%d Wide=%d Center=%d Weight=%ld iServoPID=%d\n",pre_cycle/2,EXPOSURE_timer,camera_Max,camera_Min,camera_Ave,Wide,Center,center_Weight,iServoPID);
//	printf("Center=%d Weight=%ld iServoPID=%d\n",Center,center_Weight,iServoPID);
}

/************************************************************************/
/* A/D�l�ǂݍ���(AN4)                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l A/D�l 0�`1023                                                 */
/************************************************************************/
int get_ad_camera( void )
{
    /* A/D�R���o�[�^�̐ݒ� */
	admod   = 0x03;									/* �P�����[�h�ɐݒ�						*/
    adinsel = 0x00;									/* ���͒[�qAN0(P0_7)��I��			*/
    adcon1  = 0x30;									/* A/D����\								*/
    asm(" nop ");									/* ��AD��1�T�C�N���E�G�C�g�����	*/
    adcon0 = 0x01;									/* A/D�ϊ��X�^�[�g							*/
    while( adcon0 & 0x01 ) ;						/* A/D�ϊ��I���҂�							*/
    return ad0;
}