/*************************************************************************** 
 *	Module Name:	General_Funcs
 *
 *	Abstract:	һЩͨ�ú���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	
 ***************************************************************************/
#ifndef __GENERAL_FUNCS_H__
#define __GENERAL_FUNCS_H__

//-- λ���붨�� --
#define BIT0		0X0001
#define BIT1		0X0002
#define BIT2		0x0004
#define BIT3		0x0008
#define BIT4		0x0010
#define BIT5		0x0020
#define BIT6		0x0040
#define BIT7		0x0080
#define BIT8		0x0100
#define BIT9		0x0200
#define BIT10		0x0400
#define BIT11		0x0800
#define BIT12		0x1000
#define BIT13		0x2000
#define BIT14		0x4000
#define BIT15		0x8000

//ȡ��8λ
#define LOBYTE(w)	((BYTE)(w))
//ȡ��8λ
#define HIBYTE(w)	((BYTE)(((WORD)(w) >> 8) & 0xFF))
//Make A Word
#define MAKEWORD(low, high)		((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

//ȷ�� X �ķ�Χ
#define	IS_RANGE(x, min, max)	((x)>=(min) && (x)<=(max))
//�Ƚ� X,Y
#define BMAX(x,y) 	(((x) > (y)) ? (x) : (y))
#define BMIN(x,y) 	(((x) < (y)) ? (x) :(y))

//ѹ����λת��ϵ��
#define mmHgTokPa		7.5	// mmHg/ϵ��
#define mmHgTocmH2O	1.33	// mmHg*ϵ��
#define kPaTocmH2O		10.0	// kPa*ϵ��

//mmHg -> �ٷֱȣ�����Ũ�ȣ�
#define mmHgToPer	7.55     	//mmHg/ϵ��

/*
	����ת��
	C->F   :   C*1.8+32
	F->C  :   (F-32)/1.8
*/

//����ʱ�亯��CalcTime()�ı���
#define TIME_AFTER		0	//����֮���ʱ��
#define TIME_BEFORE	1	//����֮ǰ��ʱ��

//���߶ε�����(��β)
#define WIDELINE_DOT	0	//���߶�
#define WIDELINE_RECT	1	//�����߶�
#define WIDELINE_CYCLE	2	//Բ���߶�
//���߶ε�����
#define REALWIDELINE	0	//ʵ��	

#endif	//__GENERAL_FUNCS_H__
