/*======================================*/
/* �v���g�^�C�v�錾                     */
/*======================================*/
int initLcd( void );
void lcdShowProcess( void );
int lcdPrintf(char far *format, ...);
void lcdPosition(char x ,char y);

/* DataFlash�֘A */
#define DF_ADDR_START   0x3000          /* �������݊J�n�A�h���X         */
#define DF_ADDR_END     0x33ff          /* �������ݏI���A�h���X         */
#define DF_PARA_SIZE    64              /* DataFlash�p�����[�^��        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlash�`�F�b�N            */
/* �T�[�{�Z���^�[�l */
#define DF_SERVO1       0x01            /* �T�[�{�Z���^�l               */
#define DF_SERVO2       0x02            /* �T�[�{�Z���^�l               */
/* �X�s�[�h�̍ő�l�ݒ� */
#define DF_PWM          0x03            /* �X�s�[�h�l                   */

#define DF_CRANK_PWM    0x05            /* �N�����NPWM�l                */
#define DF_KP			0x06			/* ���l Kp */
#define DF_KI			0x07			/* �ϕ��l Ki */
#define DF_KD			0x08			/* �����l Kd*/

/* �X�g�b�v�^�C�}�[ */
#define DF_STOP1	    0x09            /* �X�g�b�v�^�C�}�[ */
#define DF_STOP2      	0x0A            /* �X�g�b�v�^�C�}�[ */

#define DF_crank_motorS 0x0B			/*�N�����N���x*/
#define DF_crank_motor1 0x0C            /*�N�����N�O�փ��[�^�[*/
#define DF_crank_motor2 0x0D            /*�N�����N���փ��[�^�[*/

#define DF_laneL_PWM     0x0e
#define DF_laneR_PWM     0x12

#define DF_lane_motorL  0x0f
#define DF_lane_motorR  0x10
#define DF_lane_motorS  0x11

#define DF_crank_handlepwm 0x13
