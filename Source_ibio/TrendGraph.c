/*************************************************************************** 
 *	Module Name:	TrendGraph
 *
 *	Abstract:	����ͼ��ʾ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-13 14:23:57
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

//DID_TRENDGRAPH
#define DID_TG_HELP				DID_TRENDGRAPH+1
#define DID_TG_OK				DID_TRENDGRAPH+2
#define DID_TG_CURSOR			DID_TRENDGRAPH+3
#define DID_TG_PAGEUP			DID_TRENDGRAPH+4
#define DID_TG_TIME				DID_TRENDGRAPH+5
#define DID_TG_TIME_SET			DID_TRENDGRAPH+6
#define DID_TG_PRINT				DID_TRENDGRAPH+7
#define DID_TG_P0				DID_TRENDGRAPH+8
#define DID_TG_P0_SET			DID_TRENDGRAPH+9
#define DID_TG_P1				DID_TRENDGRAPH+10
#define DID_TG_P1_SET			DID_TRENDGRAPH+11
#define DID_TG_P2				DID_TRENDGRAPH+12
#define DID_TG_P2_SET			DID_TRENDGRAPH+13
#define DID_TG_PAGEDOWN		DID_TRENDGRAPH+14
#define DID_TG_TRENDLIST		DID_TRENDGRAPH+15
#define DID_TG_NIBPLIST			DID_TRENDGRAPH+16

static HWND editHelp;
static HWND btnOK;
static HWND btnCursor;
static HWND btnPageUp;
static HWND btnSetTimeScale;
static HWND cobTimeScale;
static HWND btnPrint;
static HWND btnSetPara0;
static HWND cobPara0;
static HWND btnSetPara1;
static HWND cobPara1;
static HWND btnSetPara2;
static HWND cobPara2;
static HWND btnPageDown;
static HWND btnToTrendList;
static HWND btnToNibpList;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgTrendGraph= {
	WS_BORDER | WS_VISIBLE ,
	WS_EX_NOCLOSEBOX,   
// 	5, 220, 545, 352,    
//	5, 200, 555, 375,    
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

static CTRLDATA CtrlTrendGraph[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 80, 1,
		DID_TG_HELP,
		"",
		0
	}
};

/**
	��������ͼ�Ķ���
*/
//��������ͼ��ʾ������ID
#define TG_PARA_NONE		0
#define TG_PARA_HR			1
#define TG_PARA_SPO2		2
#define TG_PARA_PR			3
#define TG_PARA_NIBP		4
#define TG_PARA_RR			5
#define TG_PARA_TEMP		6
#define TG_PARA_CO2		7
#define TG_PARA_IBP1		8
#define TG_PARA_IBP2		9

#define TG_PARA_COUNT	10

//�����߶εĿ��
#define TG_LINE_WIDTH		1

//����ͼ��ʾ�ķ�Χ
//HR
#define TG_HR_MIN			HR_MIN
#define TG_HR_MAX			HR_MAX
//SpO2
#define TG_SPO2_MIN		80
#define TG_SPO2_MAX		SPO2_MAX
//PR
#define TG_PR_MIN			PR_MIN
#define TG_PR_MAX			PR_MAX
//RR
#define TG_RR_MIN			RR_MIN
#define TG_RR_MAX			RR_MAX
//TEMP
#define TG_TEMP_MIN		TEMP_MIN
#define TG_TEMP_MAX		TEMP_MAX
//NIBP
#define TG_NIBP_MIN			NIBP_MIN
#define TG_NIBP_MAX			NIBP_MAX
//CO2
#define TG_CO2_MIN			CO2_MIN
#define TG_CO2_MAX			70
//IBP1
#define TG_IBP1_MIN			IBP_MIN
#define TG_IBP1_MAX			IBP_MAX
//IBP2
#define TG_IBP2_MIN			IBP_MIN
#define TG_IBP2_MAX			IBP_MAX


//��ɫ
#define TG_BKCOLOR				COLOR_black
#define TG_TEXTCOLOR			COLOR_darkgray
#define TG_PENCOLOR			COLOR_darkgray
#define TG_CUSORCOLOR			COLOR_yellow


//�α��ƶ�����
#define CURSOR_INIT			0	//��ʼ���α�
#define CURSOR_LEFT			1	//����(OLd)
#define CURSOR_RIGHT			2	//����(New)

//��ҳ�ķ�Χ
#define MOVE_FULLPAGE		0	//�ƶ�����ҳ��
#define MOVE_PARTPAGE		1	//�ƶ��ֲ�ҳ��

//����ͼʱ��̶ȱ�ǵ���С���(min)
#define GRAPH_TIME_MININTER	15

//ÿҳ������ͼ������
#define TREND_GRAPH_COUNT	3

//��������
extern CA_TRENDARRAY gTrendDatas;

typedef struct {
	BYTE bID;
	RECT rcGraph;		//����ͼ��ʾ����
	RECT rcData;		//������ʾ����
	int xScale;		//xֵ�ķ�Χ(����),���ں���Ҫ��ʾָ��ʱ���ڵ�ȫ������
	int yMin;			//y����Сֵ�������������Ƶ���Сֵ
	int yMax;			//y�����ֵ�������������Ƶ����ֵ
	double fRatio_x;       //X��Ļ�ͼ����
	double fRatio_y	;	//Y��Ļ�ͼ����	
	int iColor;			//����ͼ������ɫ
}TREND_GRAPH,*PTREND_GRAPH;

//��������ͼ
TREND_GRAPH gTrendGraphs[TREND_GRAPH_COUNT];

//����ͼ�������
static RECT gRcGraphFrame;	
//����ͼ״̬�������ڣ�ҳ��..	
static RECT gRcGraphStatus;
//����ͼʱ��̶���ʾ����
static RECT gRcGraphTime;

//ʱ���Ӧ���ص�ı���������ʱ��̶����ͼ����Ŀ�����
static double gfTimeToPixel = 0.0;		//Time / gfTimeToPixel = Pixel

//�������ݴ���, ���ݴ�����(��)������ǰ(��)��, ����ʱ�������
static S_TIME  gRightTime = {0};
static S_TIME  gLeftTime = {0};

//����ͼ�α��λ��
static S_TIME  gCursorTime = {0};

//����ͼʱ��̶�(��Χ, Сʱ)
static int giGraphTimeRange = 2;

//����ƶ���ť�Ƿ���
static BOOL gbBtnCursor = FALSE;

//ҳ�水ť�Ƿ���
static BOOL gbBtnPage = FALSE;
static int giHavePage=0;
static int giCurPage=0;
unsigned char *strHaveibpandco2[]={"HR","SPO2","PR","NIBP","RR","TEMP","EtCO2","IBP1", "IBP2"};
unsigned char *strTemp[15];
extern int gbTrendStatus;
static BOOL bPageUp=FALSE;
//--------------------- �������� ---------------------
//��ʼ����ͼ����
static int InitGraphRects(HWND hWnd);
//��������ͼ���
static int DrawGraphFrame(HWND  hWnd);
//��ʼ������ͼ
static int InitTrendGraph(HWND hWnd);
//��������ͼ
static int UpdateGraphPage(HWND hWnd);
//�ƶ��α�	
static int GraphMoveCursor(HWND hWnd,  BYTE bMove);
//��ʼ������ͼ����
static int InitGraphCoord(HWND hWnd, PTREND_GRAPH pTrendGraph);
//���Ʋ���  �����ػ�ʱ����
static int DrawData(HWND hWnd);
//��ҳ
static int GraphMovePage(HWND hWnd, BYTE bMove, BYTE bSize);

/**
	����ʵ�ֵĺ���
*/
//��������ľ��
static HWND hWndSelf = (HWND)NULL;
HWND hWnd_TrendGraph = (HWND)NULL;
static TRENDDATA tmpTrendData[4320];
static int GetHavePage(int giGraphTimeRange)
{
	int page=0;
	page=MAXTRENDCOUNT/(giGraphTimeRange*60);
	if(giGraphTimeRange<48)
		page=page-1;
	return page;
}
static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	switch(message){
		case MSG_SETFOCUS:{
			if(gbBtnCursor || gbBtnPage){
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
						case DID_TG_CURSOR:{
							gbBtnCursor = !gbBtnCursor;
							if(gbBtnCursor){
								//gbKeyType = KEY_LR;
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
						case DID_TG_PAGEUP:{
								GraphMovePage(hWndSelf, CURSOR_LEFT, MOVE_FULLPAGE);
							
						}break;		
						//case DID_TG_PAGEDOWN:{
					//		GraphMovePage(hWndSelf, CURSOR_RIGHT, MOVE_FULLPAGE);
					//	}break;
						case DID_TG_PAGEDOWN:{
							gbBtnPage = !gbBtnPage;
							if(gbBtnPage){
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
				//case SCANCODE_CURSORBLOCKLEFT:{
				case SCANCODE_A:{
					if(gbBtnCursor){
						SetFocus(btnCursor);
						GraphMoveCursor(hWndSelf, CURSOR_LEFT);
						return TRUE;
 					} 
					if(gbBtnPage){
						SetFocus(btnPageUp);
						GraphMovePage(hWndSelf, CURSOR_LEFT, MOVE_FULLPAGE);
						return TRUE;	
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(gbBtnCursor){
						SetFocus(btnCursor);
						GraphMoveCursor(hWndSelf, CURSOR_RIGHT);
						return TRUE;
					} 
 					if(gbBtnPage){
						SetFocus(btnPageUp);
 						GraphMovePage(hWndSelf, CURSOR_RIGHT, MOVE_FULLPAGE);
 						return TRUE;	
 					}
				}break;
			}//end switch(wParam)
		}//end MSG_KEYUP
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}       
static int GetScaleTime(BYTE index)
{
	int TempScaleTime;
	switch(index){
		case 0: TempScaleTime = 1; break;
		case 1: TempScaleTime = 2; break;
		case 2: TempScaleTime = 4; break;
		case 3: TempScaleTime = 6; break;
		case 4: TempScaleTime = 8; break;
		case 5: TempScaleTime = 10; break;
		case 6: TempScaleTime = 12; break;
		case 7: TempScaleTime = 24; break;
		case 8: TempScaleTime = 48; break;
		case 9: TempScaleTime = 72; break;
		default: TempScaleTime = 1; break;
	}
	
	return TempScaleTime;
}
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	RECT rc;
	HDC hdc= (HDC)NULL;
	
	GetWindowRect(hWnd, &rc);
	
	id = GetDlgCtrlID(hWnd);
	
	switch(message){
		case MSG_SETFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_KILLFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
					int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
					switch(id){
						case DID_TG_TIME:{
							//���ʱ��̶�
							if(TrendGraphCfg.bScale!=index){
								TrendGraphCfg.bScale=index;
								SetIntValueToResFile(gFileSetup, "TrendGraphSetup", "Scale",  TrendGraphCfg.bScale, 1);
						
								giGraphTimeRange=GetScaleTime(TrendGraphCfg.bScale);
								giHavePage=GetHavePage(giGraphTimeRange);
								
								//�ؽ�����ͼ
								UpdateWindow(hWndSelf,TRUE);
							}
							SetFocus(btnSetTimeScale);
						}break;
						case DID_TG_P0:{
							if(TrendGraphCfg.bPara1!=index){
								while(index==TrendGraphCfg.bPara2||index==TrendGraphCfg.bPara3){
									index=(index+1)%Count;
								}
								TrendGraphCfg.bPara1=index;
								gTrendGraphs[0].bID = index+1;
								SendMessage(cobPara0, CB_SETCURSEL, gTrendGraphs[0].bID-1, 0);
								SetIntValueToResFile(gFileSetup, "TrendGraphSetup", "Para1",  TrendGraphCfg.bPara1, 1);
								//�ؽ�����ͼ
								UpdateWindow(hWndSelf,TRUE);
							}
							SetFocus(btnSetPara0);
						}break;
						case DID_TG_P1:{
							if(TrendGraphCfg.bPara2!=index){
								while(index==TrendGraphCfg.bPara1||index==TrendGraphCfg.bPara3){
									index=(index+1)%Count;
								}
								TrendGraphCfg.bPara2=index;
								gTrendGraphs[1].bID = index+1;
								SendMessage(cobPara1, CB_SETCURSEL, gTrendGraphs[1].bID-1, 0);

								SetIntValueToResFile(gFileSetup, "TrendGraphSetup", "Para2",  TrendGraphCfg.bPara2, 1);
								
								//�ؽ�����ͼ
								UpdateWindow(hWndSelf,TRUE);
							}
							SetFocus(btnSetPara1);
						}break;
						case DID_TG_P2:{
							if(TrendGraphCfg.bPara3!=index){
								while(index==TrendGraphCfg.bPara1||index==TrendGraphCfg.bPara2){
									index=(index+1)%Count;
								}
								TrendGraphCfg.bPara3=index;
								gTrendGraphs[2].bID = index+1;
								SendMessage(cobPara2, CB_SETCURSEL, gTrendGraphs[2].bID-1, 0);
								SetIntValueToResFile(gFileSetup, "TrendGraphSetup", "Para3",  TrendGraphCfg.bPara3, 1);

								//�ؽ�����ͼ
								UpdateWindow(hWndSelf,TRUE);
							}
							SetFocus(btnSetPara2);
						}break;
					}
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}
static int SetCobList(HWND hDlg)
{
	unsigned char *strPara1[10],*strPara2[10],*strPara3[10];
	int ilen;
	int icount =0;
	int i;
	//��ȡÿ���б���ַ�
	for(i=0;i<6;i++)
		strTemp[i]=strHaveibpandco2[i];
	ilen=6;
	
	if(gbHaveCo2){
		strTemp[6]=strHaveibpandco2[6];
		ilen=7;
	}else if(gbHaveIbp){
		for(i=6;i<8;i++)
			strTemp[i]=strHaveibpandco2[i+1];
		ilen=8;
	}else if(gbHaveCo2 && gbHaveIbp){
		for(i=6;i<9;i++)
			strTemp[i]=strHaveibpandco2[i];
		ilen=9;
	}

	//����б���ַ�
	for(i=0;i<ilen;i++){
		SendMessage(cobPara0, CB_ADDSTRING, 0, (LPARAM)strTemp[i]);
		SendMessage(cobPara1, CB_ADDSTRING, 0, (LPARAM)strTemp[i]);
		SendMessage(cobPara2, CB_ADDSTRING, 0, (LPARAM)strTemp[i]);
	}
	
	SendMessage(cobPara0, CB_SETCURSEL, TrendGraphCfg.bPara1, 0);	
	SendMessage(cobPara1, CB_SETCURSEL, TrendGraphCfg.bPara2, 0);	
	SendMessage(cobPara2, CB_SETCURSEL, TrendGraphCfg.bPara3, 0);	
	
	return 0;
}
static int DlgProcTrendGraph(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	unsigned char *strTime[]={"1 Hr", "2 Hr", "4 Hr", "6 Hr", "8 Hr", "10 Hr", "12 Hr", "24 Hr", "48 Hr", "72 Hr"};
	unsigned char *strTemp[10];
	
	int i;
	
	switch(message){
		case MSG_INITDIALOG:{
			gbKeyType = KEY_TAB;
						
			hWndSelf = hDlg;
			hWnd_TrendGraph=hDlg;
			gbTrendStatus=0;
			gbBtnCursor = FALSE;
			gbBtnPage = FALSE;
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDGRAPH", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);
				
			//����0
 			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "parameter1", strMenu, sizeof strMenu,"Area 1");	
 			btnSetPara0 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
 					DID_TG_P0_SET, 2, 5, 60, 26, hDlg, 0);
 			OldBtnProc = SetWindowCallbackProc(btnSetPara0, BtnProc);
			
			cobPara0 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_TG_P0, 65, 5, 60, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobPara0, CobProc);
			
			//����1
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "parameter2", strMenu, sizeof strMenu,"Area 2");	
			btnSetPara1 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_P1_SET, 2, 35, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetPara1, BtnProc);
			
			cobPara1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_TG_P1, 65, 35, 60, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobPara1, CobProc);
			
			//����2
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "parameter3", strMenu, sizeof strMenu,"Area 3");	
			btnSetPara2 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_P2_SET, 2, 65, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetPara2, BtnProc);
			
			cobPara2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_TG_P2, 65, 65, 60, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobPara2, CobProc);

			//���ò���1~3��cobѡ��
			SetCobList(hDlg);
			
			//ʱ��̶�
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "time_scale", strMenu, sizeof strMenu,"Scale");	
 			btnSetTimeScale = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
 					       DID_TG_TIME_SET, 2, 95, 60, 26, hDlg, 0);
 			OldBtnProc = SetWindowCallbackProc(btnSetTimeScale, BtnProc);           
			
			cobTimeScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_TG_TIME, 65, 95, 60, 28, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobTimeScale, CobProc);
			for(i=0; i<10; i++){
				SendMessage(cobTimeScale, CB_ADDSTRING, 0, (LPARAM)strTime[i]);
			}
			//Ĭ��ʱ��̶�Ϊ2Сʱ
			SendMessage(cobTimeScale, CB_SETCURSEL, TrendGraphCfg.bScale, 0);
			giGraphTimeRange=GetScaleTime(TrendGraphCfg.bScale);
/*			
			//Page 
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDGRAPH", "page_up", strMenu, sizeof strMenu);	
			btnPageUp = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_PAGEUP,2, 230, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPageUp, BtnProc);
*/
			//Page Down
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "page", strMenu, sizeof strMenu,"Page");	
			btnPageDown = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_PAGEDOWN,2, 125, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPageDown, BtnProc);
			
			//������
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDGRAPH", "cursor", strMenu, sizeof strMenu,"Browse");	
			btnCursor = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_CURSOR,2, 155, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCursor, BtnProc);
			//EnableWindow(btnCursor, FALSE);			//��ʱ���Σ� ��������п��ܵ������� 2009-12-18 14:29:37
			
			//��ӡ
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "TRENDGRAPH", "print", strMenu, sizeof strMenu);	
// 			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
// 					DID_TG_PRINT, 350, 323, 90, 25, hDlg, 0);
// 			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
// 			EnableWindow(btnPrint, FALSE);
			//�л���TrendList
			memset(strMenu, 0, sizeof strMenu);                                      
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendlist", strMenu, sizeof strMenu,"List");	
			btnToTrendList= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_TRENDLIST,2, 185, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToTrendList, BtnProc);
			
			//�л���NIBPList
			memset(strMenu, 0, sizeof strMenu);                                      
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "nibplist", strMenu, sizeof strMenu,"NIBP");	
			btnToNibpList= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_NIBPLIST,2, 215, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToNibpList, BtnProc);
			
			//�˳�
			memset(strMenu, 0, sizeof strMenu);                                      
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu,"Exit");	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TG_OK,2, 245, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			

			
			InitTrendGraph(hDlg);
			SetFocus(btnOK);
		}break;
		case MSG_PAINT:{
			//��ʼ������ͼ
			InitTrendGraph(hDlg);
		
			break;
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_TG_TIME_SET:{
					gbKeyType = KEY_UD;
					SetFocus(cobTimeScale);
				}break;
				case DID_TG_P0_SET:{
					gbKeyType = KEY_UD;
					SetFocus(cobPara0);
				}break;
				case DID_TG_P1_SET:{
					gbKeyType = KEY_UD;
					SetFocus(cobPara1);
				}break;
				case DID_TG_P2_SET:{
					gbKeyType = KEY_UD;
					SetFocus(cobPara2);
				}break;
				case DID_TG_TRENDLIST:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_T, 0);
				}break;
				case DID_TG_NIBPLIST:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_N, 0);
				}break;
				case DID_TG_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
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


//----------------- ����ͼ���ƺ��� -------------------
/**
	��ʼ������ͼ����,�Ի�����ʱ����
	@param	hWnd: ��ͼ�Ĵ���
*/
static int InitGraphRects(HWND hWnd)
{
	RECT rc;
	
	GetClientRect(hWnd, &rc);
	
	//��ͼ����
	SetRect(&gRcGraphFrame, rc.left+128, rc.top+1, rc.right-2, rc.bottom -1);
		
	//Para0
	SetRect(&(gTrendGraphs[0].rcGraph),rc.left+130, rc.top+3, rc.right-5, rc.top+81);
	SetRect(&(gTrendGraphs[0].rcData),rc.left+5, rc.top+35, rc.left+120, rc.top+65);
	//Para1
	SetRect(&(gTrendGraphs[1].rcGraph),rc.left+130, rc.top+83, rc.right-5, rc.top+163);
	SetRect(&(gTrendGraphs[1].rcData),rc.left+5, rc.top+100, rc.left+120, rc.top+130);
	//Para2
	SetRect(&(gTrendGraphs[2].rcGraph),rc.left+130, rc.top+165, rc.right-5, rc.top+245);
	SetRect(&(gTrendGraphs[2].rcData),rc.left+5, rc.top+165, rc.left+120, rc.top+195);
	
	//Time
	SetRect(&gRcGraphTime,rc.left+130, rc.top+246, rc.right-5, rc.bottom -3);
	
	//Status (Date, Page .etc)
//	SetRect(&gRcGraphStatus,rc.left+130, rc.bottom -60, rc.right-5, rc.bottom -37);

	return 0;
}

/**
	��ʼ������ͼ����
*/
static int InitGraphCoord(HWND hWnd, PTREND_GRAPH pTrendGraph)
{
	
	HDC hdc;
	//������������
	int iHeight; 
	//������
	unsigned char strname[10]={0};
	unsigned char strTop[5] = {0};
	unsigned char strMiddle[5] = {0};
	unsigned char strBottom[5] = {0};
	
	if(pTrendGraph == NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	
	//xֵ�ķ�Χ(����),���ں���Ҫ��ʾָ��ʱ���ڵ�ȫ������
	pTrendGraph->xScale = giGraphTimeRange * 60;
	
	//Y��ķ�Χ
	switch(pTrendGraph->bID){
		case TG_PARA_HR:{
			pTrendGraph->yMin = TG_HR_MIN;
			pTrendGraph->yMax = TG_HR_MAX;
			snprintf(strname, sizeof strname, "HR");
			snprintf(strTop, sizeof strTop, "%d", TG_HR_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_HR_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_HR_MAX-TG_HR_MIN)/2+TG_HR_MIN);
		}break;
		case TG_PARA_SPO2:{
			pTrendGraph->yMin = TG_SPO2_MIN;
			pTrendGraph->yMax = TG_SPO2_MAX;
			snprintf(strname, sizeof strname, "SPO2");
			snprintf(strTop, sizeof strTop, "%d", TG_SPO2_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_SPO2_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_SPO2_MAX-TG_SPO2_MIN)/2+TG_SPO2_MIN);
		}break;
		case TG_PARA_PR:{
			pTrendGraph->yMin = TG_PR_MIN;
			pTrendGraph->yMax = TG_PR_MAX;
			snprintf(strname, sizeof strname, "PR");
			snprintf(strTop, sizeof strTop, "%d", TG_PR_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_PR_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_PR_MAX-TG_PR_MIN)/2+TG_PR_MIN);
		}break;
		case TG_PARA_RR:{
			pTrendGraph->yMin = TG_RR_MIN;
			pTrendGraph->yMax = TG_RR_MAX;
			snprintf(strname, sizeof strname, "RR");
			snprintf(strTop, sizeof strTop, "%d", TG_RR_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_RR_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_RR_MAX-TG_RR_MIN)/2+TG_RR_MIN);
		}break;
		case TG_PARA_NIBP:{
			pTrendGraph->yMin = TG_NIBP_MIN;
			pTrendGraph->yMax = TG_NIBP_MAX;
			snprintf(strname, sizeof strname, "NIBP");
			snprintf(strTop, sizeof strTop, "%d", TG_NIBP_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_NIBP_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_NIBP_MAX-TG_NIBP_MIN)/2+TG_NIBP_MIN);
		}break;
		case TG_PARA_TEMP:{
			pTrendGraph->yMin = TG_TEMP_MIN;
			pTrendGraph->yMax = TG_TEMP_MAX;
			snprintf(strname, sizeof strname, "TEMP");
			snprintf(strTop, sizeof strTop, "%f", (float)TG_TEMP_MAX / 10.0);
			snprintf(strBottom, sizeof strBottom, "%f", (float)TG_TEMP_MIN / 10.0);
			snprintf(strMiddle, sizeof strMiddle, "%f", (float)((TG_TEMP_MAX-TG_TEMP_MIN)/2+TG_TEMP_MIN) / 10.0);
		}break;
		case TG_PARA_CO2:{
			pTrendGraph->yMin = TG_CO2_MIN;
			pTrendGraph->yMax = TG_CO2_MAX;
			snprintf(strname, sizeof strname, "CO2");
			snprintf(strTop, sizeof strTop, "%d", TG_CO2_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_CO2_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_CO2_MAX-TG_CO2_MIN)/2+TG_CO2_MIN);
		}break;
		case TG_PARA_IBP1:{
			pTrendGraph->yMin = TG_IBP1_MIN;
			pTrendGraph->yMax = TG_IBP1_MAX;
			snprintf(strname, sizeof strname, "IBP1");
			snprintf(strTop, sizeof strTop, "%d", TG_IBP1_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_IBP1_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_IBP1_MAX-TG_IBP1_MIN)/2+TG_IBP1_MIN);
		}break;
		case TG_PARA_IBP2:{
			pTrendGraph->yMin = TG_IBP2_MIN;
			pTrendGraph->yMax = TG_IBP2_MAX;
			snprintf(strname, sizeof strname, "IBP2");
			snprintf(strTop, sizeof strTop, "%d", TG_IBP2_MAX);
			snprintf(strBottom, sizeof strBottom, "%d", TG_IBP2_MIN);
			snprintf(strMiddle, sizeof strMiddle, "%d", (TG_IBP2_MAX-TG_IBP2_MIN)/2+TG_IBP2_MIN);
		}break;
	}
			
	//X����ʱ�������صı���
	pTrendGraph->fRatio_x = gfTimeToPixel;

	//Y�������������صı���
	iHeight = pTrendGraph->rcGraph.bottom - pTrendGraph->rcGraph.top;
	pTrendGraph->fRatio_y  = (double)((double)iHeight / (double)(pTrendGraph->yMax - pTrendGraph->yMin)) ;	
		
	//��������
	SetBkColor(hdc, TG_BKCOLOR);
	SetTextColor(hdc, TG_TEXTCOLOR);
	SelectFont(hdc, gFontSystem);
	DrawText(hdc, strname, -1, &(pTrendGraph->rcGraph), DT_NOCLIP | DT_SINGLELINE | DT_BOTTOM | DT_CENTER);
	DrawText(hdc, strTop, -1, &(pTrendGraph->rcGraph), DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_LEFT);
	DrawText(hdc, "    ", -1, &(pTrendGraph->rcGraph), DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	DrawText(hdc, strMiddle, -1, &(pTrendGraph->rcGraph), DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	DrawText(hdc, strBottom, -1, &(pTrendGraph->rcGraph), DT_NOCLIP | DT_SINGLELINE | DT_BOTTOM | DT_LEFT);
	
	//���
	SetPenColor(hdc, TG_PENCOLOR);
	Rectangle(hdc, 
		  pTrendGraph->rcGraph.left, 
		  pTrendGraph->rcGraph.top, 
		  pTrendGraph->rcGraph.right, 
		  pTrendGraph->rcGraph.bottom);
	
	ReleaseDC(hdc);
	return 0;
}

static int DrawGraphsCoord(HWND hWnd)
{
	int i;
	
	if(hWnd == (HWND)NULL) return -1;
	
	//��ʼ������ͼ����
	for(i=0; i<TREND_GRAPH_COUNT; i++){
		InitGraphCoord(hWnd, &(gTrendGraphs[i]));
	}
		
	return 0;
}

/**
	��������ͼ���
 */
static int DrawGraphFrame(HWND hWnd)
{
	int i;
	HDC hdc;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, 
		gRcGraphFrame.left,
		gRcGraphFrame.top, 
		gRcGraphFrame.right - gRcGraphFrame.left,
		gRcGraphFrame.bottom - gRcGraphFrame.top);

	SetPenColor(hdc, COLOR_darkgray);
	//Graph
	for(i=0; i<TREND_GRAPH_COUNT; i++){
		Rectangle(hdc, 
			  gTrendGraphs[i].rcGraph.left, 
			  gTrendGraphs[i].rcGraph.top, 
			  gTrendGraphs[i].rcGraph.right, 
			  gTrendGraphs[i].rcGraph.bottom);
	}
	
	//Time
	Rectangle(hdc, 
		  gRcGraphTime.left, 
		  gRcGraphTime.top, 
		  gRcGraphTime.right, 
		  gRcGraphTime.bottom);
/*		
	gRcGraphStatus.top = gRcGraphTime.bottom+3;

	//Status 
	Rectangle(hdc, 
		  gRcGraphStatus.left, 
		  gRcGraphStatus.top, 
		  gRcGraphStatus.right, 
		  gRcGraphStatus.bottom);

	
	SetPenColor(hdc, COLOR_darkgray);
	
	//Data
	for(i=0; i<TREND_GRAPH_COUNT; i++){
		Rectangle(hdc, 
			  gTrendGraphs[i].rcData.left, 
			  gTrendGraphs[i].rcData.top, 
			  gTrendGraphs[i].rcData.right, 
			  gTrendGraphs[i].rcData.bottom);
	}
	*/	
	ReleaseDC(hdc);
	
	//��������
	DrawGraphsCoord(hWnd);
	
	return 0;
}

/**
	��������
*/
static int UpdateGraphDate(HDC hdc, RECT rcDate, S_TIME CurTime)
{
	unsigned char strDate[11] = {0};
	int nType;
	
	if(hdc == (HDC)NULL) return -1;
	
	nType = GetRasterOperation(hdc);
	SetRasterOperation(hdc, ROP_XOR);
	SetBkColor(hdc, TG_BKCOLOR);
	SetTextColor(hdc, TG_TEXTCOLOR);
	
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		snprintf(strDate, sizeof strDate, "%.2d/%.2d/%.4d", CurTime.bDay, CurTime.bMonth,CurTime.wYear);
	else
		snprintf(strDate, sizeof strDate, "%.4d/%.2d/%.2d", CurTime.wYear, CurTime.bMonth, CurTime.bDay);
	SelectFont(hdc, gFontSystem);
	DrawText(hdc, strDate, -1, &(rcDate), DT_NOCLIP | DT_SINGLELINE | DT_LEFT);
	
	SetRasterOperation(hdc, nType);
	
	return 0;
}
/**
	���²�����ֵ
*/
static int NewUpdateGraphData(HDC hdc, PTREND_GRAPH pTG, S_TIME CurTime, PTRENDDATA  pData,long int iTimePlace)
{
	RECT rcLeft, rcRight;	
	unsigned char strData[50] = {0};
	int nType;
	
	if(hdc == (HDC)NULL) return -1;
	
//  	nType = GetRasterOperation(hdc);
//  	SetRasterOperation(hdc, ROP_XOR);
//  	SetBkColor(hdc, TG_BKCOLOR);
//  	SetTextColor(hdc, TG_TEXTCOLOR);
		
	switch(pTG->bID){
		case TG_PARA_HR:{
			SetTextColor(hdc, gCfgEcg.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
				
			}else if(pData->dwHaveFlag & HAVEHR){
				snprintf(strData, sizeof strData, "%d", pData->wHR);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		
		
		case TG_PARA_SPO2:{
			SetTextColor(hdc, gCfgSpO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if(pData->dwHaveFlag & HAVESPO2){
				snprintf(strData, sizeof strData, "%d", pData->bSpO2);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		case TG_PARA_NIBP:{
			SetTextColor(hdc, gCfgNibp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if(pData->bLogFlag & TRENDNIBP){
				snprintf(strData, sizeof strData, "%d/%d %d", pData->sNIBP.wSys, pData->sNIBP.wDia, pData->sNIBP.wMean);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		
		case TG_PARA_PR:{
			SetTextColor(hdc, gCfgSpO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if(pData->dwHaveFlag & HAVEPR){
				snprintf(strData, sizeof strData, "%d", pData->wPR);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		
		case TG_PARA_TEMP:{
			SetTextColor(hdc, gCfgTemp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if((pData->dwHaveFlag & HAVET1)  || (pData->dwHaveFlag & HAVET2)){
				snprintf(strData, sizeof strData, "%.1f/%.1f %.1f", 
					 (double)pData->wTemps[0] /10.0,  (double)pData->wTemps[1] /10.0,
					 (double)(abs(pData->wTemps[0] - pData->wTemps[1])) / 10.0);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		case  TG_PARA_CO2:{
			SetTextColor(hdc, gCfgCO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if(pData->dwHaveFlag & HAVECO2){
				snprintf(strData, sizeof strData, "%d", pData->sCO2.wEtCO2);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		case  TG_PARA_RR:{
			SetTextColor(hdc, gCfgResp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "");	
			}else if(pData->dwHaveFlag & HAVERESP){
				snprintf(strData, sizeof strData, "%d", pData->wRR);	
			}else{
				snprintf(strData, sizeof strData, "");	
			}
		}break;
		
	}
	
	SetPenColor(hdc, TG_CUSORCOLOR);
	SetBkColor(hdc, TG_BKCOLOR);
	
	//SetTextColor(hdc, TG_CUSORCOLOR);;
	/*
	FillBox(hdc, 
		pTG->rcData.left+1, 
		pTG->rcData.top+1, 
		pTG->rcData.right - pTG->rcData.left -1,
		pTG->rcData.bottom - pTG->rcData.top -1);
	*/
	
	SelectFont(hdc, gFontSystem);
	rcLeft = pTG->rcGraph;
	rcRight = pTG->rcGraph;
	rcRight.left = pTG->rcGraph.right - iTimePlace;
	rcLeft.right = rcLeft.left + (pTG->rcGraph.right - pTG->rcGraph.left - iTimePlace);
	if(iTimePlace > (pTG->rcGraph.right - pTG->rcGraph.left)/2 ){
		DrawText(hdc, strData, -1, &rcRight, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_LEFT);

	}
	else{
		DrawText(hdc, strData, -1, &rcLeft, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_RIGHT);

	}
	//DrawText(hdc, strData, -1, &(pTG->rcData), DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_LEFT);
	
  //	SetRasterOperation(hdc, nType);
	
	return 0;
}


/**
	���²�����ֵ
*/
static int UpdateGraphData(HDC hdc, PTREND_GRAPH pTG, S_TIME CurTime, PTRENDDATA  pData)
{
	unsigned char strData[50] = {0};
	int nType;
	
	if(hdc == (HDC)NULL) return -1;
	
//  	nType = GetRasterOperation(hdc);
//  	SetRasterOperation(hdc, ROP_XOR);
//  	SetBkColor(hdc, TG_BKCOLOR);
//  	SetTextColor(hdc, TG_TEXTCOLOR);
		
	switch(pTG->bID){
		case TG_PARA_HR:{
			//SetTextColor(hdc, gCfgEcg.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "---");	
			}else if(pData->dwHaveFlag & HAVEHR){
				snprintf(strData, sizeof strData, "%d", pData->wHR);	
			}else{
				snprintf(strData, sizeof strData, "---");	
			}
		}break;
		
		
		case TG_PARA_SPO2:{
			//SetTextColor(hdc, gCfgSpO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "---");	
			}else if(pData->dwHaveFlag & HAVESPO2){
				snprintf(strData, sizeof strData, "%d", pData->bSpO2);	
			}else{
				snprintf(strData, sizeof strData, "---");	
			}
		}break;
		case TG_PARA_NIBP:{
			//SetTextColor(hdc, gCfgNibp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "---/--- ---");	
			}else if(pData->bLogFlag & TRENDNIBP){
				snprintf(strData, sizeof strData, "%d/%d %d", pData->sNIBP.wSys, pData->sNIBP.wDia, pData->sNIBP.wMean);	
			}else{
				snprintf(strData, sizeof strData, "---/--- ---");	
			}
		}break;
		
		case TG_PARA_PR:{
			//SetTextColor(hdc, gCfgSpO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "---");	
			}else if(pData->dwHaveFlag & HAVEPR){
				snprintf(strData, sizeof strData, "%d", pData->wPR);	
			}else{
				snprintf(strData, sizeof strData, "---");	
			}
		}break;
		
		case TG_PARA_TEMP:{
			//SetTextColor(hdc, gCfgTemp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "--.-/--.- --.-");	
			}else if((pData->dwHaveFlag & HAVET1)  && (pData->dwHaveFlag & HAVET2)){
				snprintf(strData, sizeof strData, "%.1f/%.1f %.1f", 
					 (double)pData->wTemps[0] /10.0,  (double)pData->wTemps[1] /10.0,
					 (double)(abs(pData->wTemps[0] - pData->wTemps[1])) / 10.0);	
			}else{
				snprintf(strData, sizeof strData, "--.-/--.- --.-");	
			}
		}break;
		case  TG_PARA_CO2:{
			//SetTextColor(hdc, gCfgCO2.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "--");	
			}else if(pData->dwHaveFlag & HAVECO2){
				snprintf(strData, sizeof strData, "%d", pData->sCO2.wEtCO2);	
			}else{
				snprintf(strData, sizeof strData, "--");	
			}
		}break;
		case  TG_PARA_RR:{
			//SetTextColor(hdc, gCfgResp.iColor);
			if(pData == NULL){
				snprintf(strData, sizeof strData, "--");	
			}else if(pData->dwHaveFlag & HAVERESP){
				snprintf(strData, sizeof strData, "%d", pData->wRR);	
			}else{
				snprintf(strData, sizeof strData, "--");	
			}
		}break;
		
	}
	
	SetBrushColor(hdc, COLOR_lightgray);
	SetBkColor(hdc, COLOR_lightgray);
	SetTextColor(hdc, COLOR_black);
	FillBox(hdc, 
		pTG->rcData.left+1, 
		pTG->rcData.top+1, 
		pTG->rcData.right - pTG->rcData.left -1,
		pTG->rcData.bottom - pTG->rcData.top -1);
	SelectFont(hdc, gFontEnglish_16);
	DrawText(hdc, strData, -1, &(pTG->rcData), DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_LEFT);
	
//  	SetRasterOperation(hdc, nType);
	
	return 0;
}

/**
	���Ʋ���
	�����ػ�ʱ����
*/
static int DrawData(HWND hWnd)
{
	TRENDDATA CurData;
	HDC hdc;
	int i, res;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	res = GetOneTrendRecord(gCursorTime, &CurData);
	if(res ==0){
		for(i=0; i<TREND_GRAPH_COUNT; i++){
			UpdateGraphData(hdc, &gTrendGraphs[i], gCursorTime, &CurData);
		}
	}
	ReleaseDC(hdc);
	
	return 0;
}

/*
	��������ͼʱ��̶�
	���˼·��
	1���ұ�Ϊ���µ�ʱ��
	2�������Դ˱�ǳ�ʱ�䷶Χ�ڵı�־ʱ�䣨��ͬ�̶ȵļ����ͬ��
	����1Сʱ�Ŀ̶ȣ������������ǳ����Ϊ15���ӵĿ̶�
	3�����ʱ��Ϊ5�ı���������̶�Ϊ1Сʱ����ʼʱ��Ϊ12��09����˳��Ϊ
	(11:09)<-11:20<-11:35<-11:50<-12:05<-(12:09)
	4�����ʱ�����涨
	�̶�(Сʱ)		���
	 1				15 min
	 2				15*2=30 min
	 4				15*4=60 min = 1 hr
	 6				15*6=90 min
	 �Դ�����	��� = 15min*�̶�(hr)
	 5�����������Ҫ��ҳʱ��ÿ�η�һ����������ݣ�Ȼ�����ʱ����
*/
/*
	��������ͼʱ��̶�
	RightTime: ���ұ�(�����ʱ��)
	bTimeRange: ʱ��̶�
*/
static int UpdateGraphTime(HDC hdc, RECT rcTime, S_TIME RightTime, BYTE bTimeRange)
{
	unsigned char strTime[20]={0};
	S_TIME FirstLabelTime;		//��һ�����ʱ��
	S_TIME TmpTime;			//���ʱ��
	int iRectWidth = 0;			//����Ŀ��(pixel)
	int iTimeWidth = 0;			//ʱ��Ŀ��(min)			
	double fTimeToPixel = 0.0;		//ʱ���Ӧ���ص�ı�����Time / fTimeToPixel = Pixel
	int iTimePlace = 0;			//�������ʼʱ���λ��(��λ:min), �������ʱ��
	int iTimeInter = 0;			//���ʱ��ļ��(min)
	int i;
	
	if(hdc == (HDC)NULL) return -1;
	SetPenColor(hdc, TG_PENCOLOR);
	SetBkColor(hdc, TG_BKCOLOR);
	SetBkMode(hdc, BM_TRANSPARENT);
	SetTextColor(hdc, TG_TEXTCOLOR);
	
	//���ݿ̶ȼ����������ʱ��(min)�ı���
	iTimeWidth = bTimeRange *60;
	iRectWidth = rcTime.right - rcTime.left;
	if(iRectWidth==0) iRectWidth =1;	//��ֹ����
// 	fTimeToPixel = (double)((double)iTimeWidth / (double)iRectWidth);
	fTimeToPixel = (double)((double)iRectWidth / (double)iTimeWidth);
	gfTimeToPixel = fTimeToPixel;
//  	if(B_PRINTF) printf("%s:%d Time Width :%d, RECT width: %d Scale: %f\n", __FILE__, __LINE__, iTimeWidth, iRectWidth, fTimeToPixel);	
	
// 	if(B_PRINTF) printf("%s:%d Current time : %.2d/%.2d  %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
// 		 RightTime.bMonth, RightTime.bDay, RightTime.bHour, RightTime.bMin, RightTime.bSec);
	
		
	//�����һ�����ʱ��(5�ı���)��˳��Ϊ��->��(New->Old)
	FirstLabelTime = RightTime;
	while(FirstLabelTime.bMin % 10){
		FirstLabelTime.bMin -=1;
	}
		
	//�����һ��ʱ������λ��
	iTimePlace = ((int)CompTime(&RightTime, &FirstLabelTime))/60;
//	iTimePlace = RightTime.bMin-FirstLabelTime.bMin;
 	if(B_PRINTF) printf("%s:%d First label time : %.2d/%.2d  %.2d:%.2d:%.2d, inter %d.\n", __FILE__, __LINE__, 
 		 FirstLabelTime.bMonth, FirstLabelTime.bDay, FirstLabelTime.bHour, FirstLabelTime.bMin, FirstLabelTime.bSec, iTimePlace);
	
	//��ǵ�һ��ʱ��
// 	iTimePlace = (int)((double)iTimePlace / (double)fTimeToPixel);
	iTimePlace = (int)((double)iTimePlace * (double)fTimeToPixel);
// 	if(B_PRINTF) printf("%s:%d time place is %d.\n", __FILE__, __LINE__, iTimePlace);
	MoveTo(hdc, rcTime.right - iTimePlace, rcTime.top	);
	LineTo(hdc, rcTime.right - iTimePlace, rcTime.bottom);
	snprintf(strTime, sizeof strTime, "%.2d:%.2d", FirstLabelTime.bHour, FirstLabelTime.bMin);
// 	if(B_PRINTF) printf("%s:%d size of strtime is %d.\n", __FILE__, __LINE__, strlen(strTime));
	SelectFont(hdc, gFontSystem);
	//12x12��������
	TextOut(hdc, rcTime.right - iTimePlace -(strlen(strTime)*8), rcTime.bottom-15, strTime);
	
	
	/*---------- ���㲢���ʣ�µ�ʱ���� -----------*/
	//���ݿ̶ȼ�����ʱ��ļ��
	iTimeInter = GRAPH_TIME_MININTER * bTimeRange;
// 	if(B_PRINTF) printf("%s:%d time inter is %d.\n", __FILE__, __LINE__, iTimeInter);
	
	//������Сʱ����(15min)����С�̶�(1hr)�ó�����4��ʱ����
	for(i=1; i<4; i++){
		CalcTime(&FirstLabelTime, &TmpTime, iTimeInter*60*i , 1);
// 		if(B_PRINTF) printf("%s:%d label time : %.2d/%.2d  %.2d:%.2d:%.2d, inter %d.\n", __FILE__, __LINE__, 
// 			 TmpTime.bMonth, TmpTime.bDay, TmpTime.bHour, TmpTime.bMin, TmpTime.bSec, iTimeInter);
		
		//����ʱ������λ��
		iTimePlace = ((int)CompTime(&RightTime, &TmpTime))/60;
	//	iTimePlace = RightTime.bMin-TmpTime.bMin;
		
 //		if(B_PRINTF) printf("%s:%d First label time : %.2d/%.2d  %.2d:%.2d:%.2d, inter %d.\n", __FILE__, __LINE__, 
 //			 TmpTime.bMonth, TmpTime.bDay, TmpTime.bHour, TmpTime.bMin, TmpTime.bSec, iTimePlace);
	
		//���ʱ��
// 		iTimePlace = (int)((double)iTimePlace / (double)fTimeToPixel);
		iTimePlace = (int)((double)iTimePlace * (double)fTimeToPixel);
// 		if(B_PRINTF) printf("%s:%d time place is %d.\n", __FILE__, __LINE__, iTimePlace);
	
		MoveTo(hdc, rcTime.right - iTimePlace, rcTime.top	);
		LineTo(hdc, rcTime.right - iTimePlace, rcTime.bottom);
		snprintf(strTime, sizeof strTime, "%.2d:%.2d", TmpTime.bHour, TmpTime.bMin);
// 		if(B_PRINTF) printf("%s:%d size of strtime is %d.\n", __FILE__, __LINE__, strlen(strTime));
		//12x12��������
		SelectFont(hdc, gFontSystem);
// 		TextOut(hdc, rcTime.right - iTimePlace -(strlen(strTime)*3), rcTime.bottom-12, strTime);
		TextOut(hdc, rcTime.right - iTimePlace -(strlen(strTime)*8), rcTime.bottom-15, strTime);
	}

	return 0;
}
/**
	�����α�
	����ʱ��,����ʱ�������صı�������
	@param rcTime: ʱ����ʾ����
	@param RightTime: ʱ������Ҷ�ʱ��(��)
	@param CursorTime: ��ǰ������ڵ�ʱ��
	@param fTimeToPixel: ʱ�������صı���
*/
static int NewUpdateGraphCursor(HDC hdc, RECT rcTime, S_TIME RightTime, S_TIME CursorTime, double fTimeToPixel)
{
	unsigned char strTime[20]={0};
	unsigned char strValueLeft[30]={0};		//�ұ߲���ֵ
	unsigned char strValueRight[30]={0};	      //��߲���ֵ
	long int iTimePlace;
	int ioffset;
	int nType;
	long int iTmpIndex = 0;		
	long int i, res;
	S_TIME TmpTime;
	TRENDDATA  CurData;
	RECT rcLeft, rcRight;			//�α����ҵ�����, �����Ʋ���ֵʹ��
	WORD wHead, wTail, wSize;
	
	if(hdc == (HDC)NULL) return -1;
	nType = GetRasterOperation(hdc);
	SetBkMode(hdc, BM_TRANSPARENT);
	SetRasterOperation(hdc, ROP_XOR);
	SetPenColor(hdc, TG_CUSORCOLOR);
	SetBkColor(hdc, TG_BKCOLOR);
	SetTextColor(hdc, TG_CUSORCOLOR);
	
	//��ȡ�������ݵ���Ϣ
//	wHead = (gTrendDatas.wHeadIndex-(giCurPage*giGraphTimeRange*60)+MAXTRENDCOUNT)%MAXTRENDCOUNT;
	wHead =0;
	//����ʱ������λ��
	iTimePlace = ((long int)CompTime(&RightTime, &CursorTime))/60;
	ioffset=iTimePlace;
//	printf("***ioffset=%d*********\n",ioffset);
//	printf("%s:%d Currsor Time time : %.2d/%.2d  %.2d:%.2d:%.2d, inter %d\n", __FILE__, __LINE__, 
// 		 CursorTime.bMonth, CursorTime.bDay, CursorTime.bHour, CursorTime.bMin, CursorTime.bSec, iTimePlace);
	
	//����ʱ�����α�
// 	iTimePlace = (long int)((double)iTimePlace / (double)fTimeToPixel);
	iTimePlace = (long int)((double)iTimePlace * (double)fTimeToPixel);
// 	if(B_PRINTF) printf("%s:%d cursor  place is %d.\n", __FILE__, __LINE__, iTimePlace);
	MoveTo(hdc, rcTime.right - iTimePlace, gRcGraphFrame.top+2);
	LineTo(hdc, rcTime.right - iTimePlace, rcTime.bottom);
	//�����α����ҵ�����
	rcLeft = rcTime;
	rcRight = rcTime;
	rcRight.left = rcTime.right - iTimePlace;
	rcLeft.right = rcLeft.left + (rcTime.right - rcTime.left - iTimePlace);
	
	//�����α�ʱ��
//	snprintf(strTime, sizeof strTime, "%.2d:%.2d", CursorTime.bHour, CursorTime.bMin);
	snprintf(strTime, sizeof strTime, "%.3d/%.2d/%.2d %.2d:%.2d",CursorTime.wYear,CursorTime.bMonth,CursorTime.bDay, CursorTime.bHour, CursorTime.bMin);
	
	//New���Ʋ���
	
// 	if(B_PRINTF) printf("%s:%d size of cursor time  is %d.\n", __FILE__, __LINE__, strlen(strTime));
	//���α����е���Ҳ࣬��������ʾ���α����࣬��֮��Ȼ	
	SelectFont(hdc, gFontSystem);
	if(iTimePlace > (rcTime.right - rcTime.left)/2 ){
		DrawText(hdc, strTime, -1, &rcRight, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_LEFT);

	}
	else{
		DrawText(hdc, strTime, -1, &rcLeft, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_RIGHT);

	}
	
	CurData=tmpTrendData[ioffset];
	if(B_PRINTF)printf("*******hr:%d,temp2:%d,iTimePlace=%ld,offset=%ld\n",CurData.wHR,CurData.wTemps[1],iTimePlace,CurData.liOffset);
	
	#if 1
	//���²�������
	//�õ�Cursor Time������������
	
	for(i=0; i<TREND_GRAPH_COUNT; i++){
			//UpdateGraphData(hdc, &gTrendGraphs[i], gCursorTime, &CurData);
			NewUpdateGraphData(hdc, &gTrendGraphs[i], gCursorTime, &CurData,iTimePlace);
		}
	#endif
	SetRasterOperation(hdc, nType);
	return 0;
}

/**
	�����α�
	����ʱ��,����ʱ�������صı�������
	@param rcTime: ʱ����ʾ����
	@param RightTime: ʱ������Ҷ�ʱ��(��)
	@param CursorTime: ��ǰ������ڵ�ʱ��
	@param fTimeToPixel: ʱ�������صı���
*/
static int UpdateGraphCursor(HDC hdc, RECT rcTime, S_TIME RightTime, S_TIME CursorTime, double fTimeToPixel)
{
	unsigned char strTime[20]={0};
	unsigned char strValueLeft[30]={0};		//�ұ߲���ֵ
	unsigned char strValueRight[30]={0};	      //��߲���ֵ
	long int iTimePlace;
	int nType;
	long int iTmpIndex = 0;		
	long int i, res;
	S_TIME TmpTime;

	RECT rcLeft, rcRight;			//�α����ҵ�����, �����Ʋ���ֵʹ��
	
	if(hdc == (HDC)NULL) return -1;
	nType = GetRasterOperation(hdc);
	SetBkMode(hdc, BM_TRANSPARENT);
	SetRasterOperation(hdc, ROP_XOR);
	SetPenColor(hdc, TG_CUSORCOLOR);
	SetBkColor(hdc, TG_BKCOLOR);
	SetTextColor(hdc, TG_CUSORCOLOR);
	
	//����ʱ������λ��
	iTimePlace = ((long int)CompTime(&RightTime, &CursorTime))/60;

// 	if(B_PRINTF) printf("%s:%d Currsor Time time : %.2d/%.2d  %.2d:%.2d:%.2d, inter %d\n", __FILE__, __LINE__, 
// 		 CursorTime.bMonth, CursorTime.bDay, CursorTime.bHour, CursorTime.bMin, CursorTime.bSec, iTimePlace);
	
	//����ʱ�����α�
// 	iTimePlace = (long int)((double)iTimePlace / (double)fTimeToPixel);
	iTimePlace = (long int)((double)iTimePlace * (double)fTimeToPixel);
// 	if(B_PRINTF) printf("%s:%d cursor  place is %d.\n", __FILE__, __LINE__, iTimePlace);
	MoveTo(hdc, rcTime.right - iTimePlace, gRcGraphFrame.top+5);
	LineTo(hdc, rcTime.right - iTimePlace, rcTime.bottom+3);
	//�����α����ҵ�����
	rcLeft = rcTime;
	rcRight = rcTime;
	rcRight.left = rcTime.right - iTimePlace;
	rcLeft.right = rcLeft.left + (rcTime.right - rcTime.left - iTimePlace);
	//�����α�ʱ��
	snprintf(strTime, sizeof strTime, "%.2d:%.2d", CursorTime.bHour, CursorTime.bMin);
	//New���Ʋ���
	
// 	if(B_PRINTF) printf("%s:%d size of cursor time  is %d.\n", __FILE__, __LINE__, strlen(strTime));
	//���α����е���Ҳ࣬��������ʾ���α����࣬��֮��Ȼ	
	SelectFont(hdc, gFontSystem);
	if(iTimePlace > (rcTime.right - rcTime.left)/2 ){
		DrawText(hdc, strTime, -1, &rcRight, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_LEFT);
	}
	else{
		DrawText(hdc, strTime, -1, &rcLeft, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_RIGHT);
	}

	SetRasterOperation(hdc, nType);
	return 0;
}
#if 0
/**
	�ƶ��α�	
	@param bMove: �ƶ��α�ķ��� CURSOR_XXX
*/
static int GraphMoveCursor(HWND hWnd,  BYTE bMove)
{
	HDC hdc;
	S_TIME RightTime;
	S_TIME LeftTime;					//��߽�ʱ��
	S_TIME CursorTime;					//�µ��α�ʱ��
	S_TIME TmpTime;					//��ʱ��������������ʱ��
	long int iCompTimeRes=0;			//ʱ��ȽϵĽ��, 0:��ͬ���Ƚϵĵ�λΪ���ӣ�
	long int iTimeRange;
	int i;
	TRENDDATA  CurData;
	int res;
	
	
	if(hWnd == (HWND)NULL) return -1;
		
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	//����ұ߽�ʱ��
	RightTime = gRightTime;				
	//���ʱ��̶�
	iTimeRange = giGraphTimeRange;
		
	//������߽�ʱ��
	CalcTime(&RightTime, &LeftTime, iTimeRange*60*60 , 1);
//  	if(B_PRINTF) printf("%s:%d From time : %.2d/%.2d  %.2d:%.2d:%.2d \n  \
//  			to time: %.2d/%.2d  %.2d:%.2d:%.2d \n", __FILE__, __LINE__, 
// 	 	RightTime.bMonth, RightTime.bDay, RightTime.bHour, RightTime.bMin, RightTime.bSec, 
//  		LeftTime.bMonth, LeftTime.bDay, LeftTime.bHour, LeftTime.bMin, LeftTime.bSec);	
	
	//�����߽�ʱ��
	gLeftTime = LeftTime;
	//��������
	//UpdateGraphDate(hdc, gRcGraphTime, gCursorTime);
	//�����α�
	//	UpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	
	
	//���ݷ�������α�
	switch(bMove){
		case CURSOR_LEFT:{
			//�Ƚ�ʱ��
			iCompTimeRes = CompTime(&gCursorTime, &LeftTime)/60;
			//���gCursorTime����LeftTime, --
			if(iCompTimeRes >0){
				//�����µ��α�ʱ��
				CalcTime(&gCursorTime, &CursorTime, 60 ,1);
			}			
			else{
				CursorTime = LeftTime;
				ReleaseDC(hdc);
				GraphMovePage(hWnd, CURSOR_LEFT, MOVE_FULLPAGE);
				return 0;
			}	
		}break;
		case CURSOR_RIGHT:{
			iCompTimeRes = CompTime(&gCursorTime, &RightTime)/60;
			//���gCursorTime����RightTime, ++
			if(iCompTimeRes <0){
				//�����µ��α�ʱ��
				CalcTime(&gCursorTime, &CursorTime, 60 , 0);
			}
			else{
				CursorTime = RightTime;	
				ReleaseDC(hdc);
				GraphMovePage(hWnd, CURSOR_RIGHT, MOVE_FULLPAGE);
				return 0;
			}
		}break;
		default:{
			CursorTime = gCursorTime;
			ReleaseDC(hdc);
			return 0;
		}break;
	}
	
	gCursorTime = CursorTime;
	//��������
	//UpdateGraphDate(hdc, gRcGraphTime, gCursorTime);

	//�����α�
//	UpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	/*
	//���²�������
	//�õ�Cursor Time������������
	res = GetOneTrendRecord(gCursorTime, &CurData);
	if(res ==0){
		for(i=0; i<TREND_GRAPH_COUNT; i++){
			//UpdateGraphData(hdc, &gTrendGraphs[i], gCursorTime, &CurData);
			UpdateGraphData(hdc, &gTrendGraphs[i], gCursorTime, &CurData);
		}
	}
	*/	
	ReleaseDC(hdc);
	
	return 0;
}
#endif
/**
	�ƶ��α�	
	@param bMove: �ƶ��α�ķ��� CURSOR_XXX
*/
static int GraphMoveCursor(HWND hWnd,  BYTE bMove)
{
	HDC hdc;
	S_TIME RightTime;
	S_TIME LeftTime;					//��߽�ʱ��
	S_TIME CursorTime;					//�µ��α�ʱ��
	S_TIME TmpTime;					//��ʱ��������������ʱ��
	long int iCompTimeRes=0;			//ʱ��ȽϵĽ��, 0:��ͬ���Ƚϵĵ�λΪ���ӣ�
	long int iTimeRange;
	int i;
	TRENDDATA  CurData;
	int res;
	
	
	if(hWnd == (HWND)NULL) return -1;
		
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	//����ұ߽�ʱ��
	RightTime = gRightTime;				
	//���ʱ��̶�
	iTimeRange = giGraphTimeRange;

/*	
	if(B_PRINTF) printf("%s:%d From time : %.2d/%.2d  %.2d:%.2d:%.2d \n  \
  			to time: %.2d/%.2d  %.2d:%.2d:%.2d  \n \
  			cursorteimt: %.2d/%.2d  %.2d:%.2d:%.2d ,temptime=%d  \n", __FILE__, __LINE__, 
 	 	RightTime.bMonth, RightTime.bDay, RightTime.bHour, RightTime.bMin, RightTime.bSec, 
  		LeftTime.bMonth, LeftTime.bDay, LeftTime.bHour, LeftTime.bMin, LeftTime.bSec,
  		gCursorTime.bMonth, gCursorTime.bDay, gCursorTime.bHour, gCursorTime.bMin, gCursorTime.bSec,iCompTimeRes);	
*/
	
	NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	
	
	//�����߽�ʱ��
	  LeftTime=gLeftTime;
	//���ݷ�������α�
	switch(bMove){
		case CURSOR_LEFT:{
			//�Ƚ�ʱ��
			iCompTimeRes = CompTime(&gCursorTime, &LeftTime)/60;
			
			//���gCursorTime����LeftTime, --
			if(iCompTimeRes >0){
				//�����µ��α�ʱ��
				CalcTime(&gCursorTime, &CursorTime, 60 ,1);
			}			
			else{
				bPageUp=FALSE;
				CursorTime = LeftTime;
				ReleaseDC(hdc);
				GraphMovePage(hWnd, CURSOR_LEFT, MOVE_FULLPAGE);
			//	NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, CursorTime, gfTimeToPixel);
				return 0;
			}	
		}break;
		case CURSOR_RIGHT:{
			iCompTimeRes = CompTime(&gCursorTime, &RightTime)/60;
			//���gCursorTime����RightTime, ++
			if(iCompTimeRes <0){
				//�����µ��α�ʱ��
				CalcTime(&gCursorTime, &CursorTime, 60 , 0);
			}
			else{
				bPageUp=TRUE;
				CursorTime = RightTime;
				NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, CursorTime, gfTimeToPixel);
				
				ReleaseDC(hdc);
				GraphMovePage(hWnd, CURSOR_RIGHT, MOVE_FULLPAGE);
				
				return 0;
			}
		}break;
		default:{
			CursorTime = gCursorTime;
			ReleaseDC(hdc);
			return 0;
		}break;
	}
	
	gCursorTime = CursorTime;
	//��������
	//UpdateGraphDate(hdc, gRcGraphTime, gCursorTime);
	//�����α�
//	UpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
	NewUpdateGraphCursor(hdc, gRcGraphTime, RightTime, gCursorTime, gfTimeToPixel);
		
	ReleaseDC(hdc);
	
	return 0;
}

/**
	��������ͼ
*/
/*
	��������
	����ʱ,����ʱ��������֮��ı���,��ʱ�����
*/
static int DrawValue(HDC hdc, PTRENDDATA pTrendData, int iTimeInter)
{
	int iLeft = 0, iTop = 0, iRight = 0, iBottom = 0, iWidth = 0, iHeight = 0;
	int x1, y1, x2, y2, x3, y3;
	int i;
	
	for(i=0; i<TREND_GRAPH_COUNT; i++){
		//ȡ����������
		iLeft = gTrendGraphs[i].rcGraph.left;
		iTop = gTrendGraphs[i].rcGraph.top;
		iRight = gTrendGraphs[i].rcGraph.right;
		iBottom = gTrendGraphs[i].rcGraph.bottom;
		iHeight = iBottom - iTop;
		iWidth = iRight - iLeft;	
		
		switch(gTrendGraphs[i].bID){
			case TG_PARA_HR:{
				//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gCfgEcg.iColor);
				x1 = iRight- (iTimeInter * gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVEHR){
					y1 = pTrendData->wHR;
				}
				else{
					break;
				}
				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
			}break;
			case TG_PARA_SPO2:{
				//�������»���ͼ
				SetPenColor(hdc, gCfgSpO2.iColor);
				x1 = iRight- (iTimeInter* gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVESPO2){
					y1 = pTrendData->bSpO2;
				}
				else{
					break;
				}

				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
// 				MoveTo(hdc, x1, iTop);
// 				LineTo(hdc, x1, y1);
				
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, iTop, x1, y1, WIDELINE_RECT);
			}break;
			case TG_PARA_RR:{
				//�Ե�Ϊ����,���̰�ͼ
				SetPenColor(hdc, gCfgResp.iColor);
				x1 =iRight-  (iTimeInter* gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVERESP){
					y1 = pTrendData->wRR;
				}
				else{
					break;
				}
				
				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);
			}break;
			case TG_PARA_NIBP:{
				SetPenColor(hdc, gCfgNibp.iColor);
				x1 =iRight- (iTimeInter* gfTimeToPixel);
				x2 = iRight- (iTimeInter* gfTimeToPixel);
				x3 = iRight- (iTimeInter* gfTimeToPixel);
				
				if((pTrendData->bLogFlag & TRENDNIBP)
					&& (pTrendData->dwHaveFlag & HAVENIBP)){
					y1 = pTrendData->sNIBP.wSys;
					y2 = pTrendData->sNIBP.wMean;
					y3 = pTrendData->sNIBP.wDia;
				}
				else{
					break;
				}
				
				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				
				if(y2 > gTrendGraphs[i].yMax) y2 = gTrendGraphs[i].yMax;
				if(y2 < gTrendGraphs[i].yMin) y2 = gTrendGraphs[i].yMin;
				y2 = iBottom - (y2 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				
				if(y3 > gTrendGraphs[i].yMax) y3 = gTrendGraphs[i].yMax;
				if(y3 < gTrendGraphs[i].yMin) y3 = gTrendGraphs[i].yMin;
				y3 = iBottom - (y3 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1, x3, y3, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgNibp.iColor);
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
			}break;
			case TG_PARA_TEMP:{
				//��T1��T2����ͼ
				x1 =iRight-  (iTimeInter * gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVET1){
					y1 = pTrendData->wTemps[0];
				}
				else{
					y1 = gTrendGraphs[i].yMin;
				}

				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;

				x2 =iRight-  (iTimeInter * gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVET2){
					y2 = pTrendData->wTemps[1];
				}
				else{
					y2 = gTrendGraphs[i].yMin;
				}	
				
				if(y2 > gTrendGraphs[i].yMax) y2 = gTrendGraphs[i].yMax;
				if(y2 < gTrendGraphs[i].yMin) y2 = gTrendGraphs[i].yMin;
				y2 = iBottom - (y2 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
											
				//�����һ����������,�򵥶����ƶ̰�ͼ
				if((pTrendData->dwHaveFlag & HAVET1) && (pTrendData->dwHaveFlag & HAVET2)){
					SetPenColor(hdc, gCfgTemp.iColor);
					DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1, x2, y2, WIDELINE_RECT);
					SetPenColor(hdc, ~gCfgTemp.iColor);
					DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
				}
				else{
					if(pTrendData->dwHaveFlag & HAVET1){
						SetPenColor(hdc, gCfgTemp.iColor);
						DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1-1, x1, y1+1, WIDELINE_RECT);	
					}
					if(pTrendData->dwHaveFlag & HAVET2){
						SetPenColor(hdc, ~gCfgTemp.iColor);
						DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x2, y2-1, x2, y2+1, WIDELINE_RECT);	
					}
				}
			}break;
			case TG_PARA_CO2:{
				//�������ϻ���ͼ
				SetPenColor(hdc, gCfgCO2.iColor);
				x1 =iRight- (iTimeInter* gfTimeToPixel);
				if(pTrendData->dwHaveFlag & HAVECO2){
					y1 = pTrendData->sCO2.wFiCO2;
					y2 = pTrendData->sCO2.wEtCO2;
				}
				else{
					break;
				}

				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				if(y2 > gTrendGraphs[i].yMax) y2 = gTrendGraphs[i].yMax;
				if(y2 < gTrendGraphs[i].yMin) y2 = gTrendGraphs[i].yMin;
				y2 = iBottom - (y2 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1, x1, y2, WIDELINE_RECT);
			}break;
			case TG_PARA_IBP1:{
				SetPenColor(hdc, gCfgIbp1.iColor);
				x1 =iRight-  (iTimeInter* gfTimeToPixel);
				x2 =iRight-  (iTimeInter* gfTimeToPixel);
				x3 = iRight-  (iTimeInter* gfTimeToPixel);
				
				if(pTrendData->dwHaveFlag & HAVEIBP1){
					y1 = pTrendData->sIBPs[0].iSys;
					y2 = pTrendData->sIBPs[0].iMean;
					y3 = pTrendData->sIBPs[0].iDia;
				}
				else{
					break;
				}
				
				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;

				if(y2 > gTrendGraphs[i].yMax) y2 = gTrendGraphs[i].yMax;
				if(y2 < gTrendGraphs[i].yMin) y2 = gTrendGraphs[i].yMin;
				y2 = iBottom - (y2 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				if(y3 > gTrendGraphs[i].yMax) y3 = gTrendGraphs[i].yMax;
				if(y3 < gTrendGraphs[i].yMin) y3 = gTrendGraphs[i].yMin;
				y3 = iBottom - (y3 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
                                                                                                     
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1, x3, y3, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgIbp1.iColor);
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
			}break;	
			case TG_PARA_IBP2:{
				SetPenColor(hdc, gCfgIbp2.iColor);
				x1 =iRight-  (iTimeInter* gfTimeToPixel);
				x2 =iRight-  (iTimeInter* gfTimeToPixel);
				x3 = iRight-  (iTimeInter* gfTimeToPixel);
				
				if(pTrendData->dwHaveFlag & HAVEIBP2){
					y1 = pTrendData->sIBPs[1].iSys;
					y2 = pTrendData->sIBPs[1].iMean;
					y3 = pTrendData->sIBPs[1].iDia;
				}
				else{
					break;
				}
				if(y1 > gTrendGraphs[i].yMax) y1 = gTrendGraphs[i].yMax;
				if(y1 < gTrendGraphs[i].yMin) y1 = gTrendGraphs[i].yMin;
				y1 = iBottom - (y1 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;

				if(y2 > gTrendGraphs[i].yMax) y2 = gTrendGraphs[i].yMax;
				if(y2 < gTrendGraphs[i].yMin) y2 = gTrendGraphs[i].yMin;
				y2 = iBottom - (y2 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;
				
				if(y3 > gTrendGraphs[i].yMax) y3 = gTrendGraphs[i].yMax;
				if(y3 < gTrendGraphs[i].yMin) y3 = gTrendGraphs[i].yMin;
				y3 = iBottom - (y3 - gTrendGraphs[i].yMin)*gTrendGraphs[i].fRatio_y;

				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x1, y1, x3, y3, WIDELINE_RECT);
				SetPenColor(hdc, ~gCfgIbp2.iColor);
				DrawWideLine(hdc, REALWIDELINE, TG_LINE_WIDTH, x2, y2, x2, y2, WIDELINE_RECT);
			}break;
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
static int DrawTrendValue(HDC hdc)
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
	sGetStartTime = gRightTime;
	sGetEndTime = gLeftTime;
	
// 	if(B_PRINTF) printf("DrawValue StartTime %.2d:%.2d  EndTime %.2d:%.2d.\n", 
// 	       sGetStartTime.bHour, sGetStartTime.bMin, sGetEndTime.bHour, sGetEndTime.bMin);

	//����������(Head)��ǰ(Tail)ȡָ��ʱ���ڵ�����, ��ȡ�߻�
	
	if(wHead < wTail){
//  		if(B_PRINTF) printf(" Rollback \n");
		for(i=wHead; i>=0; i--){
//  			if(B_PRINTF) printf(" %d ", gTrendDatas.Datas[i].liOffset);
 			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
// 				if(B_PRINTF) printf("Get Over.\n");
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
// 		if(B_PRINTF) printf("\n");
		for(i=(wSize-1); i>=wTail; i--){
//  			if(B_PRINTF) printf(" %d ", gTrendDatas.Datas[i].liOffset);
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
// 				if(B_PRINTF) printf("Get Over.\n");
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
	}
	else if(bLoop){
//  		if(B_PRINTF) printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
//  			if(B_PRINTF) printf(" %d ", gTrendDatas.Datas[i].liOffset);
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
//  				if(B_PRINTF) printf("Get Over.\n");
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
// 		if(B_PRINTF) printf("\n");
	}
	else{
//  		if(B_PRINTF) printf(" Normal ");
		for(i=wHead; i>wTail; i--){
// 			if(B_PRINTF) printf(" %d -->", gTrendDatas.Datas[i].liOffset);
			sTmpTime = gTrendDatas.Datas[i].sTime;
			iTimeInter = CompTimeWithMin(&sGetEndTime, &sTmpTime);
			//�����������ʱ�����ڽ�ֹʱ��,�򷵻�, �ж�����Ϊ>1����,��Ϊ1���ӵ��Զ��洢����ڿ��ܺ���NIBP��������
			if(iTimeInter >0) {
//  				if(B_PRINTF) printf("Get Over.\n");
				goto GETOVER;
			}
			//��������
			tmpTrendData = gTrendDatas.Datas[i];
			DrawValue(hdc, &tmpTrendData, abs(iTimeInter));
		}
//  		if(B_PRINTF) printf("\n");
	}	
	
	
GETOVER:
			
	
	
	
		return 0;
}
#endif
/*
	��������ͼ����
	1���ӵ�ǰ���������ݿ�ʼ��ȡ��ָ���̶ȵ���������
	2����ȡ��������ȫ������ָ������
*/
static int DrawTrendValue(HDC hdc)
{
	S_TIME sGetStartTime;			//���µ��������ݵ�ʱ��
	S_TIME sGetEndTime;			//ȡ���ݵĽ�ֹʱ��
	S_TIME sTmpTime;				//���ݵ�ʱ��
	WORD wHead, wTail, wSize;
	BOOL bLoop;	//�����Ƿ�ת				
	//TRENDDATA tmpTrendData;
	int iTGCount=0;
	int iTimeInter = 0;
	int i;
	
	if(hdc == (HDC)NULL) return -1;
	
	//��ȡ�������ݵ���Ϣ
	wHead = (gTrendDatas.wHeadIndex-(giCurPage*giGraphTimeRange*60)+MAXTRENDCOUNT)%MAXTRENDCOUNT;
	
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	//�ж����������Ƿ�Ϊ��
	if(wHead == wTail) return -1;
	//�������û�ȡ��Ҫ�����ٸ���
	iTimeInter = giGraphTimeRange*60;
	
	if(B_PRINTF)printf("whead=%d,bloop=%d,count=%d,giCurPage=%d\n",wHead,bLoop,iTGCount,giCurPage);	
	
	for(i=0;i<iTimeInter;i++){
		if(((wHead-i+MAXTRENDCOUNT)%MAXTRENDCOUNT==gTrendDatas.wHeadIndex)&&i!=0){
			if(B_PRINTF)printf("111whead=%d,bloop=%d,count=%d,giCurPage=%d\n",wHead,bLoop,iTGCount,giCurPage);
			//goto GETOVER;
			return 0;
		}
		
		tmpTrendData[i]=gTrendDatas.Datas[(wHead-i+MAXTRENDCOUNT)%MAXTRENDCOUNT];
		tmpTrendData[i].liOffset=i;
		DrawValue(hdc,  &tmpTrendData[i], i);
			
	}

	
	

//GETOVER:

		return 0;
}

/**
	��ʼ������ͼ
 */
static int InitTrendGraph(HWND hWnd)
{
	
	HDC hdc;
	long int res;
	
	if(hWnd == (HWND)NULL) return -1;
	
	//��ʼ����ͼ����
	InitGraphRects(hWnd);
	
	//��������ͼ���
	DrawGraphFrame(hWnd);
	
	giHavePage=GetHavePage(giGraphTimeRange);
	giCurPage=0;
	//��ȡϵͳ��ǰʱ��
//	GetSysTime(&gRightTime);
	
	gRightTime=gTrendDatas.Datas[gTrendDatas.wHeadIndex].sTime;
	
	//Ĭ���α������Ҷ�
	gCursorTime = gRightTime;
	
	//������߽�ʱ��
	CalcTime(&gRightTime, &gLeftTime, giGraphTimeRange*60*60 , 1);
  	
	
	//��û�ͼDC
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	//����ʱ��̶�
// 	if(B_PRINTF) printf("%s:%d  Time Range is %d hour,page=%d.\n", __FILE__, __LINE__, giGraphTimeRange,giHavePage);
	UpdateGraphTime(hdc, gRcGraphTime, gRightTime, giGraphTimeRange);
				
	//��������
	DrawTrendValue(hdc);

	//��ʼ��ʱ,���ƶ�
	GraphMoveCursor(hWnd, CURSOR_INIT);
	
	ReleaseDC(hdc);


	return 0;
}

/**
	��ҳʱ�ػ�ҳ��
*/
static int UpdateGraphPage(HWND hWnd)
{
	HDC hdc;
		
	//Ĭ���α������Ҷ�
	if(!bPageUp)
		gCursorTime = gRightTime;
	else
		gCursorTime = gLeftTime;
// 	if(B_PRINTF) printf("Start time: %d/%d/%d %d:%d:%d \n", gRightTime.wYear, gRightTime.bMonth, gRightTime.bDay, 
// 	       gRightTime.bHour, gRightTime.bMin, gRightTime.bSec);
	
	//��������ͼ���
	DrawGraphFrame(hWnd);
	
	
	//���Ʋ���
//	DrawData(hWnd);
	
	//��û�ͼDC
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
//��������
	DrawTrendValue(hdc);
	//����ʱ��̶�
// 	if(B_PRINTF) printf("%s:%d  Time Range is %d hour.\n", __FILE__, __LINE__, giGraphTimeRange);
	UpdateGraphTime(hdc, gRcGraphTime, gRightTime, giGraphTimeRange);
				
	ReleaseDC(hdc);
	
		//��ʼ��ʱ,���ƶ�
	GraphMoveCursor(hWnd, CURSOR_INIT);
	
	return 0;
}

/**
	��ҳ, 
	@param:bMove �ƶ��ķ��� CURSOR_XXX
	@param:bSize �ƶ��ķ�Χ MOVE_FULLPAGE, MOVE_PARTPAGE
*/
static int GraphMovePage(HWND hWnd, BYTE bMove, BYTE bSize)
{
	S_TIME sTmpTime;
	S_TIME sStartTime;	//�������ݿ�ʼ�洢��ʱ��
	S_TIME sCurTime;	//����ʱ��
	long int iMoveTime = 0;	//�ƶ���ʱ��(second)
	int res;
	static int iOldPage=0;
	
	//���ݲ���bSize�͵�ǰ��ʱ��̶� �������ƶ���ʱ�䷶Χ
	if(bSize == MOVE_FULLPAGE){
		//�ƶ������̶�
		iMoveTime = giGraphTimeRange * 60 * 60;
	}else if(bSize == MOVE_PARTPAGE){
		//�ƶ��ֲ��̶�(�˴��Է���Ϊ��λ)
		iMoveTime = (giGraphTimeRange * 60 )/2 *60;
	}else{
		//���ƶ�
		return -1;
	}
	
	sTmpTime = gRightTime;
	if(bMove == CURSOR_LEFT){
			if(giCurPage<giHavePage){
				giCurPage++;
				//����(Old)�ƶ�
				CalcTime(&gRightTime, &sTmpTime, iMoveTime, TIME_BEFORE);
			}
	}
	else{
		if(giCurPage==0) bPageUp=FALSE;
		if(giCurPage>0){
			giCurPage--;
			//����(New)�ƶ�
			CalcTime(&gRightTime, &sTmpTime, iMoveTime, TIME_AFTER);
		}
		
	}
	
	//�ұ�ʱ��
	gRightTime = sTmpTime;

	//������߽�ʱ��
	CalcTime(&gRightTime, &gLeftTime, giGraphTimeRange*60*60 , 1);
  
	//�ж��ұ�ʱ��, �������CurTime�����CurTime, �������StartTime+�̶�,�����StartTime+�̶�
//	res = CompTimeWithMin(&gRightTime, &sCurTime);

	if(iOldPage!=giCurPage)
	//�ػ�ҳ��	
	UpdateGraphPage(hWnd);
	
	iOldPage=giCurPage;
	return 0;
}


//-------------------------------------------------- interface ----------------------------------------------------
/**
	����������ʾ�Ի���
*/
void CreateTrendGraph(HWND hWnd)
{
	//��ʼ������ͼ����
	int High_Dlg;
	gTrendGraphs[0].bID = TrendGraphCfg.bPara1+1;
	gTrendGraphs[1].bID = TrendGraphCfg.bPara2+1;
	gTrendGraphs[2].bID = TrendGraphCfg.bPara3+1;

	if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=HEIGHT_CTL;

#if	SCREEN_640
	DlgTrendGraph.x = SCREEN_LEFT,
	DlgTrendGraph.y = SCREEN_BOTTOM-285, 		
#else
	DlgTrendGraph.x = SCREEN_LEFT,
	DlgTrendGraph.y = SCREEN_BOTTOM-285-High_Dlg, 		
#endif
	
	DlgTrendGraph.controls = CtrlTrendGraph;
	DialogBoxIndirectParam(&DlgTrendGraph, hWnd, DlgProcTrendGraph, 0L);    
	
}

