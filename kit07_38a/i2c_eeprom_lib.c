/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     I2C�ɂ��EEP-ROM(24C256B)����                               */
/* �o�[�W����   Ver.1.02                                                    */
/* Date         2011.08.13                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include    "sfr_r838a.h"               /* R8C/38A SFR�̒�`�t�@�C��    */
#include    "i2c_eeprom_lib.h"          /* EEP-ROM�֘A����              */

/*======================================*/
/* �V���{����`                         */
/*======================================*/

/* setPageWriteI2CEeprom�֐��P��ŕۑ��o����T�C�Y */
#define EEP_SAVE_SIZE       16          /* �ő�ۑ���                   */

/* ���̑���` */
#define ACK                 0           /* ���[�h����ACK�L��(ACK=0)     */
#define NO_ACK              1           /* ���[�h����ACK����(ACK=1)     */

#define STOP_RD             0           /* �X�g�b�v�̒��O�͓ǂݍ���     */
#define STOP_WR             1           /* �X�g�b�v�̒��O�͏�������     */

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/

static volatile unsigned char   eep_address;
                                        /* �A�h���X�̑I��               */
static volatile int             write_mode;
                                        /* �������e                     */
static volatile unsigned char   write_eep_address;
                                        /* ��������EEP-ROM�̃A�h���X    */
static volatile unsigned int    write_address;
                                        /* �������݃A�h���X             */
static volatile int             write_count;
                                        /* �������݌�                 */
static volatile signed char     write_buff[EEP_SAVE_SIZE];
                                        /* �������݃f�[�^�ۑ��o�b�t�@   */
static volatile signed char     *write_buff_p;
                                        /* �������݃f�[�^�ǂݍ��݈ʒu   */

/*======================================*/
/* �v���g�^�C�v�錾                     */
/*======================================*/

/************************************************************************/
/* ���W���[���� i2c_start                                               */
/* �����T�v     �X�^�[�g�֘A���W�X�^�ݒ�A�X�^�[�g�M�����M              */
/************************************************************************/
void i2c_start( void )
{
    stop_icsr = 0;
    while( bbsy_iccr2 == 1 );
    iccr1 |= 0x30;                      /* �}�X�^���M���[�h             */
    iccr2 = 0x90;                       /* �X�^�[�g�������s             */
}

/************************************************************************/
/* ���W���[���� i2c_stop                                                */
/* �����T�v     �X�g�b�v�M�����M                                        */
/************************************************************************/
void i2c_stop( int mode )
{
    if( mode == STOP_WR ) {
        iccr2 = 0x10;                   /* �X�g�b�v�������s             */
        tend_icsr = 0;
        nackf_icsr = 0;
        while(stop_icsr == 0);
        stop_icsr = 0;
    }
    iccr1 &= 0xcf;                      /* �X���[�u��M���[�h           */
}

/************************************************************************/
/* ���W���[���� i2c_write                                               */
/* �����T�v     EEP-ROM�@�P�o�C�g��������                               */
/* ����         signed char �f�[�^                                      */
/* �߂�l       int  acknowledge 0:�L��  1:����                         */
/************************************************************************/
int i2c_write( signed char data )
{
    int ret;

    icdrt = data;
    while( tdre_icsr == 0 );
    while( tend_icsr == 0 );

    ret = ackbr_icier;

    return ret;
}

/************************************************************************/
/* ���W���[���� i2c_read                                                */
/* �����T�v     EEP-ROM�@�P�o�C�g�ǂݍ���                               */
/* ����         int ACK�F�f�[�^������ NO_ACK�F�f�[�^�I��                */
/* �߂�l       signed char �f�[�^                                      */
/************************************************************************/
signed char i2c_read( int ack )
{
    signed char ret;

    tend_icsr = 0;
    iccr1 &= 0xef;
    tdre_icsr = 0;
    ackbt_icier = 1;
    ret = icdrr;                        /* �_�~�[���[�h                 */

    if( ack == NO_ACK ) {
        rcvd_iccr1 = 1;                 /* ���̎�M������֎~           */
    }

    while( rdrf_icsr == 0 );
    iccr2 = 0x10;
    while( stop_icsr == 0 );
    ret = icdrr;
    rcvd_iccr1 = 0;
    ackbt_icier = 0;

    return ret;
}

/************************************************************************/
/* ���W���[���� initI2CEeprom                                           */
/* �����T�v     EEP-ROM�̃|�[�g������                                   */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void initI2CEeprom( void )
{
    pd3 &= 0x5f;                        /* P3_7:SDA P3_5:SCL            */

    iicsel = 1;
    stop_icsr = 0;
    ice_iccr1 = 1;
    icmr  = 0x00;
    iccr1 = 0x82;
    icier = 0x00;
    icsr  = 0x00;
    sar   = 0x00;

    selectI2CEepromAddress( 0 );        /* �A�h���X�̑I��               */
    write_mode  = 0;
}

/************************************************************************/
/* ���W���[���� selectI2CEepromAddress                                  */
/* �����T�v     �ǂ̔ԍ���EEP-ROM���g�p���邩�I��                       */
/* ����         unsigned char EEP-ROM�̃A�h���X 0�`3                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void selectI2CEepromAddress( unsigned char address )
{
    address &= 0x03;
    eep_address = address << 1;
}

/************************************************************************/
/* ���W���[���� readI2CEeprom                                           */
/* �����T�v     EEP-ROM�ǂݍ���                                         */
/* ����         unsigned long   �A�h���X 0-32767                        */
/* �߂�l       signed char     �f�[�^                                  */
/************************************************************************/
signed char readI2CEeprom( unsigned long address )
{
    signed char ret;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    i2c_start();
    i2c_write( 0xa0 | eep_address );    /* device address(write)        */
    i2c_write( (address >> 8) & 0xff ); /* first address                */
    i2c_write( address & 0xff );        /* second address               */
    iccr2 = 0x90;                       /* �X�^�[�g�đ�                 */
    i2c_write( 0xa1 | eep_address );    /* device address(read)         */
    ret = i2c_read( NO_ACK );           /* data read                    */
    i2c_stop( STOP_RD );

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */

    return ret;
}

/************************************************************************/
/* ���W���[���� writeI2CEeprom                                          */
/* �����T�v     EEP-ROM��������                                         */
/* ����         unsigned long �A�h���X 0-32767 ,signed char �f�[�^      */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void writeI2CEeprom( unsigned long address, signed char write )
{
    int i;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    i2c_start();
    i2c_write( 0xa0 | eep_address );    /* device address(write)        */
    i2c_write( (address >> 8) & 0xff ); /* first address                */
    i2c_write( address & 0xff );        /* second address               */
    i2c_write( write );                 /* data write                   */
    i2c_stop( STOP_WR );

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */
}

/************************************************************************/
/* ���W���[���� setPageWriteI2CEeprom                                   */
/* �����T�v     EEP-ROM�y�[�W��������                                   */
/*              ���̊֐��Ńy�[�W�������݂��J�n����                      */
/* ����         unsigned long �A�h���X 0-32767 , int ��,              */
/*              signed char* �f�[�^������A�h���X                       */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void setPageWriteI2CEeprom(
                unsigned long address, int count, signed char* data )
{
    /* �������ݒ��Ȃ炱�̊֐��͒��~ */
    if( write_mode != 0 ) return;

    write_mode          = 1;
    write_eep_address   = eep_address;
    write_address       = address;
    write_count         = count;
    write_buff_p        = write_buff;

    if( count >= EEP_SAVE_SIZE ) count = EEP_SAVE_SIZE;
    do {
        *write_buff_p++ = *data++;
    } while( --count );

    write_buff_p = write_buff;
}

/************************************************************************/
/* ���W���[���� I2CEepromProcess                                        */
/* �����T�v     EEP-ROM�y�[�W��������                                   */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void I2CEepromProcess( void )
{
    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    switch( write_mode ) {
    case 1:
        i2c_start();
        write_mode = 2;
        break;
    case 2:
        i2c_write( 0xa0 | write_eep_address );  /* device address(write)*/
        write_mode = 3;
        break;
    case 3:
        i2c_write( (write_address >> 8) & 0xff );   /* first address    */
        write_mode = 4;
        break;
    case 4:
        i2c_write( write_address & 0xff );      /* second address       */
        write_mode = 5;
        break;
    case 5:
        i2c_write( *write_buff_p++ );           /* data write           */
        if( !(--write_count) ) write_mode = 6;
        break;
    case 6:
        i2c_stop( STOP_WR );
        write_mode = 0;
        break;
    }

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */
}

/************************************************************************/
/* ���W���[���� clearI2CEeprom                                          */
/* �����T�v     EEP-ROM�@�I�[���N���A                                   */
/* ����         char* �N���A���Ƀ��j�^����LED�̃|�[�g                   */
/*              int   �N���A���Ƀ��j�^����LED�̃r�b�g                   */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void clearI2CEeprom( char *led_port , int bit )
{
    unsigned int    address = 0;
    int             i;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    while( address < 32768 ) {
        if( !(address % 0x800) ) *led_port ^= (1<<bit);

        i2c_start();
        i2c_write( 0xa0 | eep_address ); /* device address(write)       */
        i2c_write( address >> 8 );      /* first address                */
        i2c_write( address & 0xff );    /* second address               */
        for( i=0; i<64; i++ ) {
            i2c_write( 0 );             /* data write                   */
        }
        i2c_stop( STOP_WR );

        while( !checkI2CEeprom() );     /* wait                         */

        address += 64;
    }

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */
}

/************************************************************************/
/* ���W���[���� checkI2CEeprom                                          */
/* �����T�v     EEP-ROM�������݌�A���ɓǂݏ����ł��邩�`�F�b�N         */
/* ����         �Ȃ�                                                    */
/* �߂�l       1:���ǂݏ���OK 0:�܂�                                   */
/************************************************************************/
int checkI2CEeprom( void )
{
    int ret;

    if( write_mode != 0 ) return 0;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    i2c_start();
    ret = !i2c_write( 0xa0 | eep_address ); /* device address(write)    */
    i2c_stop( STOP_WR );

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */

    return ret;
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2010.04.01 Ver.1.00 �쐬
2011.04.01 Ver.1.01 �ϐ���volatile�ǉ�
2011.08.13 Ver.1.02 EEP-ROM�ɕۑ�����f�[�^�^��char�^����unsigned char�^�ɕύX
*/
