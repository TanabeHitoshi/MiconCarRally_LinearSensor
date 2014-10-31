#ifndef	ISCHECK_H
#define	ISCHECK_H

#define white 0.4		/* 白色の閾値 */

int check( int start, int stop);	/* ライン検出処理 */
int check_RightOutLine( void );		/* 右アウトライン検出処理 */
int check_LeftOutLine( void );		/* 左アウトライン検出処理 */
int check_crossline( void );		/* クロスライン検出処理 */
int check_rightline( void );		/* 右ハーフライン検出処理 */
int check_leftline( void );			/* 左ハーフライン検出処理  */
int check_black( void );			/* 全黒検出処理 */
int check_center( void );			/* 中心検出処理 */

#endif