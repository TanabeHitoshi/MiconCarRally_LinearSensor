#ifndef	TRACE_H
#define	TRACE_H



extern int		nowServo;             /* ���݂̃n���h���p�x�L�^       */
extern int		servo_angle;					/* �T�[�{�ɂ��v�Z */
extern int 		servo_Gap;						/* �T�[�{�̍� */

/* �v���g�^�C�v�錾                     */
void	servo_Trace(void);			/* �J�����T�[�{�̒ǐ� */
int motor_Trace(void);				/* �T�[�{�Ɏԑ̂��Ǐ] */

#endif