/*======================================*/
/* �v���g�^�C�v�錾                     */
/*======================================*/
int initLcd( void );
void lcdShowProcess( void );
int lcdPrintf(char far *format, ...);
void lcdPosition(char x ,char y);
void mcrProgram( void );
extern char				MCR_program[15][3];		/* �R�[�X���v���O���� */
extern int mcrStart;

/* DataFlash�֘A */
#define DF_ADDR_START   0x3000          /* �������݊J�n�A�h���X         */
#define DF_ADDR_END     0x33ff          /* �������ݏI���A�h���X         */
#define DF_PARA_SIZE    64              /* DataFlash�p�����[�^��        baisuudefuyasu*/
extern signed char     data_buff[ DF_PARA_SIZE ];

#define DF_CHECK        0x00            /* DataFlash�`�F�b�N            */
/* �T�[�{�Z���^�[�l */
#define DF_SERVO1       0x01            /* �T�[�{�Z���^�l               */
#define DF_SERVO2       0x02            /* �T�[�{�Z���^�l               */
/* �X�s�[�h�̍ő�l�ݒ� */
#define DF_PWM          0x03            /* �X�s�[�h�l                   */

#define DF_RACE			0x04            /* ���[�X�̎��                */
/* �T�[�{�̐ݒ�			*/
#define DF_KSP			0x05			/* ���l KSp 				*/
#define DF_KSI			0x06			/* �ϕ��l KSi				*/
#define DF_KSD			0x07			/* �����l KSd					*/
/* �X�g�b�v�^�C�}�[ */
#define DF_STOP		    0x08            /* �X�g�b�v�^�C�}�[ */
/* �}�V���̐ݒ�			*/
#define DF_KMP			0x09			/* ���l KMp 				*/
#define DF_KMI			0x0A			/* �ϕ��l KMi				*/
#define DF_KMD			0x0B			/* �����l KMd					*/
/* �J�����̐ݒ�			*/
#define DF_KP_S			0x0C			/* ���l Kp 				*/
#define DF_KI_S			0x0D			/* �ϕ��l Ki				*/
#define DF_KD_S			0x0E			/* �����l Kd					*/

#define DF_LineStart	0x0F	
#define DF_LineStop		0x10

#define DF_KP_C			0x11			/* ���l Kp 				*/
#define DF_KI_C			0x12			/* �ϕ��l Ki				*/
#define DF_KD_C			0x13			/* �����l Kd					*/

/* �X�v�����g�̐ݒ� */
#define DF_PWM_S		0x14			/* �X�v�����g�̓��葬�x	*/
#define DF_DISTANCE		0x15			/* �X�v�����g�̋���	*/
#define DF_KP_SP		0x16			/* ���l Kp 				*/
#define DF_KI_SP		0x17			/* �ϕ��l Ki				*/
#define DF_KD_SP		0x18			/* �����l Kd					*/
#define DF_PWM_SP		0x19			/* �X�v�����g�̑��x	*/
#define DF_KP_SP2		0x1A			/* ���l Kp 				*/
#define DF_KI_SP2		0x1B			/* �ϕ��l Ki				*/
#define DF_KD_SP2		0x1C			/* �����l Kd					*/
