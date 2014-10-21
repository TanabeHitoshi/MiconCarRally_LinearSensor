/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     �t�����䃉�C�u����                                          */
/* �o�[�W����   Ver.1.02                                                    */
/* Date         2012.04.01                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include    <stdio.h>                   /* �W��Cײ���� ���o��           */
#include    <stdarg.h>                  /* �W��Cײ���� �ό��̎����� */
#include    "sfr_r838a.h"               /* R8C/38A SFR�̒�`�t�@�C��    */
#include    "lcd_lib.h"                 /* �t���֘A����                 */

/*======================================*/
/* �V���{����`                         */
/*======================================*/
/*
���t���EmicroSD��̉t���ڑ��|�[�g
P5_7    P5_6    P5_5    P5_4    P5_3    P5_2    P5_1    P5_0
���g�p  E       RW      RS      D7      D6      D5      D4
��D7�`D4�́A�K���|�[�g��bit3�`0�ɐڑ����܂��B
*/

/* �t���̃|�[�g�A�r�b�g�z�u */
#define LCD_PORT            p5          /* �t���̐ڑ�����Ă���|�[�g   */
#define LCD_PORT_DIR        pd5         /* ��L�|�[�g�̓��o�͐ݒ�|�[�g */
#define LCD_PORT_PULLUP     pu12        /* D7�[�q���v���A�b�v���郌�W�X�^*/

#define LCD_NODATA_BIT      0x80        /* �t���Ŏg���Ă��Ȃ��r�b�g     */
#define LCD_E_BIT           0x40        /* �t�� E bit                   */
#define LCD_RW_BIT          0x20        /* �t�� RW bit                  */
#define LCD_RS_BIT          0x10        /* �t�� RS bit                  */
#define LCD_D7_BIT          0x08        /* �t�� D7 bit                  */

/* �t���֘A�ϐ� */
#define LCD_MAX_X           16          /* �\�������� �� 16 or 20       */
#define LCD_MAX_Y           2           /* �\�������� �c  2 or  4       */

/* �t������R�[�h */
#define LCD_INST            0x00        /* �C���X�g���N�V����           */
#define LCD_DATA            LCD_RS_BIT  /* �f�[�^                       */

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/
static volatile char            buffLcdData[ LCD_MAX_X * LCD_MAX_Y ];
                                        /* �\���o�b�t�@                 */
static volatile char            buffLcdData2[ LCD_MAX_X * LCD_MAX_Y + 10 ];
                                        /* �\���o�b�t�@�ꎞ��ƃG���A   */
static volatile int             lcdBuffPosition;
                                        /* �o�b�t�@�ɏ������ވʒu       */
static volatile int             lcdMode = 1;
                                        /* �\������No�Ǘ�               */
static volatile int             lcdNowLocate;
                                        /* ���݂̕\�����Ă���ʒu       */
static volatile unsigned long   lcdBusyCnt;
                                        /* busy�X�^�[�g�J�E���^         */
static volatile int             lcdRefreshFlag;
                                        /* ���t���b�V���t���O           */
static volatile int             lcdConnect = 1;
                                        /* �t�����ڑ�����Ă��邩       */
static volatile unsigned long   lcdCnt; /* �t���^�C�~���O�p�J�E���^     */

/************************************************************************/
/* �^�C�}                                                               */
/* �ɒZ���^�C�~���O�p�^�C�} 1������1us���炢                            */
/************************************************************************/
static void lcd_nop( volatile int i )
{
    while( i-- );
}

/************************************************************************/
/* �^�C�}                                                               */
/* �����@ �^�C�}�l 1=1ms                                                */
/************************************************************************/
static void lcdTimer( unsigned long timer_set )
{
    volatile unsigned long t;

    t = lcdCnt;

    while( lcdCnt - t < timer_set );
}

/************************************************************************/
/* ���W���[���� lcdBusyStart                                            */
/* �����T�v     �t����busy�X�^�[�g                                      */
/* ����         busy�l                                                  */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
static void lcdBusyStart( void )
{
    lcdBusyCnt = lcdCnt;
}

/************************************************************************/
/* ���W���[���� lcdOut2                                                 */
/* �����T�v     lcdOut�̏�ʁA���ʂɕ����ē]�����镔��                  */
/* ����         4bit�f�[�^                                              */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
static void lcdOut2( char data2 )
{
    volatile unsigned char b;

    LCD_PORT = LCD_PORT & LCD_NODATA_BIT | data2;
    lcd_nop(1);
    LCD_PORT |= LCD_E_BIT;
    lcd_nop(5);
    LCD_PORT &= ~LCD_E_BIT;
    lcd_nop(5);
}

/************************************************************************/
/* ���W���[���� lcdOut                                                  */
/* �����T�v     �t���R�}���h�o��                                        */
/* ����         �R�}���h��ށA�f�[�^                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
static void lcdOut( char command, char data )
{
    volatile unsigned char work;

    /* ���4bit�]�� */
    work = (unsigned char)command | ((unsigned char)data >> 4);
    lcdOut2( work );

    /* ����4bit�]�� */
    work = (unsigned char)command | ((unsigned char)data & 0x0f);
    lcdOut2( work );
}

/************************************************************************/
/* ���W���[���� lcdLocate                                               */
/* �����T�v     �t���J�[�\���ړ�                                        */
/* ����         x , y                                                   */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
static void lcdLocate( int x, int y )
{
    volatile unsigned char work = 0x80;

    /* x�̌v�Z */
    work += x;

    /* y�̌v�Z */
    if( y == 1 ) {
        work += 0x40;
    } else if( y == 2 ) {
        work += 0x14;
    } else if( y == 3 ) {
        work += 0x54;
    }

    /* �J�[�\���ړ� */
    lcdOut( LCD_INST, work );
    lcdBusyStart();
}

/************************************************************************/
/* ���W���[���� checkLcdBusy                                            */
/* �����T�v     �t����busy�`�F�b�N                                      */
/* ����         �Ȃ�                                                    */
/* �߂�l       2:�^�C���A�E�g 1:ok 0:busy��                            */
/************************************************************************/
static int checkLcdBusy( void )
{
    volatile int ret;

    /* D3�`D0����͒[�q�ɂ��� */
    LCD_PORT_PULLUP = 1;
    LCD_PORT_DIR   &= 0xf0;

    LCD_PORT &= ( ~LCD_E_BIT & ~LCD_RW_BIT & ~LCD_RS_BIT );

    LCD_PORT |= LCD_RW_BIT;
    lcd_nop(1);

    LCD_PORT |= LCD_E_BIT;
    lcd_nop(5);

    ret = (LCD_PORT & LCD_D7_BIT) ? 0 : 1;
    if( ret == 0 ) {
        /* busy�Ȃ�A�^�C���A�E�g���`�F�b�N */
        if( lcdCnt - lcdBusyCnt >= 11 ) ret = 2;
    }

    LCD_PORT &= ~LCD_E_BIT;
    lcd_nop(5);

    LCD_PORT |= LCD_E_BIT;              /* 4bit���[�h�Ȃ̂�             */
    lcd_nop(5);                         /* �_�~�[��E bit���Ă�          */

    LCD_PORT &= ~LCD_E_BIT;
    lcd_nop(5);

    LCD_PORT &= ~LCD_RW_BIT;

    /* D3�`D0���o�͒[�q�ɂ��� */
    LCD_PORT_PULLUP = 0;
    LCD_PORT_DIR   |= 0x0f;

    return ret;
}

/************************************************************************/
/* ���W���[���� initLcd                                                 */
/* �����T�v     �t�������@������                                        */
/* ����         �Ȃ�                                                    */
/* �߂�l       0:�ُ� 1:����                                           */
/************************************************************************/
int initLcd( void )
{
    volatile int i;

    for( i=0; i<=LCD_MAX_X*LCD_MAX_Y-1; i++ ) { /* �ϐ�������           */
        buffLcdData[ i ] = ' ';
    }

    lcdTimer( 15 );                     /* 15ms�ȏ�̃^�C�}             */

    lcdOut2( 0x03 );                    /* ̧ݸ��ݾ�� 8bit��            */
    lcdTimer( 5 );                      /* 4.1ms�ȏ�̃^�C�}            */

    lcdOut2( 0x03 );                    /* ̧ݸ��ݾ�� 8bit��            */
    lcdTimer( 2 );                      /* 100us�ȏ�̃^�C�}            */

    lcdOut2( 0x03 );                    /* ̧ݸ��ݾ�� 8bit��            */
    lcdBusyStart();
    while( !(i = checkLcdBusy()) );
    if( i == 2 ) {
        lcdConnect = 0;
        return 0;                       /* �G���[�I��                   */
    }

    lcdOut2( 0x02 );                    /* ̧ݸ��ݾ�� 4bit��            */
    lcdBusyStart();
    while( !checkLcdBusy() );

    lcdOut( LCD_INST, 0x28 );           /* 1�s�Afont:5*7�ݒ�            */
    lcdBusyStart();
    while( !checkLcdBusy() );

    lcdOut( LCD_INST, 0x08 );           /* �\��off�ݒ�                  */
    lcdBusyStart();
    while( !checkLcdBusy() );

    lcdOut( LCD_INST, 0x01 );           /* �\���N���A                   */
    lcdBusyStart();
    while( !checkLcdBusy() );

    lcdOut( LCD_INST, 0x06 );           /* �ݸ���Ă���A�\����Ė���     */
    lcdBusyStart();
    while( !checkLcdBusy() );

    lcdOut( LCD_INST, 0x0c );           /* �\��ON                       */
    lcdBusyStart();
    while( !checkLcdBusy() );

    return 1;
}

/************************************************************************/
/* ���W���[���� lcdShowProcess                                          */
/* �����T�v     �t���\������                                            */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/* ����         ���̊֐��͊��荞�݂�1ms���ƂɎ��s���Ă�������           */
/************************************************************************/
void lcdShowProcess( void )
{
    if( !lcdConnect ) return;          /* �ڑ�����Ă��邩�`�F�b�N  */

    lcdCnt++;

    switch( lcdMode ) {
    case 1: /* �f�[�^�X�V���ꂽ���`�F�b�N */
        if( lcdRefreshFlag ) {
            lcdRefreshFlag = 0;
            lcdMode = 2;
        }
        break;

    case 2: /* �ʒu������*/
        if( checkLcdBusy() ) {
            lcdNowLocate = 0;
            lcdLocate( 0, 0 );
            lcdMode = 3;
        }
        break;

    case 3: /* ���s�ʒu�̊m�F */
        if( checkLcdBusy() ) {
            if( lcdNowLocate % LCD_MAX_X == 0 ) {
                lcdLocate( 0, lcdNowLocate / LCD_MAX_X );
            }
            lcdMode = 4;
        }
        break;

    case 4: /* �f�[�^�\������ */
        if( checkLcdBusy() ) {
            lcdOut( LCD_DATA, buffLcdData[ lcdNowLocate++ ] );
            lcdBusyStart();
            if( lcdNowLocate >= LCD_MAX_X * LCD_MAX_Y ) {
                lcdMode = 1;
            } else {
                lcdMode = 3;
            }
        }
        break;

    default:
        lcdMode = 1;
        break;
    }
    pd5 &= 0xf0;                        /* LCD�ƌ��p�̒[�q����͂ɐݒ�  */
}

/************************************************************************/
/* ���W���[���� lcdPrintf                                               */
/* �����T�v     �t���֕\���@�\���ʒu�͉ߋ��ɕ\�������ʒu�̎�����        */
/* ����         printf�Ɠ���                                            */
/* �߂�l       ���펞�F�o�͂���������@�ُ펞�F���̐�                  */
/************************************************************************/
int lcdPrintf(char *format, ...)
{
    volatile va_list argptr;
    volatile char    *p;
    volatile int     ret = 0;

    va_start(argptr, format);
    ret = vsprintf( buffLcdData2, format, argptr );
    va_end(argptr);

    if( ret > 0 ) {
        /* vsprintf������Ȃ�t���o�b�t�@�֓]�� */
        p = buffLcdData2;
        while( *p ) {
            buffLcdData[lcdBuffPosition++] = *p++;
            if( lcdBuffPosition >= LCD_MAX_X * LCD_MAX_Y ) {
                lcdBuffPosition = 0;
            }
        }
        lcdRefreshFlag = 1;
    }
    return ret;
}

/************************************************************************/
/* ���W���[���� lcdPosition                                             */
/* �����T�v     �t���̕\���ʒu�w��                                      */
/* ����         ���ʒu , �c�ʒu                                         */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void lcdPosition(char x ,char y)
{
    if( x >= LCD_MAX_X ) return;
    if( y >= LCD_MAX_Y ) return;

    lcdBuffPosition = x + y * LCD_MAX_X;
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2011.04.01 Ver.1.00 �쐬
2011.09.28 Ver.1.01 checkLcdBusy�֐��̃^�C���A�E�g�����C��
2012.04.01 Ver.1.02 �œK����ON�ɂ��Ă����삷��悤�Ή�
*/
