#ifndef	ISLINE_H
#define	ISLINE_H

extern signed char     data_buff[ DF_PARA_SIZE ];

extern int ImageData[16];		/* �Z���T�[�̒l */
extern int val_ImageData[16];	/* �Z���T�[�̒l */
extern int ad_white[16];		/* ���C���� */
extern int ad_black[16];		/* �� */
extern float sval[16];			/* ���K���Z���T�l */
extern int bi_sensor[16];			/* �Q�l�� */
extern int sensor8,sensor16;
extern int	White;					/* ���F�̐� */
extern float pos,pre_pos;			/* ���S�ʒu����̂��� */

void get_ad( void );
void LPF(void);							/* ���[�p�X�t�B���^�[ */
void AutoCalibration(void);
void Normanaization(void);
void binarization(void);				/* �Q�l�� */
int PID(void);
unsigned int allBlack(void);
unsigned int startbar_get( void );
void float_printf(float val,int n);

#endif