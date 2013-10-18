/*************************************************************************** 
 *	Module Name:	NibpList
 *
 *	Abstract:	NIBP�б���ͼ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-08-15 14:32:04
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "DrawWave.h"

//��ɫ
#define NLIST_BKCOLOR		COLOR_blue
#define NLIST_TEXTCOLOR	COLOR_lightgray
#define NLIST_PENCOLOR		COLOR_lightgray

//NibpList��ʾ���
#define NLISTPANEL		4000
#define NLISTLABEL		4001
static HWND gPanel = (HWND)NULL;
static HWND gLabel = (HWND)NULL;
static WNDPROC  OldProcPanel;
static WNDPROC  OldProcLabel;

//������ڵ�����
static RECT rcLabel = {5, 380, 540, 400};
static RECT rcPanel= {5, 403, 540, 565};

//�����Ƶ�NIBP����
#define NLIST_COUNT	7
//����
#define NLIST_ROW_COUNT	(NLIST_COUNT+1)
//����(ʱ��, NSYS, NDIA, NMEAN, HR, SpO2, PR)
#define NLIST_COL_COUNT 	7
//�б���ʾ�����ֵĳ���
#define ITEM_TXTLENGTH		25	
//�б�����
typedef struct {
	RECT rect;
	unsigned char *strValue[ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;
//�¼��б�(������ͷ) 
static TABLE_ITEM gTableEvent[NLIST_ROW_COUNT][NLIST_COL_COUNT];

//�¼��������
static RECT gRcForm;

//��������
extern CA_TRENDARRAY gTrendDatas;

//����б�����
static int FillItem(int iRowIndex, const PTRENDDATA pTrendData)
{
// 	unsigned char strValue[ITEM_TXTLENGTH] = {0};
	S_TIME sTmpTime;
	int i;
	
	if(pTrendData == NULL) return -1;
	
	//0��Label
	if(iRowIndex<1) iRowIndex = 1;
	
	//Time
	sTmpTime = pTrendData->sTime;
	snprintf((unsigned char *)(gTableEvent[iRowIndex][0].strValue), ITEM_TXTLENGTH, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
	
	//NSYS
	snprintf((unsigned char *)(gTableEvent[iRowIndex][1].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wSys);
	
	//NDIA
	snprintf((unsigned char *)(gTableEvent[iRowIndex][2].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wDia);
	
	//NMEAN
	snprintf((unsigned char *)(gTableEvent[iRowIndex][3].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wMean);
	
	//HR
	if(pTrendData->dwHaveFlag & HAVEHR)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "%d", pTrendData->wHR);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "---");
	
	//SpO2
	if(pTrendData->dwHaveFlag & HAVESPO2)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][5].strValue), ITEM_TXTLENGTH, "%d", pTrendData->bSpO2);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][5].strValue), ITEM_TXTLENGTH, "--");
	
	//RR
	if(pTrendData->dwHaveFlag & HAVERESP)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][6].strValue), ITEM_TXTLENGTH, "%d", pTrendData->wRR);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][6].strValue), ITEM_TXTLENGTH, "---");
		
	
	return 0;
}

/*
	˼·:�����������������NLIST_COUNT��NIBP��¼���
*/
static int FillDatas()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
		
	//��ʼ���������
	for(i=0; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			memset(gTableEvent[i][j].strValue, 0, ITEM_TXTLENGTH);
			memset(gTableEvent[i][j].strValue, 0, ITEM_TXTLENGTH);
		}
	}

	//��д��ͷ
	GetStringFromResFile(gsLanguageRes, "GENERAL", "time", gTableEvent[0][0].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nsys", gTableEvent[0][1].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "ndia", gTableEvent[0][2].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nmean", gTableEvent[0][3].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "hr", gTableEvent[0][4].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "spo2", gTableEvent[0][5].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "rr", gTableEvent[0][6].strValue, ITEM_TXTLENGTH);	
		
	if(gTrendDatas.wCount == 0){
		if(B_PRINTF) printf("%s:%d No Trend data.\n", __FILE__, __LINE__);
		return -1;	
	}

	//������������ȡ��ǰ NLIST_COUNT ��NIBP����
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;

	iCount =1;
	if(wHead < wTail){
		for(i=wHead; i>=0; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}         
		for(i=(wSize-1); i>=wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
// 					if(B_PRINTF) printf("----Found NIBP Trend index %d.\n", i);
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else{
		for(i=wHead; i>wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
// 					if(B_PRINTF) printf("----Found NIBP Trend index %d.\n", i);
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	
SEARCH_OVER:
	
	return 0;
}

//���Ʊ�ǩ
static int DrawLabel(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	unsigned char strLabel[50]={0};
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	
	//����
// 	SetBrushColor(hdc, COLOR_darkblue);
	SetBrushColor(hdc, COLOR_lightgray);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	//���Ʊ�ǩ
// 	SetBkColor(hdc, COLOR_darkblue);
// 	SetTextColor(hdc, NLIST_TEXTCOLOR);	
	SetBkColor(hdc, COLOR_lightgray);
	SetTextColor(hdc, COLOR_black);	
	SelectFont(hdc, gFontSystem);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nibplist", strLabel, sizeof strLabel);				
	DrawText(hdc, strLabel, -1, &rc, DT_NOCLIP | DT_SINGLELINE | DT_CENTER | DT_TOP | DT_VCENTER);
	
	ReleaseDC(hdc);
	
	return 0;
}

/*
	���ƿ��
*/
static int DrawFrame(HDC hdc)
{                                                                                                     
	int i, j;
	
	if(hdc == (HDC)NULL) return -1;
	
	SetPenColor(hdc, NLIST_PENCOLOR);
	SetBkColor(hdc, NLIST_BKCOLOR);
	SetTextColor(hdc, NLIST_TEXTCOLOR);
	SelectFont(hdc, gFontSystem);	
	
	Rectangle(hdc, gRcForm.left, gRcForm.top, gRcForm.right, gRcForm.bottom);
							
	//�����Ʊ�ǩ��
	for(i=0; i<1; i++){	
		MoveTo(hdc, gTableEvent[0][i].rect.left, gTableEvent[0][i].rect.bottom);
		LineTo(hdc, gRcForm.right, gTableEvent[0][i].rect.bottom);		
	}
	//��
	for(i=0; i<NLIST_COL_COUNT-1; i++){
		MoveTo(hdc, gTableEvent[0][i].rect.right, gTableEvent[0][i].rect.top);
		LineTo(hdc, gTableEvent[0][i].rect.right, gRcForm.bottom);
	}
	

// 	//�¼����
// 	for(i=0; i<NLIST_ROW_COUNT; i++){
// 		for(j=0; j<NLIST_COL_COUNT; j++){
// 			Rectangle(hdc, 
// 				  gTableEvent[i][j].rect.left, 
// 				  gTableEvent[i][j].rect.top, 
// 				  gTableEvent[i][j].rect.right, 
// 				  gTableEvent[i][j].rect.bottom);
// 		}
// 	}

	return 0;
}

//������������
static int DrawDatas(HWND hWnd)
{
	HDC hdc;
	RECT rc;
// 	unsigned char strLabel[50]={0};
	int i, j;
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	SetBkColor(hdc, NLIST_BKCOLOR);
	SetTextColor(hdc, NLIST_TEXTCOLOR);
	SelectFont(hdc, gFontEnglish_16);
	
	//�������
	FillDatas();
	
	//����
	SetBrushColor(hdc, NLIST_BKCOLOR);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
		
	//��ͷ
	for(i=0; i<NLIST_COL_COUNT; i++){
		DrawText(hdc, (unsigned char *)(gTableEvent[0][i].strValue), -1, 
			 &(gTableEvent[0][i].rect), 
			 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

// 	SelectFont(hdc, gFontSystem);
	//����
	for(i=1; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			DrawText(hdc, (unsigned char *)(gTableEvent[i][j].strValue), -1, 
				 &(gTableEvent[i][j].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
	
	DrawFrame(hdc);
			
	ReleaseDC(hdc);
	
	return 0;
}

/**
	�������Ļص�����
 */
static int ProcPanel(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;		
	HDC hdc;
                                                                                 
	temp = (*OldProcPanel)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			DrawDatas(hWnd);
		}break;
	}
		
	return(temp);
}

static int ProcLabel(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;		
	
	temp = (*OldProcLabel)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
 			DrawLabel(hWnd);
		}break;
	}

	return(temp);
}

		
static int CreateNList(HWND hParentWnd)
{
	RECT rc;
	int i,j;
	int iWidth;
	int iHeight;
	
	if(hParentWnd == (HWND)NULL) return -1;
	
	//������ڵ�����
// 	static RECT rcLabel = {5, 380, 540, 400};
// 	static RECT rcPanel= {5, 403, 540, 565};
	
	if(gbHaveIbp){
		SetRect(&rcLabel, 5, VW_NIBPLIST_BOTTOM_EXT+10, 540, VW_NIBPLIST_BOTTOM_EXT+30);
		SetRect(&rcPanel, 5, VW_NIBPLIST_BOTTOM_EXT+30+2, 540, VW_NORMAL_BOTTOM_EXT+10);	
	}
	else{
		SetRect(&rcLabel, 5, 380, 540, 400);
		SetRect(&rcPanel, 5, 403, 540, 565);	
	}
	
	gPanel =  CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
			       NLISTPANEL, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel), hParentWnd, 0L);   	
	OldProcPanel = SetWindowCallbackProc(gPanel, ProcPanel);
	
	gLabel =  CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
				NLISTLABEL, rcLabel.left, rcLabel.top, RECTW(rcLabel), RECTH(rcLabel), hParentWnd, 0L);   	
	OldProcLabel = SetWindowCallbackProc(gLabel, ProcLabel);
	
	//��ʼ���б����ݽṹ
	GetClientRect(gPanel, &rc);
	SetRect(&gRcForm, rc.left+3, rc.top+1, rc.right-3, rc.bottom-1);
	
	//Timeռ����
	iWidth = RECTW(gRcForm) / (NLIST_COL_COUNT+1);
	iHeight = RECTH(gRcForm) / NLIST_ROW_COUNT;

	
	for(i=0; i<NLIST_ROW_COUNT; i++){
		//Time
		SetRect(&(gTableEvent[i][0].rect), 
			  gRcForm.left, 
			  gRcForm.top+i*iHeight, 
			  gRcForm.left+2*iWidth, 
			  gRcForm.top+(i+1)*iHeight);
		//Other
		for(j=1; j<NLIST_COL_COUNT; j++){
			SetRect(&(gTableEvent[i][j].rect), 
				  gRcForm.left+ (j+1)*iWidth, 
				  gRcForm.top+i*iHeight, 
				  gRcForm.left+(j+2)*iWidth, 
				  gRcForm.top+(i+1)*iHeight);
		}
	}
	
	
	return 0;
}

/**
	�����������
 */
static int DestroyPanel(HWND *phWnd)
{
	
	if(*phWnd == (HWND)NULL) return -1;
	
	DestroyWindow(*phWnd);
	*phWnd = (HWND)NULL;		
	
	return 0;	
}

//------------------------- interface ------------------------------
/**
	��ʼ��������	
	rcShortTrend : �������������
 */
int InitNibpList(HWND hWnd)
{
	int res;
	if(hWnd == (HWND)NULL) return -1;
				

	CreateNList(hWnd);
	//��ʾ���
	ShowWindow(gPanel, SW_SHOW);
	ShowWindow(gLabel, SW_SHOW);
	
	return 0;
}

/**
	���ٶ�����
 */
int DestroyNibpList()
{
	int res;
	
	//�����������
	res = DestroyPanel(&gPanel);
	res = DestroyPanel(&gLabel);

	return res;
}

/*
	����NIBP�б�
*/
int UpdateNibpList()
{
	if(gPanel != (HWND)NULL){
		DrawDatas(gPanel);
	}	
	return 0;
}

