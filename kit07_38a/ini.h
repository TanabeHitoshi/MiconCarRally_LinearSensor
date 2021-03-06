#ifndef	INI_H
#define	INI_H

extern unsigned long	cnt0;                   /* timer関数用                  */
extern unsigned long	cnt1;                   /* main内で使用                 */
extern unsigned long	cnt_lcd;                /* LCD処理で使用                */
extern unsigned long	stop_timer;			   /* 走行タイマー					*/
extern unsigned int		cnt_Curve;				/* カーブで使用  */
extern unsigned int   	cnt_Stright;			/* 直進で使用  */

extern int				pattern;                /* パターン番号                 */
extern int				Srevo_state;			/* サーボの制御あり -> 1　なり -> 0	*/

/* microSD関連変数 */
extern signed char     msdBuff[ 512+32 ];      /* 一時保存バッファ(32は予備)   */
extern int             msdFlag;                /* 1:データ記録 0:記録しない    */
extern int             msdTimer;               /* 取得間隔計算用               */
extern int             msdError;               /* エラー番号記録               */

void init( void );
void timer( unsigned long timer_set );
void memPrint(void);

#endif