/*************************************************************************** 
 *	Module Name:	Dialog
 *
 *	Abstract:	�˵���غ���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-09 14:09:42
 ***************************************************************************/
#ifndef __DIALOG_H__
#define __DIALOG_H__

//-- ��ͬ״̬�°�ť�ı���ɫ��--
#define 	SETFOCUS_COLOR	COLOR_darkcyan
#define 	KILLFOCUS_COLOR	COLOR_lightgray

//���ʱ��ť����ɫ
#define BROWSE_COLOR		COLOR_green


//�����ھ��
HWND ghWndMain;
//���ֵ��ڿؼ�

#define ADJUST_INT			0	//��������
#define ADJUST_FLOAT	       1	//���ڸ�����

/*
	��������
	rc:��ǰ��Ҫ�������ֵĿؼ�������
*/
int AdjustIntNumber(HWND hWnd, RECT rc, int iNumber, int iMax, int iMin, int iInter, unsigned char *strUnit);
int AdjustFloatNumber(HWND hWnd, RECT rc, double fNumber, double fMax, double fMin, double fInter, unsigned char *strUnit);

//��ǰ��Ҫ�������ֵĿؼ�
HWND ghWnd_Adjust;
//�ؼ�������
RECT gRC_Adjust;
//�Ի��������
RECT gRCDlg_Adjust;
//��������
#define DLG_X 			0
#define DLG_Y  			35
#define DLG_LENGHT  	250
#define DLG_HIGH  		370

#define FIRSTBTN_X		5
#define FIRSTBTN_Y		30
#define BTN_HIGH		30
#define BTN_LEN			115

//SetWindowElementColorEx �������1.3.3��1.6.10
#define FONTCOLOR 	1

/*
	С��������
*/
//��Ҫ��������Ŀؼ�
HWND ghWnd_Input;
//�ؼ�������
RECT gRC_Input;
//�Ի��������
RECT gRCDlg_Input;
//С�������뺯��

#define B_ES_BLACKBK	0x00000005L	

/*
	ȷ����Ϣ�Ի���
*/
//��Ϣ���س̶�
#define ACK_NORMAL			0
#define ACK_ATTENTION		1
#define ACK_TERRIBLE		2


//panel���ID
#define DID_NIBPLIST			1000
#define PANEL_EXTEND		1200

//�Ի���ID�ͽ���������Ϣ�Ի���
//other
#define IDD_COLOR			1300
#define DID_POWER			 1400
#define DID_KEYINPUT		 	1500
#define DID_DF				 1600
#define DID_ACk				 1700
#define DID_SETCOLOR		 1800
#define DID_PATIENT			 1900
#define DID_ALARMCTRL		 2000
#define DID_TRENDGRAPH	  	2100
#define DID_TRENDTABLE		  2200
#define DID_SCREENREVIEW	 2300
#define DID_ALARMREVIEW	 2400
#define DID_WAVEREVIEW	 	2500
#define DID_MAINMENU		 2600
#define DID_MONITORMENU	 2700
#define DID_WAVESET		 	2800
#define DID_TRENDSET		 2900
#define DID_SHORTTRENDSET	 3000
#define DID_SYSTEMSET		 3100
#define DID_SYSINFO			 3200
#define DID_RECORDER		 3300
#define DID_TIMESET			 3400
#define DID_MODECONF		 3500
#define DID_ALARMLEVEL		 3600
#define DID_MACHINE			 3700
#define DID_PASSWORD		 3800
#define DID_MAINTMODULE	 3900
#define DID_MAINTNIBP		 4000
#define DID_MAINTSYS		 	 4100
#define DID_MODULESET		 4200
#define DID_ALMSETUP		 4300
#define DID_COMMONALM		 4400
#define DID_DRUG			 4500
#define DID_DRUGTITR		 4600
#define DID_NEWECG			 4700
#define DID_NEWECGSET		 4800
#define DID_NEWECGALM		 4900
#define DID_NEWECGST		 5000
#define DID_NEWRESP		 5100
#define DID_NEWRESPALM	 	 5200
#define DID_NEWSPO2		 	 5300
#define DID_NEWSPO2ALM	 	 5400
#define DID_NEWPRALM		 5500
#define DID_NEWTEMP		 5600
#define DID_NEWNIBP		 	 5700
#define DID_NEWNIBPALM		 5800
#define DID_NEWFACTORY	 	 5900
#define DID_ALMINFO			 6000
#define DID_NET				 6100
#define DID_PRINTER			 6200
#define DID_CO2 				 6300
#define DID_NEWIBP1                     6400
#define DID_NEWIBP1a                     6600
#define DID_NEWIBP2                     6700
#define DID_NEWIBP2a                     6800
#define DID_NEWECGARR                   6900

int ACKDialog(HWND hWnd, const char *info, BYTE bClass);
void CreateDlgPatient(HWND hWnd);
void CreateDlgColor(HWND hWnd);
void CreateDlgAlarmCtrl(HWND hWnd);
void CreateDlgAlarmPrint(HWND hWnd);
void CreateTrendGraph(HWND hWnd); 
void CreateTrendTable(HWND hWnd);
int CreateScreenReview(HWND hWnd);
void CreateAlarmReview(HWND hWnd);
void CreateWaveReview(HWND hWnd);
void CCreateMainMenu(HWND hWnd);
void CreateMonitorSetupMenu(HWND hWnd);
void CreateWaveformSet(HWND hWnd);
void CreateTrendSet(HWND hWnd);
void CreateShortTrendSet(HWND hWnd);
void CreateSystemSet(HWND hWnd);
void CreateSystemINFO(HWND hWnd);
void CreateRecorderSet(HWND hWnd);
void CreateTimeSet(HWND hWnd);
void CreateModeConfig(HWND hWnd);
void CreateAlarmLevel(HWND hWnd);
void CreateAlarmLevelSet(HWND hWnd);
void CreateMachineSet(HWND hWnd);
int PasswordInput(HWND hWnd, RECT rc,BYTE bType);
void CreateMaintModule(HWND hWnd);
void CreateMaintNIBP(HWND hWnd);
void CreateMaintSYS(HWND hWnd);
void CreateModuleSet(HWND hWnd);
void CreateAlarmSetup(HWND hWnd);
void CreateCommonAlarm(HWND hWnd);
void CreateDRUG(HWND hWnd);
void CreateDRUGTITR(HWND hWnd);
void CreateDlgNewECG(HWND hWnd);
void CreateDlgNewECGSET(HWND hWnd);
void CreateDlgNewECGALM(HWND hWnd);
void CreateDlgNewECGSTAdjust(HWND hWnd);
void CreateDlgNewRESP(HWND hWnd);
void CreateDlgNewRESPALM(HWND hWnd);
void CreateDlgNewSpo2(HWND hWnd);
void CreateDlgNewSpo2ALM(HWND hWnd);
void CreateDlgNewPRALM(HWND hWnd);
void CreateDlgNewTemp(HWND hWnd);
void CreateDlgNewNIBP(HWND hWnd);
void CreateDlgNewNIBPALM(HWND hWnd);
void CreateDlgNewFactory(HWND hWnd);
void CreateDlgNewIBP1(HWND hWnd);
void CreateDlgNewIBP1a(HWND hWnd);
void CreateDlgNewIBP2(HWND hWnd);
void CreateDlgNewIBP2a(HWND hWnd);
void CreateDlgNewEcgARR(HWND hWnd);

#endif	//__DIALOG_H__
