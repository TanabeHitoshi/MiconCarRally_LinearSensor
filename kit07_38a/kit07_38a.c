/************************************************************************/
/* �Ώۃ}�C�R�� R8C/38A                                                 */
/* ̧�ٓ��e     ���C���Z���T�[��p����B								*/
/* �o�[�W����   Ver.1.02                                                */
/* Date         2014.10.22                                              */
/* Copyright    ���{������H�ȍ��Z									*/
/************************************************************************/
#include <stdio.h>
#include "sfr_r838a.h"                  /* R8C/38A SFR�̒�`�t�@�C��    */
#include "printf_lib.h"                 /* printf�g�p���C�u����         */
#include "microsd_lib.h"                /* microSD���䃉�C�u����        */
#include "lcd_lib.h"                    /* LCD�\���p�ǉ�                */
#include "switch_lib.h"                 /* �X�C�b�`�ǉ�                 */
#include "data_flash_lib.h"             /* �f�[�^�t���b�V�����C�u����   */
#include "isline.h"
#include "isCheck.h"
#include "drive.h"
#include "ini.h"

/*======================================*/
/* �v���g�^�C�v�錾                     */
/*======================================*/
void init( void );
void timer( unsigned long timer_set );
unsigned char dipsw_get( void );
unsigned char pushsw_get( void );
void led_out( unsigned char led );
int diff( int pwm );
void readDataFlashParameter( void );
void writeDataFlashParameter( void );
int lcdProcess( void );

/*======================================*/
/* �O���[�o���ϐ��̐錾                 */
/*======================================*/
const char *C_DATE = __DATE__;          /* �R���p�C���������t           */
const char *C_TIME = __TIME__;          /* �R���p�C����������           */

/* DataFlash�֌W */
signed char     data_buff[ DF_PARA_SIZE ];

const int revolution_difference[] = {   /* �p�x������ցA�O�։�]���v�Z */
    100, 99, 97, 96, 95,
    93, 92, 91, 89, 88,
    87, 86, 84, 83, 82,
    81, 79, 78, 77, 76,
    75, 73, 72, 71, 70,
    69, 67, 66, 65, 64,
    62, 61, 60, 59, 58,
    56, 55, 54, 52, 51,
    50, 48, 47, 46, 44,
    43 };

/************************************************************************/
/* ���C���v���O����                                                     */
/************************************************************************/
void main( void )
{
    int     i, ret, sensor ,t;
    char    fileName[ 8+1+3+1 ];        /* ���O�{'.'�{�g���q�{'\0'      */
    unsigned char b;                    /* �I�ǉ��E�ύX�I               */
	int CL_number,CL_BR[5],CL_SP[5],CL_BK_TM[5];
	int	CL_BRAKE_TIME; 
	unsigned int j;
	CL_number = 0;

    /* �}�C�R���@�\�̏����� */
    init();                             /* ������                       */
    init_uart0_printf( SPEED_9600 );    /* UART0��printf�֘A�̏�����    */
    setMicroSDLedPort( &p6, &pd6, 0 );  /* microSD ���j�^LED�ݒ�        */
    asm(" fset I ");                    /* �S�̂̊��荞�݋���           */
    initLcd();                          /* LCD������                    */
	initSwitch();                       /* �X�C�b�`������               */

    readDataFlashParameter();           /* DataFlash�p�����[�^�ǂݍ���  */
    servo_center  = (unsigned char)data_buff[DF_SERVO1] * 0x100;
    servo_center |= (unsigned char)data_buff[DF_SERVO2];

    /* microSD������ */
    ret = initMicroSD();
    if( ret != 0x00 ) msdError = 1;

    /* FAT32�Ń}�E���g */
    if( msdError == 0 ) {
        ret = mountMicroSD_FAT32();
        if( ret != 0x00 ) msdError = 2;
    }

    if( msdError != 0 ) {
        /* microSD�����ɃG���[�������3�b�ԁALED�̓_�����@��ς��� */
        cnt1 = 0;
        while( cnt1 < 3000 ) {
            if( cnt1 % 200 < 100 ) {
                led_out( 0x3 );
            } else {
                led_out( 0x0 );
            }
        }
    }
    /* �}�C�R���J�[�̏�ԏ����� */
	printf("Hello!!\n");
    handle( 0 );
    motor( 0, 0 );
	pattern = 1000;		/* ���j�^�[��� */

    while( 1 ) {
    lcdProcess();		/* LCD�\���A�p�����[�^�ݒ菈��*/
	get_ad();			/* �Z���T�[�l�̓ǂݍ��� */
	LPF();				/* ���[�p�X�t�B���^�[ */
	Normanaization();	/* ���K�� */
	binarization();		/* �Q�l�� */
	potition();			/* ���C���ʒu�̌v�Z */
//	printf("pattern = %d\n",pattern);
    switch( pattern ) {

    /*****************************************************************
    �p�^�[���ɂ���
     0�F�X�C�b�`���͑҂�
	 1: �I�[�g�L�����u���[�V����
     5�F�X�^�[�g�o�[���J�������`�F�b�N
    11�F�ʏ�g���[�X
    21�F�P�{�ڂ̃N���X���C�����o���̏���
    22�F�Q�{�ڂ�ǂݔ�΂�
    23�F�N���X���C����̃g���[�X�A�N�����N���o
    31�F���N�����N�N���A�����@���肷��܂ŏ����҂�
    32�F���N�����N�N���A�����@�Ȃ��I���̃`�F�b�N
    41�F�E�N�����N�N���A�����@���肷��܂ŏ����҂�
    42�F�E�N�����N�N���A�����@�Ȃ��I���̃`�F�b�N
    51�F�P�{�ڂ̉E�n�[�t���C�����o���̏���
    52�F�Q�{�ڂ�ǂݔ�΂�
    53�F�E�n�[�t���C����̃g���[�X
    54�F�E���[���`�F���W�I���̃`�F�b�N
    61�F�P�{�ڂ̍��n�[�t���C�����o���̏���
    62�F�Q�{�ڂ�ǂݔ�΂�
    63�F���n�[�t���C����̃g���[�X
    64�F�����[���`�F���W�I���̃`�F�b�N
    *****************************************************************/

    case 0:
		lcdPosition( 0, 0 );lcdPrintf( "init");
        led_out( 0x0 );
        // �p�����[�^�ۑ�
        writeDataFlashParameter();
        pattern = 1;
        cnt1 = 0;
        if( msdError == 0 ) {
           	/* microSD�̋󂫗̈悩��ǂݍ��� */
        	i = readMicroSDNumber();
            if( i == -1 ) {
            	msdError = 3;
            }
         }
         if( msdError == 0 ) {
             /* microSD�̋󂫗̈�֏�������  */
             i++;
             if( i >= 10000 ) i = 1;
             ret = writeMicroSDNumber( i );
             if( ret == -1 ) {
                  msdError = 4;
             } else {
                /* �t�@�C�����ϊ� */
                  sprintf( fileName, "log_%04d.csv", i );
             }
         }
         if( msdError == 0 ) {
             /* �t�@�C���̃^�C���X�^���v�Z�b�g */
             setDateStamp( getCompileYear( C_DATE ),
             getCompileMonth( C_DATE ),  getCompileDay( C_DATE ) );
             setTimeStamp( getCompileHour( C_TIME ),
             getCompilerMinute( C_TIME), getCompilerSecond( C_TIME ) );

            /* �������݃t�@�C�����쐬 */
            // �������݂���������[ms] : x = 10[ms] : 64�o�C�g
            // 60000ms�Ȃ�Ax = 60000 * 64 / 10 = 384000
            // ���ʂ�512�̔{���ɂȂ�悤�ɌJ��グ����B
             ret = writeFile( fileName, 384000 );
             if( ret != 0x00 ) msdError = 11;
         }
         pattern = 1;
         cnt1 = 0;
 		lcdPosition( 0, 0 );lcdPrintf( "AutoCalibration");
		break;
	case 1:/* �I�[�g�L�����u���[�V���� */
		if( pushsw_get()){
			timer(1000);
			pattern = 2;
		}
		break;
	case 2: /* �I�[�g�L�����u���[�V���� */
		if( pushsw_get() ) {
			AutoCalibration();
/*			printf("black = ");
			for(i = 0; i < 16; i++){
				printf("%4d ",ad_black[i]);
			}
			printf("\n");
			printf("white = ");
			for(i = 0; i < 16; i++){
				printf("%4d ",ad_white[i]);
			}
			printf("\n");
*/
		}else{
			if(dipsw_get() == 0){
				pattern = 4;
			}else{
				pattern = 2000;
			}
		}			
        if( cnt1 < 100 ) {              /* LED�_�ŏ���                  */
            led_out( 0x1 );
        } else if( cnt1 < 200 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
        break;
	case 4:
        /* �X�^�[�g�o�[�܂ōs�� */
        if( startbar_get()) {
			timer(1000);
			pattern = 5;
		}
        led_out( 0x0 );
		break;
	case 5:
		if( pushsw_get()){
			timer(1000);
			pattern = 6;
		}
        led_out( 0x3 );
		break;
    case 6:
        /* �X�^�[�g�o�[���J�������`�F�b�N */
        if( !startbar_get() || pushsw_get() ) {
            /* �X�^�[�g�I�I */
            led_out( 0x0 );
            pattern = 11;
            if( msdError == 0 ) msdFlag = 1;    /* �f�[�^�L�^�J�n       */
            cnt1 = 0;
			timer( 100 );
			lEncoderTotal	= 0;
			lEncoderCrank	= 0;
			lEncoderCrank2	= 0;			
			lEncoderHarf	= 0;
            break;
        }
        if( cnt1 < 50 ) {               /* LED�_�ŏ���                  */
            led_out( 0x1 );
        } else if( cnt1 < 100 ) {
            led_out( 0x2 );
        } else {
            cnt1 = 0;
        }
 		stop_timer = 0;
		//mem_ad = 0;
        break;

    case 11:
        /* �ʏ�g���[�X */
		t = ((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2];
		if( stop_timer >= ((unsigned long)(t*10)) ){
			msdFlag = 0;
			motor( 0, 0 );
		}
		if (stop_timer>300) {//�ʏ�g���[�X�サ�΂炭�N���X�A�n�[�t�̓`�F�b�N���Ȃ�
	       	if( check_crossline() ) {       /* �N���X���C���`�F�b�N         */
				lEncoderCrank = lEncoderTotal;
            	pattern = 21;
            	break;
        	}
        	if( check_rightline() ) {       /* �E�n�[�t���C���`�F�b�N       */
            	lEncoderTotal = lEncoderHarf;
				pattern = 51;
            	break;
        	}
        	if( check_leftline() ) {        /* ���n�[�t���C���`�F�b�N       */
            	lEncoderTotal = lEncoderHarf;
            	pattern = 61;
            	break;
        	}
		}
		handle( PID());
		if( pos > 0)	motor(100-pos*10.0,100-pos*3.0);
		else			motor(100+pos*3.0,100+pos*10.0);

        break;

    case 21:
        /* �P�{�ڂ̃N���X���C�����o���̏��� */
        led_out( 0x3 );
        handle( 0 );
		if(10 < iEncoder){//�G���R�[�_�ɂ�鑬�x����
//			motor((data_buff[DF_crank_motorS] - iEncoder),(data_buff[DF_crank_motorS] - iEncoder));
			motor(-70,-70);
		}else{
			motor( data_buff[DF_crank_motorS] ,data_buff[DF_crank_motorS] );
		}
		pattern = 22;
        break;

    case 22:
        /* �Q�{�ڂ�ǂݔ�΂� */
//		motor( data_buff[DF_crank_motorS] ,data_buff[DF_crank_motorS] );
		if( lEncoderTotal-lEncoderCrank >= 100 ) {   /* ��10cm���������H */				
			pattern = 23;
		}
        break;

	case 23:
        /* �N���X���C����̃g���[�X�A�N�����N���o */
        if( check_leftline() ) {   /* �I�ǉ��E�ύX�I */
            /* ���N�����N�Ɣ��f�����N�����N�N���A������ */
            led_out( 0x0 );
            handle( data_buff[DF_crank_handlepwm] );
			motor( data_buff[DF_crank_motor2] ,data_buff[DF_crank_motor1] );
            pattern = 31;
            cnt1 = 0;
            break;
        }
        if( check_rightline() ) {   /* �I�ǉ��E�ύX�I           */
            /* �E�N�����N�Ɣ��f���E�N�����N�N���A������ */
            led_out( 0x3 );
            handle( -data_buff[DF_crank_handlepwm] );
			motor( data_buff[DF_crank_motor1] ,data_buff[DF_crank_motor2] );
            pattern = 41;
            cnt1 = 0;
            break;
        }
		handle( PID());
		motor( data_buff[DF_crank_motorS] ,data_buff[DF_crank_motorS] );
        break;

    case 31:
        /* ���N�����N�N���A�����@���肷��܂ŏ����҂� */
//        if( cnt1 > 70 && check_black() ) {
        if( check_black() ) {
            pattern = 34;
            cnt1 = 0;
        }
        break;
	
	case 32:
        /* ���N�����N�N���A�����@�O���̔����ƌ��Ԉ��Ȃ��悤�ɂ��� */
        /* �I�ǉ��E�ύX�I �������� */
        if( check_leftline() ) {
            pattern = 34;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
		
    case 33:
        /* ���N�����N�N���A�����@�Ȃ��I���̃`�F�b�N */
/*        if( sensor_inp(MASK2_0) ) {
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �������� */
/*        if( sensor_inp(MASK3_3) == 0x07 ) {
            pattern = 34;
            break;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;

    case 34:
        /* ���N�����N�N���A�����@�O���̔����ƌ��Ԉ��Ȃ��悤�ɂ��� */
        /* �I�ǉ��E�ύX�I �������� */
        if( check_center() ) {
			lEncoderCrank = lEncoderTotal;
            pattern = 35;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
	case 35:
		if( lEncoderTotal - lEncoderCrank > 300 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		motor(50,50);
		break;

    case 41:
        /* �E�N�����N�N���A�����@���肷��܂ŏ����҂� */
//        if( cnt1 > 50 && check_black()) {
        if( check_black()) {
            pattern = 44;
            cnt1 = 0;
        }
        break;
    
	case 42:
        /* �E�N�����N�N���A�����@�O���̔����ƌ��Ԉ��Ȃ��悤�ɂ��� */
        /* �I�ǉ��E�ύX�I �������� */
/*        b = sensor_inp(0x03);
        if( b ) {
            pattern = 43;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
		
    case 43:
        /* �E�N�����N�N���A�����@�Ȃ��I���̃`�F�b�N */
/*        if( sensor_inp(MASK0_2) ) {
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �������� */
/*        if( sensor_inp(MASK3_3) == 0xe0 ) {
            pattern = 44;
            break;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;

    case 44:
        /* �E�N�����N�N���A�����@�O���̔����ƌ��Ԉ��Ȃ��悤�ɂ��� */
        /* �I�ǉ��E�ύX�I �������� */
        if( check_center() ) {
			lEncoderCrank = lEncoderTotal;
            pattern = 45;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
	case 45:
		if( lEncoderTotal - lEncoderCrank > 500 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		motor(50,50);
		break;

    case 51:
        /* �P�{�ڂ̉E�n�[�t���C�����o���̏��� */
        led_out( 0x2 );
//		if(data_buff[DF_crank_motorS] < iEncoder){//�G���R�[�_�ɂ�鑬�x����
//			motor((data_buff[DF_crank_motorS] - iEncoder),(data_buff[DF_crank_motorS] - iEncoder));
			motor(-100,-100);
//		}else{
//			motor( data_buff[DF_crank_motorS] ,data_buff[DF_crank_motorS] );
//		}
        pattern = 53;
        cnt1 = 0;
        break;
	case 52:
		/* �ԈႢ�`�F�b�N */
		if( lEncoderTotal - lEncoderHarf > 10 ){
            pattern = 53;
            cnt1 = 0;
        }
        if( !check_rightline() ) {
            pattern = 11;
            break;
        }
		break;
    case 53:
        /* �Q�{�ڂ�ǂݔ�΂� */
		if( lEncoderTotal - lEncoderHarf > 140 ){
            pattern = 54;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �������� */
        if( check_crossline() ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;

    case 54:
        /* �E�n�[�t���C����̃g���[�X�A���[���`�F���W */
        if( check_black() ) {
           	handle( -data_buff[DF_laneR_PWM] );
			motor(data_buff[DF_lane_motorL],data_buff[DF_lane_motorR]);
            pattern = 55;
            cnt1 = 0;
            break;
        }
		handle( PID() + 0);
        motor( data_buff[DF_lane_motorS] ,data_buff[DF_lane_motorS] );
        break;

    case 55:
        /* �E���[���`�F���W�I���̃`�F�b�N */
        /* �I�ǉ��E�ύX�I �������� */
        if( check_center() ) {
            led_out( 0x0 );
			lEncoderTotal = lEncoderHarf;
            pattern = 56;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
	case 56:
		if( lEncoderTotal - lEncoderHarf > 500 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		motor(50,50);
		break;

    case 61:
        /* �P�{�ڂ̍��n�[�t���C�����o���̏��� */
        led_out( 0x1 );
        handle( 0 );
//		if(data_buff[DF_crank_motorS] < iEncoder){//�G���R�[�_�ɂ�鑬�x����
//			motor((data_buff[DF_crank_motorS] - iEncoder),(data_buff[DF_crank_motorS] - iEncoder));
			motor(-100,-100);
//		}else{
//			motor( data_buff[DF_crank_motorS] ,data_buff[DF_crank_motorS] );
//		}
        pattern = 63;
        cnt1 = 0;
        break;
	case 62:
		/* �ԈႢ�`�F�b�N */
		if( lEncoderTotal - lEncoderHarf > 10 ){
            pattern = 63;
            cnt1 = 0;
        }
        if( check_rightline() ) {
            pattern = 11;
            break;
        }
		break;
    case 63:
        /* �Q�{�ڂ�ǂݔ�΂� */
		if( lEncoderTotal - lEncoderHarf > 150 ){
            pattern = 64;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �������� */
        if( check_crossline() ) {
            pattern = 21;
			lEncoderCrank = lEncoderTotal;
            break;
        }
		/* �I�ǉ��E�ύX�I �����܂� */
        break;

    case 64:
        /* ���n�[�t���C����̃g���[�X�A���[���`�F���W */
        if( check_black() ) {
			handle( data_buff[DF_laneL_PWM] );
			motor(data_buff[DF_lane_motorR],data_buff[DF_lane_motorL]);
            pattern = 65;
            cnt1 = 0;
            break;
        }
		handle( PID() + 10);
        motor( data_buff[DF_lane_motorS] ,data_buff[DF_lane_motorS] );
        break;

    case 65:
        /* �����[���`�F���W�I���̃`�F�b�N */
        /* �I�ǉ��E�ύX�I �������� */
        if( check_center() ) {
            led_out( 0x0 );
            pattern = 66;
			lEncoderTotal = lEncoderHarf;
            cnt1 = 0;
        }
        /* �I�ǉ��E�ύX�I �����܂� */
        break;
		
	case 66:
		if( lEncoderTotal - lEncoderHarf > 150 ){
            led_out( 0x0 );
            pattern = 11;
            cnt1 = 0;
		}
		handle( PID());
		motor(50,50);
		break;

    case 101:
        /* microSD�̒�~���� */
        /* �E�ւ����ۂ̎�����~������́A�K�����̏������s���Ă������� */
        handle( 0 );
        motor( 0, 0 );
        msdFlag = 0;
        pattern = 102;
        break;

    case 102:
        /* �Ō�̃f�[�^���������܂��܂ő҂�*/
        if( microSDProcessEnd() == 0 ) {
            pattern = 103;
        }
        break;

    case 103:
        /* �������ݏI�� */
        led_out( 0x3 );
        break;
		
	case 500:
		/* �������̓ǂݏo�� */
		/*if( pushsw_get() ) {
			for(j = 0; j < 5000; j++){
				printf("%d\n",mem[j]);
			}
		}*/
		break;

	case 1000:
		/* ���j�^��� */
		printf("MiconCar parameter \n");
		printf("   Kp %d  Ki %d  Kd  %d \n",data_buff[DF_KP],data_buff[DF_KI],data_buff[DF_KD]);
		printf("   servo_center = %d\n",servo_center);
		printf("   Stop Timer = %d0\n",((unsigned char)data_buff[DF_STOP1]*0x100)|(unsigned char)data_buff[DF_STOP2]);
		printf("   Max Speed = %d\n",data_buff[DF_PWM]);
		printf("\n");

		printf("Crank parameter\n");	
		printf("   Speed  = %3d\n",data_buff[DF_crank_motorS]);
		printf("   handle = %d\n",data_buff[DF_crank_handlepwm]);
		printf("   MOTOR  OUT = %3d   IN = %3d \n",data_buff[DF_crank_motor1],data_buff[DF_crank_motor2]);
		printf("\n");

		printf("LaneChange parameter\n");	
		printf("   Speed  = %3d\n",data_buff[DF_lane_motorS]);
		printf("   R handle = %d  L handle = %d\n",data_buff[DF_laneR_PWM],data_buff[DF_laneL_PWM]);
		printf("   MOTOR  OUT = %3d   IN = %3d \n",data_buff[DF_lane_motorL],data_buff[DF_lane_motorR]);
		printf("\n");
	
		pattern = 0;
		
	case 2000:
		get_ad();
		printf("raw: ");
		for(i = 0; i < 16; i++){
			printf("%4d ",ImageData[i]);
		}
		printf("\n");

		printf("LPF: ");
		for(i = 0; i < 16; i++){
			printf("%4d ",val_ImageData[i]);
		}
		printf("\n");

		printf("Nor: ");
		for(i = 0; i < 16; i++){
			float_printf(sval[i],2);
			printf(" ");
		}
		printf("\n");

		printf("bi:  ");
		for(i = 0; i < 16; i++){
			printf("%4d ",bi_sensor[i]);
		}
		printf("\n");

		printf("pos = ");
		float_printf(pos,3);
		printf(" PID = %d  ",PID());
		printf("lEncoderTotal = %ld\n",lEncoderTotal);
		printf("startbar_get = %d  allBlack = %d\n\n",startbar_get(),allBlack());
		timer(1500);
		break;
		
    default:
        /* �ǂ�ł��Ȃ��ꍇ�͑ҋ@��Ԃɖ߂� */
        pattern = 0;
        break;
    }
    }
}
/************************************************************************/
/* �f�B�b�v�X�C�b�`�l�ǂݍ���                                           */
/* �߂�l �X�C�b�`�l 0�`15                                              */
/************************************************************************/
unsigned char dipsw_get( void )
{
    unsigned char sw;

    sw = p1 & 0x0f;                     /* P1_3�`P1_0�ǂݍ���           */

    return  sw;
}

/************************************************************************/
/* �v�b�V���X�C�b�`�l�ǂݍ���                                           */
/* �߂�l �X�C�b�`�l ON:1 OFF:0                                         */
/************************************************************************/
unsigned char pushsw_get( void )
{
    unsigned char sw;

    sw  = ~p2;                          /* �X�C�b�`�̂���|�[�g�ǂݍ��� */
    sw &= 0x01;

    return  sw;
}

/************************************************************************/
/* LED����                                                              */
/* �����@�X�C�b�`�l LED0:bit0 LED1:bit1  "0":���� "1":�_��              */
/* ��)0x3��LED1:ON LED0:ON  0x2��LED1:ON LED0:OFF                       */
/************************************************************************/
void led_out( unsigned char led )
{
    unsigned char data;

    led = ~led;
    led <<= 6;
    data = p2 & 0x3f;
    p2 = data | led;
}

/************************************************************************/
/* �O�ւ�PWM����A���ւ�PWM������o���@�n���h���p�x�͌��݂̒l���g�p     */
/* �����@ �O��PWM                                                       */
/* �߂�l ����PWM                                                       */
/************************************************************************/
int diff( int pwm )
{
    int ret;

    if( pwm >= 0 ) {
        /* PWM�l�����̐��Ȃ� */
        if( angle_buff < 0 ) {
            angle_buff = -angle_buff;
        }
        ret = revolution_difference[angle_buff] * pwm / 100;
    } else {
        /* PWM�l�����̐��Ȃ� */
        ret = pwm;                      /* ���̂܂ܕԂ�             */
    }
    return ret;
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/