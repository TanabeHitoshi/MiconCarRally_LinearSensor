#ifndef	DRIVE_H
#define	DRIVE_H

extern signed char     data_buff[ DF_PARA_SIZE ];
/* 現在の状態保存用 */
extern signed int	handleBuff;             /* 現在のハンドル角度記録       */
extern signed int   leftMotorBuff;          /* 現在の左モータPWM値記録      */
extern signed int   rightMotorBuff;         /* 現在の右モータPWM値記録      */
extern int			angle_buff;             /* 現在ハンドル角度保持用   */
extern int          servo_center;           /* サーボセンタ値               */

/* プロトタイプ宣言                     */
void motor( int accele_l, int accele_r );
void handle( int angle );

#endif