/*======================================*/
/* �C���N���[�h                         */
/*======================================*/
#include <stdio.h>
#include "lcd_lib.h"                    /* LCD�\���p�ǉ�                */
#include "switch_lib.h"                 /* �X�C�b�`�ǉ�                 */
#include "data_flash_lib.h"             /* �f�[�^�t���b�V�����C�u����   */

/* DataFlash�֘A */

#define DF_ADDR_START   0x3000          /* �������݊J�n�A�h���X         */
#define DF_ADDR_END     0x33ff          /* �������ݏI���A�h���X         */
#define DF_PARA_SIZE    64              /* DataFlash�p�����[�^��        baisuudefuyasu*/

#define DF_CHECK        0x00            /* DataFlash�`�F�b�N            */

extern void handle( int angle );


extern signed char     data_buff[ DF_PARA_SIZE ];
extern unsigned long   cnt_lcd;                /* LCD�����Ŏg�p                */
extern int             pattern;                /* �p�^�[���ԍ�                 */
extern int             servo_center;           /* �T�[�{�Z���^�l               */
extern int sensor8;

/* LCD�֘A */
int             lcd_pattern = 2;

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
            data_buff[DF_PWM]       = 100;
            data_buff[DF_CRANK_PWM] = 40;
			data_buff[DF_crank_motor1] = 0;
			data_buff[DF_crank_motor2] = 0;
			data_buff[DF_laneL_PWM] = 0;
			data_buff[DF_laneR_PWM] = 0;
			data_buff[DF_lane_motorL] = 0;
			data_buff[DF_lane_motorR] = 0;
			data_buff[DF_lane_motorS] = 0;
			data_buff[DF_crank_handlepwm] = 0;
			i = 3000;
            data_buff[DF_STOP1] 	= i >> 8;
            data_buff[DF_STOP2] 	= i & 0xff;

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
    int i,n;

    if( pattern != 1 ) {
        if( cnt_lcd >= 250 ) {
            cnt_lcd = 0;
            lcdPosition( 0, 0 );
                     /* 0123456789abcbef 1�s16���� */
            lcdPrintf( "pattern = %3d   ", pattern );
                     /* 01234567..89abcde.f 1�s16���� */
            lcdPrintf( "sensor=%02x bar=%d ",
                        sensor8, startbar_get() );
        }
        return;
    }

    /* �X�C�b�`4�@�ݒ�l�ۑ� */
    if( getSwFlag(SW_4) ) {
        // �p�����[�^�ۑ�
        writeDataFlashParameter();
    }

    /* �X�C�b�`3�@���j���[�{�P */
    if( getSwFlag(SW_3) ) {
        lcd_pattern++;
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );
        if( lcd_pattern == 16 ) lcd_pattern = 1;
    }

    /* �X�C�b�`2�@���j���[�|�P */
    if( getSwFlag(SW_2) ) {
        lcd_pattern--;
		lcdPosition( 0, 0 );
		lcdPrintf( "                ", i );
		lcdPrintf( "                ", i );

        if( lcd_pattern == 0 ) lcd_pattern = 11;
    }

    /* LCD�A�X�C�b�`���� */
    switch( lcd_pattern ) {
    case 1:
        /* �T�[�{�Z���^�l���� */
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
        lcdPrintf( "sensor=%02x bar=%d ",
                    sensor8, startbar_get() );
        break;

    case 2:
        /* PWM�l���� */
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
        lcdPrintf( "02 Max SPEED         %03d        ", i );
                 /* 01234567..89abcde.f 1�s16���� */
        break;

    case 3:
        /* ���l Kp */
        i = data_buff[DF_KP];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KP] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "03 Kp    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1�s16���� */
        break;	

    case 4:
        /* ���l Kp */
        i = data_buff[DF_KI];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KI] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "04 Ki    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1�s16���� */
        break;	
		
	case 5:
        /* ���l Kp */
        i = data_buff[DF_KD];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 100 ) i = 100;
        }
        if( getSwFlag(SW_0) ) {
            i--;
            if( i < -100 ) i = -100;
        }
        data_buff[DF_KD] = i;
		
		if(i > 0 ){
			n = i  % 10;
			i /= 10;		
		}else{
			n = -(i  % 10);
			i /= 10;
		}						
			
        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789..bcdef0123456789..bcdef 1�s16���� */
        lcdPrintf( "05 Kd    %d.%d                   ", i,n );
                 /* 01234567..89abcde.f 1�s16���� */
        break;	
		
    case 6:
        /* �^�C�}�[�l���� */
		i = ((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2];
        if( getSwFlag(SW_1) ) {
            i+=100;
            if( i > 9000 ) i = 9000;
        }
        if( getSwFlag(SW_0) ) {
            i-=100;
            if( i < 0 ) i = 0;
        }
        data_buff[DF_STOP1] = i >> 8;
        data_buff[DF_STOP2] = i & 0xff;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "06  stop time       %04d mS", i );
        break;
	case 7://�N�����N�����[�^�[���x
			i = data_buff[DF_crank_motorS];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_crank_motorS] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "07 Crank Speed      %03d", i );
        break;	
	case 8:
		i = data_buff[DF_crank_handlepwm];
        if( getSwFlag(SW_1) ) {
            i++;
            if( i > 35 ) i = 35;
        }
        if( getSwFlag(SW_0) ) {
           	i--;
           	if( i < 0 ) i = 0;
       	}
        	data_buff[DF_crank_handlepwm] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "08 Crank handle     %03d", i );
        break;
	case 9://�N�����N�����[�^�[�@OUT
			i = data_buff[DF_crank_motor1];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_crank_motor1] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "09 Crank motor  OUT   %03d", i );
        break;
	case 10://�N�����N�����[�^�[�@IN
			i = data_buff[DF_crank_motor2];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < -100 ) i = -100;
        	}
        	data_buff[DF_crank_motor2] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "10 Crank motor  IN   %03d", i );
        break;

	case 11:
			i = data_buff[DF_lane_motorS];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorS] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "11 LaneChange   Speed    %3d", i );
        break;
	case 12:
			i = data_buff[DF_laneL_PWM];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 30 ) i = 30;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_laneL_PWM] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "12 LaneChange   L handle   %03d", i );
        break;
	case 13:
			i = data_buff[DF_laneR_PWM];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 30 ) i = 30;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_laneR_PWM] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "13 LaneChange   R handle   %03d", i );
        break;
	case 14:
			i = data_buff[DF_lane_motorL];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorL] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "14 LaneChange   Motor OUT %03d", i );
        break;
	case 15:
			i = data_buff[DF_lane_motorR];
        	if( getSwFlag(SW_1) ) {
            	i++;
            	if( i > 100 ) i = 100;
        	}
        	if( getSwFlag(SW_0) ) {
            	i--;
            	if( i < 0 ) i = 0;
        	}
        	data_buff[DF_lane_motorR] = i;

        /* LCD���� */
        lcdPosition( 0, 0 );
                 /* 0123456789abcdef0123456789abcdef 1�s16���� */
        lcdPrintf( "15 LaneChange   Motor IN  %03d", i );
        break;

	}
}