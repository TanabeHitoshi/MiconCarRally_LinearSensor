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
#define DF_KP_SP1		0x16			/* ���l Kp 				*/
#define DF_KI_SP1		0x17			/* �ϕ��l Ki				*/
#define DF_KD_SP1		0x18			/* �����l Kd					*/
#define DF_KP_SP2		0x19			/* ���l Kp 				*/
#define DF_KI_SP2		0x1A			/* �ϕ��l Ki				*/
#define DF_KD_SP2		0x1B			/* �����l Kd					*/


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

