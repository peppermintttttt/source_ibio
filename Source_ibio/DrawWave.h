/*************************************************************************** 
 *	Module Name:	DrawWave
 *
 *	Abstract:	��ͼ������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-18 09:23:48
 ***************************************************************************/
#ifndef __DRAWWAVE_H__
#define __DRAWWAVE_H__


//���岨����ֵ
#define WAVEMAX_ECG	4095
#define WAVEMAX_SPO2	255	
#define WAVEMAX_RESP	255	
#define WAVEMAX_CO2	99
#define WAVEMAX_IBP		400

//����
//#define BASELINE_ECG	127
//#define BASELINE_SPO2	50
//#define BASELINE_RESP	127
//#define BASELINE_CO2	(WAVEMAX_CO2/2)

//������屳����ɫ
#define	PANEL_BKMODE		SS_BLACKRECT
#define	PANEL_BKCOLOR	COLOR_black		

//��������ַ���ɫ
#define	PANEL_TEXTCOLOR	COLOR_lightgray 

//��嶨��


//���֮��ļ��
#define PANEL_INTER		3	

//������ͼ, BigChar, MultiECG, ��ͼ��ʾ��������
#define VW_NORMAL_LEFT	5
#define VW_NORMAL_TOP		35
#define VW_NORMAL_RIGHT	545
#define VW_NORMAL_BOTTOM	560	//565
//����չ����(IBP)�������
#define VW_NORMAL_BOTTOM_EXT	440	

//��������ͼ, ��ͼ��ʾ��������
#define VW_SHORTTREND_LEFT	145
#define VW_SHORTTREND_TOP		35
#define VW_SHORTTREND_RIGHT	540
#define VW_SHORTTREND_BOTTOM	565
//����չ����(IBP)�������
#define VW_SHORTTREND_BOTTOM_EXT	440	

//Big Chaacter ��ͼ
#define VW_BIGCHAR_LEFT		5
#define VW_BIGCHAR_TOP		35
#define VW_BIGCHAR_RIGHT		545
#define VW_BIGCHAR_BOTTOM	450

//NIBP List ��ͼ��ʾ��������
#define VW_NIBPLIST_LEFT			5
#define VW_NIBPLIST_TOP			35
#define VW_NIBPLIST_RIGHT		540
#define VW_NIBPLIST_BOTTOM		370
#define VW_NIBPLIST_BOTTOM_EXT	250


//OxyCRG ��ͼ��ʾ��������
#define VW_OXYCRG_LEFT	5
#define VW_OXYCRG_TOP		35
#define VW_OXYCRG_RIGHT	540
#define VW_OXYCRG_BOTTOM	275		//450
#define VW_OXYCRG_BOTTOM_EXT	200


//ɨ�����ȣ�ɨ���ٶȲ�ͬ�����Ҳ��ͬ��
#define FILLWIDTH_6		10	
#define FILLWIDTH_12		10	
#define FILLWIDTH_25		5
#define FILLWIDTH_50		5
#define FILLWIDTH		8


//�����������
typedef struct {
	int left;
	int top;
	int right;
	int bottom;
	int height;
	int width;
}WP_RECT, *PWP_RECT;

//�������,ϵͳʹ��
typedef struct {
	BYTE bID;			//�������ͱ�ʶ
	HWND hWnd;			//�����
	HDC hdc;				//��ͼ����
	WP_RECT wpRc;		//����ͼ����
	BOOL bReset; 		//��λ��ͼ(���¿�ʼ��ͼ)
	BOOL bCascade;		//�Ƿ���
	BYTE bWaveCount;	//���ƵĲ�������
	float fXAxis;			//X������ϵ��������ɨ���ٶ�����	
	float fYAxis;			//Y������ϵ��, ���ݲ�����������	
	int iXStep;			//X��Ĳ���ֵ��X�ϵ�����ϵ�̶�����Ϊ�������ٶȣ�ͨ���ı䲽��ֵ���ı�ɨ���ٶ�
	int iYMax;			//Y�����ֵ
	int iYMin;			//Y����Сֵ
	//���β������Ӧ��Ļ���صı���, ���ݲ�����������(ECG����ʱʹ��)
	float fSampleToPixel;	
	//���������ƶ��ķ��ȣ����ݲ�����������(ECG����ʱʹ��)
	int iMove;
}WAVE_PANEL, *PWAVE_PANEL;

#endif //__DRAWWAVE_H__
