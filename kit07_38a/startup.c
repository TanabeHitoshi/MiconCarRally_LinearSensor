/****************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                     */
/* ̧�ٓ��e     �X�^�[�g�A�b�v�v���O����(C�����)                           */
/* �o�[�W����   Ver.1.22                                                    */
/* Date         2012.03.08                                                  */
/* Copyright    �W���p���}�C�R���J�[�����[���s�ψ���                        */
/*              ���l�T�X�}�C�R���J�[�����[������                            */
/*              �����C���^�[���f�B�b�N�X�������                            */
/****************************************************************************/
/*======================================*/
/* �v���g�^�C�v�錾(���[�J��)           */
/*======================================*/
void main( void );

/*======================================*/
/* �X�^�b�N�T�C�Y�̐ݒ�                 */
/*======================================*/
#pragma STACKSIZE       0x200
#pragma ISTACKSIZE      0x100

/*======================================*/
/* CPU���W�X�^�̐錾                    */
/*======================================*/
#pragma CREG    _flg_   flg
#pragma CREG    _isp_   isp
#pragma CREG    _sp_    sp
#pragma CREG    _sb_    sb
#pragma CREG    _fb_    fb
#pragma CREG    _intbh_ intbh
#pragma CREG    _intbl_ intbl

unsigned int _flg_;
unsigned int _sb_;
unsigned int _fb_;
unsigned int *_sp_;
unsigned int *_isp_;
unsigned int *_intbh_;
unsigned int *_intbl_;

/*======================================*/
/* SB�̒l���R���p�C���ɐݒ�             */
/*======================================*/
    _asm("       .glb    __SB__\n"
         "__SB__ .equ    0400H   ");

/*======================================*/
/* �I�v�V�����@�\�I�����W�X�^�̐ݒ�     */
/*======================================*/
    _asm("  .ofsreg 0BFH ");            /* OFS = 0xbf (��ܰ��ؾ�Ďg�p)  */

/*======================================*/
/* ID�R�[�h�̐ݒ�                       */
/*======================================*/
    _asm("  .id ""\"#FFFFFFFFFFFFFF\" ");

/*======================================*/
/* RAM������������֐��̒�`            */
/*======================================*/
#define scopy(X,Y,Z)    _asm("  .initsct "X","Y","Z"\n"\
            "   .initsct "X"I,rom"Y",noalign\n"\
            "   mov.w   #(topof "X"I)&0ffffH,A0\n"\
            "   mov.b   #(topof "X"I)>>16,R1H\n"\
            "   mov.w   #(topof "X")&0ffffH,A1\n"\
            "   mov.w   #sizeof "X",R3\n"\
            "   smovf.b")

#define sclear(X,Y,Z)   _asm("  .initsct "X","Y","Z"\n"\
            "   mov.b   #00H,R0L\n"\
            "   mov.w   #(topof "X") ,A1\n"\
            "   mov.w   #sizeof "X",R3\n"\
            "   sstr.b")

/*======================================*/
/* �Z�N�V�����̐擪�A�h���X�̌^��`     */
/*======================================*/
extern unsigned int _stack_top;
extern unsigned int _istack_top;
extern unsigned int _vector_top;

/*======================================*/
/* �q�[�v�̈�̐ݒ�                     */
/*======================================*/
#pragma sectaddress heap_NE,DATA
#define __HEAPSIZE__    0x100

unsigned char heap_area[ __HEAPSIZE__ ]; /* �q�[�v�̈�m��              */
extern unsigned char  _far *_mnext;     /* ���Ɏg�p�ł�����؂̐擪���ڽ */
extern unsigned long        _msize;     /* �c��̃o�C�g��               */

/*======================================*/
/* �Œ芄�荞�݃x�N�^�A�h���X�̐ݒ�     */
/*======================================*/
#pragma sectaddress fvector,ROMDATA 0xffd8

    _asm("  .addr 0FFFFFFH");           /* �\��                         */
    _asm("  .byte 0FFH");               /* OFS2                         */
#pragma interrupt/v _dummy_int          /* ����`���ߊ��荞�݃x�N�^     */
#pragma interrupt/v _dummy_int          /* �I�[�o�t���[���荞�݃x�N�^   */
#pragma interrupt/v _dummy_int          /* BRK���ߊ��荞�݃x�N�^        */
#pragma interrupt/v _dummy_int          /* �A�h���X��v���荞�݃x�N�^   */
#pragma interrupt/v _dummy_int          /* �ݸ�ٽï�ߊ��荞�݃x�N�^     */
#pragma interrupt/v _dummy_int          /* �����ޯ����ςȂǂ̊��荞���޸�*/
#pragma interrupt/v _dummy_int          /* ���ڽ��ڲ����荞�݃x�N�^     */
#pragma interrupt/v _dummy_int          /* �\��                         */
#pragma interrupt/v start               /* ���Z�b�g�x�N�^               */

/*======================================*/
/* �Œ芄�荞�݃v���O����               */
/*======================================*/
#pragma sectaddress interrupt,CODE
#pragma interrupt _dummy_int()
void _dummy_int(void)
{
    /* �_�~�[�֐� */
}

/*======================================*/
/* �ϊ��荞�݃x�N�^�̐ݒ�             */
/*======================================*/
#pragma sectaddress vector,ROMDATA

    /* �����ł̓Z�N�V�������̐ݒ�̂ݍs�� */

/*======================================*/
/* RAM�̏�����                          */
/*======================================*/
#pragma sectaddress program,CODE
void initsct(void)
{
    sclear("bss_SE","data","align");
    sclear("bss_SO","data","noalign");
    sclear("bss_NE","data","align");
    sclear("bss_NO","data","noalign");

    scopy("data_SE","data","align");
    scopy("data_SO","data","noalign");
    scopy("data_NE","data","align");
    scopy("data_NO","data","noalign");
}

/*======================================*/
/* �X�^�[�g�A�b�v�v���O����             */
/*======================================*/
#pragma entry start
void start( void )
{
    _isp_   = &_istack_top;             /* ISP�Ɋ��荞�ݽ����̱��ڽ�ݒ� */
    _flg_   = 0x0080;                   /* FLG��U="1"                   */
    _sp_    = &_stack_top;              /* USP��հ�޽����̱��ڽ�ݒ�     */
    _sb_    = 0x0400U;                  /* SB���΃A�h���b�V���O�̐ݒ�   */
    _intbh_ = (unsigned int*)0x00;      /* INTBH = vector(���)�ɐݒ�   */
    _intbl_ = &_vector_top;             /* INTBL = vector(����)�ɐݒ�   */
    initsct();                          /* RAM�̏�����                  */
    _mnext = &heap_area[0];             /* �q�[�v�̈�ϐ��̐ݒ�         */
    _msize = (unsigned long)__HEAPSIZE__;
    _fb_    = 0U;                       /* FB = 0                       */

    main();                             /* main�֐����s                 */

    while(1);
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/

/*
�����o��

2010.04.19 Ver.1.20 �e�����
2011.04.01 Ver.1.21 main�֐��̃v���g�^�C�v�錾�ǉ�
2012.04.01 Ver.1.22 STACKSIZE�̃T�C�Y��0x100��0x200�ɕύX
*/
