/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
int initLcd( void );
void lcdShowProcess( void );
int lcdPrintf(char far *format, ...);
void lcdPosition(char x ,char y);
void mcrProgram( void );
extern char				MCR_program[15][3];		/* コースをプログラム */
extern int mcrStart;

/* DataFlash関連 */
#define DF_ADDR_START   0x3000          /* 書き込み開始アドレス         */
#define DF_ADDR_END     0x33ff          /* 書き込み終了アドレス         */
#define DF_PARA_SIZE    64              /* DataFlashパラメータ数        baisuudefuyasu*/
extern signed char     data_buff[ DF_PARA_SIZE ];

#define DF_CHECK        0x00            /* DataFlashチェック            */
/* サーボセンター値 */
#define DF_SERVO1       0x01            /* サーボセンタ値               */
#define DF_SERVO2       0x02            /* サーボセンタ値               */
/* スピードの最大値設定 */
#define DF_PWM          0x03            /* スピード値                   */

#define DF_RACE			0x04            /* レースの種類                */
/* サーボの設定			*/
#define DF_KSP			0x05			/* 比例値 KSp 				*/
#define DF_KSI			0x06			/* 積分値 KSi				*/
#define DF_KSD			0x07			/* 微分値 KSd					*/
/* ストップタイマー */
#define DF_STOP		    0x08            /* ストップタイマー */
/* マシンの設定			*/
#define DF_KMP			0x09			/* 比例値 KMp 				*/
#define DF_KMI			0x0A			/* 積分値 KMi				*/
#define DF_KMD			0x0B			/* 微分値 KMd					*/
/* カメラの設定			*/
#define DF_KP_S			0x0C			/* 比例値 Kp 				*/
#define DF_KI_S			0x0D			/* 積分値 Ki				*/
#define DF_KD_S			0x0E			/* 微分値 Kd					*/

#define DF_LineStart	0x0F	
#define DF_LineStop		0x10

#define DF_KP_C			0x11			/* 比例値 Kp 				*/
#define DF_KI_C			0x12			/* 積分値 Ki				*/
#define DF_KD_C			0x13			/* 微分値 Kd					*/

/* スプリントの設定 */
#define DF_PWM_S		0x14			/* スプリントの入り速度	*/
#define DF_DISTANCE		0x15			/* スプリントの距離	*/
#define DF_KP_SP1		0x16			/* 比例値 Kp 				*/
#define DF_KI_SP1		0x17			/* 積分値 Ki				*/
#define DF_KD_SP1		0x18			/* 微分値 Kd					*/
#define DF_KP_SP2		0x19			/* 比例値 Kp 				*/
#define DF_KI_SP2		0x1A			/* 積分値 Ki				*/
#define DF_KD_SP2		0x1B			/* 微分値 Kd					*/


//#define DF_MCR_P_N	0x19
#define DF_MCR_P00	0x20
#define DF_MCR_P01	0x21
#define DF_MCR_P02	0x22
#define DF_MCR_P10	0x23
#define DF_MCR_P11	0x24
#define DF_MCR_P12	0x25
#define DF_MCR_P20	0x26
#define DF_MCR_P21	0x27
#define DF_MCR_P22	0x28
#define DF_MCR_P30	0x29
#define DF_MCR_P31	0x2a
#define DF_MCR_P32	0x2b
#define DF_MCR_P40	0x2c
#define DF_MCR_P41	0x2d
#define DF_MCR_P42	0x2e
#define DF_MCR_P50	0x2f
#define DF_MCR_P51	0x30
#define DF_MCR_P52	0x31
#define DF_MCR_P60	0x32
#define DF_MCR_P61	0x33
#define DF_MCR_P62	0x34
#define DF_MCR_P70	0x35
#define DF_MCR_P71	0x36
#define DF_MCR_P72	0x37
#define DF_MCR_P80	0x38
#define DF_MCR_P81	0x39
#define DF_MCR_P82	0x3a
#define DF_MCR_P90	0x3b
#define DF_MCR_P91	0x3c
#define DF_MCR_P92	0x3d
#define DF_MCR_P100	0x3e
#define DF_MCR_P101	0x3f
#define DF_MCR_P102	0x40
#define DF_MCR_P110	0x41
#define DF_MCR_P111	0x42
#define DF_MCR_P112	0x43
#define DF_MCR_P120	0x44
#define DF_MCR_P121	0x45
#define DF_MCR_P122	0x46
#define DF_MCR_P130	0x47
#define DF_MCR_P131	0x48
#define DF_MCR_P132	0x49
#define DF_MCR_P140	0x4a
#define DF_MCR_P141	0x4b
#define DF_MCR_P142	0x4c

//#define DF_prgram_Address 0x20

