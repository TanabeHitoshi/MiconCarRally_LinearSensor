#ifndef	ISENCODER_H
#define	ISENCODER_H

/* �G���R�[�_�֘A */
extern	int				iTimer10;			   	/* 10ms�J�E���g�p		  	*/
extern	int   			iEncoder;			   	/* 10ms���̍ŐV�l		   	*/
extern	int				pre_iEncoder;			/* �O�̑��x					*/
extern	int				Acceleration;			/* �����x					*/
extern	unsigned long   lEncoderTotal;		  	/* �ώZ�l�ۑ��p				*/
extern	unsigned int	uEncoderBuff;		   	/* �v�Z�p�@���荞�ݓ��Ŏg�p */
extern	unsigned long	tripmeter_set;			/* �g���b�v���[�^�[�̃Z�b�g	*/

void odometer_ini(void);
long odometer(void);
void tripmeter_ini(void);
long tripmeter(void);
int speedmeter(void);


#endif