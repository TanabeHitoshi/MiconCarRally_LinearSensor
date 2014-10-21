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
/* 曲げハンドル値 */
#define SERVO1_PWM 	    0x06            /* 小曲げ */
#define SERVO2_PWM      0x07            /* 中曲げ */
#define SERVO3_PWM		0x08			/* 中中曲げ */
#define SERVO4_PWM		0x2A			/* 大曲げ */
#define SERVO5_PWM		0x2B			/* 大大曲げ */
/* ストップタイマー */
#define DF_STOP1	    0x09            /* ストップタイマー */
#define DF_STOP2      	0x0A            /* ストップタイマー */

#define DF_MOTOR_R		0x0B			/* 大曲内輪PWM値*/

#define DF_crank_motor1 0x0c            /*クランク外輪モーター*/
#define DF_crank_motor2 0x0d            /*クランク内輪モーター*/

#define DF_lane_PWM     0x0e

#define DF_lane_motorL  0x0f
#define DF_lane_motorR  0x10
#define DF_lane_motorS  0x11

#define Expose_Time		0x12
#define CL_BR_TM			0x04
#define DF_crank_handlepwm 0x13
#define DF_CL_BR1 0x14
#define DF_CL_BR2 0x15
#define DF_CL_BR3 0x16
#define DF_CL_BR4 0x17
#define DF_CL_BR5 0x18
#define DF_CL_SP1 0x19
#define DF_CL_SP2 0x1A 
#define DF_CL_SP3 0x1B
#define DF_CL_SP4 0x1C
#define DF_CL_SP5 0x1D
#define DF_CL_NUM 0x1E
#define TH_Number 0x1F
#define DF_MOTOR1 0x20	//01gai
#define DF_MOTOR2 0x21	//01nai
#define DF_MOTOR3 0x22	//02gai
#define DF_MOTOR4 0x23	//02nai
#define DF_MOTOR5 0x24	//03gai
#define DF_MOTOR6 0x25	//03nai
#define DF_MOTOR7 0x26	//04gai
#define DF_MOTOR8 0x27	//04nai
#define DF_MOTOR9 0x28	//05gai
#define DF_MOTOR10 0x29	//05nai
#define DF_CL_BT1  0x2A	//05nai
#define DF_CL_BT2  0x2B	//05nai
#define DF_CL_BT3  0x2C	//05nai
#define DF_CL_BT4  0x2D	//05nai
#define DF_CL_BT5  0x2E	//05nai