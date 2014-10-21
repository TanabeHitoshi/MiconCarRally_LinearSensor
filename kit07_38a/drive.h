#ifndef	DRIVE_H
#define	DRIVE_H

extern signed char     data_buff[ DF_PARA_SIZE ];
/* 現在の状態保存用 */
extern signed int	handleBuff;             /* 現在のハンドル角度記録       */
extern signed int   leftMotorBuff;          /* 現在の左モータPWM値記録      */
extern signed int   rightMotorBuff;         /* 現在の右モータPWM値記録      */
extern int			angle_buff;             /* 現在ハンドル角度保持用   */
extern int          servo_center;           /* サーボセンタ値               */
/* エンコーダ関連 */
extern int				iTimer10;			   	/* 10msカウント用		  	*/
extern int				iEncoder;			   	/* 10ms毎の最新値		   	*/
extern unsigned long	lEncoderTotal;		  	/* 積算値保存用				*/
extern unsigned int		uEncoderBuff;		   	/* 計算用　割り込み内で使用 */
extern unsigned long	lEncoderCrank;		  	/* クロスライン検出時の積算値 	*/
extern unsigned long 	lEncoderCrank2;			/* クロスライン検出時の積算値2 	*/
extern unsigned long	lEncoderHarf;		  	/* ハーフライン検出時の積算値 	*/

/* プロトタイプ宣言                     */
void motor( int accele_l, int accele_r );
void handle( int angle );

#endif