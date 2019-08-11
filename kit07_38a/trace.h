#ifndef	TRACE_H
#define	TRACE_H



extern int		nowServo;             /* 現在のハンドル角度記録       */
extern int		servo_angle;					/* サーボによる計算 */
extern int 		servo_Gap;						/* サーボの差 */

/* プロトタイプ宣言                     */
void	servo_Trace(void);			/* カメラサーボの追跡 */
int motor_Trace(void);				/* サーボに車体が追従 */

#endif