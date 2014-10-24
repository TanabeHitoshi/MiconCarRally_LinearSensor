#ifndef	ISCHECK_H
#define	ISCHECK_H

#define white 0.5		/* 白色の閾値 */
 
int check_crossline( void );	/* クロスライン検出処理 */
int check_rightline( void );	/* 右ハーフライン検出処理 */
int check_leftline( void );		/* 左ハーフライン検出処理  */
int check_black( void );		/* 全黒検出処理 */
int check_center( void );		/* 中心検出処理 */

#endif