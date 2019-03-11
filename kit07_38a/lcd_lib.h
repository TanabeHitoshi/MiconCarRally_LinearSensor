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
#define DF_KP_SP		0x16			/* 比例値 Kp 				*/
#define DF_KI_SP		0x17			/* 積分値 Ki				*/
#define DF_KD_SP		0x18			/* 微分値 Kd					*/
#define DF_PWM_SP		0x19			/* スプリントの速度	*/
#define DF_KP_SP2		0x1A			/* 比例値 Kp 				*/
#define DF_KI_SP2		0x1B			/* 積分値 Ki				*/
#define DF_KD_SP2		0x1C			/* 微分値 Kd					*/
