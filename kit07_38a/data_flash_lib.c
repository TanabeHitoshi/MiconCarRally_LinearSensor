/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     �f�[�^�t���b�V������                                        */
/* �o�[�W����   Ver.1.03                                                    */
/* Date         2011.04.01                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include    "sfr_r838a.h"               /* R8C/38A SFR�̒�`�t�@�C��    */
#include    "data_flash_lib.h"          /* �f�[�^�t���b�V�����C�u����   */

/*======================================*/
/* �V���{����`                         */
/*======================================*/
/* �f�[�^�t���b�V���֌W */
#define DF_SELECT_A 0                   /* �u���b�NA�I��                */
#define DF_SELECT_B 1                   /* �u���b�NB�I��                */
#define DF_SELECT_C 2                   /* �u���b�NC�I��                */
#define DF_SELECT_D 3                   /* �u���b�ND�I��                */

/*======================================*/
/* �v���g�^�C�v�錾(���[�J��)           */
/*======================================*/
int clrStatusRegister( unsigned int c_address );
int checkBlockAddress( unsigned int chk_address );

/************************************************************************/
/* ���W���[���� readDataFlash                                           */
/* �����T�v     �f�[�^�t���b�V���ǂݍ���                                */
/* ����         unsigned int    �ǂݍ��݌��A�h���X 0x3000-0x3fff        */
/*              char*           �ǂݍ��ݐ�A�h���X                      */
/*              int             �ǂݍ��ރf�[�^��                        */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void readDataFlash( unsigned int r_address, signed char *w_address, int count )
{
    if( count >= 1 && count <= 4096 ) {
        while( count-- ) {
            *w_address = *((signed char*)r_address);
            w_address++;
            r_address++;
        }
    }
}

/************************************************************************/
/* ���W���[���� blockEraseDataFlash                                     */
/* �����T�v     �u���b�N�C���[�Y                                        */
/* ����         unsinged int    �A�h���X                                */
/* �߂�l       1:�G���[�Ȃ� 0:�G���[����                               */
/* ����         ������135ms���x                                         */
/************************************************************************/
int blockEraseDataFlash( unsigned int address )
{
    volatile int ret = 1;
    volatile int block;

    block = checkBlockAddress( address );
    if( block == -1 ) return 0;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    fmr01 = 0;                          /* CPU�����������[�h�ɐݒ�      */
    asm(" ");                           /* �œK���}��                   */
    fmr01 = 1;
    fmr02 = 0;                          /* EW1���[�h�I��                */
    asm(" ");                           /* �œK���}��                   */
    fmr02 = 1;
    cmderie = 0;                        /* �ڰ��/ײĴװ���荞�݋֎~     */
    bsyaeie = 0;                        /* �ׯ�������װ���荞�݋֎~     */
    rdystie = 0;                        /* �ׯ����ި�ð�����荞�݋֎~   */

    /* �ް��ׯ����ۯ����������֎~�r�b�g�������������ɂ��� */
    switch( block ) {
        case DF_SELECT_A:
            fmr14 = 1;
            asm(" ");
            fmr14 = 0;                  /* ��ۯ�A ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_B:
            fmr15 = 1;
            asm(" ");
            fmr15 = 0;                  /* ��ۯ�B ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_C:
            fmr16 = 1;
            asm(" ");
            fmr16 = 0;                  /* ��ۯ�C ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_D:
            fmr17 = 1;
            asm(" ");
            fmr17 = 0;                  /* ��ۯ�D ��ĳ������ގ�t�\   */
            break;
    }

    fmr20 = 0;                          /* �ڰ�޻������(ES)�֎~         */
    fmr22 = 0;                          /* ���荞�ݗv����ESظ��ċ֎~    */
    fmr27 = 0;                          /* �����d��ذ��Ӱ�ދ֎~       */

    /* �u���b�N�C���[�Y */
    *((char *)address) = 0x20;          /* �u���b�N�C���[�Y�R�}���h     */
    *((char *)address) = 0xd0;
    while( fst7 == 0 );                 /* �����҂�                     */

    ret = clrStatusRegister( address );

    *((char *)address) = 0xff;          /* �f�[�^�t���b�V���ǂݍ��ݏ�� */

    /* �ް��ׯ����ۯ����������֎~�r�b�g�����������֎~�ɂ��� */
    switch( block ) {
        case DF_SELECT_A:
            fmr14 = 1;
            break;
        case DF_SELECT_B:
            fmr15 = 1;
            break;
        case DF_SELECT_C:
            fmr16 = 1;
            break;
        case DF_SELECT_D:
            fmr17 = 1;
            break;
    }
    fmr01 = 0;                          /* CPU�����������[�h����        */

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */

    return ret;
}

/************************************************************************/
/* ���W���[���� writeDataFlash                                          */
/* �����T�v     �v���O����(��������)����                                */
/* ����         unsigned int    �������݌��A�h���X 0x3000-0x3fff        */
/*              char*           �������ݐ�A�h���X                      */
/*              int             �������ރf�[�^��                        */
/* �߂�l       1:�G���[�Ȃ� 0:�G���[����                               */
/* ����         ������64�o�C�g�������݂�9.5ms                           */
/************************************************************************/
int writeDataFlash( unsigned int w_address, signed char *r_address, int count )
{
    volatile int ret = 1, i;
    volatile int block, block2;

    // �J�n�A�h���X�̃`�F�b�N
    block = checkBlockAddress( w_address );
    if( block == -1 ) return 0;

    // �I���A�h���X�̃`�F�b�N
    block2 = checkBlockAddress( w_address + count - 1 );
    if( block2 == -1 ) return 0;

    // �����u���b�N���`�F�b�N
    if( block != block2 ) return 0;

    asm( "FCLR I" );                    /* �S�̂̊��荞�݋֎~           */

    fmr01 = 0;                          /* CPU�����������[�h�ɐݒ�      */
    asm(" ");                           /* �œK���}��                   */
    fmr01 = 1;
    fmr02 = 0;                          /* EW1���[�h�I��                */
    asm(" ");                           /* �œK���}��                   */
    fmr02 = 1;
    cmderie = 0;                        /* �ڰ��/ײĴװ���荞�݋֎~     */
    bsyaeie = 0;                        /* �ׯ�������װ���荞�݋֎~     */
    rdystie = 0;                        /* �ׯ����ި�ð�����荞�݋֎~   */

    /* �ް��ׯ����ۯ����������֎~�r�b�g�������������ɂ��� */
    switch( block ) {
        case DF_SELECT_A:
            fmr14 = 1;
            asm(" ");
            fmr14 = 0;                  /* ��ۯ�A ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_B:
            fmr15 = 1;
            asm(" ");
            fmr15 = 0;                  /* ��ۯ�B ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_C:
            fmr16 = 1;
            asm(" ");
            fmr16 = 0;                  /* ��ۯ�C ��ĳ������ގ�t�\   */
            break;
        case DF_SELECT_D:
            fmr17 = 1;
            asm(" ");
            fmr17 = 0;                  /* ��ۯ�D ��ĳ������ގ�t�\   */
            break;
    }

    fmr20 = 0;                          /* �ڰ�޻������(ES)�֎~         */
    fmr22 = 0;                          /* ���荞�ݗv����ESظ��ċ֎~    */
    fmr27 = 0;                          /* �����d��ذ��Ӱ�ދ֎~       */

    /* �v���O���� */
    while( count-- ) {
        *((signed char *)w_address) = 0x40;
        *((signed char *)w_address) = *r_address;
        while( fst7 == 0 );             /* �����҂�                     */

        ret = clrStatusRegister( w_address );
        if( ret == 0 ) break;
        w_address++;
        r_address++;
    }

    *((unsigned char*)(w_address-1)) = 0xff;  /* �ް��ׯ���ǂݍ��ݏ��  */

    /* �ް��ׯ����ۯ����������֎~�r�b�g�����������֎~�ɂ��� */
    switch( block ) {
        case DF_SELECT_A:
            fmr14 = 1;
            break;
        case DF_SELECT_B:
            fmr15 = 1;
            break;
        case DF_SELECT_C:
            fmr16 = 1;
            break;
        case DF_SELECT_D:
            fmr17 = 1;
            break;
        default:
            break;
    }
    fmr01 = 0;                          /* CPU�����������[�h����        */

    asm( "FSET I" );                    /* �S�̂̊��荞�݋���           */

    return ret;
}

/************************************************************************/
/* ���W���[���� clrStatusRegister                                       */
/* �����T�v     �N���A�X�e�[�^�X���W�X�^                                */
/* ����         �A�h���X                                                */
/* �߂�l       1:�G���[�Ȃ� 0:�G���[����                               */
/************************************************************************/
int clrStatusRegister( unsigned int c_address )
{
    volatile int ret = 1;

    if( fst5 == 1 || fst4 == 1 ) {
        *((char*)c_address) = 0x50;
        ret = 0;
    }

    return ret;
}

/************************************************************************/
/* ���W���[���� checkBlockAddress                                       */
/* �����T�v     �u���b�N�A�h���X�̃`�F�b�N                              */
/* ����         �����������Ƃ��Ă���A�h���X                            */
/* �߂�l       -1: �G���[ 0-3:�u���b�NNo                               */
/************************************************************************/
int checkBlockAddress( unsigned int chk_address )
{
    volatile int ret = -1;

    /* �u���b�N�`�F�b�N */
    if( chk_address >= 0x3000 && chk_address <= 0x33ff ) {
        ret = DF_SELECT_A;
    } else if( chk_address >= 0x3400 && chk_address <= 0x37ff ) {
        ret = DF_SELECT_B;
    } else if( chk_address >= 0x3800 && chk_address <= 0x3bff ) {
        ret = DF_SELECT_C;
    } else if( chk_address >= 0x3c00 && chk_address <= 0x3fff ) {
        ret = DF_SELECT_D;
    }

    return ret;
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2010.04.01 Ver.1.00 �쐬
2011.03.18 Ver.1.02 writeDataFlash�֐��́A�C���[�Y���Ȃ��悤�ɏC��
2011.04.01 Ver.1.03 �ϐ���volatile�̒ǉ�
*/
