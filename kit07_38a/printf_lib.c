/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     printf,scanf�֘A����                                        */
/* �o�[�W����   Ver.1.02                                                    */
/* Date         2011.04.01                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include    <stdio.h>
#include    "sfr_r838a.h"               /* R8C/38A SFR�̒�`�t�@�C��    */
#include    "printf_lib.h"              /* printf�֘A����               */

/*======================================*/
/* �V���{����`                         */
/*======================================*/
#define         SEND_BUFF_SIZE  64      /* ���M�o�b�t�@�T�C�Y           */
#define         RECV_BUFF_SIZE  32      /* ��M�o�b�t�@�T�C�Y           */
#define         UART0_INT_LEVEL 1

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/
/* ���M�o�b�t�@ */
static volatile char    send_buff[SEND_BUFF_SIZE];
static volatile char    *send_w = send_buff;
static volatile char    *send_r = send_buff;
static volatile int     send_count = 0;

/* ��M�o�b�t�@ */
static volatile char    recv_buff[RECV_BUFF_SIZE];
static volatile char    *recv_w = recv_buff;
static volatile char    *recv_r = recv_buff;

/* printf,scanf�֌W */
volatile FILE           _iob[4];

/*======================================*/
/* �v���g�^�C�v�錾                     */
/*======================================*/
int write( int s );
int read( void );
void setSendBuff( char c);
int getSendBuff( char *c );

/************************************************************************/
/* UART0�̏������A�y��printf�֌W��UART0�Ɋ��蓖��                       */
/* �����@ �ʐM���x                                                      */
/* �߂�l �Ȃ�                                                          */
/************************************************************************/
void init_uart0_printf( int sp )
{
    stdin->_cnt = stdout->_cnt = stdaux->_cnt = stdprn->_cnt = 0;
    stdin->_flag = _IOREAD;
    stdout->_flag = _IOWRT;
    stdaux->_flag = _IORW;
    stdprn->_flag = _IOWRT;

    stdin->_mod = _TEXT;
    stdout->_mod = _TEXT;
    stdaux->_mod = _BIN;
    stdprn->_mod = _TEXT;

    stdin->_func_in = read;
    stdout->_func_in = NULL;
    stdaux->_func_in = read;
    stdprn->_func_in = NULL;

    stdin->_func_out = NULL;
    stdout->_func_out = write;
    stdaux->_func_out = write;
    stdprn->_func_out = NULL;

    /* UART0�̐ݒ� */
    u0sr = 0x05;                        /* P14=TXD0,P15=RXD0�ɐݒ�      */
    if( sp == SPEED_4800 ) {
        /* 4800bps u0brg = 2.5MHz / (4800 * 16) - 1 = 31.552 = 32 */
        u0c0 = 0x01;                    /* �J�E���g�\�[�X�Ȃǂ̐ݒ�     */
        u0c1 = 0x05;                    /* ���M���M����                */
        u0brg = 32;                     /* �ʐM���x = 4800pbs           */
    } else if( sp == SPEED_9600 ) {
        /* 9600bps u0brg = 20MHz / (9600 * 16) - 1 =129.208 = 129 */
        u0c0 = 0x00;                    /* �J�E���g�\�[�X�Ȃǂ̐ݒ�     */
        u0c1 = 0x05;                    /* ���M���M����                */
        u0brg = 129;                    /* �ʐM���x = 9600pbs           */
    } else if( sp == SPEED_19200 ) {
        /* 19200bps u0brg = 20MHz / (19200 * 16) - 1 = 64.104 = 64 */
        u0c0 = 0x00;                    /* �J�E���g�\�[�X�Ȃǂ̐ݒ�     */
        u0c1 = 0x05;                    /* ���M���M����                */
        u0brg = 64;                     /* �ʐM���x = 19200pbs          */
    } else if( sp == SPEED_38400 ) {
        /* 38400bps u0brg = 20MHz / (38400 * 16) - 1 = 31.552 = 32 */
        u0c0 = 0x00;                    /* �J�E���g�\�[�X�Ȃǂ̐ݒ�     */
        u0c1 = 0x05;                    /* ���M���M����                */
        u0brg = 32;                     /* �ʐM���x = 38400pbs          */
    }
    u0mr = 0x05;                        /* UART0 �ް���8bit 1�į���ޯ�  */
}

/************************************************************************/
/* printf�ŌĂяo�����֐�                                             */
/* ���[�U�[����͌Ăяo���܂���                                         */
/************************************************************************/
int write( int c )
{
    volatile char   put_data;

    if( c == '\n' )  {
        setSendBuff( '\r' );
    } else if( c == '\b' ) {
        setSendBuff( '\b' );
        setSendBuff( ' ' );
    }
    setSendBuff( c );

    if( (s0tic & 0x07) == 0 ) {
        getSendBuff( &put_data );
        s0tic = 0x01;                   /* ���M���荞�ݗD�惌�x���ݒ�   */
        put_uart0( put_data );
    }

    return 1;   // TRUE
}

/************************************************************************/
/* scanf�ŌĂяo�����֐�                                              */
/* ���[�U�[����͌Ăяo���܂���                                         */
/************************************************************************/
int read( void )
{
    volatile char   c;

    if( recv_r == recv_w ) {
        do {
            /* ��M�҂� */
            while( get_uart0( &c ) != 1 );

            switch( c ) {
                case '\b':  /* �o�b�N�X�y�[�X */
                    /* �����o�b�t�@�ɂȂ��Ȃ�BS�͖��� */
                    if( recv_r == recv_w ) continue;
                    /* ����Ȃ��߂� */
                    recv_w--;
                    break;
                case '\r':  /* Enter�L�[ */
                    *recv_w++ = c = '\n';
                    *recv_w++ = '\r';
                    break;
                default:
                    if( recv_w >= recv_buff+RECV_BUFF_SIZE-2 ) continue;
                    *recv_w++ = c;
                    break;
            }
            /* �G�R�[�o�b�N ���͂��ꂽ������Ԃ� */
            write( c );
        } while( c != '\n' );
    }
    c = *recv_r++;
    if( recv_r == recv_w ) recv_r = recv_w = recv_buff;

    return c;
}

/************************************************************************/
/* ���M�o�b�t�@�ɕۑ�                                                   */
/* �����@ �i�[����                                                      */
/* �߂�l �Ȃ�                                                          */
/* ����   �o�b�t�@���t���̏ꍇ�A�󂭂܂ő҂��܂�                        */
/************************************************************************/
void setSendBuff( char c )
{
    /* �o�b�t�@���󂭂܂ő҂� */
    while( SEND_BUFF_SIZE == send_count );

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    *send_w++ = c;
    if( send_w >= send_buff+SEND_BUFF_SIZE ) send_w = send_buff;
    send_count++;

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */
}

/************************************************************************/
/* ���M�o�b�t�@����擾                                                 */
/* �����@ �i�[���镶���̃A�h���X                                        */
/* �߂�l 0:�f�[�^�Ȃ� 1:�f�[�^����                                     */
/************************************************************************/
int getSendBuff( char *c )
{
    volatile int    ret = 0;

    if( send_count ) {
        asm( "FCLR I" );                /* �S�̂̊��荞�݋֎~           */

        *c = *send_r++;
        if( send_r >= send_buff+SEND_BUFF_SIZE ) send_r = send_buff;
        send_count--;
        ret = 1;

        asm( "FSET I" );                /* �S�̂̊��荞�݋���           */
    }
    return ret;
}

/************************************************************************/
/* URAT0 ���M���荞��                                                   */
/* �����@ �Ȃ�                                                          */
/* �߂�l �Ȃ�                                                          */
/************************************************************************/
#pragma interrupt   _uart0_trance(vect=17)
void _uart0_trance( void )
{
    volatile char   c;
    volatile int    ret;

    ir_s0tic = 0;

    ret = getSendBuff( &c );
    if( ret ) {
        put_uart0( c );                 /* �f�[�^����Ȃ瑗�M           */
    } else {
        s0tic = 0x00;                   /* �f�[�^�����Ȃ�I���         */
    }
}

/************************************************************************/
/* �P������M                                                           */
/* �����@ ��M�����i�[�A�h���X                                          */
/* �߂�l -1:��M�G���[ 0:��M�Ȃ� 1:��M���� ������*s�Ɋi�[            */
/************************************************************************/
int get_uart0( char *s )
{
    volatile int ret = 0;
    volatile int data, i;

    if (ri_u0c1 == 1){                  /* ��M�f�[�^����H             */
        data = u0rb;
        *s = (char)data;
        ret = 1;
        if( data & 0xf000 ) {           /* �G���[����H                 */
            /* �G���[���͍Đݒ� */
            re_u0c1 = 0;
            for( i=0; i<50; i++ );
            re_u0c1 = 1;

            ret = -1;
        }
    }
    return ret;
}

/************************************************************************/
/* �P�����o��                                                           */
/* �����@ ���M�f�[�^                                                    */
/* �߂�l 0:���M���̂��߁A���M�ł��� 1:���M�Z�b�g����                   */
/************************************************************************/
int put_uart0( char r )
{
    if(ti_u0c1 == 1) {                  /* ���M�f�[�^�Ȃ��H             */
        u0tbl = r;
        return 1;
    } else {
        /* ��ɑ��M��(����̃f�[�^�͑��M�����ɏI��) */
        return 0;
    }
}

/************************************************************************/
/* fgetc(�X�g���[������P�������͂���)����Ă΂��1�������͊֐�         */
/* �����@ ����                                                          */
/* �߂�l ����ɓ��͂ł����ꍇ�́A���̕���                              */
/*        �G���[�̏ꍇ�́AEOF                                           */
/************************************************************************/
int _sget( void ) {

    return 0;

}

/************************************************************************/
/* fgetc(�X�g���[������P�������͂���)����Ă΂��1�����o�͊֐�         */
/* �����@ �o�͕���                                                      */
/* �߂�l ����:1 �ȏ�:EOF                                               */
/************************************************************************/
int _sput( int put_data ) {

    return put_data;

}

/************************************************************************/
/* fgetc(�X�g���[������P�������͂���)����Ă΂��1�����o�͊֐�         */
/* �����@ �o�͕���                                                      */
/* �߂�l ����:1 �ȏ�:EOF                                               */
/************************************************************************/
int _pput( int put_data ) {

    return put_data;

}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2010.04.01 Ver.1.00 �쐬
2011.03.17 Ver.1.01 ���M���荞�݂��~�܂�s��̏C��(write�֐����̏C��)
2011.04.01 Ver.1.02 �ϐ���volatile�ǉ�
*/
