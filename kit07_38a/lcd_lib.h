/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
int initLcd( void );
void lcdShowProcess( void );
int lcdPrintf(char far *format, ...);
void lcdPosition(char x ,char y);

/* DataFlash関連 */
#define DF_ADDR_START   0x3000          /* 書き込み開始アドレス         */
#define DF_ADDR_END     0x33ff          /* 書き込み終了アドレス         */
#define DF_PARA_SIZE    64              /* DataFlashパラメータ数        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlashチェック            */
/* サーボセンター値 */
#define DF_SERVO1       0x01            /* サーボセンタ値               */
#define DF_SERVO2       0x02            /* サーボセンタ値               */
/* スピードの最大値設定 */
#define DF_PWM          0x03            /* スピード値                   */

#define DF_CRANK_PWM    0x05            /* クランクPWM値                */
#define DF_KP			0x06			/* 比例値 Kp */
#define DF_KI			0x07			/* 積分値 Ki */
#define DF_KD			0x08			/* 微分値 Kd*/

/* ストップタイマー */
#define DF_STOP1	    0x09            /* ストップタイマー */
#define DF_STOP2      	0x0A            /* ストップタイマー */

#define DF_crank_motorS 0x0B			/*クランク速度*/
#define DF_crank_motor1 0x0C            /*クランク外輪モーター*/
#define DF_crank_motor2 0x0D            /*クランク内輪モーター*/

#define DF_laneL_PWM     0x0e
#define DF_laneR_PWM     0x12

#define DF_lane_motorL  0x0f
#define DF_lane_motorR  0x10
#define DF_lane_motorS  0x11

#define DF_crank_handlepwm 0x13
