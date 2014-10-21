/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     �t���EmicroSD��� �X�C�b�`���䃉�C�u����                    */
/* �o�[�W����   Ver.1.00                                                    */
/* Date         2011.04.01                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/****************************************************************************/

/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include    "sfr_r838a.h"               /* R8C/38A SFR�̒�`�t�@�C��    */
#include    "switch_lib.h"              /* �X�C�b�`���䃉�C�u����       */

/*======================================*/
/* �萔�ݒ�                             */
/*======================================*/
#define     SWITCH_BIT              5   /* �X�C�b�`�̐�                 */
#define     SWITCH_INTERVAL         10  /* �X�C�b�`�����C���^�[�o�� 1ms */
#define     SWITCH_ON_COUNT         3   /* ON�Ɣ��f����܂ł̉�       */
                                        /* SWITCH_INTERVAL *            */
                                        /*         SWITCH_ON_COUNT [ms] */
#define     SWITCH_REPEAT_INTERVAL  50  /* ON���Ă�����߰Ă���܂ł̎���*/
                                        /* SWITCH_INTERVAL *            */
                                        /*  SWITCH_REPEAT_INTERVAL [ms] */
#define     SWITCH_REPEAT_COUNT     5   /* ���s�[�g�Ԋu                 */
                                        /* SWITCH_INTERVAL *            */
                                        /*     SWITCH_REPEAT_COUNT [ms] */

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/
static volatile int     iSwitchIntervalCount;
                                        /* ���񂲂Ƃɏ������邩�̶���   */

/* 1bit���Ƃ̃L�[��� */
struct {
    int     mode;                       /* �X�C�b�`�������[�hNo�p       */
    int     time;                       /* �M���������Ă��鎞��         */
    int     flag;                       /* "0"��"1"��"1"�ɂȂ�          */
    int     count;                      /* �J�E���g                     */
} static volatile sw_data[ SWITCH_BIT ];

/************************************************************************/
/* ���W���[���� initSwitch                                              */
/* �����T�v     �X�C�b�`�֘A�ϐ�������                                  */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void initSwitch( void )
{
    volatile int i;

    iSwitchIntervalCount = 0;
    pur1                |= 0x0c;        /* 3:P5_4�`P5_7 �v���A�b�vON    */
                                        /* 2:P5_0�`P5_3 �v���A�b�vON    */
    /* �X�C�b�` */
    for( i=0; i<SWITCH_BIT; i++ ) {
        sw_data[i].mode     = 0;
        sw_data[i].time     = 0;
        sw_data[i].flag     = 0;
        sw_data[i].count    = 0;
    }
}

/************************************************************************/
/* ���W���[���� getSwNow                                                */
/* �����T�v     �X�C�b�`���ݏ�Ԏ擾                                    */
/* ����         �Ȃ�                                                    */
/* �߂�l       unsigned char �X�C�b�`�l                                */
/************************************************************************/
unsigned char getSwNow( void )
{
/*
�X�C�b�`�̓��̓|�[�g��ς���ꍇ�́A���̊֐���ύX���܂��B
bit0����l�߂Ĕz�u�����悤�ɂ��܂��B
���݂̐ݒ� bit  7    6    5    4    3    2    1    0
                --  --  --  P5_7 P5_3 P5_2 P5_1 P5_0
*/
    volatile unsigned char sw;

    pd5 &= 0xf0;                        /* LCD�ƌ��p�̒[�q����͂ɐݒ�  */

    sw  = (~p5 & 0x80) >> 3;            /* P5_7�ǂݍ���                 */
    sw |= (~p5 & 0x0f);                 /* P5_3�`P5_0�ǂݍ���           */

    pd5 |= 0x0f;                        /* LCD�ƌ��p�̒[�q���o�͂ɐݒ�  */

    return sw;
}

/************************************************************************/
/* ���W���[���� getSwFlag                                               */
/* �����T�v     �X�C�b�`�t���O��Ԏ擾                                  */
/* ����         �}�X�N�l                                                */
/* �߂�l       unsigned char �X�C�b�`�l                                */
/************************************************************************/
unsigned char getSwFlag( unsigned char flag )
{
    volatile unsigned char ret;

    switch( flag ) {
    case SW_4:
        ret = sw_data[4].flag;
        sw_data[4].flag = 0;
        break;
    case SW_3:
        ret = sw_data[3].flag;
        sw_data[3].flag = 0;
        break;
    case SW_2:
        ret = sw_data[2].flag;
        sw_data[2].flag = 0;
        break;
    case SW_1:
        ret = sw_data[1].flag;
        sw_data[1].flag = 0;
        break;
    case SW_0:
        ret = sw_data[0].flag;
        sw_data[0].flag = 0;
        break;
    }
    return ret;
}

/************************************************************************/
/* ���W���[���� switchProcess                                           */
/* �����T�v     �X�C�b�`����                                            */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void switchProcess( void )
{
    volatile int            i;
    volatile unsigned char  sw;

    /* �����Ԋu�̃`�F�b�N */
    iSwitchIntervalCount++;
    if( iSwitchIntervalCount < SWITCH_INTERVAL ) return;

    iSwitchIntervalCount = 0;
    sw = getSwNow();

    /* �X�C�b�`�̃r�b�g���ɏ������� */
    for( i=0; i<SWITCH_BIT; i++ ) {

        switch( sw_data[i].mode ) {
        case 0:
            /* ON�̔��� */
            if( sw & 0x01 ) {
                sw_data[i].time++;
                if( sw_data[i].time >= SWITCH_ON_COUNT ) {
                    sw_data[i].mode = 1;
                    sw_data[i].count++;
                    sw_data[i].flag = 1;
                    sw_data[i].time = 0;
                }
            } else {
                sw_data[i].time = 0;
            }
            break;

        case 1:
            /* �L�[���s�[�g�̔��� */
            if( sw & 0x01 ) {
                sw_data[i].time++;
                if( sw_data[i].time >= SWITCH_REPEAT_INTERVAL ) {
                    sw_data[i].mode = 2;
                    sw_data[i].count++;
                    sw_data[i].flag = 1;
                    sw_data[i].time = 0;
                }
            } else {
                sw_data[i].mode = 0;
                sw_data[i].time = 0;
            }
            break;

        case 2:
            /* �L�[���s�[�g���A���Ԋu�Ńt���O�n�m�ɂ��锻�� */
            if( sw & 0x01 ) {
                sw_data[i].time++;
                if( sw_data[i].time >= SWITCH_REPEAT_COUNT ) {
                    sw_data[i].count++;
                    sw_data[i].flag = 1;
                    sw_data[i].time = 0;
                }
            } else {
                sw_data[i].mode = 0;
                sw_data[i].time = 0;
            }
            break;
        }

        sw >>= 1;
    }
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2011.04.01 Ver.1.00 �쐬
*/
