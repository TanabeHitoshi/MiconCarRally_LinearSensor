#ifndef	ISCHECK_H
#define	ISCHECK_H

#define white 0.4		/* ���F��臒l */

int check( int start, int stop);	/* ���C�����o���� */
int check_RightOutLine( void );		/* �E�A�E�g���C�����o���� */
int check_LeftOutLine( void );		/* ���A�E�g���C�����o���� */
int check_crossline( void );		/* �N���X���C�����o���� */
int check_rightline( void );		/* �E�n�[�t���C�����o���� */
int check_leftline( void );			/* ���n�[�t���C�����o����  */
int check_black( void );			/* �S�����o���� */
int check_center( void );			/* ���S���o���� */

#endif