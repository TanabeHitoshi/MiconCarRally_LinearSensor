#ifndef	DRIVE_H
#define	DRIVE_H

extern signed char     data_buff[ DF_PARA_SIZE ];
/* ���݂̏�ԕۑ��p */
extern signed int	handleBuff;             /* ���݂̃n���h���p�x�L�^       */
extern signed int   leftMotorBuff;          /* ���݂̍����[�^PWM�l�L�^      */
extern signed int   rightMotorBuff;         /* ���݂̉E���[�^PWM�l�L�^      */
extern int			angle_buff;             /* ���݃n���h���p�x�ێ��p   */
extern int          servo_center;           /* �T�[�{�Z���^�l               */

/* �v���g�^�C�v�錾                     */
void motor( int accele_l, int accele_r );
void handle( int angle );

#endif