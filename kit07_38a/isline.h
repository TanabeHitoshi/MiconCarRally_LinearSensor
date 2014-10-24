#ifndef	ISLINE_H
#define	ISLINE_H

extern signed char     data_buff[ DF_PARA_SIZE ];

extern int ImageData[16];		/* センサーの値 */
extern int val_ImageData[16];	/* センサーの値 */
extern int ad_white[16];		/* ライン白 */
extern int ad_black[16];		/* 黒 */
extern float sval[16];			/* 正規化センサ値 */
extern int bi_sensor[16];			/* ２値化 */
extern int sensor8,sensor16;
extern int	White;					/* 白色の数 */
extern float pos,pre_pos;			/* 中心位置からのずれ */

void get_ad( void );
void LPF(void);							/* ローパスフィルター */
void AutoCalibration(void);
void Normanaization(void);
void binarization(void);				/* ２値化 */
int PID(void);
unsigned int allBlack(void);
unsigned int startbar_get( void );
void float_printf(float val,int n);

#endif