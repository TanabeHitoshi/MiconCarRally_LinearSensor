#ifndef	INI_H
#define	INI_H

extern unsigned long   cnt0;                   /* timer関数用                  */
extern unsigned long   cnt1;                   /* main内で使用                 */
extern unsigned long   cnt_lcd;                /* LCD処理で使用                */
extern unsigned long   stop_timer;				/* 走行タイマー					*/

extern int             pattern;                /* パターン番号                 */

/* microSD関連変数 */
extern signed char     msdBuff[ 512+32 ];      /* 一時保存バッファ(32は予備)   */
extern int             msdFlag;                /* 1:データ記録 0:記録しない    */
extern int             msdTimer;               /* 取得間隔計算用               */
extern int             msdError;               /* エラー番号記録               */
/* エンコーダ関連 */
extern int				iEncoder;			   	/* 10ms毎の最新値		   	*/
extern int				Acceleration;			/* 加速度					*/
extern unsigned long	lEncoderTotal;		  	/* 積算値保存用				*/
extern unsigned int		uEncoderBuff;		   	/* 計算用　割り込み内で使用 */
extern unsigned long	lEncoderCrank;		  	/* クロスライン検出時の積算値 	*/
extern unsigned long 	lEncoderCrank2;			/* クロスライン検出時の積算値2 	*/
extern unsigned long	lEncoderHarf;		  	/* ハーフライン検出時の積算値 	*/

void init( void );
void timer( unsigned long timer_set );

#endif