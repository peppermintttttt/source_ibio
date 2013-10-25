/*************************************************************************** 
 *	Module Name:	TrendTable
 *
 *	Abstract:	���Ʊ���ʾ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-20 09:24:50
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

//DID_TRENDTABLE	
#define DID_TT_HELP			DID_TRENDTABLE+1
#define DID_TT_OK			DID_TRENDTABLE+2
#define DID_TT_CURSOR		DID_TRENDTABLE+3
#define DID_TT_TURNOVER		DID_TRENDTABLE+4
#define DID_TT_PRINT			DID_TRENDTABLE+5
#define DID_TT_SETUP			DID_TRENDTABLE+6
#define DID_TT_NIBP			DID_TRENDTABLE+7
#define DID_TT_GRAPH		DID_TRENDTABLE+8
#define DID_TT_NIBPLIST		DID_TRENDTABLE+9

static HWND editHelp;
static HWND btnOK;
static HWND btnCursor;
static HWND btnTurnover;
static HWND btnPrint;
static HWND btnSetup;
static HWND btnNibp;
static HWND btnToGraph;
static HWND btnToNIBPLIST;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgTrendTable = {
	WS_BORDER | WS_VISIBLE ,
	WS_EX_NOCLOSEBOX,   

#if SCREEN_640
	5, 200, 535-115,285,    
#else
	5, 200, 535,285,    
#endif	
 
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlTrendTable[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 80, 1,
		DID_TT_HELP,
		"",
		0
	}
};

//���Ʊ������־
#define TT_PARA_NONE		0
#define TT_PARA_MARK		1
#define TT_PARA_TIME		2
#define TT_PARA_HR			3
#define TT_PARA_SPO2		4
#define TT_PARA_PR			5
#define TT_PARA_NSYS		6
#define TT_PARA_NDIA		7
#define TT_PARA_NMEAN		8
#define TT_PARA_RR			9
#define TT_PARA_T1			10
#define TT_PARA_T2			11
#define TT_PARA_TD			12
#define TT_PARA_ETCO2		13
#define TT_PARA_FICO2		14
#define TT_PARA_AWRR		15
#define TT_PARA_IBP1			16
#define TT_PARA_IBP2			17

//�α��ƶ�����
#define PAGE_INIT		0	//��ʼ��ҳ��
#define PAGE_UP			1	//����(New)
#define PAGE_DOWN		2	//����(Old)

//���Ʊ�����ɫ
#define COLOR_TABLEFRAME	COLOR_darkgray	
//���Ʊ�ı�����ɫ
#define COLOR_TABLEBK		COLOR_black
//���Ʊ��������ɫ
#define COLOR_TABLETEXT		COLOR_lightgray
//���Ʊ�̶���(Mark, Time)�Ŀ��
#define WIDTH_FIXEDCOL		45

//���Ʊ����Ŀ�����ֵĳ���
#define TABLE_ITEM_TXTLENGTH		20		
//���Ʊ������Ŀ�Ľṹ
typedef struct {
	RECT rect;
	unsigned char *strValue[TABLE_ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;

//���Ʊ����ɻ��Ƶ�����(Label, Datas),������Status
#define ROW_MAXCOUNT		11
//���Ʊ����ɻ��Ƶ�����(Mark, Time, Paras)
#define COL_MAXCOUNT		13	
//���Ʊ������ݵĽṹ
typedef struct _TABLE_COL_CONTENT{
	BYTE bColID;	//�б�־
	int iRowCount;	//ʵ�ʵ�����
	TABLE_ITEM ColDatas[ROW_MAXCOUNT];	//������
}TABLE_COL_CONTENT, *PTABLE_COL_CONTENT;

//���Ʊ����ݵĽṹ
typedef struct _TABLE_CONTENT{
	int iColCount;		//ʵ�ʵ�����
	TABLE_COL_CONTENT Datas[COL_MAXCOUNT];	
}TABLE_CONTENT, *PTABLE_CONTENT;

//���Ʊ���ʾ������
static TABLE_CONTENT	gTableContent;

//���Ʊ���
static RECT gRcTableFrame;
//���Ʊ�״̬����
static RECT gRcTableStatus;

//���Ʊ��еĸ߶�
static int giRowHeight;

//--- �ֱ���(����) ----
static int gTableInter = 1;  

//��������
extern CA_TRENDARRAY gTrendDatas;

//�����������������
static CA_TRENDARRAY gSortedTrendDatas;

//�������ݴ���, ���ݴ�����(Head)������ǰ(Tail)��, ����������
static WORD  gCurrentIndex = 0;

//��������ľ��
static HWND hWndSelf = (HWND)NULL;
HWND hWnd_TrendTable = (HWND)NULL;
//�Ƿ���������ε�״̬
static BOOL bBrowsePage = FALSE;
static BOOL bBrowsePage_trend = FALSE;

//��������
static int giTrendDataCount = 0;

//������Ļ����
static int giScreenIndex = 0;
extern int gbTrendStatus;

//��ӡ�����ź���
extern sem_t semReal_Print;

//---------- �������� ------------
//�������Ʊ�
static int InitTrendTable(HWND hWnd);
//����ҳ������
static int UpdateTablePage(BYTE bType);
static int MoveTableScreen();
/*
	�������������
*/
static int SortTrendData()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
	
	//������Ϣ
	gSortedTrendDatas.sPatientInfo = gTrendDatas.sPatientInfo;
	//��ʼʱ��
	gSortedTrendDatas.sStartTime = gTrendDatas.sStartTime;
	
	//��ʼ����������
	gSortedTrendDatas.wHeadIndex = 0;
	gSortedTrendDatas.wTailIndex = 0;
	gSortedTrendDatas.bLoop = FALSE;
	gSortedTrendDatas.wCount = 0;
	
	//������������ȡ��NIBP����, ���MAXTRENDCOUNT���������ڷ�ת�����
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	iCount = 0;
	
	if(wHead < wTail){

		for(i=wTail; i<=(wSize-1); i++){
			
			gSortedTrendDatas.Datas[gSortedTrendDatas.wHeadIndex++] = gTrendDatas.Datas[i]; 
			//gSortedTrendDatas.Datas[0]=gTrendDatas.Datas[i]; 
			iCount++;
		}
		for(i=0; i<=wHead; i++){
			//gSortedTrendDatas.wHeadIndex ++;
			gSortedTrendDatas.Datas[gSortedTrendDatas.wHeadIndex++] = gTrendDatas.Datas[i]; 
			//gSortedTrendDatas.Datas[0]=gTrendDatas.Datas[i]; 
			iCount++;
		}

		
		
	}
	else if(bLoop){
		for(i=wTail; i<=wTail; i++){
	//		gSortedTrendDatas.wHeadIndex ++;
			gSortedTrendDatas.Datas[gSortedTrendDatas.wHeadIndex++] = gTrendDatas.Datas[i]; 
			iCount++;
		}
	}
	else{
		for(i=(wTail+1); i<wHead; i++){
			gSortedTrendDatas.wHeadIndex ++;	
			gSortedTrendDatas.Datas[gSortedTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
			iCount++;
		}
	}
	
	gSortedTrendDatas.wCount = iCount;	
	if(B_PRINTF) printf("%s:%d Sorted %d Trend datas.\n", __FILE__, __LINE__, iCount);
		
	return 0;
}

//��ӡ��ǰ�б�
 int PrintTrendTable()
{
	REC_STRING  sInfo;
	int i,j;
	int res;
	
	sInfo.iRowCount = ROW_MAXCOUNT;
	
	for(j=1; j<gTableContent.iColCount; j++){
		
		//���
		memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
		//���
		for(i=0; i<ROW_MAXCOUNT; i++){
			snprintf(sInfo.strText[i], MAX_TEXT_LENGTH, " %s", 
				 gTableContent.Datas[j].ColDatas[i].strValue);	
// 			if(B_PRINTF) printf("Col:%d Row:%d --- %s\n", j, i, sInfo.strText[i]);
		}
		//��ӡ
		res = Printer_PrintString(&sInfo);
	}
	
	//�հ���ֽ
	Printer_Move_Paper(15);
	return 0;
}

static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	switch(message){
		case MSG_SETFOCUS:{
			if(bBrowsePage_trend){
				SetWindowBkColor(hWnd, BROWSE_COLOR);	
			}
			else{
				SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
			}
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ENTER:{
					switch(id){
						case DID_TT_CURSOR:{
							bBrowsePage_trend = !bBrowsePage_trend;
							if(bBrowsePage_trend){
								//gbKeyType = KEY_LR;
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
					}//end switch(id)
				}break;
			}
		}break;
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}       

static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;

	id = GetDlgCtrlID(hWnd);

	switch(message){
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);

					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}

static int DlgProcTrendTable(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	
	switch(message){
		case MSG_INITDIALOG:{
			gbKeyType = KEY_TAB;
			gbTrendStatus=1;			
			hWndSelf = hDlg;
			hWnd_TrendTable=hDlg;
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);
				
			//����
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "turnover", strMenu, sizeof strMenu);	
			btnSetup = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TT_SETUP, 5, 10, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetup, BtnProc);
// 			EnableWindow(btnSetup, FALSE);
			
			//���
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "cursor", strMenu, sizeof strMenu);	
			btnCursor = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_TT_CURSOR, 5, 40, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCursor, BtnProc);

			//��ӡ
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "print", strMenu, sizeof strMenu);	
			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TT_PRINT, 5, 70, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
			if(gPrinterStatus == PRINT_STATUS_NOPRN)
				EnableWindow(btnPrint, FALSE);
			
			//�л�������ͼ
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendgraph", strMenu, sizeof strMenu,"Graph");	
			btnToGraph= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_TT_GRAPH, 5, 100, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToGraph, BtnProc);

			//�л���NIBPList
			memset(strMenu, 0, sizeof strMenu);                                      
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "nibplist", strMenu, sizeof strMenu,"NIBP");	
			btnToNIBPLIST= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TT_NIBPLIST,5, 130, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToNIBPLIST, BtnProc);
			
			//�˳�
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_TT_OK, 5, 160, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			//��������������
			SortTrendData();
			SetFocus(btnOK);
			
		}break;
		case MSG_PAINT:{
			InitTrendTable(hDlg);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_TT_PRINT:{
					gPrintingType=PRN_WORK_TREND;
					sem_post(&semReal_Print);
					
				}break;
				case DID_TT_SETUP:{
					MoveTableScreen();
				}break;
				case DID_TT_GRAPH:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_G, 0);
				}break;
				case DID_TT_NIBPLIST:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_N, 0);
				}break;
				case DID_TT_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				//case SCANCODE_CURSORBLOCKLEFT:{
				case SCANCODE_A:{
					if(bBrowsePage_trend){
						SetFocus(btnCursor);
						UpdateTablePage(PAGE_UP);
						return TRUE;	
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(bBrowsePage_trend){
						SetFocus(btnCursor);
						UpdateTablePage(PAGE_DOWN);
						return TRUE;	
					}
				}break;
			}
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;
			
			hWndSelf  = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}
	
	temp = temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}



/**
	�������ó�ʼ�����Ʊ�����
	@param:pContent: ���Ʊ�����
*/
static int InitTableContent(PTABLE_CONTENT pContent, int iIndex)
{
	int i, j;
	
	if(pContent == NULL) return -1;
	
	//�������������б�־
	pContent->Datas[0].bColID = TT_PARA_MARK;
	pContent->Datas[1].bColID = TT_PARA_TIME;
	switch(iIndex){
		case 1:{//T1,T2,TD
			pContent->Datas[2].bColID = TT_PARA_T1;
			pContent->Datas[3].bColID = TT_PARA_T2;
			pContent->Datas[4].bColID = TT_PARA_TD;
			pContent->iColCount = 5;		//Mark & Time is Fixed, �������й̶�����
		}break;
		case 2:{//EtCO2,FiCO2,awRR
			pContent->Datas[2].bColID = TT_PARA_ETCO2;
			pContent->Datas[3].bColID = TT_PARA_FICO2;
			pContent->Datas[4].bColID = TT_PARA_AWRR;
			pContent->iColCount = 5;		//Mark & Time is Fixed, �������й̶�����
		}break;
		case 3:{//IBP1,IBP2
			pContent->Datas[2].bColID = TT_PARA_IBP1;
			pContent->Datas[3].bColID = TT_PARA_IBP2;
			pContent->iColCount = 4;		//Mark & Time is Fixed, �������й̶�����	
		}break;
		default :{//HR,SpO2,PR,RR,NSYS,NDIA,NMAP
			pContent->Datas[2].bColID = TT_PARA_HR;
			pContent->Datas[3].bColID = TT_PARA_SPO2;
			pContent->Datas[4].bColID = TT_PARA_PR;
			pContent->Datas[5].bColID = TT_PARA_RR;
			pContent->Datas[6].bColID = TT_PARA_NSYS;
			pContent->Datas[7].bColID = TT_PARA_NDIA;
			pContent->Datas[8].bColID = TT_PARA_NMEAN;	
			//���ʵ������
			pContent->iColCount = 9;		//Mark & Time is Fixed, �������й̶�����
		}break;
	}
	
	//����������������������������ROW_MAXCOUNT��
	pContent->Datas[0].iRowCount =1;	//������һ�б�ǩ	
	pContent->Datas[1].iRowCount =1;	//������һ�б�ǩ	
	
	//��ʼ���б������
	//��
	for(j=0; j<gTableContent.iColCount; j++){
		//��
		for(i=0; i<ROW_MAXCOUNT; i++){
			memset(pContent->Datas[j].ColDatas[i].strValue, 0, TABLE_ITEM_TXTLENGTH);
		}
	}
	
	return 0;
}

/**
	��ʼ�����Ʊ���������
	@param:hWnd: ���ھ��
*/
static int InitTableRects(HWND hWnd)
{
	RECT rcWnd;
	int i, j;
	int iCount;			//�ɱ��е�����
	int iTotalWidth;		//�ɱ�����ռ���ܿ��	
	int iItemWidth;		//�ɱ�����Ŀ��ռ�Ŀ��

	
	if(hWnd == (HWND)NULL){ printf("++++++++\n"); return -1;}

	//��ô��ڵ�����
	GetClientRect(hWnd, &rcWnd);

	//���ÿ�ܵ�����, ȥ����ߵĲ�������
	SetRect(&gRcTableFrame, rcWnd.left+85, rcWnd.top+1, rcWnd.right-1, rcWnd.bottom-1);

	//����������������еĸ߶�
	giRowHeight = (gRcTableFrame.bottom - gRcTableFrame.top)/(ROW_MAXCOUNT+1);	 //����status
	
	//����״̬������
	SetRect(&gRcTableStatus, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right, gRcTableFrame.top+giRowHeight + giRowHeight/4-10);


	//�������ݵ�����
	//�̶���(Mark & Time)
	for(i=0; i<ROW_MAXCOUNT; i++){
		//MARK
		gTableContent.Datas[0].ColDatas[i].rect.left = gRcTableStatus.left;
		gTableContent.Datas[0].ColDatas[i].rect.top = gRcTableStatus.bottom+giRowHeight*i;
		gTableContent.Datas[0].ColDatas[i].rect.right= gTableContent.Datas[0].ColDatas[i].rect.left + 1;
		gTableContent.Datas[0].ColDatas[i].rect.bottom = gTableContent.Datas[0].ColDatas[i].rect.top + giRowHeight;
			
		//TIME
		gTableContent.Datas[1].ColDatas[i].rect.left = gTableContent.Datas[0].ColDatas[i].rect.right;
		gTableContent.Datas[1].ColDatas[i].rect.top = gTableContent.Datas[0].ColDatas[i].rect.top;
		gTableContent.Datas[1].ColDatas[i].rect.right = gTableContent.Datas[1].ColDatas[i].rect.left + WIDTH_FIXEDCOL*2;
		gTableContent.Datas[1].ColDatas[i].rect.bottom = gTableContent.Datas[1].ColDatas[i].rect.top + giRowHeight;
	}


	//�ɱ��У�����������̬�趨����
	iCount = gTableContent.iColCount -2;
	if(iCount !=0){
		iTotalWidth = (gRcTableFrame.right- gTableContent.Datas[1].ColDatas[0].rect.right);
		iItemWidth = iTotalWidth / iCount;
//  		if(B_PRINTF) printf("%s:%d Total %d Cols, Width is %d, Item Width is %d.\n", __FILE__, __LINE__, gTableContent.iColCount, 
//  			iTotalWidth, iItemWidth);
		
		//��
		for(j=2; j<gTableContent.iColCount; j++){
			//��
			for(i=0; i<ROW_MAXCOUNT; i++){
				gTableContent.Datas[j].ColDatas[i].rect.left = gTableContent.Datas[j-1].ColDatas[i].rect.right;
				gTableContent.Datas[j].ColDatas[i].rect.top = gTableContent.Datas[j-1].ColDatas[i].rect.top;
				gTableContent.Datas[j].ColDatas[i].rect.right = gTableContent.Datas[j].ColDatas[i].rect.left + iItemWidth;
				gTableContent.Datas[j].ColDatas[i].rect.bottom = gTableContent.Datas[j].ColDatas[i].rect.top + giRowHeight;
			}
		}
	}


	return 0;
}

/**
	�������Ʊ�Ŀ��
*/
static int DrawTableFrame(HDC hdc)
{
	int i, j;
	int iColCount;		//����
	
	if(hdc == (HDC)NULL) return -1;
	
	SetBrushColor(hdc, COLOR_TABLEBK);
	SetPenColor(hdc, COLOR_TABLEFRAME);
	
	//����
	FillBox(hdc, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right - gRcTableFrame.left, gRcTableFrame.bottom - gRcTableFrame.top);
	
	//���
	Rectangle(hdc, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right, gRcTableFrame.bottom);
	
	
	//״̬����
	MoveTo(hdc, gRcTableStatus.left, gRcTableStatus.bottom);
	LineTo(hdc, gRcTableStatus.right, gRcTableStatus.bottom);

	//��������
	
	//�õ��е�����
	iColCount = gTableContent.iColCount;
	//��
//   for(i=0; i<ROW_MAXCOUNT; i++){
	//�����Ʊ�ǩ��
	for(i=0; i<1; i++){	
		MoveTo(hdc, gTableContent.Datas[0].ColDatas[i].rect.left, gTableContent.Datas[0].ColDatas[i].rect.bottom);
		LineTo(hdc, gRcTableFrame.right, gTableContent.Datas[0].ColDatas[i].rect.bottom);		
	}
	//��
	for(i=1; i<iColCount-1; i++){
		MoveTo(hdc, gTableContent.Datas[i].ColDatas[0].rect.right, gTableContent.Datas[i].ColDatas[0].rect.top);
		LineTo(hdc, gTableContent.Datas[i].ColDatas[0].rect.right, gRcTableFrame.bottom);
	}

// 	//����ȫ������
// 	for(j=0; j<iColCount; j++){
// 			
//  		for(i=0; i<ROW_MAXCOUNT; i++){
// 			Rectangle(hdc, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.left,
// 				    gTableContent.Datas[j].ColDatas[i].rect.top, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.right, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.bottom
// 				   );
// 		}
// 	}
// 
	return 0;
}

/**
	ȡ�����Ʊ�ı�ǩ��Ϣ
*/
static int GetTableLabels(PTABLE_CONTENT pContent)
{
	//������ǩ���������־��Ӧ,���ǵ���ӡ������Ӣ��
// 	unsigned char *strLabels[]={"","", "time", 
// 		"hr", "spo2", "pr", "nsys", "ndia", "nmean", "rr", "temp1", "temp2", "td", "etco2", "fico2","awrr","ibp1","ibp2"};
	unsigned char *strLabels[]={"","", "TIME", 
		"HR", "SpO2", "PR", "SYS", "DIA", "MAP", "RR", "TEMP1", "TEMP2", "TD", "EtCO2", "FiCO2","awRR","IBP1","IBP2"};
	unsigned char *strLabels_Span[]={"","", "Hora", 
		"FC", "SpO2", "FP", "SIS", "DIA", "MED", "FR", "T1", "T2", "DT", "EtCO2", "FiCO2","awRR","IBP1","IBP2"};
		
		
			
	unsigned char strMenu[20]={0};
	BYTE bID;
	int i;		
		
	if(pContent == NULL) return -1;
	
	for(i=0; i<(pContent->iColCount); i++){
		//���ǵ���ӡ������Ӣ��
// 		memset(strMenu, 0, sizeof strMenu);
// 		GetStringFromResFile(gsLanguageRes, "GENERAL", strLabels[pContent->Datas[i].bColID], strMenu, sizeof strMenu);	
// 		snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strMenu);
		if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strLabels_Span[pContent->Datas[i].bColID]);
		else
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strLabels[pContent->Datas[i].bColID]);
	}
		
	return 0;
}

/**
	�������Ʊ�ı�ǩ
*/
static int UpdateTableLabels(HDC hdc)
{
	RECT rc;
	int i;
	int res;
	
	if(hdc == (HDC)NULL) return -1;
	
	//��ñ�ǩ
	res = GetTableLabels(&gTableContent);
			
	if(res != -1){
		SetBkColor(hdc, COLOR_TABLEBK);
		SetTextColor(hdc, COLOR_TABLETEXT);

		for(i=0; i<(gTableContent.iColCount); i++){
//  			if(B_PRINTF) printf("Label(%d) is %s.\n", i, gTableContent.Datas[i].ColDatas[0].strValue);
			DrawText(hdc, (unsigned char *)(gTableContent.Datas[i].ColDatas[0].strValue), -1, 
				 &(gTableContent.Datas[i].ColDatas[0].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
	else{
		return -1;
	}
	
	return 0;
}

/**
	���һ������
	@param:iRowIndex �к�
*/
static int FillTableItem(PTABLE_CONTENT pContent, const PTRENDDATA pTrendData, int iRowIndex)
{
	BYTE bID;
	int i;
	unsigned char strValue[TABLE_ITEM_TXTLENGTH] = {0};
	S_TIME sTmpTime;
	double fTmp;
	int iTmp;
	
	if(pContent == NULL) return -1;
	
	//0��Label
	if(iRowIndex<1) iRowIndex = 1;
	
	if(pTrendData == NULL){
		//��ձ��
		for(i=0; i<(pContent->iColCount); i++){
			bID = pContent->Datas[i].bColID;
			pContent->Datas[i].iRowCount = iRowIndex +1;
			memset(strValue, sizeof strValue, 0);
			memset((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, 0);
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, "%s", strValue);
		}	
			
		return -1;
	} 
	
	for(i=0; i<(pContent->iColCount); i++){
		bID = pContent->Datas[i].bColID;
		pContent->Datas[i].iRowCount = iRowIndex +1;
		memset(strValue, sizeof strValue, 0);
		//��ü�¼��ʱ��
		sTmpTime = pTrendData->sTime;
			
		switch(bID){
			case TT_PARA_TIME:{
				snprintf(strValue, sizeof strValue, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
			}break;
			case TT_PARA_HR:{
				if(pTrendData->dwHaveFlag & HAVEHR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wHR);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case TT_PARA_SPO2:{
				if(pTrendData->dwHaveFlag & HAVESPO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->bSpO2);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case TT_PARA_NSYS:{
                		//�ж��Ƿ���NIBP����
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
						snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wSys);
					else
						snprintf(strValue, sizeof strValue, "%.1f", (float)(pTrendData->sNIBP.wSys) / 7.5);
				}
				else{
					snprintf(strValue, sizeof strValue, "---");
				}
			}break;
			case TT_PARA_NDIA:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
						snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wDia);                         
					else
						snprintf(strValue, sizeof strValue, "%1f", (float)(pTrendData->sNIBP.wDia) / 7.5);
						
				}
				else{
					snprintf(strValue, sizeof strValue, "---");
				}
			}break;
			case TT_PARA_NMEAN:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
						snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wMean);                         
					else
						snprintf(strValue, sizeof strValue, "%1f", pTrendData->sNIBP.wMean / 7.5);
				}
				else{
					snprintf(strValue, sizeof strValue, "---");
				}
			}break;
			case TT_PARA_RR:{
				if(pTrendData->dwHaveFlag & HAVERESP)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wRR);                         
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;             
			case TT_PARA_PR:{
				if(pTrendData->dwHaveFlag & HAVEPR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wPR);                           
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case TT_PARA_T1:{
				if(pTrendData->dwHaveFlag & HAVET1){
					if(gCfgTemp.bUnit == TEMP_UNIT_C)
						snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[0]))/10.0));
					else
						snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[0]))/10.0)*1.8 + 32.0);
				}
				else
					snprintf(strValue, sizeof strValue, "--.-");
			}break;
			case TT_PARA_T2:{
				if(pTrendData->dwHaveFlag & HAVET2){
					if(gCfgTemp.bUnit == TEMP_UNIT_C)
						snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[1]))/10.0));
					else
						snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[1]))/10.0)*1.8 + 32.0);
				}
				else
					snprintf(strValue, sizeof strValue, "--.-");
			}break;
			case TT_PARA_TD:{
				if((pTrendData->dwHaveFlag & HAVET2) &&(pTrendData->dwHaveFlag & HAVET1)) {
					if(gCfgTemp.bUnit == TEMP_UNIT_C)
						snprintf(strValue, sizeof strValue, "%3.1f", (abs(pTrendData->wTemps[0]-pTrendData->wTemps[1]) ) /10.0);
					else
						snprintf(strValue, sizeof strValue, "%3.1f", ((abs(pTrendData->wTemps[0]-pTrendData->wTemps[1])) /10.0) * 1.8);
				}
				else
					snprintf(strValue, sizeof strValue, "--.-");
			}break;
			case TT_PARA_ETCO2:{
				if(pTrendData->dwHaveFlag & HAVECO2){
					if(gCfgCO2.bUnit == CO2UNIT_MMHG)
						snprintf(strValue, sizeof strValue, "%d", pTrendData->sCO2.wEtCO2);                           
					else
						if(gCfgCO2.bUnit == CO2UNIT_PER)
							snprintf(strValue, sizeof strValue, "%3.1f", (float)(pTrendData->sCO2.wEtCO2*100.0/gValueCO2.wBaro));
				}	
				else
					snprintf(strValue, sizeof strValue, "--");	
			}break;
			case TT_PARA_FICO2:{
				if(pTrendData->dwHaveFlag & HAVECO2){
					if(gCfgCO2.bUnit == CO2UNIT_MMHG)
						snprintf(strValue, sizeof strValue, "%d", pTrendData->sCO2.wFiCO2);                           
					else if(gCfgCO2.bUnit == CO2UNIT_PER)
						snprintf(strValue, sizeof strValue, "%3.1f", (float)(pTrendData->sCO2.wFiCO2*100.0/gValueCO2.wBaro));
				}	
				else
					snprintf(strValue, sizeof strValue, "--");	
			}break;
			case TT_PARA_AWRR:{
				if(pTrendData->dwHaveFlag & HAVECO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sCO2.wAwRR);                           
				else
					snprintf(strValue, sizeof strValue, "--");	
			}break;
			case TT_PARA_IBP1:{
				if(pTrendData->dwHaveFlag & HAVEIBP1){
					switch(gCfgIbp1.bUnit){
						//case IBPUNIT_MMHG
							default:{
							snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
									pTrendData->sIBPs[0].iSys, pTrendData->sIBPs[0].iDia, pTrendData->sIBPs[0].iMean);
						}break;
						case IBPUNIT_KPA:{
							snprintf(strValue, sizeof strValue, "%.1f/%.1f (%.1f)", 
									(float)(pTrendData->sIBPs[0].iSys)/7.5, (float)(pTrendData->sIBPs[0].iDia)/7.5, (float)(pTrendData->sIBPs[0].iMean)/7.5);
						}break;                   
						/*case IBPUNIT_CMH2O:{
							snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
									(int)(pTrendData->sIBPs[0].iSys*1.36), (int)(pTrendData->sIBPs[0].iDia*1.36), (int)(pTrendData->sIBPs[0].iMean*1.36));
						}break;*/
					}
				}
				else
					snprintf(strValue, sizeof strValue, "--/-- (--)");	
			}break;
			case TT_PARA_IBP2:{
				if(pTrendData->dwHaveFlag & HAVEIBP2){
					switch(gCfgIbp2.bUnit){
						//case IBPUNIT_MMHG
							default:{
							snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
									pTrendData->sIBPs[1].iSys, pTrendData->sIBPs[1].iDia, pTrendData->sIBPs[1].iMean);
						}break;
						case IBPUNIT_KPA:{
							snprintf(strValue, sizeof strValue, "%.1f/%.1f (%.1f)", 
									(float)(pTrendData->sIBPs[1].iSys)/7.5, (float)(pTrendData->sIBPs[1].iDia)/7.5, (float)(pTrendData->sIBPs[1].iMean)/7.5);
						}break;                   
						/*case IBPUNIT_CMH2O:{
							snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
									(int)(pTrendData->sIBPs[1].iSys*1.36), (int)(pTrendData->sIBPs[1].iDia*1.36), (int)(pTrendData->sIBPs[1].iMean*1.36));
						}break;*/
					}
				}
				else
					snprintf(strValue, sizeof strValue, "--/-- (--)");	
			}break;
			default:{
				snprintf(strValue, sizeof strValue, "");
			}break;
		}
//  		if(B_PRINTF) printf("%s:%d COL %d  : %s \n", __FILE__, __LINE__, i, strValue);
		memset((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, 0);
		snprintf((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, "%s", strValue);
// 		if(B_PRINTF) printf("%s:%d COL %d  : %s \n", __FILE__, __LINE__, i, 
// 			 pContent->Datas[i].ColDatas[iRowIndex].strValue);
	}
	return 0;
}

/**
	�������������
	@param:pContent ���
	@param:wStartIndex ��ʼ����
	@param:iCount ��ȡ������
*/
static int FillTable(PTABLE_CONTENT pContent, WORD wStartIndex, int iCount)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	long int  i,j;              
	int iRow = 1;
	
	//��ȡ���������ܳ�������ʾ����������
	if(iCount > ROW_MAXCOUNT-1) iCount = ROW_MAXCOUNT - 1;
	
	//���Table
	for(i=1; i<ROW_MAXCOUNT; i++){
		FillTableItem(pContent, NULL, i);
	}
	
	//�� wStartIndex ����ȡ iCount�����ݣ�����䵽���ṹ��
	wHead = wStartIndex;
	wTail = gSortedTrendDatas.wTailIndex;
	bLoop = gSortedTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	//����������
	for(i=wHead, j=0; i>wTail && j<iCount; i--, j++){
		FillTableItem(pContent, &(gSortedTrendDatas.Datas[i]), iRow);
		iRow ++;
	}

	return 0;
}

/*
	������������
*/
static int UpdateTableDatas(HWND hWnd)
{
	int res;
	int i, j;
	HDC hdc = (HDC)NULL;
	
	hdc = GetClientDC(hWnd);
	
	if(hdc == (HDC)NULL) return -1;
	
	SelectFont(hdc, gFontSystem);
	
	res = FillTable(&gTableContent, gCurrentIndex, ROW_MAXCOUNT);
	
	if(res != -1){
		//�����
		SetBkColor(hdc, COLOR_TABLEBK);
		SetTextColor(hdc, COLOR_TABLETEXT);
		SetBrushColor(hdc, COLOR_TABLEBK);
		
		for(i=0; i<(gTableContent.iColCount); i++){
			for(j=1; j<ROW_MAXCOUNT; j++){
				//�������
				FillBox(hdc,
					gTableContent.Datas[i].ColDatas[j].rect.left+1, 
					gTableContent.Datas[i].ColDatas[j].rect.top+1,
					gTableContent.Datas[i].ColDatas[j].rect.right-1 - gTableContent.Datas[i].ColDatas[j].rect.left,
					gTableContent.Datas[i].ColDatas[j].rect.bottom-1 - gTableContent.Datas[i].ColDatas[j].rect.top
				       );
				
				//�������		
				DrawText(hdc, (unsigned char *)(gTableContent.Datas[i].ColDatas[j].strValue), -1, 
					 &(gTableContent.Datas[i].ColDatas[j].rect), 
					 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
		}
	}
	else{
		ReleaseDC(hdc);
		return -1;
	}
	
	ReleaseDC(hdc);
	return 0;

}

/*
	����״̬���� 
*/
static int UpdateTableStatus(HWND hWnd)
{
	HDC hdc = (HDC)NULL;
	unsigned char strStatus[50] = {0};	
	int iPageCount=0;
	int iCurPage=0;

	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	//��������������һҳ��ʾ����������ҳ��
	if((giTrendDataCount % ROW_MAXCOUNT)>0){
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1) +1;
	}
	else{
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1);
	}
				
	//���㵱ǰҳ��
 	iCurPage = iPageCount -  gCurrentIndex / (ROW_MAXCOUNT -1);

	memset(strStatus, 0, sizeof strStatus);
 	snprintf(strStatus, sizeof strStatus, "%d / %d ", iCurPage, iPageCount);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, gRcTableStatus.left, gRcTableStatus.top, RECTW(gRcTableStatus), RECTH(gRcTableStatus) );
	
	SetBkColor(hdc, COLOR_TABLEBK);
	SetTextColor(hdc, COLOR_TABLETEXT);
	DrawText(hdc, "Trend List", -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	DrawText(hdc, strStatus, -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

	ReleaseDC(hdc);
	
	return 0;
}

/**
	����
*/
static int MoveTableScreen()
{
	int res;
	HDC hdc = (HDC)NULL;
	printf("11111111111111%x\n",hWndSelf);
	giScreenIndex ++;
	
	if(!gbHaveCo2 && giScreenIndex == 2) giScreenIndex ++;
	if(!gbHaveIbp && giScreenIndex == 3) giScreenIndex++;
	
	if(giScreenIndex>3) giScreenIndex = 0;

	//��ʼ�����Ʊ����
	InitTableContent(&gTableContent, giScreenIndex);
		printf("22222222222%x\n",hWndSelf);	
	//��ʼ�����Ʊ���������
	res = InitTableRects(hWndSelf);
		printf("333333\n");	
 	//��û�ͼDC
	hdc = GetClientDC(hWndSelf);
	if(hdc == (HDC)NULL) return -1;
	SelectFont(hdc, gFontSystem);
 	
 	//���ƿ��
	res = DrawTableFrame(hdc);
	if(res == -1){
		if(B_PRINTF) printf("%s:%d Draw Table Frame Failure.\n", __FILE__, __LINE__);
		ReleaseDC(hdc);
		return -1;
	}

 	//���±�ǩ
	UpdateTableLabels(hdc);
	
	ReleaseDC(hdc);
		printf("4444444444444\n");	
	UpdateTableDatas(hWndSelf);
		printf("5555555555555555\n");	
	UpdateTableStatus(hWndSelf);
		printf("66666666\n");	
	return 0;
}


/**
	��ҳ
	�ı� startIndex ��ֵ
	@param:bType PAGE_XXX
*/
static int MoveTablePage(BYTE bType)
{
	
	giTrendDataCount = gSortedTrendDatas.wCount;
	
	switch(bType){
		case PAGE_UP:{
			gCurrentIndex += ROW_MAXCOUNT-1;	
		}break;
		case PAGE_DOWN:{
			gCurrentIndex -= ROW_MAXCOUNT-1;	
		}break;
		default:{
			gCurrentIndex = gSortedTrendDatas.wHeadIndex;
		}break;
	}
	
	if(gCurrentIndex > gSortedTrendDatas.wHeadIndex) gCurrentIndex = gSortedTrendDatas.wHeadIndex;
	if(gCurrentIndex < gSortedTrendDatas.wTailIndex) gCurrentIndex = gSortedTrendDatas.wTailIndex;

	return 0;
}

/**
	����ҳ������
	@param:bType PAGE_XXX
 */
static int UpdateTablePage(BYTE bType)
{
	//��ǰ����������
	MoveTablePage(bType);
 	//��������
	UpdateTableDatas(hWndSelf);
	UpdateTableStatus(hWndSelf);
	
	return 0;
}

/*
	�������ô��������б�
*/
static int InitTrendTable(HWND hWnd)
{
	HDC hdc = (HDC)NULL;
	int res;
	
	if(hWnd == (HWND)NULL) return -1;
	
	//�������ó�ʼ�����Ʊ�����
	InitTableContent(&gTableContent, giScreenIndex);
	
	//��ʼ�����Ʊ���������
	res = InitTableRects(hWnd);
	
 	//��û�ͼDC
 	hdc = GetClientDC(hWnd);
 	if(hdc == (HDC)NULL) return -1;
 	SelectFont(hdc, gFontSystem);
 	
 	//���ƿ��
 	res = DrawTableFrame(hdc);
 	if(res == -1){
 		if(B_PRINTF) printf("%s:%d Draw Table Frame Failure.\n", __FILE__, __LINE__);
 		ReleaseDC(hdc);
 		return -1;
 	}
 			
 	//���±�ǩ
 	UpdateTableLabels(hdc);
	
	

	ReleaseDC(hdc);
	
	//��ǰ����������
	MoveTablePage(PAGE_INIT);
			
	//�õ�������                                     
	giTrendDataCount = gSortedTrendDatas.wCount;
	if(B_PRINTF) printf("%s:%d Count is %d.\n", __FILE__, __LINE__, giTrendDataCount);
	
 	//��������
 	UpdateTableDatas(hWnd);
	UpdateTableStatus(hWnd);

	return 0;
}


//-------------------------------------------------- interface ----------------------------------------------------
/**
	����������ʾ�Ի���
 */
void CreateTrendTable(HWND hWnd)
{

	//��ʼ������ͼ����
	int High_Dlg;
	if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=HEIGHT_CTL;


#if	SCREEN_640
	DlgTrendTable.x = SCREEN_LEFT,
	DlgTrendTable.y = SCREEN_BOTTOM-285, 		

#else
	DlgTrendTable.x = SCREEN_LEFT,
	DlgTrendTable.y = SCREEN_BOTTOM-285-High_Dlg, 		
#endif

	
	DlgTrendTable.controls = CtrlTrendTable;
	DialogBoxIndirectParam(&DlgTrendTable, hWnd, DlgProcTrendTable, 0L);
}


