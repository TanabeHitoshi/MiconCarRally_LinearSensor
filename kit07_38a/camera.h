#ifndef	CAMERA_H
#define	CAMERA_H

/* LED Light	  ＮＪＷ４６１６	*/
#define	Light_ON		p0_addr.bit.b3 = 1
#define	Light_OFF		p0_addr.bit.b3 = 0

/* カメラのオフセット	*/
#define		OFFSET				9

/* PIDの設定 */
#define		StrightPID		0
#define		CurvePID		1
#define		SprintPID		2
#define		Sprint_2PID		3
/* public 変数	*/
extern	int		camera_Offset;							/* カメラの中心位置のオフセット	*/
extern	int		White;									/* 白色の個数								*/
extern	int		camera_Mode;							/* 0=通常 1=左マスク 2=右マスク 3=両脇無視	*/
extern	unsigned int 	Wide;							/* ラインの幅 */
extern	int		Center;									/* ラインの中心 */
extern	long		center_Weight;							/* ラインの重心 */
extern	unsigned long pre_cycle;				/* １処理の時間	*/
extern	int				pid_angle;

/* publicプロトタイプ宣言                     */
void	camera_Caputure(void);					/* カメラからのデータ取得					*/
void	set_PID(int s);							/* PIDの設定 */
void 	expose( void );							/* 露光時間調整 		*/
void	raw_view(void);							/* RAWデータの表示		*/
void	bi_view(void);							/* ２値化データの表示	*/

#endif