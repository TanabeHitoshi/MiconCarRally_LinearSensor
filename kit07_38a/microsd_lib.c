/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     microSD ���䃉�C�u����                                      */
/* �o�[�W����   Ver.3.00                                                    */
/* Date         2012.04.01                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/
/*

��FAT32���g��Ȃ��ꍇ
microSD���������Ƃ��Ă̂ݎg�p���AFAT32���g��Ȃ��ꍇ�́AFAT32�����̃v���O����
��OFF�ɂ��邱�Ƃ��ł��܂��B
OFF�ɂ���ꍇ�́A���L�̃p�����[�^���c�[���`�F�C���̃R���p�C���I�v�V������
�ǉ����Ă��������B
-DNO_FAT32

*/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "sfr_r838a.h"                  /* R8C/38A SFR�̒�`�t�@�C��    */
#include "microsd_lib.h"                /* microSD���䃉�C�u����        */

/*======================================*/
/* �萔�ݒ�                             */
/*======================================*/
#define MSD_CS_BIT      p6_1            /* CS�[�q�̃r�b�g               */
#define MSD_CS_BIT_DIM  pd6_1           /* CS�[�q�̃r�b�g�̓��o�͐ݒ�   */
#define LED_BLINK       0x40            /* LED�@�A�N�Z�X���̓_�Ŏ���    */
#define LED_ERRORBLINK  0x200           /* LED�@�G���[���̓_�Ŏ���      */

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/
static unsigned char            msdlibBuff[514];
                                        /* �ꎞ�ۊǃo�b�t�@             */
static volatile int             msdlibMode;
                                        /* ���                         */
static volatile int             msdlibCnt;
                                        /* �������ݐ�                   */
static volatile unsigned char*  msdlibRead;
                                        /* �ǂݍ��݃f�[�^�̃A�h���X     */
static volatile unsigned char*  msdlibWrite;
                                        /* �������݃f�[�^�̃A�h���X     */
static volatile int             msdlibError;
                                        /* �G���[�ԍ�                   */
static volatile char*           led_port;
                                        /* ���j�^LED�̂���|�[�g        */
static volatile char*           led_port_dim;
                                        /* ��L�̓��o�͐ݒ�|�[�g       */
static volatile char            led_port_bit;
                                        /* ���j�^LED�̂���|�[�g�̃r�b�g*/
static volatile unsigned int    msdLed;
                                        /* microSD ���j�^�pLED          */
                                        /* 0:�g�p���� 0�ȊO:�g�p        */
static volatile int             msdType;
                                        /* microSD�̃^�C�v              */
                                        /* 1:MMC or SD�J�[�hVer.1.x     */
                                        /* 2:SD�J�[�hVer.2.0            */
                                        /* 3:SDHC�J�[�h                 */
static volatile unsigned long   msdSize;
                                        /* microSD�̃T�C�Y(kB�P��)      */
static volatile int             modeSector;
                                        /* 1:�Z�N�^�w�胂�[�h 0:�A�h���X*/
/////// FAT32�֌W�ϐ� �������� /////////
#ifndef NO_FAT32
static int                      fFileOpen;
                                        /* �������ݍ�ƒ���1            */
static unsigned long            BPB_SctorNo;
                                        /* BPB�̃Z�N�^No                */
static unsigned long            BPB_SectorsPerFAT;
                                        /* FAT�̃Z�N�^��                */
static unsigned int             BPB_SctorsPerCluster;
                                        /* 1�N���X�^�̃Z�N�^��          */
static int                      BPB_NumberOfFATs;
                                        /* FAT�̐� �ʏ��2              */
static int                      BPB_ReservedSectors;
                                        /* BPB�̗̈�̃Z�N�^��          */
static unsigned long            BPB_bigTotalSectors;
                                        /* ���Z�N�^��                   */
static unsigned long            BPB_RootDirCluster;
                                        /* ٰ��ިڸ�؂�����N���X�^�ԍ� */
static unsigned long            Sec_FAT[4];
                                        /* FAT�̐擪�A�h���X            */
static unsigned long            Sec_Data;
                                        /* �f�[�^�̈�̐擪�A�h���X     */
static unsigned long            Sec_Dir;
                                        /* �f�B���N�g���̐擪�A�h���X   */
static unsigned long            writeSectorStart;
                                        /* �������݊J�n�Z�N�^           */
static unsigned long            writeSectorEnd;
                                        /* �������ݏI���Z�N�^           */
static unsigned long            writeSectorWork;
                                        /* �������ݒ��̃Z�N�^           */

static const char monthStr[] = { "JanFebMarAprMayJunJulAugSepOctNovDec" };
                                        /* ���ϊ��e�[�u��               */
static unsigned char            fileYear  = 2012-1980;  /* �������݁@�N */
static unsigned char            fileMonth = 4;          /* �������݁@�� */
static unsigned char            fileDay   = 1;          /* �������݁@�� */
static unsigned char            fileHour  = 12;         /* �������݁@�� */
static unsigned char            fileMin   = 0;          /* �������݁@�� */
static unsigned char            fileSec   = 0 / 2;      /* �������݁@�b */
#endif
/////// FAT32�֌W�ϐ� �����܂� /////////

/************************************************************************/
/* ���W���[���� msd_write                                               */
/* �����T�v     microSD�@�P�o�C�g��������                               */
/* ����         char �f�[�^                                             */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
static void msd_write( unsigned char data )
{
    volatile unsigned int i;

    u1tbl = data;
    while( ri_u1c1 == 0 );              /* ��M�f�[�^�Ȃ��Ȃ�J��Ԃ�   */
    i = u1rb;                           /* �_�~�[���[�h                 */
}

/************************************************************************/
/* ���W���[���� msd_read                                                */
/* �����T�v     microSD�@�P�o�C�g�ǂݍ���                               */
/* ����         �Ȃ�                                                    */
/* �߂�l       char �f�[�^                                             */
/************************************************************************/
static unsigned char msd_read( void )
{
    volatile unsigned char  ret;
    volatile unsigned int   data;

    u1tbl = 0xff;                       /* �_�~�[���C�g                 */
    while( ri_u1c1 == 0 );              /* ��M�f�[�^�Ȃ��Ȃ�J��Ԃ�   */

    data = u1rb;
    ret = data & 0x00ff;

    return ret;
}

/************************************************************************/
/* ���W���[���� sendCMD                                                 */
/* �����T�v     �R�}���h���M                                            */
/* ����         �R�}���h�A����1,����2,����3,����4,CRC                   */
/* �߂�l       microSD����̖߂�l                                     */
/************************************************************************/
static unsigned char sendCMD(
    unsigned char cmd,  unsigned char arg1, unsigned char arg2,
    unsigned char arg3, unsigned char arg4, unsigned char crc   )
{
    volatile int            i;
    volatile unsigned char  ret;

    msd_write( 0xff );

    msd_write( cmd );
    msd_write( arg1 );
    msd_write( arg2 );
    msd_write( arg3 );
    msd_write( arg4 );
    msd_write( crc );

    /* ���X�|���X��M */
    i = 500;                            /* ���g���C��                   */
    while( --i ) {
        MSD_CS_BIT = 1;

        MSD_CS_BIT = 0;

        ret = msd_read();
        if( ret != 0xff ) break;        /* 0xff�ȊO�ŏI��               */
    }
    return ret;
}

/************************************************************************/
/* ���W���[���� initMicroSD                                             */
/* �����T�v     microSD�J�[�h�̏�����                                   */
/* ����         �Ȃ�                                                    */
/* �߂�l       0:���� 1�ȏ�:�G���[                                     */
/************************************************************************/
int initMicroSD( void )
{
    volatile int i, r, ret = 0;
    unsigned char rr[16];

    msdlibMode = 1;

    MSD_CS_BIT_DIM = 1;                 /* CS_BIT�͏o�͒[�q�ɐݒ�       */

    /* UART1�̐ݒ� */
    pd6_4 = 0;                          /* P6_4/RXD1�͓��͒[�q�ɐݒ�    */
    u1sr  = 0x2a;                       /* UART1�̒[�q�I��              */
    s1tic = 0x00;                       /* UART1���M���荞�݋֎~        */
    s1ric = 0x00;                       /* UART1��M���荞�݋֎~        */
    u1mr  = 0x01;                       /* �ۯ��������ر�I/OӰ�ނɐݒ�  */
    u1c0  = 0x80;                       /* MSB�t�@�[�X�g�Af1��I��      */
    u1c1  = 0x00;                       /* ���M���荞�ݗv���I��         */
    u1brg = 200-1;                      /* �r�b�g���[�g 20MHz * 1/f1 *  */
                                        /*       1/200 * 1/2 = 50000bps */
    te_u1c1 = 1;                        /* ���M���r�b�g�F���M����     */
    re_u1c1 = 1;                        /* ��M���r�b�g�F��M����     */

    /* �e�[�q��ݒ� */
    MSD_CS_BIT = 1;

    for( i=0; i<10; i++ );

    /* 10�o�C�g�_�~�[�ő���Ȃ���A���͒[�q�`�F�b�N */
    for( i=0; i<10; i++ ) {
        r = msd_read();
        if( r != 0xff ) {               /* ���ڑ��H                     */
            ret = 1;
            goto initMicroSD_End;
        }
    }

    MSD_CS_BIT = 0;

    /* CMD0���M */
    i = 0;
    do {
        /* LED�_�ŏ��� */
        if( msdLed == 0x4000 ) msdLed += LED_BLINK;

        r = sendCMD( 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 );
        i++;
        if( i >= 10 ) {
            ret = 2;
            goto initMicroSD_End;
        }
    } while( (r&0xfe) != 0x00 );

    /* CMD8���M */
    msdType = 0;
    i = 0;
    do {
        /* LED�_�ŏ��� */
        if( msdLed == 0x4000 ) msdLed += LED_BLINK;

        r = sendCMD( 0x48, 0x00, 0x00, 0x01, 0xaa, 0x87 );
        i++;
        if( i >= 20 ) {
            msdType = MSD_TYPE_SD100;
            break;
        }
    } while( r == 0xff );

    if( msdType != MSD_TYPE_SD100 ) {
        /* R7�R�}���h�ǂݍ��� */
        for( i=0; i<5; i++ ) {
            rr[i] = msd_read();
        }
        if( (rr[2] != 0x01) || (rr[3] != 0xaa) ) {
            ret = 3;
            goto initMicroSD_End;
        }
    }

    if( msdType == MSD_TYPE_SD100 ) {
        // MMC�J�[�h Ver.2�ȑO�̃J�[�h

        /* CMD1���M */
        i = 0;
        do {
            /* LED�_�ŏ��� */
            if( msdLed == 0x4000 ) msdLed += LED_BLINK;

            MSD_CS_BIT = 1;
            msd_write( 0xff );
            MSD_CS_BIT = 0;

            r = sendCMD( 0x41, 0x00, 0x00, 0x00, 0x00, 0xf9 );
            i++;
            if( i >= 500 ) {
                ret = 4;
                goto initMicroSD_End;
            }
        } while( r != 0x00 );

        /* CMD16���M */
        i = 0;
        do {
            r = sendCMD( 0x50, 0x00, 0x00, 0x02, 0x00, 0x00 );
            i++;
            if( i >= 10 ) {
                ret = 5;
                goto initMicroSD_End;
            }
        } while( r != 0x00 );
    } else {
        // Ver.2.00�ȍ~��SD�J�[�h

        // CMD58
        i = 0;
        do {
            /* LED�_�ŏ��� */
            if( msdLed == 0x4000 ) msdLed += LED_BLINK;

            r = sendCMD( 0x7a, 0x40, 0x00, 0x00, 0x00, 0xaf );
            i++;
            if( i >= 50 ) {
                ret = 6;
                goto initMicroSD_End;
            }
        } while( r != 0x01 );
        // �c���M
        for( i=0; i<6; i++ ) {
            rr[i] = msd_read();
        }

        // ACMD41
        i = 0;
        do {
            /* LED�_�ŏ��� */
            if( msdLed == 0x4000 ) msdLed += LED_BLINK;

            // CMD55
            r = sendCMD( 0x77, 0x00, 0x00, 0x00, 0x00, 0x65 );
            // ACMD41
            r = sendCMD( 0x69, 0x40, 0x00, 0x00, 0x00, 0x77 );
            i++;
            if( i >= 1000 ) {
                ret = 7;
                goto initMicroSD_End;
            }
        } while( r != 0x00 );

        // CMD58
        i = 0;
        do {
            /* LED�_�ŏ��� */
            if( msdLed == 0x4000 ) msdLed += LED_BLINK;

            r = sendCMD( 0x7a, 0x40, 0x00, 0x00, 0x00, 0xaf );
            i++;
            if( i >= 10 ) {
                ret = 8;
                goto initMicroSD_End;
            }
        } while( r != 0x00 );
        // �c���M
        for( i=0; i<6; i++ ) {
            rr[i] = msd_read();
        }
        if( rr[0] & 0x40 ) msdType = MSD_TYPE_SDHC;
        else msdType = MSD_TYPE_SD200;
    }

    /* �e�ʂ̌v�Z */
    r = getMicroSD_CSD( rr );
    if( r != 0x00 ) {
        ret = 9;
        goto initMicroSD_End;
    }

    if( msdType != MSD_TYPE_SDHC ) {
        // SDHC�ȊO
        msdSize   = ((unsigned int)rr[ 6]&0x03) << 10;
        msdSize  |=  (unsigned int)rr[ 7]       <<  2;
        msdSize  |= ((unsigned int)rr[ 8]&0xc0) >>  6;
        msdSize  += 1;
        i               = ((unsigned int)rr[ 9]&0x03) <<  1;
        i              |= ((unsigned int)rr[10]&0x80) >>  7;
        i              += 2;
        msdSize <<= i;
        i               = (unsigned int)rr[5] & 0x0f;
        msdSize <<= i;
        msdSize >>= 10;                 /* �P�� kB                      */
    } else {
        // SDHC�̏ꍇ
        msdSize   = ((unsigned int)rr[7]&0x3f) << 16;
        msdSize  |=  (unsigned int)rr[8] << 8;
        msdSize  |=  (unsigned int)rr[9];
        msdSize  += 1;
        msdSize <<= 9;                  /* �P�� kB                      */
    }

initMicroSD_End:
    MSD_CS_BIT = 1;

    if( ret == 0 ) {
        // �������@����
        te_u1c1 = 0;                    /* ���M���r�b�g�F���M�֎~     */
        re_u1c1 = 0;                    /* ��M���r�b�g�F��M�֎~     */
        u1brg   = 122-1;                /* �r�b�g���[�g 20MHz * 1/f1 *  */
                                        /*      1/122 * 1/2 = 81967bps  */
        te_u1c1 = 1;                    /* ���M���r�b�g�F���M����     */
        re_u1c1 = 1;                    /* ��M���r�b�g�F��M����     */

        msdlibMode = 0;
    } else {
        // �������@�G���[
        msdlibMode = 99;
        if( msdLed ) msdLed = 0x8000;
    }
    return ret;
}

/************************************************************************/
/* ���W���[���� getMsdType                                              */
/* �����T�v     microSD�̃^�C�v���擾                                   */
/* ����         ����                                                    */
/* �߂�l       MSD_TYPE_SD100�AMSD_TYPE_SD200�AMSD_TYPE_SDHC           */
/************************************************************************/
int getMsdType( void )
{
    return msdType;
}

/************************************************************************/
/* ���W���[���� getMsdSize                                              */
/* �����T�v     microSD�̃T�C�Y���擾                                   */
/* ����         ����                                                    */
/* �߂�l       �T�C�Y�ikB�P��)                                         */
/************************************************************************/
unsigned long getMsdSize( void )
{
    return msdSize;
}

/************************************************************************/
/* ���W���[���� readMicroSD                                             */
/* �����T�v     microSD����f�[�^�ǂݍ���(512�o�C�g)                    */
/* ����         unsigned long   �A�h���X                                */
/*              signed char     *�ǂݍ��ޔz��̃A�h���X                 */
/* �߂�l       0:���� 1�ȏ�:�G���[                                     */
/************************************************************************/
int readMicroSD( unsigned long address, signed char *read )
{
    volatile int            i, r, ret = 0;
    volatile unsigned char  a1, a2, a3, a4;

    if( msdlibMode != 0 ) {             /* �ʂȍ�ƒ��Ȃ�G���[         */
        return 1;
    }
    msdlibMode = 1;

    /* LED�_�ŏ��� */
    if( msdLed == 0x4000 ) msdLed += LED_BLINK;

    if( modeSector == 1 ) {
        // �Z�N�^�w��
        if( msdType != MSD_TYPE_SDHC ) {
            // SDHC�łȂ��A�h���X���Z�N�^�ԍ��ɕϊ�
            address <<= 9;
        }
    } else {
        // �A�h���X�w��
        if( msdType == MSD_TYPE_SDHC ) {
            // SDHC�Ȃ�A�h���X���Z�N�^�ԍ��ɕϊ�
            address >>= 9;
        }
    }

    a1 = (unsigned char)( (address&0xff000000) >> 24);
    a2 = (unsigned char)( (address&0x00ff0000) >> 16);
    a3 = (unsigned char)( (address&0x0000ff00) >>  8);
    a4 = (unsigned char)(  address&0x000000ff       );

    MSD_CS_BIT = 1;

    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );

    MSD_CS_BIT = 0;

    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );

    /* CMD17���M */
    r = sendCMD( 0x51, a1, a2, a3, a4, 0xff );

    /* ���X�|���X�m�F */
    if( r != 0x00 ) {
        ret = 11;
        goto readMicroSD_End;
    }

    /* 0xfe���m�F */
    i = 0;
    do {
        r = msd_read();
        i++;
        if( i >= 1000 ) {
            ret = 12;
            goto readMicroSD_End;
        }
    } while( r != 0xfe );

    /* �f�[�^�ǂݍ��� */
    for( i=0; i<512; i++ ) {
        *read++ = msd_read();
    }

    /* CRC 2�o�C�g�ǂݍ��� */
    msd_read();
    msd_read();

readMicroSD_End:
    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );
    MSD_CS_BIT = 1;
    msdlibMode = (ret != 0) ? 99 : 0;   /* �������G���[�Ȃ�99���Z�b�g   */

    return ret;
}

/************************************************************************/
/* ���W���[���� writeMicroSD                                            */
/* �����T�v     microSD��������(512�o�C�g)                              */
/* ����         unsigned long   �A�h���X                                */
/*              signed char     *�������ޔz��̃A�h���X                 */
/* �߂�l       0:���� 1�ȏ�:�G���[                                     */
/************************************************************************/
int writeMicroSD( unsigned long address, signed char *write )
{
    volatile int            i, r, ret = 0;
    volatile unsigned char  a1, a2, a3, a4;

    if( msdlibMode != 0 ) {             /* �ʂȍ�ƒ��Ȃ�G���[         */
        return 1;
    }
    msdlibMode = 1;

    /* LED�_�ŏ��� */
    if( msdLed == 0x4000 ) msdLed += LED_BLINK;

    if( modeSector == 1 ) {
        // �Z�N�^�w��
        if( msdType != MSD_TYPE_SDHC ) {
            // SDHC�łȂ��A�h���X���Z�N�^�ԍ��ɕϊ�
            address <<= 9;
        }
    } else {
        // �A�h���X�w��
        if( msdType == MSD_TYPE_SDHC ) {
            // SDHC�Ȃ�A�h���X���Z�N�^�ԍ��ɕϊ�
            address >>= 9;
        }
    }

    MSD_CS_BIT = 0;

    /* CMD24���M */
    a1 = (unsigned char)( (address&0xff000000) >> 24);
    a2 = (unsigned char)( (address&0x00ff0000) >> 16);
    a3 = (unsigned char)( (address&0x0000ff00) >>  8);
    a4 = (unsigned char)(  address&0x000000ff       );
    r = sendCMD( 0x58, a1, a2, a3, a4, 0x00 );

    /* ���X�|���X�m�F */
    if( (r&0x1f) != 0x00 ) {
        ret = 21;
        goto writeMicroSD_End;
    }

    /* 1�o�C�g���Ԃ��󂯂� */
    r = msd_read();

    /* �J�n���} */
    msd_write( 0xfe );

    /* �f�[�^���M */
    for( i=0; i<512; i++ ) {
        msd_write( *write++ );
    }

    /* CRC���M */
    msd_write( 0xff );
    msd_write( 0xff );

    /* ���X�|���X�ǂݍ��� */
    r = msd_read();
    r &= 0x1f;

    if( r == 0x05 ) {
        ret = 0;        /* �������ݐ���I�� */
    } else if( r == 0x0d ) {
        ret = 22;       /* �������݃G���[   */
    } else {
        ret = 23;       /* ����ȊO�̃G���[ */
    }

    /* busy��Ԃ��I���܂ő҂� */
    for( i=0; i<10000; i++ ) {
        if( msd_read() != 0x00 ) break;
    }

writeMicroSD_End:
    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );
    MSD_CS_BIT = 1;
    msdlibMode = (ret != 0) ? 99 : 0;   /* �������G���[�Ȃ�99���Z�b�g   */

    return ret;
}

/************************************************************************/
/* ���W���[���� getMicroSD_CSD                                          */
/* �����T�v     microSD Card Specific Data�擾                          */
/* ����         signed char *�ǂݍ��ݔz��(16�o�C�g�ȏ�)                 */
/* �߂�l       0:���� 1�ȏ�:�G���[                                     */
/************************************************************************/
int getMicroSD_CSD( unsigned char *p )
{
    volatile int i, r, ret = 0;

    /* LED�_�ŏ��� */
    if( msdLed == 0x4000 ) msdLed += LED_BLINK;

    MSD_CS_BIT = 0;

    /* CMD9���M */
    r = sendCMD( 0x49, 0x00, 0x00, 0x00, 0x00, 0xaf );

    /* ���X�|���X�m�F */
    i = 0;
    do {
        r = msd_read();
        i++;
        if( i >= 1000 ) {
            ret = 31;
            goto getMicroSD_CSD_End;
        }
    } while( r & 0x80 );

    *p++ = r;

    /* CSD�ǂݍ��� */
    for( i=1; i<16; i++ ) {
        *p++ = msd_read();
    }

    /* �_�~�[���[�h */
    msd_read();
    msd_read();

getMicroSD_CSD_End:
    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );
    MSD_CS_BIT = 1;

    return ret;
}

/************************************************************************/
/* ���W���[���� eraseMicroSD                                            */
/* �����T�v     microSD�̃f�[�^�C���[�X(0x00��������)                   */
/* ����         unsinged long �J�n�A�h���X , �I���A�h���X               */
/* �߂�l       0:���� 1�ȏ�:�G���[                                     */
/************************************************************************/
int eraseMicroSD( unsigned long st_address, unsigned long ed_address )
{
    volatile int            i, r, ret = 0;
    volatile unsigned char  a1, a2, a3, a4;

    if( msdlibMode != 0 ) {             /* �ʂȍ�ƒ��Ȃ�G���[         */
        return msdlibMode;
    }
    msdlibMode = 1;

    if( msdLed ) msdLed += LED_BLINK;

    if( modeSector == 1 ) {
        // �Z�N�^�w��
        if( msdType != MSD_TYPE_SDHC ) {
            // SDHC�łȂ��Z�N�^���A�h���X�ԍ��ɕϊ�
            st_address <<= 9;
            ed_address <<= 9;
            ed_address += 511;
        }
    } else {
        // �A�h���X�w��
        if( msdType == MSD_TYPE_SDHC ) {
            // SDHC�Ȃ�A�h���X���Z�N�^�ԍ��ɕϊ�
            st_address >>= 9;
            ed_address >>= 9;
        }
    }

    MSD_CS_BIT = 0;

    /* CMD32���M(�C���[�X�J�n�A�h���X�̃Z�b�g) */
    a1 = (unsigned char)( (st_address&0xff000000) >> 24);
    a2 = (unsigned char)( (st_address&0x00ff0000) >> 16);
    a3 = (unsigned char)( (st_address&0x0000ff00) >>  8);
    a4 = (unsigned char)(  st_address&0x000000ff       );
    r = sendCMD( 0x60, a1, a2, a3, a4, 0xff );

    /* ���X�|���X�m�F */
    if( r != 0x00 ) {
        ret = 41;
        goto eraseMicroSD_End;
    }

    /* CMD33���M(�C���[�X�I���A�h���X�̃Z�b�g) */
    a1 = (unsigned char)( (ed_address&0xff000000) >> 24);
    a2 = (unsigned char)( (ed_address&0x00ff0000) >> 16);
    a3 = (unsigned char)( (ed_address&0x0000ff00) >>  8);
    a4 = (unsigned char)(  ed_address&0x000000ff       );
    r = sendCMD( 0x61, a1, a2, a3, a4, 0xff );

    /* ���X�|���X�m�F */
    if( r != 0x00 ) {
        ret = 42;
        goto eraseMicroSD_End;
    }

    /* CMD38���M(�C���[�X) */
    r = sendCMD( 0x66, 0, 0, 0, 0, 0xff );

    /* ���X�|���X�m�F */
    if( r != 0x00 ) {
        ret = 43;
        goto eraseMicroSD_End;
    }

    /* busy��Ԃ��I���܂ő҂� */
    for( i=0; i<10000; i++ ) {
        if( msd_read() != 0x00 ) break;
    }

eraseMicroSD_End:
    /* �_�~�[�N���b�N���M */
    msd_write( 0xff );
    MSD_CS_BIT = 1;

    /* �C���[�X��A�P��_�~�[���C�g */
    if( ret == 0 ) {
        for( i=0; i<512; i++ ) msdlibBuff[i] = 0;   /* �o�b�t�@�N���A   */

        msdlibMode = 0;
        r = writeMicroSD( st_address, (signed char*)msdlibBuff );
        if( r != 0x00 ) {
            /* �������݂ł��� */
            // ���܂ɃG���[��Ԃ�microSD�����邪�A����Ȃ悤�Ȃ̂�
            // �G���[�͖������Ă���
            //ret = 44;
        }
    }

    msdlibMode = (ret != 0) ? 99 : 0;   /* �������G���[�Ȃ�99���Z�b�g   */

    return ret;
}

/************************************************************************/
/* ���W���[���� setMicroSDdata                                          */
/* �����T�v     microSD�Ƀf�[�^�Z�b�g                                   */
/* ����         microSD�ɏ������ރf�[�^�̂���z��                       */
/* �߂�l       12:����ɏI�� ����ȊO:�������ݒ��ō���̃Z�b�g�͖���   */
/************************************************************************/
int setMicroSDdata( signed char *p )
{
    volatile int ret = 0;

    /* LED�_�ŏ��� */
    if( msdLed == 0x4000 ) msdLed += LED_BLINK;

#ifndef NO_FAT32
    /* �]�����̃G���[���Ȃ����`�F�b�N */
    if( msdlibMode == 0 ) {
        if( fFileOpen ) {
            if( msdLed ) msdLed = 0x8000;
            fFileOpen = 0;
            return 1;
        }
    }

    /* �t�@�C���Ƃ��ď������ݎ��̏���̃`�F�b�N */
    if( fFileOpen ) {
        if( writeSectorWork > writeSectorEnd ) {
            fFileOpen = 0;
            return 1;
        }
        writeSectorWork++;
    }
#endif

    if( msdlibMode != 11) {             /* ���ݏ������ݏ��������H       */
        goto setMicroSDdata_End;
    }

    /* 512�o�C�g msdlibBuff�֓]�� */
    msdlibRead = (unsigned char*)p;     /* �]�����Z�b�g                 */
    _asm( " mov.b _msdlibRead >> 16   ,r1h " );
    _asm( " mov.w _msdlibRead & 0ffffh, a0 " );
    _asm( " mov.w #_msdlibBuff        , a1 " );
    _asm( " mov.w #256                , r3 " );
    _asm( " smovf.w" );

    msdlibBuff[512] = 0xff;
    msdlibBuff[513] = 0xff;

    msdlibCnt = 514;                    /* �o�b�t�@�������ݐ�           */
    msdlibWrite = msdlibBuff;
    s1tic = 0x02;
    u1tbl = 0xfc;                       /* 1�������M(�c��͊����݂�)  */
    msdlibMode = 12;                    /* microSDProcess �����J�n      */

setMicroSDdata_End:
    return msdlibMode;
}

/************************************************************************/
/* ���W���[���� microSDProcessStart                                     */
/* �����T�v     microSDProcess�J�n����                                  */
/* ����         microSD�̏������݃A�h���X                               */
/* �߂�l       0:����ɏI�� ����ȊO:���ɏ������ݒ�                    */
/************************************************************************/
int microSDProcessStart( unsigned long address )
{
    volatile int            ret = 0, r;
    volatile unsigned char  a1, a2, a3, a4;

    if( msdlibMode != 0 ) {
        /* ���ɏ����� */
        ret = 1;
        goto microSDProcessStart_End;
    }

    if( msdLed ) msdLed += LED_BLINK;

    if( modeSector == 1 ) {
        // �Z�N�^�w��
        if( msdType != MSD_TYPE_SDHC ) {
            // SDHC�łȂ��A�h���X���Z�N�^�ԍ��ɕϊ�
            address <<= 9;
        }
    } else {
        // �A�h���X�w��
        if( msdType == MSD_TYPE_SDHC ) {
            // SDHC�Ȃ�A�h���X���Z�N�^�ԍ��ɕϊ�
            address >>= 9;
        }
    }

    /* microSD�ɏ������݃A�h���X�Z�b�g */
    a1 = (unsigned char)( (address&0xff000000) >> 24);
    a2 = (unsigned char)( (address&0x00ff0000) >> 16);
    a3 = (unsigned char)( (address&0x0000ff00) >>  8);
    a4 = (unsigned char)(  address&0x000000ff       );

    MSD_CS_BIT = 0;

    /* CMD25���M */
    r = sendCMD( 0x59, a1, a2, a3, a4, 0xff );

    /* ���X�|���X�m�F */
    if( (r&0x80) != 0x00 ) {
        ret = 2;                        /* �Z�b�g�ł���                 */
        MSD_CS_BIT = 1;
    } else {
        msdlibMode = 11;                /* �Z�b�g����                   */
    }

microSDProcessStart_End:
    return ret;
}

/************************************************************************/
/* ���W���[���� microSDProcessEnd                                       */
/* �����T�v     microSDProcess�I������                                  */
/* ����         microSD�̏������݃A�h���X                               */
/* �߂�l       0:����ɏI�� ����ȊO:���ɏ������ݒ�                    */
/************************************************************************/
int microSDProcessEnd( void )
{
    volatile int ret = 1;

    if( msdlibMode == 11 ) {
        msdlibMode = 21;                /* �I�������Z�b�g               */
    } else if( msdlibMode == 0 ) {
        /* LED�_�ŏ��� */
        if( msdLed == 0x4000 ) msdLed += LED_BLINK;
        ret = 0;                        /* �I��                         */
    }

    return ret;
}

/************************************************************************/
/* ���W���[���� microSDProcess                                          */
/* �����T�v     microSD�@�Ԍ��������ݏ���                               */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/* ����         1ms���ƂɊ��荞�ݏ�����������s���Ă�������             */
/************************************************************************/
void microSDProcess( void )
{
    volatile int i, r;

    /* ���j�^LED���� */
    if( msdLed ) {
        if( msdLed & 0x8000 ) {
            /* �G���[�Ȃ� */
            msdLed |= LED_ERRORBLINK;
            msdLed--;
            if( msdLed & (LED_ERRORBLINK>>1) ) {
                *led_port |= led_port_bit;
            } else {
                *led_port &= ~led_port_bit;
            }
        } else if( msdLed & 0x4000 ) {
            /* �_�łȂ� */
            if( msdLed & 0x3fff ) msdLed--;
            msdLed &= ( 0x4000 | ((LED_BLINK<<1)-1) );
            if( msdLed & (LED_BLINK>>1) ) {
                *led_port |= led_port_bit;
            } else {
                *led_port &= ~led_port_bit;
            }
        }
    }

    switch( msdlibMode ) {
    case 0:
        /* �J�n�O�̑ҋ@ */
        break;

    case 1:
        /* �ʂȍ�ƒ� */
        break;

    case 11:
        /* �J�n��̑ҋ@�@�������� */
        break;

    case 12:
        /* ���M���荞�݂�microSD�Ƀf�[�^�������ݒ� */
        if( msdlibCnt == 0 ) {
            msdlibMode = 13;
        }
        break;

    case 13:
        /* �Ō�̃f�[�^����܂ő҂� */
        if( ri_u1c1 == 1 ) {
            i = u1rb;                   /* �_�~�[���[�h                 */
            msdlibMode = 14;
        }
        break;

    case 14:
        /* �I������ */

        /* ���X�|���X�ǂݍ��� */
        r = msd_read();
        r &= 0x1f;

        if( r == 0x05 ) {
            msdlibError = 2;            /* �������ݐ���I��             */
            msdlibMode = 15;
        } else if( r == 0xc ) {
            msdlibError = 3;            /* �������݃G���[               */
            msdlibMode = 31;
        } else {
            msdlibError = 4;            /* ����ȊO�̃G���[             */
            msdlibMode = 31;
        }
        break;

    case 15:
        /* busy��Ԃ��I���܂ő҂� */
        if( msd_read() != 0x00 ) {
            msdlibMode = 11;
        }
        break;

    case 21:
        /* �I������ */
        msd_write( 0xfd );
        msdlibMode = 22;
        break;

    case 22:
        /* busy��Ԃ��I���܂ő҂� */
        if( msd_read() != 0x00 ) {
            msdlibMode = 31;
        }
        break;

    case 31:
        /* CS="1" */
        msd_write( 0xff );              /* �_�~�[�N���b�N���M           */
        MSD_CS_BIT = 0;
        msdlibMode = 32;                /* �����I��                     */
        msdlibCnt = 10;
        break;

    case 32:
        msdlibCnt--;
        if( msdlibCnt == 0 ) {
            msdlibMode = 0;             /* �����I��                     */
        }
        break;

    case 99:
        /* �������G���[ */
        break;
    }
}

/************************************************************************/
/* ���W���[���� checkMicroSDProcess                                     */
/* �����T�v     microSD�@�Ԍ��������ݏ����̏I���`�F�b�N                 */
/* ����         �Ȃ�                                                    */
/* �߂�l       0:�������� 11:�J�n��̑ҋ@ ����ȊO:������              */
/************************************************************************/
int checkMicroSDProcess( void )
{
  return msdlibMode;
}

/************************************************************************/
/* UART1���M ���荞�ݏ���                                               */
/************************************************************************/
#pragma interrupt intS1T(vect=19)
void intS1T( void )
{
    u1tbl = *msdlibWrite++;             /* �f�[�^���M                   */
    msdlibCnt--;

    if( ri_u1c1 == 1 ) u1rb;            /* �_�~�[���[�h                 */

    if( msdlibCnt == 0 ) {              /* ���M�f�[�^�������Ȃ�         */
        s1tic = 0x00;
    }
}

/************************************************************************/
/* ���W���[���� setMicroSDLedPort                                       */
/* �����T�v     microSD�@���j�^LED�̐ݒ�                                */
/* ����         char* ���j�^LED�̂���|�[�g                             */
/*              char* ���j�^LED�̂���|�[�g�̓��o�͐ݒ�|�[�g           */
/*              int   ���j�^LED�̂���|�[�g�̃r�b�g                     */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void setMicroSDLedPort( char *p, char *pd, int bit )
{
    led_port     = p;
    led_port_dim = pd;
    led_port_bit = 0x01 << bit;

    *led_port_dim |= led_port_bit;      /* LED��bit���o�͂ɐݒ�         */

    msdLed = 0x4000;                    /* LED�_������ ON               */
}

//////////////////////////////////////////////////////////////////////////
// FAT32�֌W�v���O������������                                          //
//////////////////////////////////////////////////////////////////////////
#ifndef NO_FAT32

/************************************************************************/
/* ���W���[���� convertBinaryToLong                                     */
/* �����T�v     �o�C�i���[�f�[�^4bytes��long�֕ϊ�                      */
/* �����@       �ϊ����̃A�h���X                                        */
/* �߂�l       �ϊ����long�^�̒l                                      */
/************************************************************************/
static unsigned long convertBinaryToLong( char *p )
{
    unsigned long data;

    data  = *(unsigned long*)(p+3) << 24;
    data |= *(unsigned long*)(p+2) << 16;
    data |= *(unsigned long*)(p+1) <<  8;
    data |= *(unsigned long*)(p+0);

    return  data;
}

/************************************************************************/
/* ���W���[���� mountMicroSD_FAT32                                      */
/* �����T�v     FAT32�Ń}�E���g(FAT16�͖��Ή�)                          */
/* �����@       ����                                                    */
/* �߂�l       0:FAT32�Ń}�E���g���� 1:�}�E���g�ł���                  */
/************************************************************************/
int mountMicroSD_FAT32( void )
{
    int ret, i;

    modeSector = 1;                     // �A�h���X�I�����Z�N�^

    // 0�Ԓn��ǂݍ����MBR��BPB���m�F
    ret = readMicroSD( 0x0000 , (signed char*)msdlibBuff );
    if( ret != 0 ) {
        ret = 1;
        goto mountMicroSD_FAT32_Error;
    }

    // MBR �܂��� BPB�łȂ��Ȃ�G���[
    if( msdlibBuff[510] != 0x55 || msdlibBuff[511] != 0xaa ) {
        ret = 2;
        goto mountMicroSD_FAT32_Error;
    }

    // MBR�Z�N�^�Ȃ�BPB�Z�N�^�ԍ��ǂݍ���
    if( msdlibBuff[0] != 0xeb && msdlibBuff[0] != 0xe9 ) {
        BPB_SctorNo = convertBinaryToLong( msdlibBuff+454 );

        // BPB�ǂݍ���
        readMicroSD( BPB_SctorNo , (signed char*)msdlibBuff );
        if( ret != 0 ) {
            ret = 3;
            goto mountMicroSD_FAT32_Error;
        }

        // BPB�łȂ��Ȃ�G���[
        if( msdlibBuff[510] != 0x55 || msdlibBuff[511] != 0xaa ) {
            ret = 4;
            goto mountMicroSD_FAT32_Error;
        }
        // BPB�łȂ��Ȃ�G���[
        if( msdlibBuff[0] != 0xeb && msdlibBuff[0] != 0xe9 ) {
            ret = 5;
            goto mountMicroSD_FAT32_Error;
        }
    }

    // FAT16��BPB�Z�N�^�ǂݍ���
    if( msdlibBuff[22] != 0x00 || msdlibBuff[23] != 0x00 ) {
        // FAT12orFAT16�͖��Ή�
        ret = 11;
        goto mountMicroSD_FAT32_Error;
    }

    // BPB�Ɋւ���v�Z
    BPB_SctorsPerCluster = (unsigned int)msdlibBuff[13];
    BPB_NumberOfFATs     = (unsigned int)msdlibBuff[16];

    BPB_ReservedSectors  = (unsigned int)msdlibBuff[15] << 8;
    BPB_ReservedSectors |= (unsigned int)msdlibBuff[14];

    BPB_bigTotalSectors  = convertBinaryToLong( msdlibBuff+32 );

    BPB_SectorsPerFAT    = convertBinaryToLong( msdlibBuff+36 );

    BPB_RootDirCluster   = convertBinaryToLong( msdlibBuff+44 );

    // FAT�̃Z�N�^�v�Z
    for( i=0; i<BPB_NumberOfFATs; i++ ) {
        Sec_FAT[i] = BPB_SctorNo+BPB_ReservedSectors + BPB_SectorsPerFAT*i;
    }

    // �f�[�^�̃Z�N�^�v�Z
    Sec_Data = BPB_SctorNo +
            BPB_ReservedSectors + BPB_SectorsPerFAT * BPB_NumberOfFATs;

    // �f�B���N�g���Z�N�^�v�Z
    Sec_Dir  = (BPB_RootDirCluster-2) * BPB_SctorsPerCluster;
    Sec_Dir += Sec_Data;

    modeSector = 0;                     // �A�h���X�I�����A�h���X

    return 0;

mountMicroSD_FAT32_Error:
    modeSector = 0;                     // �A�h���X�I�����A�h���X
    if( msdLed ) msdLed = 0x8000;

    return ret;
}

/************************************************************************/
/* ���W���[���� ClusterToSector                                         */
/* �����T�v     �N���X�^����Z�N�^���v�Z                                */
/* �����@       unsigned long �N���X�^                                  */
/* �߂�l       �Z�N�^�ԍ�                                              */
/************************************************************************/
static unsigned long ClusterToSector( unsigned long cluster )
{
    unsigned long work;

    work  = (cluster-2) * BPB_SctorsPerCluster;
    work += Sec_Data;

    return work;
}

/************************************************************************/
/* ���W���[���� nextCluster                                             */
/* �����T�v     ���̃N���X�^����                                        */
/* �����@       unsigned long ���݂̃N���X�^                            */
/*              unsigned long ���݂̃N���X�^���Ō�̂Ƃ��A���̒l�ȏ��  */
/*                            �N���X�^�ԍ��ŐV�����N���X�^�����        */
/*                            1�ȉ��Ȃ��炸�ɁA�I������               */
/* �߂�l       ���̃N���X�^�ԍ�                                        */
/*              0�Ȃ�e�ʃI�[�o�[�ō�ꂸ�A�܂��̓G���[                 */
/************************************************************************/
static unsigned long nextCluster(
                            unsigned long cluster, unsigned long s_cls )
{
    int i, ret;
    unsigned long   i4, s_cls2 = 0xffffffff;

    // �N���X�^�̏���`�F�b�N
    if( ClusterToSector(cluster)+7 >= BPB_bigTotalSectors ) {
        return 0;
    }

    ret = readMicroSD(
                Sec_FAT[0] + (cluster>>7), (signed char*)msdlibBuff );
    if( ret != 0 ) {
        return 0;
    }

    i = (cluster & 0x0000007f) << 2;
    i4  = convertBinaryToLong( msdlibBuff+i );;

    if( i4 <= 0x0ffffff6 ) return i4;   // ���̃N���X�^����

    // �ŏI�N���X�^�����A�N���X�^�͍��Ȃ�
    if( s_cls <= 1 ) return i4;

    // �ŏI�N���X�^�Ȃ�A�N���X�^���
    while( 1 ) {
        // �N���X�^�̏���`�F�b�N
        if( ClusterToSector(s_cls)+7 >= BPB_bigTotalSectors ) {
            return 0;
        }
        if( (s_cls>>7) != (s_cls2>>7) ) {
            s_cls2 = s_cls;
            readMicroSD(
                Sec_FAT[0] + (s_cls>>7), (signed char*)msdlibBuff );
        }

        i = (s_cls & 0x0000007f) << 2;
        i4  = convertBinaryToLong( msdlibBuff+i );

        if( i4 == 0x00000000 ) {    // �󂢂Ă��邩�H
            // �ŏI�N���X�^��0x0fffffff����������
            msdlibBuff[i+3] = 0x0f;
            msdlibBuff[i+2] = 0xff;
            msdlibBuff[i+1] = 0xff;
            msdlibBuff[i+0] = 0xff;
            for( i=0; i<BPB_NumberOfFATs; i++ ) {
                writeMicroSD(
                    Sec_FAT[i] + (s_cls>>7), (signed char*)msdlibBuff );
            }
            // ���݂̃N���X�^�ɍŏI�N���X�^���Ȃ�
            readMicroSD(
                Sec_FAT[0] + (cluster>>7), (signed char*)msdlibBuff );
            i = (cluster & 0x0000007f) << 2;
            msdlibBuff[i+3] = (s_cls>>24) & 0xff;
            msdlibBuff[i+2] = (s_cls>>16) & 0xff;
            msdlibBuff[i+1] = (s_cls>> 8) & 0xff;
            msdlibBuff[i+0] = (s_cls    ) & 0xff;
            for( i=0; i<BPB_NumberOfFATs; i++ ) {
                writeMicroSD(
                    Sec_FAT[i] + (cluster>>7), (signed char*)msdlibBuff );
            }
            // �N���X�^�̎Q�Ɛ���N���A
            i4 = ClusterToSector( s_cls );
            // �C���[�Y�R�}���h�́A�C�[���Y��̃��[�h�łȂ����G���[�ɂȂ�̂ŁA
            // write�ő�p����
            // eraseMicroSD( i4, i4 + BPB_SctorsPerCluster - 1 );
            for( i=0; i<512; i++ ) msdlibBuff[i] = 0;   /* �o�b�t�@�N���A   */
            for(i=0; i<BPB_SctorsPerCluster; i++ ) {
                writeMicroSD( i4 + i, (signed char*)msdlibBuff );
            }

            return s_cls;
        }
        s_cls++;
    }
    return 0;
}

/************************************************************************/
/* ���W���[���� writeFile                                               */
/* �����T�v     �t�@�C���쐬�A�������ݏ���                              */
/* �����@       char * �t�@�C����(8+3�`��)                              */
/*              unsigned long �t�@�C���T�C�Y                            */
/* �߂�l       0:���� 0�ȊO:���s                                       */
/************************************************************************/
int writeFile( const char *s, unsigned long fileSize )
{
    int  ret = 0, r, i, j;
    char *p ,name[8+3+1+1];
    unsigned long cluster, cluster2, st_cls;
    unsigned long fileSector, sector, i4;

    if( fFileOpen ) {
        // �t�@�C���I�[�v����
        ret = 1;
        goto writeFile_Error;
    }

    modeSector = 1;                     // �A�h���X�I�����Z�N�^

    // �t�@�C���T�C�Y���ŏ��N���X�^���Ő؂�グ
    fileSector  = fileSize + (BPB_SctorsPerCluster<<9) - 1;
    fileSector /= (BPB_SctorsPerCluster<<9);
    fileSector *= BPB_SctorsPerCluster;     // �N���X�^���̔{���ɂ���
    fileSize    = fileSector << 9;

    // �t�@�C���l�[���ϊ�
    memset( name, ' ', 8+3 );
    name[11] = '\0';
    for(p = name; *s; s++,p++ ) {
        if( p > name+11 ) {
            ret = 2;
            goto writeFile_Error;
        }
        if( *s == '.' ) p = name + 8, s++;
        if( *s == ',' ) p = name + 8, s++;
        *p = toupper(*s);        /* p�̎w�����g��啶���ɕϊ� */
    }

    // �����t�@�C���������邩�`�F�b�N
    cluster = BPB_RootDirCluster;
    ret = 0;
    while( ret == 0 ) {
        sector = ClusterToSector( cluster );

        for( j=0;j<BPB_SctorsPerCluster ; j++ ) {
            // 1�N���X�^�̃Z�N�^����ǂݍ���Ń`�F�b�N
            r = readMicroSD( sector , (signed char*)msdlibBuff );
            if( r != 0 ) {
                ret = 3;
                goto writeFile_Error;
            }
            for( i=0; i<16; i++ ) {
                if( memcmp(msdlibBuff+i*32, name, 11) == 0 ) {
                    ret = 1;    // �t�@�C������
                    goto writeFile_FileCechkExit;
                }
            }
            sector++;
        }
        cluster = nextCluster( cluster, 0 );
        if( cluster >= 0x0ffffff8 ) {
            ret = 2;            // �t�@�C���Ȃ�
        }
        if( cluster == 0 ) {
            ret = 4;            // �e�ʃI�[�o�[
            goto writeFile_Error;
        }
    }
writeFile_FileCechkExit:

    if( ret == 1 ) {
        // �����t�@�C����������Ȃ�폜

        // �t�@�C���폜
        cluster  = (unsigned long)msdlibBuff[i*32+21] << 24;
        cluster |= (unsigned long)msdlibBuff[i*32+20] << 16;
        cluster |= (unsigned long)msdlibBuff[i*32+27] <<  8;
        cluster |= (unsigned long)msdlibBuff[i*32+26];
        msdlibBuff[ i*32+0 ] = 0xe5;  // �t�@�C���폜
        r = writeMicroSD( sector, (signed char*)msdlibBuff );
        if( r != 0 ) {
            ret = 11;
            goto writeFile_Error;
        }

        // FAT�̈�폜
        cluster2  = 0xffffffff;
        while( 1 ) {
            if( (cluster>>7) != (cluster2>>7) ) {
                r = readMicroSD(
                    Sec_FAT[0] + (cluster>>7), (signed char*)msdlibBuff );
                if( r != 0 ) {
                    ret = 12;
                    goto writeFile_Error;
                }
            }
            cluster2 = cluster;
            i = (cluster2 & 0x0000007f) << 2;
            cluster  = convertBinaryToLong( msdlibBuff+i );
            msdlibBuff[i+3] = 0x00;
            msdlibBuff[i+2] = 0x00;
            msdlibBuff[i+1] = 0x00;
            msdlibBuff[i+0] = 0x00;
            if( (cluster >= 0x0ffffff8) ||
                                    ((cluster>>7) != (cluster2>>7)) ) {
                for( i=0; i<BPB_NumberOfFATs; i++ ) {
                    writeMicroSD(
                        Sec_FAT[i]+(cluster2>>7), (signed char*)msdlibBuff );
                    if( r != 0 ) {
                        ret = 12;
                        goto writeFile_Error;
                    }
                }
            }
            if( cluster >= 0x0ffffff8 ) break;
        }
    }

    // �t�@�C���쐬�A�̈�m��

    // �t�@�C���T�C�Y���AFAT�̈�m��
    cluster  = 2;
    cluster2 = 0xffffffff;
    ret = 0;
    i = 0;              // �X�^�[�g�N���X�^;
    while( ret == 0 ) {
        // �N���X�^�̏���`�F�b�N
        if( ClusterToSector(cluster)+7 >= BPB_bigTotalSectors ) {
            ret = 21;   // �A�������N���X�^�͌�����Ȃ�
            goto writeFile_Error;
        }

        if( (cluster>>7) != (cluster2>>7) ) {
            cluster2 = cluster;
            r = readMicroSD(
                Sec_FAT[0]+(cluster>>7) , (signed char*)msdlibBuff );
            if( r != 0 ) {
                ret = 22;
                goto writeFile_Error;
            }
        }

        j = (cluster & 0x0000007f) << 2;
        i4  = convertBinaryToLong( msdlibBuff+j );
        if( i4 != 0 ) {
            i = 0;
        } else {
            i++;
        }
        if( i >= (fileSector/BPB_SctorsPerCluster) ) {
            // �A�������N���X�^����
            st_cls = cluster - i + 1;   // �A�������N���X�^�̐擪
            ret = 1;
            break;
        }
        cluster++;
    }

    // RDE�̋󂫂��`�F�b�N
    cluster = BPB_RootDirCluster;
    ret = 0;
    while( ret == 0 ) {
        sector = ClusterToSector( cluster );

        for( i=0;i<BPB_SctorsPerCluster ; i++ ) {
            // 1�N���X�^�̃Z�N�^����ǂݍ���Ń`�F�b�N
            r = readMicroSD( sector , (signed char*)msdlibBuff );
            if( r != 0 ) {
                ret = 31;
                goto writeFile_Error;
            }

            for( j=0; j<16; j++ ) {
                if( msdlibBuff[j*32] == 0x00 || msdlibBuff[j*32] == 0xe5 ) {
                    // �󂫗̈�@�����I
                    p = msdlibBuff + j * 32;
                    *p++ = name[ 0];
                    *p++ = name[ 1];
                    *p++ = name[ 2];
                    *p++ = name[ 3];
                    *p++ = name[ 4];
                    *p++ = name[ 5];
                    *p++ = name[ 6];
                    *p++ = name[ 7];
                    *p++ = name[ 8];
                    *p++ = name[ 9];
                    *p++ = name[10];
                    *p++ = 0x20;                                    // 11 ����
                    *p++ = '\0';                                    // 12
                    *p++ = '\0';                                    // 13
                    *p++ = ((fileMin  <<5)&0xe0) | ((fileSec     )&0x1f);//14
                    *p++ = ((fileHour <<3)&0xf8) | ((fileMin  >>3)&0x07);//15
                    *p++ = ((fileMonth<<5)&0xe0) | ((fileDay     )&0x1f);//16
                    *p++ = ((fileYear <<1)&0xfe) | ((fileMonth>>3)&0x01);//17
                    *p++ = msdlibBuff[j * 32 + 16];                 // 18
                    *p++ = msdlibBuff[j * 32 + 17];                 // 19
                    *p++ = (st_cls  >>16)&0xff;                     // 20
                    *p++ = (st_cls  >>24)&0xff;                     // 21
                    *p++ = msdlibBuff[j * 32 + 14];                 // 22
                    *p++ = msdlibBuff[j * 32 + 15];                 // 23
                    *p++ = msdlibBuff[j * 32 + 16];                 // 24
                    *p++ = msdlibBuff[j * 32 + 17];                 // 25
                    *p++ = (st_cls      )&0xff;                     // 26
                    *p++ = (st_cls  >> 8)&0xff;                     // 27
                    *p++ = (fileSize    )&0xff;                     // 28
                    *p++ = (fileSize>> 8)&0xff;                     // 29
                    *p++ = (fileSize>>16)&0xff;                     // 30
                    *p++ = (fileSize>>24)&0xff;                     // 31
                    r = writeMicroSD( sector , (signed char*)msdlibBuff );
                    if( r != 0 ) {
                        ret = 32;
                        goto writeFile_Error;
                    }
                    goto writeFile_FileMakeExit;
                }
            }
            sector++;
        }
        cluster = nextCluster(
                    cluster, st_cls + fileSector/BPB_SctorsPerCluster );
        if( cluster == 0 ) {
            ret = 33;   // �󂫖���
            goto writeFile_Error;
        }
    }
writeFile_FileMakeExit:

    // �N���X�^�̈���Ȃ�
    cluster  = st_cls;
    cluster2 = 0xffffffff;
    ret = 0;
    while( ret == 0 ) {
        if( (cluster>>7) != (cluster2>>7) ) {
            cluster2 = cluster;
            r = readMicroSD(
                Sec_FAT[0]+(cluster>>7) , (signed char*)msdlibBuff );
            if( r != 0 ) {
                ret = 41;
                goto writeFile_Error;
            }
        }
        i = (cluster & 0x0000007f) << 2;
        cluster++;
        if( cluster >= (st_cls+fileSector/BPB_SctorsPerCluster) ) {
            cluster = 0x0fffffff;
            ret = 1;
        }
        msdlibBuff[i+3] = (cluster>>24) & 0xff;
        msdlibBuff[i+2] = (cluster>>16) & 0xff;
        msdlibBuff[i+1] = (cluster>> 8) & 0xff;
        msdlibBuff[i+0] = (cluster    ) & 0xff;

        if( (cluster>>7) != (cluster2>>7) ) {
            for( i=0; i<BPB_NumberOfFATs; i++ ) {
                r = writeMicroSD(
                    Sec_FAT[i] + (cluster2>>7), (signed char*)msdlibBuff );
                if( r != 0 ) {
                    ret = 42;
                    goto writeFile_Error;
                }
            }
        }
    }

    // �m�ۂ����̈�@�N���A
    writeSectorStart = ClusterToSector( st_cls );
    writeSectorWork  = writeSectorStart;
    writeSectorEnd   = ClusterToSector(
                        st_cls + fileSector / BPB_SctorsPerCluster ) - 1;

    r = eraseMicroSD( writeSectorStart, writeSectorEnd );
    // �G���[�������Ă�����

    r = microSDProcessStart( writeSectorStart );
    if( r != 0 ) {
        ret = 43;       // �}���`�u���b�N���C�g�̏������@���s
        ret = r;
        goto writeFile_Error;
    }

    fFileOpen = 1;      // �t�@�C���I�[�v��

    return 0;           // ����

writeFile_Error:
    // �G���[����
    if( msdLed ) msdLed = 0x8000;

    return ret;
}

/************************************************************************/
/* ���W���[���� convertDecimalToStr                                     */
/* �����T�v     int�^��10�i��������ɕϊ�                               */
/* �����@       int �l                                                  */
/*              int ����(�}�C�i�X���܂񂾐�)                            */
/*              char* �ϊ������l���i�[����z��                          */
/* �߂�l       �Ȃ�                                                    */
/* ����         ������46��s ��6���̏ꍇ                                 */
/************************************************************************/
void convertDecimalToStr( int value, int keta, signed char *p )
{
    char temp;

    if( keta > 6 ) keta = 6;

    if( value < 0 ) {
        value = -value;
        *p++ = '-';
        keta--;
    }

    p = p + keta;

    while( keta ) {
        p--;
        keta--;
        temp = value % 10;
        temp += '0';
        *p = temp;
        value /= 10;
    }
}
/************************************************************************/
/* ���W���[���� convertHexToStr                                         */
/* �����T�v     int�^��16�i��������ɕϊ�                               */
/* �����@       int �l                                                  */
/*              int ����                                                */
/*              char* �ϊ������l���i�[����z��                          */
/* �߂�l       �Ȃ�                                                    */
/* ����         ������24��s ��4���̏ꍇ                                 */
/************************************************************************/
void convertHexToStr( unsigned int value, int keta, signed char *p )
{
    char temp;

    if( keta > 4 ) keta = 4;

    p = p + keta;

    while( keta ) {
        p--;
        keta--;
        temp = value & 0x000f;
        if( temp < 10 ) {
            temp += '0';
        } else {
            temp += ('a' - 10);
        }
        *p = temp;
        value >>= 4;
    }
}

/************************************************************************/
/* ���W���[���� convertBinaryToStr                                      */
/* �����T�v     unsigned char�^��2�i���̕�����ɕϊ�                    */
/* �����@       unsigned char �l                                        */
/*              int ����                                                */
/*              char* �ϊ������l���i�[����z��                          */
/* �߂�l       �Ȃ�                                                    */
/* ����         ������38��s ��8���̏ꍇ                                 */
/************************************************************************/
void convertBinaryToStr( unsigned char value, int keta, signed char *p )
{
    char temp;

    if( keta > 8 ) keta = 8;

    p = p + keta;

    while( keta ) {
        p--;
        keta--;
        temp  = value & 0x0001;
        temp += '0';
        *p = temp;
        value >>= 1;
    }
}

/************************************************************************/
/* ���W���[���� getCompileYear                                          */
/* �����T�v     �R���p�C���������̔N���擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       �N                                                      */
/************************************************************************/
int getCompileYear( const char *p )
{
    int i;

    i = atoi( p + 7 );

    if( i < 1980 || i > 2107 ) i = 2012;

    return i;
}

/************************************************************************/
/* ���W���[���� getCompileMonth                                         */
/* �����T�v     �R���p�C���������̌����擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       ��                                                      */
/************************************************************************/
int getCompileMonth( const char *p )
{
    int i, r;

    for( i=0; i<12; i++ ) {
        r = strncmp( monthStr + i * 3, p, 3 );
        if( r == 0 ) return i + 1;
    }
    return 1;
}

/************************************************************************/
/* ���W���[���� getCompileDay                                           */
/* �����T�v     �R���p�C���������̓����擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       ��                                                      */
/************************************************************************/
int getCompileDay( const char *p )
{
    int i;

    i = atoi( p + 4 );

    if( i < 1 || i > 31 ) i = 1;

    return i;
}

/************************************************************************/
/* ���W���[���� getCompileHour                                          */
/* �����T�v     �R���p�C���������̎����擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       ��                                                      */
/************************************************************************/
int getCompileHour( const char *p )
{
    int i;

    i = atoi( p );

    if( i < 0 || i > 23 ) i = 0;

    return i;
}

/************************************************************************/
/* ���W���[���� getCompilerMinute                                       */
/* �����T�v     �R���p�C���������̕����擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       ��                                                      */
/************************************************************************/
int getCompilerMinute( const char *p )
{
    int i;

    i = atoi( p + 3 );

    if( i < 0 || i > 59 ) i = 0;

    return i;
}

/************************************************************************/
/* ���W���[���� getCompilerSecond                                       */
/* �����T�v     �R���p�C���������̕b���擾                              */
/* �����@       �Ȃ�                                                    */
/* �߂�l       �b                                                      */
/************************************************************************/
int getCompilerSecond( const char *p )
{
    int i;

    i = atoi( p + 6 );

    if( i < 0 || i > 59 ) i = 0;

    return i;
}

/************************************************************************/
/* ���W���[���� setDateStamp                                            */
/* �����T�v     �t�@�C�������Ƃ��̓��t��ݒ�                          */
/* �����@       �N�A���A��                                              */
/* �߂�l       ����                                                    */
/************************************************************************/
void setDateStamp( int y, int m, int d )
{
    if( y >= 1980 && y <= 2107 ) {
        fileYear  = y - 1980;
    } else {
        fileYear  = 2012 - 1980;
    }
    if( m >= 1 && m <= 12 ) {
        fileMonth = m;
    } else {
        fileMonth = 1;
    }
    if( d >= 1 && d <= 31 ) {
        fileDay = d;
    } else {
        fileDay = 1;
    }
}

/************************************************************************/
/* ���W���[���� setTimeStamp                                            */
/* �����T�v     �t�@�C�������Ƃ��̎��Ԃ�ݒ�                          */
/* �����@       ���A���A�b                                              */
/* �߂�l       ����                                                    */
/************************************************************************/
void setTimeStamp( int h, int m, int s )
{
    if( h >= 0 && h <= 23 ) {
        fileHour = h;
    } else {
        fileHour = 0;
    }
    if( m >= 0 && m <= 59 ) {
        fileMin = m;
    } else {
        fileMin = 0;
    }
    if( s >= 0 && s <= 59 ) {
        fileSec = s / 2;
    } else {
        fileSec = 0;
    }
}

/************************************************************************/
/* ���W���[���� readMicroSDNumber                                       */
/* �����T�v     microSD����t�@�C���ԍ��擾                             */
/* �����@       ����                                                    */
/* �߂�l       -1:�G���[ 0�ȏ�F�l                                     */
/************************************************************************/
int readMicroSDNumber( void )
{
    int ret;

    ret = readMicroSD( BPB_SctorNo, (signed char*)msdlibBuff );
    if( ret != 0 ) return -1;
    ret   = (unsigned char)msdlibBuff[508];
    ret <<= 8;
    ret  |= (unsigned char)msdlibBuff[509];

    return ret;
}

/************************************************************************/
/* ���W���[���� writeMicroSDNumber                                      */
/* �����T�v     microSD�Ƀt�@�C���ԍ���������                           */
/* �����@       �������ޔԍ�                                            */
/* �߂�l       -1:�G���[ 0�F�������݊���                               */
/************************************************************************/
int writeMicroSDNumber( int number )
{
    int ret;

    // BPB�ǂݍ���
    ret = readMicroSD( BPB_SctorNo, (signed char*)msdlibBuff );
    if( ret != 0 ) return -1;

    // �������ɐݒ�
    msdlibBuff[508] = (unsigned char)(number >> 8);
    msdlibBuff[509] = (unsigned char)(number & 0xff);

    // ��������
    ret = writeMicroSD( BPB_SctorNo, (signed char*)msdlibBuff );
    if( ret != 0 ) return -1;

    return 0;
}

#endif
//////////////////////////////////////////////////////////////////////////
// FAT32�֌W�v���O���������܂�                                         //
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2011.04.01 Ver.1.00 �쐬
2012.03.16 Ver.2.00 SDHC�Ή��A���̑��ׂ����C��
2012.04.01 Ver.3.00 FAT32�ւ̃t�@�C���������݂ɑΉ�(FAT12,FAT16,LFN�͖��Ή�)
*/
