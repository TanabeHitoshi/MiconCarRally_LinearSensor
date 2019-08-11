#ifndef	CAMERA_H
#define	CAMERA_H

/* LED Light	  �m�i�v�S�U�P�U	*/
#define	Light_ON		p0_addr.bit.b3 = 1
#define	Light_OFF		p0_addr.bit.b3 = 0

/* �J�����̃I�t�Z�b�g	*/
#define		OFFSET				9

/* PID�̐ݒ� */
#define		StrightPID		0
#define		CurvePID		1
#define		SprintPID		2
#define		Sprint_2PID		3
/* public �ϐ�	*/
extern	int		camera_Offset;							/* �J�����̒��S�ʒu�̃I�t�Z�b�g	*/
extern	int		White;									/* ���F�̌�								*/
extern	int		camera_Mode;							/* 0=�ʏ� 1=���}�X�N 2=�E�}�X�N 3=���e����	*/
extern	unsigned int 	Wide;							/* ���C���̕� */
extern	int		Center;									/* ���C���̒��S */
extern	long		center_Weight;							/* ���C���̏d�S */
extern	unsigned long pre_cycle;				/* �P�����̎���	*/
extern	int				pid_angle;

/* public�v���g�^�C�v�錾                     */
void	camera_Caputure(void);					/* �J��������̃f�[�^�擾					*/
void	set_PID(int s);							/* PID�̐ݒ� */
void 	expose( void );							/* �I�����Ԓ��� 		*/
void	raw_view(void);							/* RAW�f�[�^�̕\��		*/
void	bi_view(void);							/* �Q�l���f�[�^�̕\��	*/

#endif