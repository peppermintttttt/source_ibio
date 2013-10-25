/*************************************************************************** 
 *	Module Name:	NibpTable
 *
 *	Abstract:	NIBP���Ʊ���ʾ
 *			˼·����������������ȡ��NIBP������ʾ
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

//DID_NIBPLIST

#define DID_NL_HELP				DID_NIBPLIST+1
#define DID_NL_OK				DID_NIBPLIST+2
#define DID_NL_CURSOR			DID_NIBPLIST+3
#define DID_NL_PRINT			DID_NIBPLIST+4
#define DID_NL_SETUP			DID_NIBPLIST+5
#define DID_NL_TRENDGRAPH			DID_NIBPLIST+6
#define DID_NL_TRENDLIST			DID_NIBPLIST+7

static HWND editHelp;
static HWND btnOK;
static HWND btnCursor;
static HWND btnPrint;
static HWND btnSetup;
static HWND btnNibp;
static HWND btnToTrendGraph;
static HWND btnToTrendList;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgSet = {
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

static CTRLDATA CtrlSet[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 1, 1,
		DID_NL_HELP,
		"",
		0
	}
};

//���Ʊ������־
#define NL_PARA_NONE		0
#define NL_PARA_MARK		1
#define NL_PARA_TIME		2
#define NL_PARA_HR			3
#define NL_PARA_SPO2		4
#define NL_PARA_PR			5
#define NL_PARA_NSYS		6
#define NL_PARA_NDIA		7
#define NL_PARA_NMEAN		8
#define NL_PARA_RR			9
#define NL_PARA_T1			10
#define NL_PARA_T2			11
#define NL_PARA_TD			12
#define NL_PARA_CO2			13
#define NL_PARA_IBP1		14
#define NL_PARA_IBP2		15

//�α��ƶ�����
#define PAGE_INIT			0	//��ʼ��ҳ��
#define PAGE_UP				1	//����(New)
#define PAGE_DOWN			2	//����(Old)

//���Ʊ�����ɫ
#define COLOR_TABLEFRAME		COLOR_darkgray	
//���Ʊ�ı�����ɫ                        	
#define COLOR_TABLEBK			COLOR_black
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
#define ROW_MAXCOUNT	11
//���Ʊ����ɻ��Ƶ�����(Mark, Time, Paras)
#define COL_MAXCOUNT		13	
//���Ʊ������ݵĽṹ
typedef struct _TABLE_COL_CONTENT{
	BYTE bColID;		//�б�־
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

//�����ȡNIBP�����������
static CA_TRENDARRAY gNibpTrendDatas;

//�������ݴ���, ���ݴ�����(Head)������ǰ(Tail)��, ����������
static WORD  gCurrentIndex = 0;

//��������ľ��
static HWND hWndSelf = (HWND)NULL;

HWND hWnd_NibpTable = (HWND)NULL;
//�Ƿ���������ε�״̬
static BOOL bBrowsePage = FALSE;

//��������
static int giTrendDataCount = 0;
extern int gbTrendStatus;
//��ӡ�����ź���
extern sem_t semReal_Print;
//---------- �������� ------------
//�������Ʊ�
static int InitNibpTable(HWND hWnd);
//����ҳ������
static int UpdateNibpPage(BYTE bType);


/*
	��������������ȡ��NIBP����
*/
static int PickupNibpData()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
	
	//������Ϣ
	gNibpTrendDatas.sPatientInfo = gTrendDatas.sPatientInfo;
	//��ʼʱ��
	gNibpTrendDatas.sStartTime = gTrendDatas.sStartTime;
	
	//��ʼ��NIBP����
	gNibpTrendDatas.wHeadIndex = 0;
	gNibpTrendDatas.wTailIndex = 0;
	gNibpTrendDatas.bLoop = FALSE;
	gNibpTrendDatas.wCount = 0;
	
	//������������ȡ��NIBP����, ���MAXTRENDCOUNT���������ڷ�ת�����
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	iCount = 0;
	if(wHead < wTail){
// 		for(i=wHead; i>=0; i--){
		for(i=wTail; i<=(wSize-1); i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
// 		for(i=(wSize-1); i>=wTail; i--){
		for(i=0; i<=wHead; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else if(bLoop){
// 		for(i=wHead; i>=wTail; i--){
		for(i=wTail; i<=wTail; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else{
// 		for(i=wHead; i>wTail; i--){
		for(i=(wTail+1); i<=wHead; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;	
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	
SEARCH_OVER:
		gNibpTrendDatas.wCount = iCount;	
		if(B_PRINTF) printf("%s:%d Pick up %d NIBP datas.\n", __FILE__, __LINE__, iCount);
		
	return 0;
}


//��ӡ��ǰ�б�
 int PrintNIBPTable()
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
			snprintf(sInfo.strText[i], MAX_TEXT_LENGTH, "%s ", 
				 gTableContent.Datas[j].ColDatas[i].strValue);	
 	//		if(B_PRINTF) printf("Col:%d Row:%d --- %s\n", j, i, sInfo.strText[i]);
		}
		//��ӡ
		res = Printer_PrintString(&sInfo);
	}
	
	//�հ���ֽ
//	Printer_MovePaper(15);
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
			if(bBrowsePage){
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
						case DID_NL_CURSOR:{
							bBrowsePage = !bBrowsePage;
							if(bBrowsePage){
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

static int DlgProcNIBPTable(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	
	switch(message){
		case MSG_CREATE:{
			gbKeyType = KEY_TAB;
			gbTrendStatus=2;			
			hWndSelf = hDlg;
                     hWnd_NibpTable=hDlg;
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);
				
// 			//����
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "setup", strMenu, sizeof strMenu);	
// 			btnSetup = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
// 					DID_NL_SETUP, 5, 10, 70, 25, hDlg, 0);
// 			OldBtnProc = SetWindowCallbackProc(btnSetup, BtnProc);
// 			EnableWindow(btnSetup, FALSE);
			
			//���
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "cursor", strMenu, sizeof strMenu);	
			btnCursor = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_NL_CURSOR, 5, 10, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCursor, BtnProc);
			
			//��ӡ
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "print", strMenu, sizeof strMenu);	
			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_NL_PRINT, 5, 40, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
			if(gPrinterStatus == PRNSTATUS_ERR)
				EnableWindow(btnPrint, FALSE);
			
			//�л�������ͼ
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendgraph", strMenu, sizeof strMenu,"Graph");	
			btnToTrendGraph= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_TRENDGRAPH, 5, 70, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToTrendGraph, BtnProc);		
			//�л��������б�
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendlist", strMenu, sizeof strMenu,"List");	
			btnToTrendList= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_TRENDLIST, 5, 100, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToTrendList, BtnProc);							
			//�˳�
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_OK, 5, 130, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			//��ȡNIBP����
			PickupNibpData();
			
			SetFocus(btnOK);
		}break;
		case MSG_PAINT:{
			InitNibpTable(hDlg);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NL_PRINT:{
					gPrintingType=PRN_WORK_NIBP;
					sem_post(&semReal_Print);
					
				//	PrintNIBPTable();
				}break;
				case DID_NL_TRENDGRAPH:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_G, 0);
				}break;
				case DID_NL_TRENDLIST:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_T, 0);
				}break;
				case DID_NL_OK:{
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
					if(bBrowsePage){
						SetFocus(btnCursor);
						UpdateNibpPage(PAGE_UP);
						return TRUE;	
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(bBrowsePage){
						SetFocus(btnCursor);
						UpdateNibpPage(PAGE_DOWN);
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
static int InitTableContent(PTABLE_CONTENT pContent)
{
	int i, j;
	
	if(pContent == NULL) return -1;
	
		
	//�������������б�־
	pContent->Datas[0].bColID = NL_PARA_MARK;
	pContent->Datas[1].bColID = NL_PARA_TIME;
	//TODO:���������ļ�����
	pContent->Datas[2].bColID = NL_PARA_NSYS;
	pContent->Datas[3].bColID = NL_PARA_NDIA;
	pContent->Datas[4].bColID = NL_PARA_NMEAN;
	pContent->Datas[5].bColID = NL_PARA_HR;
//	pContent->Datas[6].bColID = NL_PARA_SPO2;
//	pContent->Datas[7].bColID = NL_PARA_RR;
// 	pContent->Datas[8].bColID = NL_PARA_T1;
// 	pContent->Datas[9].bColID = NL_PARA_T2;
		
	//���ʵ������
	pContent->iColCount = 6;		//Mark & Time is Fixed, �������й̶�����
	
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
	
	if(hWnd == (HWND)NULL) return -1;
	
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
// 	unsigned char *strLabels[]={"","", "time", "hr", "spo2", "pr", "nsys", "ndia", "nmean", "rr", "temp1", "temp2", "td", "etco2", "ibp1","ibp2"};
	unsigned char *strLabels[]={"","", "TIME", "HR", "SpO2", "PR", "SYS", "DIA", "MAP", "RR", "TEMP1", "TEMP2", "Td", "EtCO2", "IBP1","IBP2"};
	unsigned char *strLabels_Span[]={"","", "Hora", "FC", "SpO2", "FP", "SIS", "DIA", "MED", "FR", "T1", "T2", "DT", "EtCO2", "IBP1","IBP2"};
	
	unsigned char strMenu[20]={0};
	BYTE bID;
	int i;		
		
	if(pContent == NULL) return -1;
	
	for(i=0; i<(pContent->iColCount); i++){
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
			case NL_PARA_TIME:{
				snprintf(strValue, sizeof strValue, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
			}break;
			case NL_PARA_HR:{
				if(pTrendData->dwHaveFlag & HAVEHR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wHR);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_SPO2:{
				if(pTrendData->dwHaveFlag & HAVESPO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->bSpO2);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_NSYS:{
                		//�ж��Ƿ���NIBP����
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wSys);
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_NDIA:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wDia);                         
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_NMEAN:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wMean);                         
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_RR:{
				if(pTrendData->dwHaveFlag & HAVERESP)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wRR);                         
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;             
			case NL_PARA_PR:{
				if(pTrendData->dwHaveFlag & HAVEPR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wPR);                           
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_T1:{
				if(pTrendData->dwHaveFlag & HAVET1)
					snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[0]))/10.0));
				else
					snprintf(strValue, sizeof strValue, "--.-");
// 				snprintf(strValue, sizeof strValue, "37.5");
			}break;
			case NL_PARA_T2:{
				if(pTrendData->dwHaveFlag & HAVET2)
					snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[1]))/10.0));
				else
					snprintf(strValue, sizeof strValue, "--.-");
				
// 				snprintf(strValue, sizeof strValue, "37.5");
			}break;
			case NL_PARA_TD:{
				snprintf(strValue, sizeof strValue, "--.-");
			}break;
			case NL_PARA_CO2:{
				if(pTrendData->dwHaveFlag & HAVECO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sCO2.wEtCO2);                           
				else
					snprintf(strValue, sizeof strValue, "--");	
			}break;
			case NL_PARA_IBP1:{
				if(pTrendData->dwHaveFlag & HAVEIBP1)
					snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
						pTrendData->sIBPs[0].iSys, pTrendData->sIBPs[0].iDia, pTrendData->sIBPs[0].iMean);                           
				else
					snprintf(strValue, sizeof strValue, "--/-- (--)");	
			}break;
			case NL_PARA_IBP2:{
				if(pTrendData->dwHaveFlag & HAVEIBP1)
					snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
						pTrendData->sIBPs[0].iSys, pTrendData->sIBPs[0].iDia, pTrendData->sIBPs[0].iMean);                           
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
	wTail = gTrendDatas.wTailIndex;
	bLoop = gNibpTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(wHead < wTail){
		for(i=wHead, j=0; i>=0 && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
		for(i=(wSize-1); i>=wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
	}
	else if(bLoop){
		for(i=wHead, j=0; i>=wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
	}
	else{
		for(i=wHead, j=0; i>wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
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
	HDC hdc;
	
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
			for(j=0; j<ROW_MAXCOUNT; j++){
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
	HDC hdc;
	unsigned char strStatus[50] = {0};	
	int iPageCount=0;
	int iCurPage=0;

	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	//��������������һҳ��ʾ����������ҳ��
	if((giTrendDataCount % (ROW_MAXCOUNT-1))>0){
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1) +1;
		//���㵱ǰҳ��
		iCurPage = iPageCount -  gCurrentIndex / (ROW_MAXCOUNT -1);
		printf("11111111111111111111\n");
	}
	else{
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1);
				//���㵱ǰҳ��
		iCurPage = iPageCount -  gCurrentIndex / (ROW_MAXCOUNT -1)+1;

		printf("222222222222222222222222\n");
	}
				
	

	memset(strStatus, 0, sizeof strStatus);
	snprintf(strStatus, sizeof strStatus, "%d / %d ", iCurPage, iPageCount);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, gRcTableStatus.left, gRcTableStatus.top, RECTW(gRcTableStatus), RECTH(gRcTableStatus) );
	
	SetBkColor(hdc, COLOR_TABLEBK);
	SetTextColor(hdc, COLOR_TABLETEXT);
	DrawText(hdc, "NIBP List", -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	DrawText(hdc, strStatus, -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

	ReleaseDC(hdc);
	
	return 0;
}

/**
	��ҳ
	�ı� startIndex ��ֵ
	@param:bType PAGE_XXX
 */
static int MoveTablePage(BYTE bType)
{
	
	giTrendDataCount = gNibpTrendDatas.wCount;
	
	switch(bType){
		case PAGE_UP:{
			gCurrentIndex += ROW_MAXCOUNT-1;	
		}break;
		case PAGE_DOWN:{
			gCurrentIndex -= ROW_MAXCOUNT-1;	
		}break;
		default:{
			gCurrentIndex = gNibpTrendDatas.wHeadIndex;
		}break;
	}
	
	if(gCurrentIndex > gNibpTrendDatas.wHeadIndex) gCurrentIndex = gNibpTrendDatas.wHeadIndex;
	if(gCurrentIndex < gNibpTrendDatas.wTailIndex) gCurrentIndex = gNibpTrendDatas.wTailIndex;

	return 0;
}

/**
	����ҳ������
	@param:bType PAGE_XXX
 */
static int UpdateNibpPage(BYTE bType)
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
static int InitNibpTable(HWND hWnd)
{
	HDC hdc;
	int res;
	
	if(hWnd == (HWND)NULL) return -1;
	
	//�������ó�ʼ�����Ʊ�����
	InitTableContent(&gTableContent);
	
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
	giTrendDataCount = gNibpTrendDatas.wCount;
	
 	//��������
	UpdateTableDatas(hWnd);
	UpdateTableStatus(hWnd);

	
	return 0;
}

//-------------------------------------------------- interface ----------------------------------------------------
/**
	����������ʾ�Ի���
 */
void CreateNibpTable(HWND hWnd)
{

	//��ʼ������ͼ����
	int High_Dlg;
	if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=HEIGHT_CTL;

#if SCREEN_640
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285, 	
#else
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285-High_Dlg, 		
#endif

	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProcNIBPTable, 0L);    
}
