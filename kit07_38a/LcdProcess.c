/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include <stdio.h>
#include "lcd_lib.h"                    /* LCD�\���p�ǉ�                */
#include "switch_lib.h"                 /* �X�C�b�`�ǉ�                 */
#include "data_flash_lib.h"             /* �f�[�^�t���b�V�����C�u����   */
#include "drive.h"
#include "ini.h"

/* DataFlash�֘A */

#define DF_ADDR_START   0x3000          /* �������݊J�n�A�h���X         */
#define DF_ADDR_END     0x33ff          /* �������ݏI���A�h���X         */
#define DF_PARA_SIZE    64              /* DataFlash�p�����[�^��        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlash�`�F�b�N            */

signed char     data_buff[ DF_PARA_SIZE ];

/* LCD�֘A */
int		lcd_pattern = 0;
int		lcd_count = 0;
int		lcd_item[3][30]={	0,2,10,11,12,13,14,15,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,3,18,19,20,21,22,23,24,25,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,-1,0};		/* �ݒ荀�� 			*/
/************************************************************************/
/* DataFlash�̃p�����[�^�ǂݍ���                                        */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void readDataFlashParameter( void )
{
    int             i;
    unsigned int    st = DF_ADDR_END + 1 - DF_PARA_SIZE;
    signed char     c;

    while( 1 ) {
        // �ǂݍ��ޔԒn��T��
        readDataFlash( st, &c, 1 );
        if( c == 0x11 ) {
            readDataFlash( st, data_buff, DF_PARA_SIZE );
            break;
        }
        st -= DF_PARA_SIZE;

        if( st < DF_ADDR_START ) {
            // �Y�������@���߂Ďg�p
            for( i=0; i<DF_PARA_SIZE; i++ ) data_buff[ i ] = 0;
            data_buff[DF_CHECK]     = 0x11;
            data_buff[DF_SERVO1]    = SERVO_CENTER >> 8;
            data_buff[DF_SERVO2]    = SERVO_CENTER & 0xff;
            data_buff[DF_STOP] 	= 30;

            blockEraseDataFlash( DF_ADDR_START );
            writeDataFlash( DF_ADDR_START, data_buff, DF_PARA_SIZE );
            break;
        }
    }
}
/************************************************************************/
/* DataFlash�փp�����[�^��������                                        */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void writeDataFlashParameter( void )
{
    unsigned int    st = DF_ADDR_START;
    signed char     c;

    while( 1 ) {
        // �������ޔԒn��T��
        readDataFlash( st, &c, 1 );
        if( c == -1 ) {
            writeDataFlash( st, data_buff, DF_PARA_SIZE );
            break;
        }

        st += DF_PARA_SIZE;

        if( st > DF_ADDR_END ) {
            // ���ׂĎg�p������A�C���[�Y���Đ擪�ɏ�������
            blockEraseDataFlash( DF_ADDR_START );
            writeDataFlash( DF_ADDR_START, data_buff, DF_PARA_SIZE );
            break;
        }
    }
}
/************************************************************************/
/* LCD�ƃX�C�b�`���g�����p�����[�^�Z�b�g����                            */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
int lcdProcess( void )
{
    int i,n,m;
	int r;
		
   if( pattern > 10 ) {
        if( cnt_lcd >= 250 ) {
            cnt_lcd = 0;
            lcdPosition( 0, 0 );
                     /* 0123456789abcbef 1�s16���� */
            lcdPrintf( "pattern = %3d   ", pattern );
                 /* 01234567..89abcde.f 1�s16���� */
	        lcdPrintf( "                ");
        }
        return;
    }
   
	switch(data_buff[DF_RACE]){
		case 0:/* Trace */
			r = 0;
		break;
		case 1:/* Sprint */
		case 2:
			r = 1;
		break;
		case 3:/* Moniter */
			r = 2;
		break;
	}	   	
	
    /* �X�C�b�`4�@�ݒ�l�ۑ� */
    if( getSwFlag(SW_4) ) {
        // �p�����[�^�ۑ�
        writeDataFlashParameter();
    }

    /* �X�C�b�`3�@���j���[�{�P */
    if( getSwFlag(SW_3) ) {
        lcd_count++;
		lcd_pattern = lcd_item[r][lcd_count];
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );
        if( lcd_pattern == -1 ){
			lcd_count = 0;
			lcd_pattern = lcd_item[r][lcd_count];
		}
    }

    /* �X�C�b�`2�@���j���[�|�P */
    if( getSwFlag(SW_2) ) {
        lcd_count--;
		lcd_pattern = lcd_item[r][lcd_count];
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );
        if( lcd_count < 0 ){
			lcd_count = 30;
			while(lcd_item[r][lcd_count] != -1)lcd_count--;
			lcd_count--;
			lcd_pattern = lcd_item[r][lcd_count];
		}
    }

    /* LCD�A�X�C�b�`���� */
    switch( lcd_pattern ) {
	case 0:/* ���[�X�I�� */
        i = data_buff[DF_RACE];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 3 ) i = 0;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 3;
        }
        data_buff[DF_RACE] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                           /* 0123456789ab..f 1�s16���� */
		if(i == 0)lcdPrintf( "  Trace       " );
							   		 /* 0123456789abcdef0123456789abcdef 1�s16���� */
		else if(i == 1)lcdPrintf( "  Sprint Left" );
					  /* 0123456789abcdef0123456789abcdef 1�s16���� */
		else if(i == 2) lcdPrintf( "  Sprint Right" );
		else  lcdPrintf( "  Moniter       " );
                 /* 01234567..89abcde.f 1�s16���� */
        lcdPrintf( "  ");		
	break;
    case 1:/* �T�[�{�Z���^�l���� */
        if( getSwFlag(SW_1) ) {
            servo_center++;
            if( servo_center > 10000 ) servo_center = 10000;
        }
        if( getSwFlag(SW_0) ) {
            servo_center--;
            if( servo_center < 1000 ) servo_center = 1000;
        }
        data_buff[DF_SERVO1] = servo_center >> 8;
        data_buff[DF_SERVO2] = servo_center & 0xff;
        handle( 0 );

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789ab..f 1�s16���� */
        lcdPrintf( "01 servo = %05d", servo_center );
                 /* 01234567..89abcde.f 1�s16���� */
        lcdPrintf( "                ");
        break;
    case 2:
        /* ���x���� */
        i = data_buff[DF_PWM];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_PWM] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "02 Trace SPEED       %03d        ", i );
                 /* 01234567..89abcde.f 1�s16���� */
        break;
    case 3:
        /* �X�v�����g���x���� */
        i = data_buff[DF_PWM_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_PWM_S] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "03 Sprint SPEED      %03d        ", i );
                 /* 01234567..89abcde.f 1�s16���� */
        break;
    case 4:
        /* �T�[�{���l KSp */
        i = data_buff[DF_KSP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "04 Servo           KSp = %d.%d ", i,n );
        break;	

    case 5:
        /* �T�[�{�ϕ��l KSi */
        i = data_buff[DF_KSI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "05 Servo           KSi = %d.%d ", i,n );
        break;			
	case 6:
        /* �T�[�{�����l KSd */
        i = data_buff[DF_KSD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KSD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "06 Servo           KSd = %d.%d ", i,n );
        break;	
    case 7:/* �}�V�����l KMp */
        i = data_buff[DF_KMP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "07 Machine         KMp = %d.%d ", i,n );
        break;	
    case 8:/* �T�[�{�ϕ��l KMi */
        i = data_buff[DF_KMI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "08 Machine         KMi = %d.%d ", i,n );
        break;			
	case 9:/* �}�V�������l KMd */
        i = data_buff[DF_KMD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KMD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "09 Machine         KMd = %d.%d ", i,n );
        break;	
    case 10:/* �g���[�X���l�i�X�g���[�g�j Kp */
        i = data_buff[DF_KP_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "10 Trace Stright  Kp_S = %d.%d ", i,n );
        break;	
    case 11:/* �g���[�X�ϕ��l�i�X�g���[�g�j Ki */
        i = data_buff[DF_KI_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "11 Trace Stright  Ki_S = %d.%d ", i,n );
        break;			
	case 12:/* �g���[�X�����l�i�X�g���[�g�j KMd */
        i = data_buff[DF_KD_S];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_S] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "12 Trace Stright  Kd_S = %d.%d ", i,n );
        break;	
    case 13:/* �g���[�X���l�i�J�[�u�j Kp */
        i = data_buff[DF_KP_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "13 Trace Curve    Kp_C = %d.%d ", i,n );
        break;	
    case 14:/* �g���[�X�ϕ��l�i�J�[�u�j Ki */
        i = data_buff[DF_KI_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "14 Trace Curve    Ki_C = %d.%d ", i,n );
        break;			
	case 15:/* �g���[�X�����l�i�J�[�u�j Kd */
        i = data_buff[DF_KD_C];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_C] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "15 Trace Curve    Kd_C = %d.%d ", i,n );
        break;	

	case 16:/* �J�����X�^�[�g�ʒu */
			i = data_buff[DF_LineStart];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 127 ) i = 127;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
       	data_buff[DF_LineStart] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "16 LineStart       %03d", i );
        break;
	case 17:/* �J�����X�g�b�v�ʒu */
			i = data_buff[DF_LineStop];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 127 ) i = 127;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
       	data_buff[DF_LineStop] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "17 LineStop       %03d", i );
        break;
	case 18:
        /* �X�v�����g���s���� */
		i = data_buff[DF_DISTANCE];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 20 ) i = 20;
        }
        data_buff[DF_DISTANCE] = i;
		
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "18  Distance        %2d00 mm", i );
        break;
    case 19:/* Sprint���l�i�X�g���[�g�j Kp */
        i = data_buff[DF_KP_SP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_SP] = i;
		
		if(i > 0 ){
			n = (i  % 100) %10;
			m = (i / 10) % 10;
			i /= 100;		
		}			
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "19 Sprint       Kp_SP = %d.%d%d", i,m,n );
        break;	
    case 20:/* Sprint�ϕ��l Ki */
        i = data_buff[DF_KI_SP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_SP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "20 Sprint        Ki_SP = %d.%d ", i,n );
        break;			
	case 21:/* Sprint�����l KMd */
        i = data_buff[DF_KD_SP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_SP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "21 Sprint        Kd_SP = %d.%d ", i,n );
        break;	
    case 22:
        /* ���x���� */
        i = data_buff[DF_PWM_SP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_PWM_SP] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "22 Sprint SPEED     %03d        ", i );
                 /* 01234567..89abcde.f 1�s16���� */
        break;
    case 23:/* �X�v�����g���l2 Kp_SP2 */
        i = data_buff[DF_KP_SP2];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KP_SP2] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "23 Sprint 2      Kp_SP2 = %d.%d", i,n );
        break;	
    case 24:/* �g���[�X�ϕ��l�i�J�[�u�j Ki */
        i = data_buff[DF_KI_SP2];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KI_SP2] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "24 Sprint 2      Ki_SP2 = %d.%d", i,n );
        break;			
	case 25:/* �g���[�X�����l�i�J�[�u�j Kd */
        i = data_buff[DF_KD_SP2];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_KD_SP2] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "25 Sprint 2      Kd_SP2 = %d.%d", i,n );
        break;	

	case 26:
        /* �^�C�}�[�l���� */
		i = data_buff[DF_STOP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 30 ) i = 30;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_STOP] = i;
		
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "26  stop time       %2d s", i );
        break;
	case 27:
        /* ���i�␳ */
		i = data_buff[DF_OFFSET_ST];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 30 ) i = 30;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -30 ) i = -30;
        }
        data_buff[DF_OFFSET_ST] = i;
		
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "27  Offset         %2d", i );
        break;

	}
}
/************************************************************************/
/* �V���A���ʐM��p�����p�����[�^�Z�b�g����                            */
/* ����         �Ȃ�                                                    */
/* �߂�l       �Ȃ�                                                    */
/************************************************************************/
void sciProcess( void )
{
	int s,t;
	int i,n;
	
	/* ���j�^��� */
	printf("\n ---- Parameter change MEMU ---- \n");
	printf("1:Machine parameter \n");
	printf("   servo_center = %d\n",servo_center);
	printf("   Stop Timer = %dS\n",data_buff[DF_STOP]);
	printf("   Servo  parameter KSp %d.%d   KSi %d.%d   KSd %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10,data_buff[DF_KSI]/10,data_buff[DF_KSI]%10,data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
	printf("   Macine parameter KMp %d.%d   KMi %d.%d   KMd %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10,data_buff[DF_KMI]/10,data_buff[DF_KMI]%10,data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
	printf("   Motor Offset %d\n",data_buff[DF_OFFSET_ST]);
//	printf("   Log time %dms\n",data_buff[DF_log_time]);
//	printf("   Log start Number %d\n",data_buff[DF_log_potion]);
//	printf("\n");

	printf("2:Camera parameter\n");	
	printf("   Camera  LineStart %d   LineStop %d\n",data_buff[DF_LineStart],data_buff[DF_LineStop]);
//	printf("\n");

	printf("3:Trace parameter\n");	
	printf("   Trace  Stright parameter Kp %d.%d   Ki %d.%d   Kd %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10,data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10,data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
	printf("   Trace  Curve parameter   Kp %d.%d   Ki %d.%d   Kd %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10,data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10,data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
	printf("   Trace Speed = %d\n",data_buff[DF_PWM]);
//	printf("\n");

	printf("4:Sprint parameter\n");	
	printf("   Sprint MAX Speed = %d\n",data_buff[DF_PWM_SP]);
	printf("   Sprint Speed = %d\n",data_buff[DF_PWM_S]);
	printf("   Distance = %d00 mm\n",data_buff[DF_DISTANCE]);
	printf("   Sprint PID  KSp %d.%d%d   KSi %d.%d   KSd %d.%d\n",data_buff[DF_KI_SP]/10,(data_buff[DF_KP_SP]/10)%10,(data_buff[DF_KP_SP]%100)%10,data_buff[DF_KI_SP]/10,data_buff[DF_KI_SP]%10,data_buff[DF_KD_SP]/10,data_buff[DF_KD_SP]%10);
	printf("   Sprint 2     Kp %d.%d     Ki  %d.%d   Kd  %d.%d \n",data_buff[DF_KP_SP2]/10,data_buff[DF_KP_SP2]%10,data_buff[DF_KI_SP2]/10,data_buff[DF_KI_SP2]%10,data_buff[DF_KD_SP2]/10,data_buff[DF_KD_SP2]%10);
	printf("\n");
	printf("change parameter number? >> ");
	scanf("%d",&s);
	switch(s){
		case 1:
			printf(" MiconCar parameter \n");
			printf("   1: servo_center = %d\n",servo_center);
			printf("   2: Stop Timer = %dS\n",data_buff[DF_STOP]);
			printf("   3: Servo parameter KSp %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10);
			printf("   4: Servo parameter KSi %d.%d\n",data_buff[DF_KSI]/10,data_buff[DF_KSI]%10);
			printf("   5: Servo parameter KSd %d.%d\n",data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
			printf("   6: Machine parameter KMp %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10);
			printf("   7: Machine parameter KMi %d.%d\n",data_buff[DF_KMI]/10,data_buff[DF_KMI]%10);
			printf("   8: Machine parameter KMd %d.%d\n",data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
			printf("   9: Motor Offset %d\n",data_buff[DF_OFFSET_ST]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
				   servo_center  = (unsigned char)data_buff[DF_SERVO1] * 0x100;
 				   servo_center |= (unsigned char)data_buff[DF_SERVO2];
					printf("1: servo_center = %d\n",servo_center);
					printf("parameter value? >> ");
					scanf("%d",&s);
					if(s > 10000) s = 10000;
					if(s < 1000) s = 1000;
 					data_buff[DF_SERVO1] = s >> 8;
        			data_buff[DF_SERVO2] = s & 0xff;
					writeDataFlashParameter();
				break;

				case 2:
					printf("2: Stop Timer = %dS\n",data_buff[DF_STOP]);
					printf("parameter value? (0 <-> 30)>> ");
					scanf("%d",&s);
					if(s > 30) s = 30;
					if(s < 0) s = 0;
			        data_buff[DF_STOP] = s;
					writeDataFlashParameter();
				break;
				case 3:
					printf("3: Servo KSp %d.%d\n",data_buff[DF_KSP]/10,data_buff[DF_KSP]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSP] = s;
					writeDataFlashParameter();
				break;
				case 4:
					printf("4: Servo KSi %d.%d\n",data_buff[DF_KSI]/10,data_buff[DF_KSI]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSI] = s;
					writeDataFlashParameter();
				break;
				case 5:
					printf("5: Servo KSd %d.%d\n",data_buff[DF_KSD]/10,data_buff[DF_KSD]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KSD] = s;
					writeDataFlashParameter();
				break;
				case 6:
					printf("6: Machine KMp %d.%d\n",data_buff[DF_KMP]/10,data_buff[DF_KMP]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMP] = s;
					writeDataFlashParameter();
				break;
				case 7:
					printf("7: Machine KMi %d.%d\n",data_buff[DF_KMI]/10,data_buff[DF_KMI]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMI] = s;
					writeDataFlashParameter();
				break;
				case 8:
					printf("8: Machine KMd %d.%d\n",data_buff[DF_KMD]/10,data_buff[DF_KMD]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KMD] = s;
					writeDataFlashParameter();
				break;
				case 9:
					printf("9: Motor Offset %d\n",data_buff[DF_OFFSET_ST]);
					printf("parameter value? (-30 <-> 30) >> ");
					scanf("%d",&s);
					if(s > 30) s = 30;
					if(s < -30) s = -30;
			        data_buff[DF_OFFSET_ST] = s;
					writeDataFlashParameter();
				break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 2:
			printf(" Camera parameter\n");	
			printf("   1: Camera LineStart %d\n",data_buff[DF_LineStart]);
			printf("   2: Camera LineStop  %d\n",data_buff[DF_LineStop]);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
					printf("1: Camera LineStart %d\n",data_buff[DF_LineStart]);
					printf("parameter value? (0 <-> 127) >> ");
					scanf("%d",&s);
					if(s > 127) s = 127;
					if(s < 0) s = 0;
			        data_buff[DF_LineStart] = s;
					writeDataFlashParameter();
				break;
				case 2:
					printf("2: Camera LineStop %d\n",data_buff[DF_LineStop]);
					printf("parameter value? (0 <-> 127) >> ");
					scanf("%d",&s);
					if(s > 127) s = 127;
					if(s < 0) s = 0;
			        data_buff[DF_LineStop] = s;
					writeDataFlashParameter();
				break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 3:
			printf("3:Trace parameter\n");	
			printf("   1: Trace Speed = %d\n",data_buff[DF_PWM]);
			printf("   2: Trace Stright parameter Kp %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10);
			printf("   3: Trace Stright parameter Ki %d.%d\n",data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10);
			printf("   4: Trace Stright parameter Kd %d.%d\n",data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
			printf("   5: Trace Curve parameter Kp %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10);
			printf("   6: Trace Curve parameter Ki %d.%d\n",data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10);
			printf("   7: Trace Curve parameter Kd %d.%d\n",data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
					printf("1: Trace Speed = %d\n",data_buff[DF_PWM]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_PWM] = s;
					writeDataFlashParameter();
				break;
				case 2:
					printf("2: Camera Stright Kp %d.%d\n",data_buff[DF_KP_S]/10,data_buff[DF_KP_S]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_S] = s;
					writeDataFlashParameter();
				break;
				case 3:
					printf("3: Camera Stright Ki %d.%d\n",data_buff[DF_KI_S]/10,data_buff[DF_KI_S]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_S] = s;
					writeDataFlashParameter();
				break;
				case 4:
					printf("4: Camera Stright Kd %d.%d\n",data_buff[DF_KD_S]/10,data_buff[DF_KD_S]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_S] = s;
					writeDataFlashParameter();
				break;
				case 5:
					printf("5: Camera Curve Kp %d.%d\n",data_buff[DF_KP_C]/10,data_buff[DF_KP_C]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_C] = s;
					writeDataFlashParameter();
				break;
				case 6:
					printf("6: Camera Curve Ki %d.%d\n",data_buff[DF_KI_C]/10,data_buff[DF_KI_C]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_C] = s;
					writeDataFlashParameter();
				break;
				case 7:
					printf("7: Camera Curvet Kd %d.%d\n",data_buff[DF_KD_C]/10,data_buff[DF_KD_C]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_C] = s;
					writeDataFlashParameter();
				break;
				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		case 4:
			printf("4:Sprint parameter\n");	
			printf("   1: SprintMax Speed = %d\n",data_buff[DF_PWM_SP]);
			printf("   2: Sprint Speed = %d\n",data_buff[DF_PWM_S]);
			printf("   3: Distance = %d00 mm\n",data_buff[DF_DISTANCE]);
			printf("   4: Sprint Kp %d.%d%d\n",data_buff[DF_KP_SP]/100,(data_buff[DF_KP_SP]/10)%10,(data_buff[DF_KP_SP]%100)%10);
			printf("   5: Sprint Ki %d.%d\n",data_buff[DF_KI_SP]/10,data_buff[DF_KI_SP]%10);
			printf("   6: Sprint Kd %d.%d\n",data_buff[DF_KD_SP]/10,data_buff[DF_KD_SP]%10);
			printf("   7: Sprint Kp 2 %d.%d\n",data_buff[DF_KP_SP2]/10,data_buff[DF_KP_SP2]%10);
			printf("   8: Sprint Ki 2 %d.%d\n",data_buff[DF_KI_SP2]/10,data_buff[DF_KI_SP2]%10);
			printf("   9: Sprint Kd 2 %d.%d\n",data_buff[DF_KD_SP2]/10,data_buff[DF_KD_SP2]%10);
			printf("\n");
			printf("change parameter number? >> ");
			scanf("%d",&s);
			switch(s){
				case 1:
					printf("1: Sprint MaxSpeed  = %3d\n",data_buff[DF_PWM_SP]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_PWM_SP] = s;
					writeDataFlashParameter();
				break;
				case 2:
					printf("2: Speed  = %3d\n",data_buff[DF_PWM_S]);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_PWM_S] = s;
					writeDataFlashParameter();
				break;
				case 3:
					printf("3: Distance = %d00\n",data_buff[DF_DISTANCE]);
					printf("parameter value? (20 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 20) s = 20;
			        data_buff[DF_DISTANCE] = s;
					writeDataFlashParameter();
				break;
				case 4:
					printf("4: Sprint Kp %d.%d%d\n",data_buff[DF_KP_SP]/100,(data_buff[DF_KP_SP]/10)%10,(data_buff[DF_KP_SP]%100)%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_SP] = s;
					writeDataFlashParameter();
				break;
				case 5:
					printf("5: Sprint Ki %d.%d\n",data_buff[DF_KI_SP]/10,data_buff[DF_KI_SP]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_SP] = s;
					writeDataFlashParameter();
				break;
				case 6:
					printf("6: Sprint Kd %d.%d\n",data_buff[DF_KD_SP]/10,data_buff[DF_KD_SP]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_SP] = s;
					writeDataFlashParameter();
				break;
				case 7:
					printf("7: Sprint Kp 2 %d.%d\n",data_buff[DF_KP_SP2]/10,data_buff[DF_KP_SP2]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KP_SP2] = s;
					writeDataFlashParameter();
				break;
				case 8:
					printf("8: Sprint Ki 2 %d.%d\n",data_buff[DF_KI_SP2]/10,data_buff[DF_KI_SP2]%10);
					printf("parameter value? (0 <-> 100)>> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KI_SP2] = s;
					writeDataFlashParameter();
				break;
				case 9:
					printf("9: Sprint Kd 2 %d.%d\n",data_buff[DF_KD_SP2]/10,data_buff[DF_KD_SP2]%10);
					printf("parameter value? (0 <-> 100) >> ");
					scanf("%d",&s);
					if(s > 100) s = 100;
					if(s < 0) s = 0;
			        data_buff[DF_KD_SP2] = s;
					writeDataFlashParameter();
				break;

				default:
					printf("NOT !!!\n");
				break;
			}
		break;
		default:
			printf("NOT !!!\n");
		break;
	}
	printf("\n\n\n");
}

