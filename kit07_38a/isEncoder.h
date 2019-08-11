#ifndef	ISENCODER_H
#define	ISENCODER_H

/* エンコーダ関連 */
extern	int				iTimer10;			   	/* 10msカウント用		  	*/
extern	int   			iEncoder;			   	/* 10ms毎の最新値		   	*/
extern	int				pre_iEncoder;			/* 前の速度					*/
extern	int				Acceleration;			/* 加速度					*/
extern	unsigned long   lEncoderTotal;		  	/* 積算値保存用				*/
extern	unsigned int	uEncoderBuff;		   	/* 計算用　割り込み内で使用 */
extern	unsigned long	tripmeter_set;			/* トリップメーターのセット	*/

void odometer_ini(void);
long odometer(void);
void tripmeter_ini(void);
long tripmeter(void);
int speedmeter(void);


#endif