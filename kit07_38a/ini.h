#ifndef	INI_H
#define	INI_H

extern unsigned long   cnt0;                   /* timer�֐��p                  */
extern unsigned long   cnt1;                   /* main���Ŏg�p                 */
extern unsigned long   cnt_lcd;                /* LCD�����Ŏg�p                */
extern unsigned long   stop_timer;				/* ���s�^�C�}�[					*/

extern int             pattern;                /* �p�^�[���ԍ�                 */

/* microSD�֘A�ϐ� */
extern signed char     msdBuff[ 512+32 ];      /* �ꎞ�ۑ��o�b�t�@(32�͗\��)   */
extern int             msdFlag;                /* 1:�f�[�^�L�^ 0:�L�^���Ȃ�    */
extern int             msdTimer;               /* �擾�Ԋu�v�Z�p               */
extern int             msdError;               /* �G���[�ԍ��L�^               */
/* �G���R�[�_�֘A */
extern int				iEncoder;			   	/* 10ms���̍ŐV�l		   	*/
extern int				Acceleration;			/* �����x					*/
extern unsigned long	lEncoderTotal;		  	/* �ώZ�l�ۑ��p				*/
extern unsigned int		uEncoderBuff;		   	/* �v�Z�p�@���荞�ݓ��Ŏg�p */
extern unsigned long	lEncoderCrank;		  	/* �N���X���C�����o���̐ώZ�l 	*/
extern unsigned long 	lEncoderCrank2;			/* �N���X���C�����o���̐ώZ�l2 	*/
extern unsigned long	lEncoderHarf;		  	/* �n�[�t���C�����o���̐ώZ�l 	*/

void init( void );
void timer( unsigned long timer_set );

#endif