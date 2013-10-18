/*************************************************************************** 
 *	Module Name:	NewOxyCRG
 *
 *	Abstract:	OxyCRG
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-02-13 09:04:14
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"

//��������ͼ����
#define CRG_NONE	0
#define CRG_HR	1
#define CRG_SPO2	2
#define CRG_RR	3	//������
#define CRG_RESP	4	//��������

//�����߶εĿ��
#define CRG_LINE_WIDTH		1

//��ʾ��Χ
//HR
#define CRG_HR_MIN			HR_MIN
#define CRG_HR_MAX		HR_MAX
//SpO2
#define CRG_SPO2_MIN		80
#define CRG_SPO2_MAX		SPO2_MAX
//RR
#define CRG_RR_MIN			RR_MIN
#define CRG_RR_MAX			RR_MAX
//RESP TODO:�ݶ�,�д���һ��ȷ��
#define CRG_RESP_MIN		0
#define CRG_RESP_MAX		64

//��������, HR, SpO2, RR or Resp
#define CRG_COUNT			3

//��ɫ
#define CRG_BKCOLOR		COLOR_black
#define CRG_TEXTCOLOR		COLOR_darkgray
#define CRG_PENCOLOR		COLOR_darkgray

//������
#define CRG_LEFT		RC_OXYCRG_LEFT
#define CRG_TOP			RC_OXYCRG_BOTTOM+20
#define CRG_RIGHT		RC_NORMAL_RIGHT
#define CRG_BOTTOM		RC_NORMAL_BOTTOM


//CRG��ʾ�������ݽṹ
typedef struct {
	BYTE bID;			
	RECT rcView;		//��ʾ����
	int xScale;		//xֵ�ķ�Χ(��),���ں���Ҫ��ʾָ��ʱ���ڵ�ȫ������
	int yMin;			//y����Сֵ�������������Ƶ���Сֵ
	int yMax;			//y�����ֵ�������������Ƶ����ֵ
	double fRatio_x;       //X��Ļ�ͼ����
	double fRatio_x_wave;       //X��Ļ�ͼ����
	double fRatio_y	;	//Y��Ļ�ͼ����	
	int iColor;			//����ͼ������ɫ
}CRG_TREND, *PCRG_TREND;

//��ʾ����
RECT gRCCrg;
RECT gRCCrgValue;
RECT gRCCrgLabel;
RECT gRCCrgTime;
static CRG_TREND gCrgPanels[CRG_COUNT];
//��������
static int giCrgCount;	

//������X����ʾ�̶ȼ�ʱ�䷶Χ, ��λ:��
static int giCrg_XScale = 120;

static unsigned char strInfo[20]={0};
static unsigned char strXCale[20]={0};
static unsigned char strUnit[10]={0};

//HDC
static HDC hdc_oxyCRG = (HDC)NULL;
//MDC
static HDC mdc_oxyCRG = (HDC)NULL;

extern HWND ghWndPanel_Crg;

//�����߳�
static pthread_t ptCrg;
static sem_t semCrg;	
static BOOL gbThreadHasCreated_Crg = FALSE;
static BOOL gbDrawCrgFinished  = TRUE;


//------------------------���ݴ���(�洢,��ȡ)-----------------------------------
/*
	˼·:��1s�ĺ�������Ϊ��׼,ѭ���洢8���ӵ�����. ��ʾ��ʱ����ʾ��ǰʱ��֮ǰָ����Χ�ڵ�����
*/
//CRG��������
typedef struct {
	WORD wHR;
	WORD wRR;
	BYTE bSpO2;
	BYTE bResp[200];		//200 pack/sec
}CRG_DATA, *PCRG_DATA;

//���洢Crg������ʱ��,��λ:��
#define CRG_DATA_MAX_COUNT	1800
//ѭ������
typedef struct {
	WORD 			wHeadIndex;					//ѭ��������ָ��
	WORD 			wTailIndex;					//ѭ������βָ��
	BOOL 			bLoop;						//���鷭ת���
	WORD 			wCount;						//ʵ�ʴ洢����������
	CRG_DATA		Datas[CRG_DATA_MAX_COUNT];
}CRG_DATAARRAY, *PCRG_DATAARRAY;

CRG_DATA    		gCrgData;	
CRG_DATAARRAY	gCrgDataArray;


/**
	���һ�����ݵ�����,��HeadIndex��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
 */
static int CrgArray_Add(const PCRG_DATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	if(pData == NULL) return -1;
	
	wHead = gCrgDataArray.wHeadIndex;
	wTail = gCrgDataArray.wTailIndex;
	bLoop = gCrgDataArray.bLoop;
	wSize = CRG_DATA_MAX_COUNT;

	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//����Ƿ�ת�����
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//�����Ѿ�����,��ת
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	gCrgDataArray.wHeadIndex = wHead;
	gCrgDataArray.wTailIndex = wTail;
	gCrgDataArray.bLoop = bLoop;
	gCrgDataArray.Datas[gCrgDataArray.wHeadIndex] = *pData;
	//����ʵ�ʴ洢������
	if(gCrgDataArray.wHeadIndex < gCrgDataArray.wTailIndex){
		//�����Ѿ�����,����ת
		gCrgDataArray.wCount = CRG_DATA_MAX_COUNT;
	}
	else{
		gCrgDataArray.wCount +=1;
	}

	return wHead;
}

/**
	��䵥��ʵʱ���� 
	@param:bFinish: ���������Ƿ����
 */
int NewFillOneCrgData(BOOL bFinish)
{
	static int iCount = 0;
	static int iPackCount = 0;
	int i;
	
	if(iCount < 125){
		//������Ӳ���
		//Resp
		gCrgData.bResp[iCount] = gValueResp.iWave;	
		iCount ++;
	}
	else{
		iCount = 0;
		iPackCount = 0;

		//��Ӳ�����Ϣ
		gCrgData.wHR = gValueEcg.wHR;
		gCrgData.wRR = gValueResp.wRR;
		gCrgData.bSpO2 = gValueSpO2.bSpO2;
		
		//����¼��ӵ�������
		CrgArray_Add(&gCrgData);
  		
//		printf("%s:%d CrgSave. Total %d records. Head %d, Tail %d. SpO2 is %d\n", __FILE__, __LINE__, 
 // 		       gCrgDataArray.wCount, gCrgDataArray.wHeadIndex, gCrgDataArray.wTailIndex, 
//    		       gValueSpO2.bSpO2);
	}
	
	return 0;
}

//////////////////////////////////

//���ö���������
static int InitCrgAttr()
{
	int iTop = 35;
	int iInter = 106;
	int i;
	
	//����ID
	for(i = 0; i < CRG_COUNT; i++){
		gCrgPanels[0].bID = CRG_NONE;
	}
	gCrgPanels[0].bID = CRG_HR;
	gCrgPanels[1].bID = CRG_SPO2;
	gCrgPanels[2].bID = CRG_RR;
	giCrgCount = 3;

	GetClientRect(ghWndPanel_Crg, &gRCCrg);
	//����
	SetRect(&gRCCrgValue, gRCCrg.left, gRCCrg.top+20, gRCCrg.right-1, gRCCrg.bottom);
	SetRect(&gRCCrgLabel, gRCCrg.left, gRCCrg.top, gRCCrg.right-1, gRCCrg.top+19);
	//SetRect(&gRCCrgTime, gRCCrg.left, gRCCrg.bottom-15, gRCCrg.right-1, gRCCrg.bottom-1);
	iInter = RECTH(gRCCrgValue)/giCrgCount;
	for(i = 0; i < giCrgCount; i++){
		SetRect(&(gCrgPanels[i].rcView), 
			  gRCCrgValue.left,
			  gRCCrgValue.top + i*iInter,
			  gRCCrgValue.right,
			  gRCCrgValue.top + (i+1)*iInter);
	}
	
	//���������ID����ϵ��
	for(i = 0; i < giCrgCount; i++){
		//ʱ��̶�ͳһ
		gCrgPanels[i].xScale = giCrg_XScale;		
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				gCrgPanels[i].yMin = CRG_HR_MIN;
				gCrgPanels[i].yMax = CRG_HR_MAX;
				gCrgPanels[i].iColor = gCfgEcg.iColor;
			}break;
			case CRG_SPO2:{
				gCrgPanels[i].yMin = CRG_SPO2_MIN;
				gCrgPanels[i].yMax = CRG_SPO2_MAX;
				gCrgPanels[i].iColor = gCfgSpO2.iColor;
			}break;
			case CRG_RR:{
				gCrgPanels[i].yMin = CRG_RR_MIN;
				gCrgPanels[i].yMax = CRG_RR_MAX;
				gCrgPanels[i].iColor = gCfgResp.iColor;	
			}break;
			case CRG_RESP:{
				gCrgPanels[i].yMin = CRG_RESP_MIN;
				gCrgPanels[i].yMax = CRG_RESP_MAX;
				gCrgPanels[i].iColor = gCfgResp.iColor;
			}break;
		}
		
		//����ʱ��̶ȼ���ʱ�������صı���
		gCrgPanels[i].fRatio_x = (double)((double)(RECTW(gCrgPanels[i].rcView))/(double)(gCrgPanels[i].xScale));
		//���ݲ�������ֵ��������ֵ�����صı���
		gCrgPanels[i].fRatio_y = (double)((double)(RECTH(gCrgPanels[i].rcView)) / (double)(gCrgPanels[i].yMax - gCrgPanels[i].yMin));
	}

	return 0;
}

static int DrawCrg_HR(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FC", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	else
		DrawText(hdc, "HR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
// 	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
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

static int DrawCrg_SpO2(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	DrawText(hdc, "SpO2", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
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

static int DrawCrg_RR(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	else
		DrawText(hdc, "RR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
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

static int DrawCrg_Resp(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	DrawText(hdc, "Resp", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
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

static int DrawCrg_Label(HDC hdc, RECT rc)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	SelectFont(hdc, gFontSystem);
	//Label
	memset(strInfo, 0, sizeof strInfo);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg", strInfo, sizeof strInfo,"oxyCRG");				
	DrawText(hdc, strInfo, -1, &(rc), DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	Rectangle(hdc, 
		  rc.left, 
		  rc.top, 
		  rc.right, 
		  rc.bottom);
	SelectFont(hdc, gFontSmall);
	return 0;
}

static int DrawCrg_Time(HDC hdc, RECT rc)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	DrawText(hdc, strXCale, -1, &(rc), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	Rectangle(hdc, 
		  rc.left, 
		  rc.top, 
		  rc.right, 
		  rc.bottom);
	
	return 0;
}

/*
	��������
	����ʱ,����ʱ��������֮��ı���,��ʱ�����
	bClean: TRUE:��ղ���
*/
static int DrawValue(HDC hdc, PCRG_DATA pData, int iCount)
{
	int iLeft = 0, iTop = 0, iRight = 0, iBottom = 0, iWidth = 0, iHeight = 0;
	static int x1=0, y1=0; 
	static int x2=0, y2=0;
	int i, j;
		
	for(i=0; i<CRG_COUNT; i++){
		//ȡ����������
		iLeft = gCrgPanels[i].rcView.left;
		iTop = gCrgPanels[i].rcView.top;
		iRight = gCrgPanels[i].rcView.right;
		iBottom = gCrgPanels[i].rcView.bottom;
		iHeight = iBottom - iTop;
		iWidth = iRight - iLeft;	
		
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gCfgEcg.iColor);
				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
				y1 = pData->wHR;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT);
				}
			}break;
			case CRG_SPO2:{
				//�������»���ͼ
				SetPenColor(hdc, gCfgSpO2.iColor);
				x1 = iRight - iCount*gCrgPanels[i].fRatio_x;
				y1 = pData->bSpO2;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
// 					DrawWideLine(hdc, REALWIDELINE, 1, x1, iTop, x1, y1, WIDELINE_RECT);
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT); ////�Ե�Ϊ����,���̰�ͼ
				}
			}break;
			case CRG_RR:{
				//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gCfgResp.iColor);
				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
				y1 = pData->wRR;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT);
				}
 			}break;
// 			case CRG_RESP:{
// 				//ѹ����������
// 				SetPenColor(hdc, gCfgResp.iColor);
// 				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
//
// 				MoveTo(hdc, x1, y2);
// 				for(j=0; j<200; j++){
// 					x2 = x1 + j*gCrgPanels[i].fRatio_x_wave;
// 					y2 = pData->bResp[j]; 
// 					if(y2 > gCrgPanels[i].yMax) y2 = gCrgPanels[i].yMax;
// 					if(y2 < gCrgPanels[i].yMin) y2 = gCrgPanels[i].yMin;
// 					y2 = iBottom - (y2 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
// 					LineTo(hdc, x2, y2);
// 				}
// 			}break;
		}
		
	}
	
	return 0;
}

//����ͼ��
static int DrawCrgValue(HDC hdc)
{
	int nType;
	
	WORD wHead, wTail, wSize;
	BOOL bLoop;	
	CRG_DATA  tmpCrgData[CRG_DATA_MAX_COUNT];
	int i;
	int iCount = 0;
	int iCRG_Count=0;
	//��ȡ�������ݵ���Ϣ
	wHead = gCrgDataArray.wHeadIndex;
	wTail = gCrgDataArray.wTailIndex;
	bLoop = gCrgDataArray.bLoop;
	wSize = CRG_DATA_MAX_COUNT;

	iCRG_Count=giCrg_XScale;
	//�ж����������Ƿ�Ϊ��
	if(wHead == wTail) return -1;
	//printf("wsize=%d\n",wSize);

	
	//��ȡ��������Ҫ��ͼ�ĵ�
	if(bLoop){//�����Ѿ����������
		for(i=0;i<giCrg_XScale;i++){
			tmpCrgData[i]=gCrgDataArray.Datas[(wHead-i+CRG_DATA_MAX_COUNT)%CRG_DATA_MAX_COUNT];
		}
	}else{//����δ����
		if(wHead < giCrg_XScale ){//����δ����������ʱ��.ֻѭ��
			for(i=0;i<wHead;i++){
				tmpCrgData[i]=gCrgDataArray.Datas[wHead-i];
			}
			iCRG_Count=wHead;
		}else{//���ݳ���ʱ�䣬
			for(i=0;i<giCrg_XScale;i++){
				tmpCrgData[i]=gCrgDataArray.Datas[wHead-i];
			}
		
		}
	}
for(i=0;i<iCRG_Count;i++){
	//	printf("st[%d].hr=%d\n",i,tmpTrendData[i].wHR);
		DrawValue(hdc,  &tmpCrgData[i], i);
	}

#if 0	
	//����������(Head)��ǰ(Tail)ȡָ��ʱ���ڵ�����, ��ȡ�߻�
	if(wHead < wTail){
		for(i=wHead; i>=0; i--, iCount++){
			//��������
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
		for(i=(wSize-1); i>=wTail; i--, iCount++){
                        //��������
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--, iCount++){
			//��������
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}
	else{
		for(i=wHead; i>wTail; i--, iCount++){
			//��������
			if(iCount < giCrg_XScale){
				
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}	
#endif
	return 0;
}

//��������ͼ
static int DrawCrg()
{
	int i;

	if((gCfgSystem.bInterface == SCREEN_OXYCRG)  && (hdc_oxyCRG != (HDC)NULL) && (mdc_oxyCRG != (HDC)NULL)) {
		//����
		FillBox(mdc_oxyCRG, 
			gRCCrg.left, 
			gRCCrg.top, 
			RECTW(gRCCrg)+1, 
			RECTH(gRCCrg)+1);

		//������������
		DrawCrgValue(mdc_oxyCRG);
	
		//���ƿ��
		SetPenColor(mdc_oxyCRG, CRG_PENCOLOR);
		for(i=0; i<giCrgCount; i++){
			switch(gCrgPanels[i].bID){
				case CRG_HR:{
					DrawCrg_HR(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_SPO2:{
					DrawCrg_SpO2(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_RR:{
					DrawCrg_RR(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_RESP:{
					DrawCrg_Resp(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
			}
		}
	
		//Label
		DrawCrg_Label(mdc_oxyCRG, gRCCrgLabel);
		//Time
		DrawCrg_Time(mdc_oxyCRG, gRCCrgValue);
		
		//����DC
		if((gCfgSystem.bInterface == SCREEN_OXYCRG)  && (hdc_oxyCRG != (HDC)NULL)) 
			BitBlt(mdc_oxyCRG, 
				gRCCrg.left, gRCCrg.top, RECTW(gRCCrg)+1, RECTH(gRCCrg)+1, 
				hdc_oxyCRG, 
				gRCCrg.left, gRCCrg.top, 
				0);
		else
			if(B_PRINTF) printf("-----------------------------------------------%s:%d  No crg hdc, bitblt failed.\n", __FILE__, __LINE__);
	}	
	else
		if(B_PRINTF) printf("-----------------------------------------------%s:%d  No crg hdc, bitblt failed.\n", __FILE__, __LINE__);
	
	
	
	return 0;
}

static int DrawCrg_hdc()
{
	int i;

	//����
	FillBox(hdc_oxyCRG, 
		gRCCrg.left, 
		gRCCrg.top, 
		RECTW(gRCCrg)+1, 
		RECTH(gRCCrg)+1);

	//������������
	DrawCrgValue(hdc_oxyCRG);
	
	//���ƿ��
	SetPenColor(hdc_oxyCRG, CRG_PENCOLOR);
	for(i=0; i<giCrgCount; i++){
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				DrawCrg_HR(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_SPO2:{
				DrawCrg_SpO2(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_RR:{
				DrawCrg_RR(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_RESP:{
				DrawCrg_Resp(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
		}
	}
	
	//Label
	DrawCrg_Label(hdc_oxyCRG, gRCCrgLabel);
	//Time
	DrawCrg_Time(hdc_oxyCRG, gRCCrgValue);
	
	return 0;
}

//��ͼ�߳�
static void *ThreadProcCrg(void *arg)
{
	
	for(;;)
	{
		sem_wait(&semCrg);
	//	if((gCfgSystem.bInterface == SCREEN_OXYCRG)){
			gbDrawCrgFinished  = FALSE;
			
			DrawCrg();
			gbDrawCrgFinished  = TRUE;
		//}
			
			//DrawCrg_hdc();
	}

	if(B_PRINTF) printf("%s:%d Cancel OxyCrg thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel OxyCrg thread.\n");
}

//����HDC
int NewDestroyCrg()
{
	while(!gbDrawCrgFinished) {
		if(B_PRINTF) printf("------------------------------------------------------%s:%d CRG not draw finished.\n", __FILE__, __LINE__);
		sleep(1);
	}
	
		if(mdc_oxyCRG != (HDC)NULL){
			DeleteCompatibleDC(mdc_oxyCRG);
			mdc_oxyCRG = (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy oxyCRG mdc.\n");
		}
		
		if(hdc_oxyCRG != (HDC)NULL){
			ReleaseDC(hdc_oxyCRG);
			hdc_oxyCRG = (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy oxyCRG hdc.\n");
		}
	
	gbDrawCrgFinished = TRUE;

	return 0;
}

//��ʼ��������
int NewInitCrg()
{
	//��ʾ���
	ShowWindow(ghWndPanel_Crg, SW_SHOW);
	
	hdc_oxyCRG = mdc_oxyCRG = (HDC)NULL;
	//������ͼmemDC
	if(hdc_oxyCRG == (HDC)NULL){
  		hdc_oxyCRG = GetClientDC(ghWndPanel_Crg);
		
		if(hdc_oxyCRG == (HDC)NULL) return -1;
		SetPenColor(hdc_oxyCRG, CRG_PENCOLOR);
		SetBkMode(hdc_oxyCRG, BM_TRANSPARENT);
		SetBkColor(hdc_oxyCRG, CRG_BKCOLOR);
		SetBrushColor(hdc_oxyCRG, CRG_BKCOLOR);
		SetTextColor(hdc_oxyCRG, CRG_TEXTCOLOR);
		SelectFont(hdc_oxyCRG, gFontSmall);
 		mdc_oxyCRG = CreateCompatibleDC(hdc_oxyCRG);
		
		if(B_PRINTF) printf("%s:%d create hdc of oxyCRG OK!\n", __FILE__, __LINE__);
	}else{
		if(B_PRINTF) printf("%s:%d Hdc of oxyCRG had created.\n", __FILE__, __LINE__);
	}
	
	//���X�̶�
	//���X�̶�
	switch (gCfgSystem.bOxyCrgScale){
		case 1:{
			giCrg_XScale= 120;
		}break;
		case 2:{
 			giCrg_XScale = 300;
//			giCrg_XScale = 240;
		}break;
		case 3:{
			giCrg_XScale = 600;
		}break;
		case 4:{
			giCrg_XScale = 900;
		}break;
		case 5:{
			giCrg_XScale = 1800;
		}break;
		default:{
			giCrg_XScale = 60;
		}break;
	}
	
	//���ʱ�䵥λ
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg_unit", strUnit, sizeof strUnit,"S");		
	memset(strXCale, 0, sizeof strXCale);
	snprintf(strXCale, sizeof strXCale, "%d %s", giCrg_XScale, strUnit);
	
	//��ʼ������
	InitCrgAttr();

	//������ͼ�߳�
	if(!gbThreadHasCreated_Crg){
		sem_init(&semCrg, 0, 0);
		pthread_create(&ptCrg, NULL, ThreadProcCrg, NULL);	
		if(B_PRINTF) printf("%s:%d create thread of drawing oxyCRG OK!\n", __FILE__, __LINE__);
		usleep(10000);
		gbThreadHasCreated_Crg = TRUE;
	}
	else{
		if(B_PRINTF) printf("%s:%d Thread of drawing oxyCRG has created!\n", __FILE__, __LINE__);
	}

	return 0;
}


int NewChangeCrgScale()
{
	//���X�̶�
	switch (gCfgSystem.bOxyCrgScale){
		case 1:{
			giCrg_XScale = 120;
		}break;
		case 2:{
 			giCrg_XScale = 300;
//			giCrg_XScale = 240;
		}break;
		case 3:{
			giCrg_XScale = 600;
		}break;
		case 4:{
			giCrg_XScale = 900;
		}break;
		case 5:{
			giCrg_XScale = 1800;
		}break;
		default:{
			giCrg_XScale = 60;
		}break;
	}

	//���ʱ�䵥λ
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg_unit", strUnit, sizeof strUnit);		
	memset(strXCale, 0, sizeof strXCale);
	snprintf(strXCale, sizeof strXCale, "%d %s", giCrg_XScale, strUnit);
	
	//��ʼ������
	InitCrgAttr();

	return 0;
}

int NewUpdateCrg()
{
//	if((gCfgSystem.bInterface == SCREEN_OXYCRG))
		sem_post(&semCrg);
			
	return 0;
	
}

