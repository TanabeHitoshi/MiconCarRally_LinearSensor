/*======================================*/
/* 定数設定                             */
/*======================================*/
#define     SW_0    (0x01 << 0)         /* getSwNow関数のbit0の位置     */
#define     SW_1    (0x01 << 1)         /* getSwNow関数のbit1の位置     */
#define     SW_2    (0x01 << 2)         /* getSwNow関数のbit2の位置     */
#define     SW_3    (0x01 << 3)         /* getSwNow関数のbit3の位置     */
#define     SW_4    (0x01 << 4)         /* getSwNow関数のbit4の位置     */

/*======================================*/
/* シンボル定義                         */
/*======================================*/
/* 定数設定 */
#define PWM_CYCLE       19999           /* モータPWMの周期       4mS     */
#define SERVO_CENTER    3863            /* サーボのセンタ値             */
#define HANDLE_STEP     22              /* 1゜分の値                    */

/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
void initSwitch( void );
unsigned char getSwNow( void );
unsigned char getSwFlag( unsigned char flag );
void switchProcess( void );
