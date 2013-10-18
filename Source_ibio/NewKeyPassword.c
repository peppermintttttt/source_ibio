/*************************************************************************** 
 *	Module Name:	NewKeyPassword
 *
 *	Abstract:	�����������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			20012-12-12 8:25:47
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

#define DID_PASSWORD_KEY_0		DID_PASSWORD
#define DID_PASSWORD_KEY_1		DID_PASSWORD+1
#define DID_PASSWORD_KEY_2		DID_PASSWORD+2
#define DID_PASSWORD_KEY_3		DID_PASSWORD+3
#define DID_PASSWORD_KEY_4		DID_PASSWORD+4
#define DID_PASSWORD_KEY_5		DID_PASSWORD+5
#define DID_PASSWORD_KEY_6		DID_PASSWORD+6
#define DID_PASSWORD_KEY_7		DID_PASSWORD+7
#define DID_PASSWORD_KEY_8		DID_PASSWORD+8
#define DID_PASSWORD_KEY_9		DID_PASSWORD+9
#define DID_PASSWORD_KEY_DEL		DID_PASSWORD+10
#define DID_PASSWORD_KEY_OK		DID_PASSWORD+11
#define DID_PASSWORD_KEY_CANCEL	DID_PASSWORD+12
#define DID_PASSWORD_EDIT_INPUT		DID_PASSWORD+13
#define DID_PASSWORD_CAPTION		DID_PASSWORD+14
#define DID_PASSWORD_ERROR		DID_PASSWORD+15
//�������������ַ���
#define MAXINPUT_PW		6
static HWND editInput;
static HWND editCaption;
static HWND staticError;
//Password OK?
static BOOL bPassResult = FALSE;

//Password Type
static BYTE bPassType;

//������
#define KEY_WIDTH	33

static unsigned char strSaveConf[50];
//-- �Ի���ص����� --
static int DlgProcKeyboard(HWND hDlg, int message, WPARAM wParam, LPARAM lParam);
//-- С���̰�ť���� --
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static WNDPROC OldBtnProc;
static WNDPROC OldEditProc;
DLGTEMPLATE DlgKeyPassword = {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NONE,
	0, 0, 175, 165,    //top���ݿռ��λ�ñ仯
	"",
	0, 0,
	1,		
	NULL,
	0
};
CTRLDATA CtrlKeyPassword[] = {
	{
		"button",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
		5, 60, 30, 30,
		DID_PASSWORD_KEY_0,
		"0",
		0
	}
};
//��ʱ�洢�����ޣ����ڵ�ʱ��ʹ��
static int iTmpPass = 2870;
//����ؼ�������
static RECT rcInput;
//�Ի��������
static RECT rcDlg;

//�����ַ�����
static char *strKey[40]={ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "DEL",  "OK","Cancel"};

/*
	�༭��ص�����
*/
static int EditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int iVaue;
	int id;
	RECT rc;
	HDC hdc= (HDC)NULL;
	
	GetWindowRect(hWnd, &rc);
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
		case MSG_KEYUP:{
			id = GetDlgCtrlID(hWnd);
			switch(wParam){
				case SCANCODE_ENTER:{
					
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}
	
	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
	return(temp);
}	

//-- С���̻ص����� --
static int DlgProcKeyboard(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	static char	strInput[MAXINPUT_PW]={0}; 	//������ַ���	
	static int	iInputCount =0;
		
	switch(message){
		case MSG_INITDIALOG:{
			HWND btnKey;
			int i, j;
			int iTopOfDlg;
			
			GetWindowRect(hDlg, &rcDlg);
			memset(strInput, 0, sizeof strInput);
			iInputCount =0;

			//caption
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "caption", strMenu, sizeof strMenu,"Input Password");	
			editCaption = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_PASSWORD_CAPTION, 0, 5, 180, 24, hDlg, 0);
			//error
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "errormsg", strMenu, sizeof strMenu,"Your password is wrong!");	
			staticError = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY | WS_VISIBLE,
					     DID_PASSWORD_ERROR, 5, 30, 160, 26, hDlg, 0);
			SetWindowBkColor(staticError,COLOR_red);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( staticError, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif		
			ShowWindow(staticError,SW_HIDE);
			
			editInput = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_PASSWORD,
					       DID_PASSWORD_EDIT_INPUT, 5, 30, 160, 26, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editInput, EditProc);
					
			btnKey = GetDlgItem(hDlg, DID_PASSWORD_KEY_0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc); 
			
			for(i=1; i<5; i++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						DID_PASSWORD_KEY_0+i, KEY_WIDTH*i+5, 60, 30, 30, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc); 				
			}
			for(i=5, j=0; i<10; i++, j++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						DID_PASSWORD_KEY_0+i, KEY_WIDTH*j+5, 95, 30, 30, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			}
			
			
			//del
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_KEYBOARD, "delete", strMenu, sizeof strMenu,"DEL");	
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      DID_PASSWORD_KEY_DEL, 5, 130, 50, 26, hDlg, 0);
			
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			
			//cancel
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_KEYBOARD, "cancel", strMenu, sizeof strMenu,"Cancel");	
		
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      DID_PASSWORD_KEY_CANCEL, 60, 130, 52, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
										
			//OK
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "ok", strMenu, sizeof strMenu,"OK");	
		
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      DID_PASSWORD_KEY_OK, 118, 130, 50, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			
			//-- Ĭ�Ͻ��� --
			SendNotifyMessage(hDlg, MSG_KEYDOWN, (WPARAM)SCANCODE_TAB, (LPARAM)0);
		
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				//-- ɾ�� --
				case DID_PASSWORD_KEY_DEL:{
					if(editInput != (HWND)NULL)
						*(strInput+strlen(strInput)-1)='\0';
						iInputCount--;
						SetWindowText(editInput, strInput);
						
				}  break;
				
				//-- �˳� --
				case DID_PASSWORD_KEY_OK:{
					//У������
							switch(bPassType){
								case PASSTYPE_DEMO:{
									if(atoi(strInput)  == PASS_DEMO) 
										{
											bPassResult = TRUE;
											SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
										}
									else {
										bPassResult = FALSE;
										ShowWindow(staticError,SW_SHOW);
									}
								}break;
								case PASSTYPE_USER:{
									if(atoi(strInput)  == PASS_USER) 
										{
											bPassResult = TRUE;
											SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
										}
									else {
										bPassResult = FALSE;
										ShowWindow(staticError,SW_SHOW);
									}
								}break;
								case PASSTYPE_FACTORY:{
									if(atoi(strInput)  == PASS_FACTORY) 
										{
											bPassResult = TRUE;
											SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
										}
									else {
										bPassResult = FALSE;
										ShowWindow(staticError,SW_SHOW);
									}
								}break;
								case PASSTYPE_MACHINE:{
									if(atoi(strInput)  == PASS_MACHINE) 
										{
											bPassResult = TRUE;
											SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
										}
									else {
										bPassResult = FALSE;
										ShowWindow(staticError,SW_SHOW);
									}
								}break;
								default:{
									bPassResult = FALSE;
								}break;
							}
					
				} break;
				case DID_PASSWORD_KEY_CANCEL:{
					SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
				} break;
			}
			//--- ��ĸ--
			if(id>=DID_PASSWORD_KEY_0 && id<=DID_PASSWORD_KEY_9){
				if(iInputCount<MAXINPUT_PW){
					strncat(strInput, strKey[id-DID_PASSWORD_KEY_0], 1);				
	 			//	printf("%s:%d You have input:%s.\n", __FILE__, __LINE__, strInput);
					if(editInput != (HWND)NULL)
						SetWindowText(editInput, strInput);	
					iInputCount++;
				}
				
			}
		
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					SendMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			
			editInput = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}

	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

//-- С���̶Ի���ť����
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	HDC hdc;
	RECT rc;
	
	switch(message){
		case MSG_SETFOCUS:{
			if(IsWindowVisible(staticError))
				ShowWindow(staticError,SW_HIDE);
			SetWindowBkColor(hWnd, SETFOCUS_COLOR);
		}break;
		case MSG_KILLFOCUS:{
			if(IsWindowVisible(staticError))
				ShowWindow(staticError,SW_HIDE);
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}
	
//����С���̶Ի��� --
static void CreatePassWordInput(HWND hWnd)
{
	if(bPassType==PASSTYPE_FACTORY)
 		DlgKeyPassword.y = rcInput.bottom+75;
	else
		DlgKeyPassword.y = rcInput.top-95;
	DlgKeyPassword.x = DLG_X;
	DlgKeyPassword.controls = CtrlKeyPassword;
	DialogBoxIndirectParam(&DlgKeyPassword, hWnd, DlgProcKeyboard, 0L);	
}	

/*
	bType:�������������
*/
int PasswordInput(HWND hWnd, RECT rc, BYTE bType)
{
	rcInput = rc;
	bPassResult = FALSE;
	bPassType = bType;
	CreatePassWordInput(hWnd);
	
	return bPassResult;
}


