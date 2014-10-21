#ifndef	DRIVE_H
#define	DRIVE_H

extern signed char     data_buff[ DF_PARA_SIZE ];
/* ���݂̏�ԕۑ��p */
extern signed int	handleBuff;             /* ���݂̃n���h���p�x�L�^       */
extern signed int   leftMotorBuff;          /* ���݂̍����[�^PWM�l�L�^      */
extern signed int   rightMotorBuff;         /* ���݂̉E���[�^PWM�l�L�^      */
extern int			angle_buff;             /* ���݃n���h���p�x�ێ��p   */
extern int          servo_center;           /* �T�[�{�Z���^�l               */
/* �G���R�[�_�֘A */
extern int				iTimer10;			   	/* 10ms�J�E���g�p		  	*/
extern int				iEncoder;			   	/* 10ms���̍ŐV�l		   	*/
extern unsigned long	lEncoderTotal;		  	/* �ώZ�l�ۑ��p				*/
extern unsigned int		uEncoderBuff;		   	/* �v�Z�p�@���荞�ݓ��Ŏg�p */
extern unsigned long	lEncoderCrank;		  	/* �N���X���C�����o���̐ώZ�l 	*/
extern unsigned long 	lEncoderCrank2;			/* �N���X���C�����o���̐ώZ�l2 	*/
extern unsigned long	lEncoderHarf;		  	/* �n�[�t���C�����o���̐ώZ�l 	*/

/* �v���g�^�C�v�錾                     */
void motor( int accele_l, int accele_r );
void handle( int angle );

#endif