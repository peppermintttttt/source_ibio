/*************************************************************************** 
 *	Module Name:	NewShortTrend
 *
 *	Abstract:	������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-01-23 16:24:05
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"

//��������ɫ
#define ST_FRAME_COLOR		COLOR_darkgray
#define ST_TEXT_COLOR		COLOR_darkgray
#define ST_BK_COLOR			COLOR_black

//������
#define ST_LEFT			RC_SHORTTREND_RIGHT+5
#define ST_TOP			RC_SHORTTREND_TOP
#define ST_RIGHT			RC_NORMAL_RIGHT
#define ST_BOTTOM		RC_SHORTTREND_BOTTOM


//������ͼ���� 
#define NSTREND_COUNT		5

//ID
#define NSTID_NONE		0
#define NSTID_HR			1
#define NSTID_NIBP			2
#define NSTID_SPO2			3
#define NSTID_PR			4
#define NSTID_RR			5
#define NSTID_TEMP		6
#define NSTID_CO2			7
#define NSTID_IBP1			8
#define NSTID_IBP2			9


//�����߶εĿ��
#define ST_LINE_WIDTH		1

//�����Ʋ�����ʾ�ķ�Χ
//HR
#define STREND_HR_MIN			HR_MIN
#define STREND_HR_MAX			HR_MAX
//SpO2
#define STREND_SPO2_MIN		80
#define STREND_SPO2_MAX		SPO2_MAX
//PR
#define STREND_PR_MIN			PR_MIN
#define STREND_PR_MAX			PR_MAX
//RR
#define STREND_RR_MIN			RR_MIN
#define STREND_RR_MAX			RR_MAX
//TEMP
#define STREND_TEMP_MIN		TEMP_MIN
#define STREND_TEMP_MAX		TEMP_MAX
//NIBP
#define STREND_NIBP_MIN		NIBP_MIN
#define STREND_NIBP_MAX		NIBP_MAX
//IBP
#define STREND_IBP1_MIN			IBP_MIN
#define STREND_IBP1_MAX			IBP_MAX
#define STREND_IBP2_MIN			IBP_MIN
#define STREND_IBP2_MAX			IBP_MAX
//CO2
#define STREND_CO2_MIN			CO2_MIN
#define STREND_CO2_MAX			75

//���������
typedef struct {
	BYTE bSwitch;		//
	BYTE bID;			
	RECT rcView;			//��ʾ����
	int xScale;			//xֵ�ķ�Χ(����),���ں���Ҫ��ʾָ��ʱ���ڵ�ȫ������
	int yMin;				//y����Сֵ�������������Ƶ���Сֵ
	int yMax;				//y�����ֵ�������������Ƶ����ֵ
	double fRatio_x;    		//X��Ļ�ͼ����
	double fRatio_y;		//Y��Ļ�ͼ����	
	int iColor;			//����ͼ������ɫ
}NST_PANEL, *PNST_PANEL;	

//��������ʾ����
RECT gRCShortTrend;
static NST_PANEL gSTPanels[NSTREND_COUNT];

//������X����ʾ�̶ȼ�ʱ�䷶Χ, ��λ:����
static int giST_XScale = 30;

static int giSTCount;	//����������
	
static unsigned char strInfo[10]={0};
static unsigned char strXCale[20]={0};
static unsigned char strUnit[10]={0};
	
//hdc
static HDC hdc_ST = (HDC)NULL;
//mdc
static HDC mdc_ST = (HDC)NULL;

//�����߳�
static pthread_t ptShortTrend;
static sem_t semShortTrend;	
static BOOL gbThreadHasCreated_st = FALSE;

static BOOL gbDrawSTFinished  = TRUE;

extern HWND ghWndPanel_ShortTrend;

//������������
extern CA_SHORTTREND gShortTrendDatas;   

extern  RECT gRCEcg;
extern  RECT gRCNibp;
extern  RECT gRCSpO2AndPR;
extern  RECT gRCResp;
extern  RECT gRCTemp;
extern  RECT gRCCo2;
extern  RECT gRCIbp1;
extern  RECT gRCIbp2;
extern  RECT gRCIbp2;


//���ö���������
static int InitSTAttr()
{
	int iTop = 35;
	int iInter = 106;
	int i;
	
	//����ID
	for(i = 0; i < NSTREND_COUNT; i++){
		gSTPanels[0].bID = NSTID_NONE;
	}
	gSTPanels[0].bID = NSTID_HR;
	gSTPanels[1].bID = NSTID_NIBP;
	gSTPanels[2].bID = NSTID_SPO2;
	gSTPanels[3].bID = NSTID_RR;
	gSTPanels[4].bID = NSTID_TEMP;
	giSTCount = 5;

				
	//����
// 	SetRect(&gRCShortTrend, ST_LEFT, ST_TOP, ST_RIGHT, ST_BOTTOM);
	GetClientRect(ghWndPanel_ShortTrend, &gRCShortTrend);

	//���������ID����ϵ��
	for(i = 0; i < giSTCount; i++){
		//ʱ��̶�ͳһ
		gSTPanels[i].xScale = giST_XScale;		
		switch(gSTPanels[i].bID){
			case NSTID_HR:{
				SetRect(&(gSTPanels[i].rcView), 
						  gRCShortTrend.left,
						  gRCShortTrend.top + gRCEcg.top+3,
						  gRCShortTrend.right-1,
						  gRCShortTrend.top + gRCEcg.bottom-3);
				gSTPanels[i].yMin = gCfgEcg.iMin_HR;
				gSTPanels[i].yMax = gCfgEcg.iMax_HR;
				gSTPanels[i].iColor = gCfgEcg.iColor;
			}break;
			case NSTID_NIBP:{
				SetRect(&(gSTPanels[i].rcView), 
						  gRCShortTrend.left,
						  gRCShortTrend.top + gRCNibp.top+3,
						  gRCShortTrend.right-1,
						  gRCShortTrend.top + gRCNibp.bottom-3);
				gSTPanels[i].yMin = gCfgNibp.wMin_NIBP;
				gSTPanels[i].yMax = gCfgNibp.wMax_NIBP;
				gSTPanels[i].iColor = gCfgNibp.iColor;
			}break;
			case NSTID_SPO2:{
				SetRect(&(gSTPanels[i].rcView), 
						  gRCShortTrend.left,
						  gRCShortTrend.top + gRCSpO2AndPR.top+3,
						  gRCShortTrend.right-1,
						  gRCShortTrend.top + gRCSpO2AndPR.bottom-3);
				gSTPanels[i].yMin = gCfgSpO2.bMin_SPO2;
				gSTPanels[i].yMax = gCfgSpO2.bMax_SPO2;
				gSTPanels[i].iColor = gCfgSpO2.iColor;
			}break;
			case NSTID_TEMP:{
				SetRect(&(gSTPanels[i].rcView), 
						  gRCShortTrend.left,
						  gRCShortTrend.top + gRCTemp.top+3,
						  gRCShortTrend.right-1,
						  gRCShortTrend.top + gRCTemp.bottom-3);
				gSTPanels[i].yMin = gCfgTemp.iMin_TEMP;
				gSTPanels[i].yMax = gCfgTemp.iMax_TEMP;
				gSTPanels[i].iColor = gCfgTemp.iColor;
			}break;
			case NSTID_CO2:{
				gSTPanels[i].yMin = STREND_CO2_MIN;
				gSTPanels[i].yMax = STREND_CO2_MAX;
				gSTPanels[i].iColor = gCfgCO2.iColor;
			}break;
			case NSTID_RR:{
				SetRect(&(gSTPanels[i].rcView), 
						  gRCShortTrend.left,
						  gRCShortTrend.top + gRCResp.top+3,
						  gRCShortTrend.right-1,
						  gRCShortTrend.top + gRCResp.bottom-3);
				gSTPanels[i].yMin = gCfgResp.bMin_RR;
				gSTPanels[i].yMax = gCfgResp.bMax_RR;
				gSTPanels[i].iColor = gCfgResp.iColor;
			}break;
			case NSTID_IBP1:{
				gSTPanels[i].yMin = STREND_IBP1_MIN;
				gSTPanels[i].yMax = STREND_IBP1_MAX;
				gSTPanels[i].iColor = gCfgIbp1.iColor;
			}break;
			case NSTID_IBP2:{
				gSTPanels[i].yMin = STREND_IBP2_MIN;
				gSTPanels[i].yMax = STREND_IBP2_MAX;
				gSTPanels[i].iColor = gCfgIbp2.iColor;
			}break;
		}
		
		//����ʱ��̶ȼ���ʱ�������صı���
		gSTPanels[i].fRatio_x = (double)((double)(RECTW(gSTPanels[i].rcView))/(double)(gSTPanels[i].xScale));
		//���ݲ�������ֵ��������ֵ�����صı���
		gSTPanels[i].fRatio_y = (double)((double)(RECTH(gSTPanels[i].rcView)) / (double)(gSTPanels[i].yMax - gSTPanels[i].yMin));
	}

	return 0;
}


/*
	��������
	����ʱ,����ʱ��������֮��ı���,��ʱ�����
*/
static int DrawValue(HDC hdc, PTRENDDATA pTrendData, int iTimeInter)
{
	int iLeft = 0, iTop = 0, iRight = 0, iBottom = 0, iWidth = 0, iHeight = 0;
	int x1, y1, x2, y2, x3, y3;
	int i;
//	static int icount=0;
//	printf("*****%d\n",icount);
//	icount=(icount+1)%100;
//printf("hr=%d,iTimeInter=%d\n",pTrendData->wHR,iTimeInter);
	for(i=0; i<giSTCount; i++){
		//ȡ����������
		iLeft = gSTPanels[i].rcView.left;
		iTop = gSTPanels[i].rcView.top;
		iRight = gSTPanels[i].rcView.right;
		iBottom = gSTPanels[i].rcView.bottom;
		iHeight = iBottom - iTop;
		iWidth = iRight - iLeft;	
		
		switch(gSTPanels[i].bID){
			case NSTID_HR:{
				//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				if(pTrendData->dwHaveFlag & HAVEHR){
					y1 = pTrendData->wHR;
				}
				else{
					break;
				}
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
			
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
			}break;
			case NSTID_SPO2:{
				//�������»���ͼ
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				if(pTrendData->dwHaveFlag & HAVESPO2){
					y1 = pTrendData->bSpO2;
				}
				else{
					break;
				}
// 				y1 = 95;
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
// 				MoveTo(hdc, x1, iTop);
// 				LineTo(hdc, x1, y1);
				
// 				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, iTop, x1, y1, WIDELINE_RECT);
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
			}break;
			case NSTID_RR:{
			//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				if(pTrendData->dwHaveFlag & HAVERESP){
					y1 = pTrendData->wRR;
				}
				else{
					break;
				}
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
			}break;
			case NSTID_NIBP:{
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight-(iTimeInter* gSTPanels[i].fRatio_x);
				
				if(pTrendData->dwHaveFlag & HAVENIBP){
					y1 = pTrendData->sNIBP.wSys;
					y2 = pTrendData->sNIBP.wMean;
					y3 = pTrendData->sNIBP.wDia;
				}
				else{
					break;
				}
				
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;

				if(y2 > gSTPanels[i].yMax) y2 = gSTPanels[i].yMax;
				if(y2 < gSTPanels[i].yMin) y2 = gSTPanels[i].yMin;
				y2 = iBottom - (y2 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
				if(y3 > gSTPanels[i].yMax) y3 = gSTPanels[i].yMax;
				if(y3 < gSTPanels[i].yMin) y3 = gSTPanels[i].yMin;
				y3 = iBottom - (y3 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
			
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1, x1, y2, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgIbp1.iColor);
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y2, x1, y3, WIDELINE_RECT);
			}break;
			case NSTID_TEMP:{
			//�������ϻ���ͼ
		//	printf("t1=%d,%d---,t2=%d,%d\n",pTrendData->wTemps[0],pTrendData->wTemps[1],pTrendData->dwHaveFlag & HAVET1,pTrendData->dwHaveFlag & HAVET2);
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				if(pTrendData->dwHaveFlag & HAVET1||pTrendData->dwHaveFlag & HAVET2){
					if(pTrendData->dwHaveFlag & HAVET1){
						y1 = pTrendData->wTemps[0];
					}
					if(pTrendData->dwHaveFlag & HAVET2){
						y2= pTrendData->wTemps[1];
					}
						
				}else{
					break;
				}
				
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				if(y2 > gSTPanels[i].yMax) y2 = gSTPanels[i].yMax;
				if(y2 < gSTPanels[i].yMin) y2 = gSTPanels[i].yMin;
				y2 = iBottom - (y2 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				if(pTrendData->dwHaveFlag & HAVET1)
					DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
				SetPenColor(hdc, ~gSTPanels[i].iColor);
				if(pTrendData->dwHaveFlag & HAVET2)
					DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y2-1, x1, y2+1, WIDELINE_RECT);

			}break;
			case NSTID_CO2:{
			//�������ϻ���ͼ
				SetPenColor(hdc, gCfgCO2.iColor);
				x1 = iRight-(iTimeInter* gSTPanels[i].fRatio_x);
				if(pTrendData->dwHaveFlag & HAVECO2){
					y1 = pTrendData->sCO2.wFiCO2;
					y2 = pTrendData->sCO2.wEtCO2;
				}
				else{
					break;
				}
// 			y1 = 95;
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				if(y2 > gSTPanels[i].yMax) y2 = gSTPanels[i].yMax;
				if(y2 < gSTPanels[i].yMin) y2 = gSTPanels[i].yMin;
				y2 = iBottom - (y2 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
// 				MoveTo(hdc, x1, iTop);
// 				LineTo(hdc, x1, y1);
				
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1, x1, y2, WIDELINE_RECT);
			}break;
			case NSTID_IBP1:{
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight-(iTimeInter* gSTPanels[i].fRatio_x);
				x2 = iRight-(iTimeInter* gSTPanels[i].fRatio_x);
				x3 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				
				if(pTrendData->dwHaveFlag & HAVEIBP1){
					y1 = pTrendData->sIBPs[0].iSys;
					y2 = pTrendData->sIBPs[0].iMean;
					y3 = pTrendData->sIBPs[0].iDia;
				}
				else{
					break;
				}
				
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;

				if(y2 > gSTPanels[i].yMax) y2 = gSTPanels[i].yMax;
				if(y2 < gSTPanels[i].yMin) y2 = gSTPanels[i].yMin;
				y2 = iBottom - (y2 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
				if(y3 > gSTPanels[i].yMax) y3 = gSTPanels[i].yMax;
				if(y3 < gSTPanels[i].yMin) y3 = gSTPanels[i].yMin;
				y3 = iBottom - (y3 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
			
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1, x3, y3, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgIbp1.iColor);
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
			}break;
			case NSTID_IBP2:{
				SetPenColor(hdc, gSTPanels[i].iColor);
				x1 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				x2 = iRight-(iTimeInter* gSTPanels[i].fRatio_x);
				x3 = iRight- (iTimeInter* gSTPanels[i].fRatio_x);
				
				if(pTrendData->dwHaveFlag & HAVEIBP2){
					y1 = pTrendData->sIBPs[1].iSys;
					y2 = pTrendData->sIBPs[1].iMean;
					y3 = pTrendData->sIBPs[1].iDia;
				}
				else{
					break;
				}
				
				if(y1 > gSTPanels[i].yMax) y1 = gSTPanels[i].yMax;
				if(y1 < gSTPanels[i].yMin) y1 = gSTPanels[i].yMin;
				y1 = iBottom - (y1 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;

				if(y2 > gSTPanels[i].yMax) y2 = gSTPanels[i].yMax;
				if(y2 < gSTPanels[i].yMin) y2 = gSTPanels[i].yMin;
				y2 = iBottom - (y2 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
				if(y3 > gSTPanels[i].yMax) y3 = gSTPanels[i].yMax;
				if(y3 < gSTPanels[i].yMin) y3 = gSTPanels[i].yMin;
				y3 = iBottom - (y3 - gSTPanels[i].yMin)*gSTPanels[i].fRatio_y;
				
			
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x1, y1, x3, y3, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgIbp2.iColor);
				DrawWideLine(hdc, REALWIDELINE, ST_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
			}break;
			default: break;
		}
	}
	
	return 0;
}
#if 0
/*
	��������ͼ����
	1���ӵ�ǰ���������ݿ�ʼ��ȡ��ָ���̶ȵ���������
	2����ȡ��������ȫ������ָ������
*/
static int DrawShortTrendValue(HDC hdc)
{
	S_TIME sGetStartTime;			//���µ��������ݵ�ʱ��
	S_TIME sGetEndTime;			//ȡ���ݵĽ�ֹʱ��
	S_TIME sTmpTime;				//���ݵ�ʱ��
	WORD wHead, wTail, wSize;
	BOOL bLoop;	//�����Ƿ�ת				
	TRENDDATA tmpTrendData;
	int iTimeInter = 0;
	int i;
	
	if(hdc == (HDC)NULL) return -1;
	

	//��ȡ�������ݵ���Ϣ
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	//�ж����������Ƿ�Ϊ��
	if(wHead == wTail) return -1;
		
	//����ʱ�䷶Χ������ʱ��,����ȡ���ݵĽ�ֹʱ��(��ǰ֮ǰ)
	sGetStartTime = gTrendDatas.Datas[gTrendDatas.wHeadIndex].sTime;
	CalcTime(&(sGetStartTime), &sGetEndTime, giST_XScale*60, TIME_BEFORE);
	
  	printf("%s:%d Get Trend Datas, From %d:%d:%d  ---  %d:%d:%d.\n", __FILE__, __LINE__, 
  	       sGetStartTime.bHour, sGetStartTime.bMin, sGetStartTime.bSec,
  	       sGetEndTime.bHour, sGetEndTime.bMin, sGetEndTime.bSec);
		
	
	//����������(Head)��ǰ(Tail)ȡָ��ʱ���ڵ�����, ��ȡ�߻�
	if(wHead < wTail){
		for(i=wHead; i>=0; i--){
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
		for(i=(wSize-1); i>=wTail; i--){
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--){
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
	}
	else{
		for(i=wHead; i>wTail; i--){
			sTmpTime = gTrendDatas.Datas[i].sTime;
 			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
	}	
	
	
GETOVER:

	
		return 0;
}
#endif
#define COMTIMETEST 0
/*
	��������ͼ����
	1���ӵ�ǰ���������ݿ�ʼ��ȡ��ָ���̶ȵ���������
	2����ȡ��������ȫ������ָ������
*/
static int DrawShortTrendValue(HDC hdc)
{
	S_TIME sGetStartTime;			//���µ��������ݵ�ʱ��
	S_TIME sGetEndTime;			//ȡ���ݵĽ�ֹʱ��
	S_TIME sTmpTime;				//���ݵ�ʱ��
	WORD wHead, wTail, wSize;
	BOOL bFull;	//�����Ƿ�ת				
	TRENDDATA tmpTrendData[MAXSHORTTREND];
	int iST_Count = 0,iST_XScale=0;
	int i;
	
	if(hdc == (HDC)NULL) return -1;
	
	//��ȡ�������ݵ���Ϣ
	wHead = gShortTrendDatas.wHead;
	bFull = gShortTrendDatas.bFull;
	wSize = MAXSHORTTREND;

	iST_XScale=giST_XScale;
	iST_Count=giST_XScale;
	//�ж����������Ƿ�Ϊ��
	if(wHead==0&&!bFull ) return -1;

	//��ȡ��������Ҫ��ͼ�ĵ�
	if(bFull){//�����Ѿ����������
		for(i=0;i<iST_XScale;i++){
			tmpTrendData[i]=gShortTrendDatas.Datas[(wHead-i+MAXSHORTTREND)%MAXSHORTTREND];
		}
	}else{//����δ����
		if(wHead < iST_XScale ){//����δ����������ʱ��.ֻѭ��
			for(i=0;i<wHead;i++){
				tmpTrendData[i]=gShortTrendDatas.Datas[wHead-i];
			}
			iST_Count=wHead;
		}else{//���ݳ���ʱ�䣬
			for(i=0;i<iST_XScale;i++){
				tmpTrendData[i]=gShortTrendDatas.Datas[wHead-i];
			}
		
		}
	}

//	printf("whead=%d,bloop=%d,count=%d\n",wHead,bFull,iST_Count);	

	for(i=0;i<iST_Count;i++){
	//	printf("st[%d].hr=%d\n",i,tmpTrendData[i].wHR);
		DrawValue(hdc,  &tmpTrendData[i], i);
	}

#if 0
	
	//����������(Head)��ǰ(Tail)ȡָ��ʱ���ڵ�����, ��ȡ�߻�
	if(wHead < wTail){
		for(i=wHead; i>=0; i--){
			
		}
		
		for(i=(wSize-1); i>=wTail; i--){
			
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--){
		
		}
	}
	else{
	
		for(i=wHead; i>wTail; i--){
		
		}
	}	
	
	#endif
	
		return 0;
}

static int DrawST_HR(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	//Label
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FC", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	else
		DrawText(hdc, "HR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}
static int DrawST_NIBP(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "NIBP", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_SpO2(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "SpO2", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_RR(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	else
		DrawText(hdc, "RR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_TEMP(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "TEMP", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%.1f", (float)(pPanel->yMin/10));
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%.1f", (float)(pPanel->yMax/10));
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_CO2(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "CO2", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_Ibp1(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "IBP1", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawST_Ibp2(HDC hdc, PNST_PANEL pPanel)
{
	SetTextColor(hdc, ST_TEXT_COLOR);
	
	DrawText(hdc, "IBP2", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));	
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}


//��������ͼ
static int DrawShortTrend()
{
	int i;
	
	if((gCfgSystem.bInterface != SCREEN_SHORTTREND)  || (hdc_ST == (HDC)NULL) || (mdc_ST == (HDC)NULL)) {
		printf("------------------------------------%s:%d get shortTrend's DC  failure.\n", __FILE__, __LINE__);
		return -1;
	}
		
	//����
	FillBox(mdc_ST, 
		gRCShortTrend.left, 
		gRCShortTrend.top, 
		RECTW(gRCShortTrend)+1, 
		RECTH(gRCShortTrend)+1);

		
	//������������
	DrawShortTrendValue(mdc_ST);
	
	//���ƿ��
	SetPenColor(mdc_ST, ST_FRAME_COLOR);
	SelectFont(mdc_ST, gFontSmall);
	for(i=0; i<giSTCount; i++){
		switch(gSTPanels[i].bID){
			case NSTID_HR:{
				DrawST_HR(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_NIBP:{
				DrawST_NIBP(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_SPO2:{
				DrawST_SpO2(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_RR:{
				DrawST_RR(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_TEMP:{
				DrawST_TEMP(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_CO2:{
				DrawST_CO2(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_IBP1:{
				DrawST_Ibp1(mdc_ST, &(gSTPanels[i]));
			}break;
			case NSTID_IBP2:{
				DrawST_Ibp2(mdc_ST, &(gSTPanels[i]));
			}break;
		}
	}
	if((gCfgSystem.bInterface == SCREEN_SHORTTREND)  && (hdc_ST != (HDC)NULL)) 
		//����DC
		BitBlt(mdc_ST, 
			gRCShortTrend.left, gRCShortTrend.top, RECTW(gRCShortTrend)+1, RECTH(gRCShortTrend)+1, 
			hdc_ST, 
			gRCShortTrend.left, gRCShortTrend.top, 
 	       0);
	else
		printf("-----------------------------------------------%s:%d  No ShortTrend hdc, bitblt failed.\n", __FILE__, __LINE__);

	return 0;
}

//��ͼ�߳�
static void *ThreadProcShortTrend(void *arg)
{
	
	for(;;)
	{
		sem_wait(&semShortTrend);
		gbDrawSTFinished = FALSE;
		DrawShortTrend();
		gbDrawSTFinished = TRUE;
	}
	
	printf("%s:%d Cancel ShortTrend  thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel ShortTrend thread.\n");
}

//����HDC
int NewDestroyShortTrend()
{
	
	while(!gbDrawSTFinished){
		printf("------------------------------------------------------%s:%d ShortTrend not draw finished.\n", __FILE__, __LINE__);		
		sleep(1);
	}
		
	if(mdc_ST != (HDC)NULL){
		DeleteCompatibleDC(mdc_ST);
		mdc_ST = (HDC)NULL;
		printf("--------------------Destroy ShortTrend mdc.\n");
	}		
	if(hdc_ST != (HDC)NULL){
		ReleaseDC(hdc_ST);
		hdc_ST = (HDC)NULL;
		printf("--------------------Destroy ShortTrend hdc.\n");
	}
	
	gbDrawSTFinished = TRUE;
	return 0;
}
		

//��ʼ��������
int NewInitShortTrend()
{
	//��ʾ���
	ShowWindow(ghWndPanel_ShortTrend, SW_SHOW);
	
	//������ͼmemDC
	if(hdc_ST == (HDC)NULL){
		hdc_ST = GetClientDC(ghWndPanel_ShortTrend);
		if(hdc_ST == (HDC)NULL) return -1;
		SetPenColor(hdc_ST, ST_FRAME_COLOR);
		SetBkMode(hdc_ST, BM_TRANSPARENT);
		SetTextColor(hdc_ST, ST_TEXT_COLOR);
		SetBkColor(hdc_ST, ST_BK_COLOR);
		SetBrushColor(hdc_ST, ST_BK_COLOR);
		mdc_ST = CreateCompatibleDC(hdc_ST);
		if(B_PRINTF) printf("%s:%d create hdc of Short Trend OK!\n", __FILE__, __LINE__);
	}else{
		if(B_PRINTF) printf("%s:%d Hdc of ShortTrend had created.\n", __FILE__, __LINE__);
	}
	
	//���X�̶�
	switch (gCfgSystem.bShortTrendScale){
		case 0:{
			giST_XScale = 5;
		}break;
		case 1:{
			giST_XScale = 10;
		}break;
		case 2:{
			giST_XScale = 15;
		}break;
		case 3:{
			giST_XScale = 20;
		}break;
		case 4:{
			giST_XScale = 30;
		}break;
		case 5:{
			giST_XScale = 60;
		}break;
		case 6:{
			giST_XScale = 120;
		}break;
		default:{
			giST_XScale = 30;
		}break;
	}
	
	//���ʱ�䵥λ
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "shorttrend_unit", strUnit, sizeof strUnit);		
	memset(strXCale, 0, sizeof strXCale);
	snprintf(strXCale, sizeof strXCale, "%d %s", giST_XScale, strUnit);

	//��ʼ������
	InitSTAttr();

	//������ͼ�߳�
	if(!gbThreadHasCreated_st){
		sem_init(&semShortTrend, 0, 0);
		pthread_create(&ptShortTrend, NULL, ThreadProcShortTrend, NULL);	
		gbThreadHasCreated_st = TRUE;
		if(B_PRINTF) printf("%s:%d create thread of drawing ShortTrend OK!\n", __FILE__, __LINE__);
	}
	else{
		if(B_PRINTF) printf("%s:%d Thread of drawing ShortTrend has created!\n", __FILE__, __LINE__);
	}
	
	return 0;
}

/**
	���Ŀ̶�
 */
int NewChangeShortTrendScale()
{
	//���X�̶�
	switch (gCfgSystem.bShortTrendScale){
		case 0:{
			giST_XScale = 5;
		}break;
		case 1:{
			giST_XScale = 10;
		}break;
		case 2:{
			giST_XScale = 15;
		}break;
		case 3:{
			giST_XScale = 20;
		}break;
		case 4:{
			giST_XScale = 30;
		}break;
		case 5:{
			giST_XScale = 60;
		}break;
		case 6:{
			giST_XScale = 120;
		}break;
		default:{
			giST_XScale = 30;
		}break;
	}
		
	if(gCfgSystem.bInterface == SCREEN_SHORTTREND){
		memset(strXCale, 0, sizeof strXCale);
		snprintf(strXCale, sizeof strXCale, "%d %s", giST_XScale, strUnit);
		
		//��ʼ������
		InitSTAttr();
		//��������ͼ
		gbDrawSTFinished = FALSE;
		DrawShortTrend();
		gbDrawSTFinished = TRUE;
	}

	return 0;
}

int NewUpdateShortTrend()
{
//	static BOOL bFlash=FALSE;
//	if(bFlash)
		sem_post(&semShortTrend);
//	bFlash=!bFlash;
	return 0;
}
