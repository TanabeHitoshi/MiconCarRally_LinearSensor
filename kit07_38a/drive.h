#ifndef	DRIVE_H
#define	DRIVE_H

#define SERVO_CENTER    3863            /* �T�[�{�̃Z���^�l             */
#define MAX_SPEED		data_buff[DF_PWM]
#define Sprint_MAX_SPEED	data_buff[DF_PWM_SP]
#define Sprint_SPEED	data_buff[DF_PWM_S]


extern signed char     data_buff[ DF_PARA_SIZE ];
/* ���݂̏�ԕۑ��p */
extern signed int	handleBuff;             /* ���݂̃n���h���p�x�L�^       */
extern signed int   leftMotorBuff;          /* ���݂̍����[�^PWM�l�L�^      */
extern signed int   rightMotorBuff;         /* ���݂̉E���[�^PWM�l�L�^      */
extern signed int   MotorBuff;   	        /* ���݂̃��[�^PWM�l�L�^      */
extern int			angle_buff;             /* ���݃n���h���p�x�ێ��p   */
extern int          servo_center;           /* �T�[�{�Z���^�l               */

/* �v���g�^�C�v�錾                     */
void set_Speed(int speed);					/* ���s���x�̐ݒ� */
void run(int speed, int turn_speed);						/* ���s�֐� */
void motor( int accele_l, int accele_r );
void handle( int angle );

#endif