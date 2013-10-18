/*************************************************************************** 
 *	Module Name:	AlarmView
 *
 *	Abstract:		������Ϣ��ʾ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-02 16:59:41
 ***************************************************************************/
/*
˼·:
	1��������������ʾ����ѭ����ʾ
	2��Ƶ��Ϊ1��2Hz
	3����ʾҪ���Ĺ�������ѭ����ʾ����������ݣ���������λ�ź�ʱ���жϵ�ǰ��˳��������ʾ		
*/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "dllist.h"
#include "AlarmManage.h"
#include "Dialog.h"

#define ID_PANEL_PHYINFO		DID_ALMINFO
#define ID_PANEL_TECINFO		DID_ALMINFO+1

#define RECTANGLE 		0
//����ʼ��ʱ�ı���ɫ
#define INFOPANEL_BKMODE	SS_BLACKRECT		//SS_GRAYRECT

//�����ھ��
extern HWND ghWndMain;

//����������
extern DLLIST  *gDListAlmPhy;		
//������������
extern DLLIST  *gDListAlmTec;		

//�Ƿ���Ҫ������ʾ������Ϣ
extern BOOL bResetView_Phy;
extern BOOL bResetView_Tec;

//������������̻߳�����
extern pthread_mutex_t mtPhyAlm;	
//�������������̻߳�����
extern pthread_mutex_t mtTecAlm;	


//TODO:���ǵ����̶߳�HDC_SCREEN�Ķ�д����ԭ���Ļ�����ķ�ʽ��Ϊ������������ʾ��� 
//��ʾ�������������Ϣ��������
static RECT rcPhyAlmInfo;	
static HWND hWndPhyAlmInfo;	
//��ʾ��������������Ϣ��������
static RECT rcTecAlmInfo;		
static HWND hWndTecAlmInfo;

//��ʾ������������߳�
pthread_t ptViewPhyAlm;		
//��ʾ�������������߳�
pthread_t ptViewTecAlm;		

//������ʾ�ı�����Ϣ
unsigned char strPhyAlmInfo[ALM_PHY_INFOMAX][ALM_PHY_INFOLEN]={0};
unsigned char strTecAlmInfo[ALM_TEC_INFOMAX][ALM_TEC_INFOLEN]={0};

//�Ƿ���뱨����ͣ״̬
BOOL gbSuspending = FALSE;
//������ͣ״̬����ʱ
int giSuspendTime = 0;


//���ر�����Ϣ����
int ReadAlmInfoFromFile()
{
	
	//�˴�������˳��Ӧ��ͷ�ļ������AlarmID���Ӧ
	unsigned char *sPhyAlmInfoIndex[] = {
		"none", 				//0 �ޱ���
		"hr_upper", 			///1	
		"hr_lower",			///2
		"ecg_weaksignal",		///3
		"rr_upper", 			///4
		"rr_lower",			//5
		"apnea",				//6
		"spo2_upper", 		//7
		"spo2_lower",			//8
		"spo2_lowest",		//9
		"pr_upper", 			//10
		"pr_lower",			//11
		"t1_upper", 			//12
		"t1_lower", 			//13
		"t2_upper", 			//14
		"t2_lower", 			//15
		"td_upper",			//16
		"nsys_upper",			//17
		"nsys_lower", 			//18
		"ndia_upper", 			//19
		"ndia_lower",			//20
		"nmean_upper", 		//21
		"nmean_lower",		//22
		"etco2_upper",		//23
		"etco2_lower",			//24
		"fico2_upper",			//25
		"isys1_upper", 		//26
		"isys1_lower", 		//27
		"idia1_upper", 		//28
		"idia1_lower",			//29
		"imean1_upper",		//30
		"imean1_lower",		//31
		"isys2_upper", 		//32
		"isys2_lower", 		//33
		"idia2_upper",			//34
		"idia2_lower",			//35
		"imean2_upper", 		//36
		"imean2_lower",		//37
		"st1_upper", 			//38
		"st1_lower",			//39
		"st2_upper", 			//40
		"st2_lower",			//41
		"arr"					//42
	};	
						
	unsigned char *sTecAlmInfoIndex[] = {
		"none", 						//0
		"ecg_comm_error",				///1
		"ecg_comm_stop",				///2
		"ecg_module_error",			///3
		"ecg_v_off",					///4
		"ecg_ra_off",					//5
		"ecg_ll_off",					//6
		"ecg_la_off",					//7
		"ecg_limit_error",				//8
		"ecg_interfere",				//9
		"ecg_over_range",				//10
		"ecg_overload1",				//11
		"ecg_overload2",				//12
		"ecg_overload3",				//13
		"resp_limit_error",				//14
		"resp_over_range",				//15
		"resp_cva",					//16
		"spo2_comm_error",			//17
		"spo2_comm_stop",			//18
		"spo2_module_initerror",		//19
		"spo2_module_error",			//20
		"spo2_limit_error",				//21
		"spo2_over_range",			//22
		"spo2_no_sensor",				//23
		"spo2_sensor_error",			//24
		"spo2_unknown",				//25
		"spo2_incompatible",			//26
		"spo2_sensor_off",				//27
		"spo2_error",					//28
		"pulse_limit_error",				//29
		"pulse_over_range",			//30
		"nibp_comm_error", 			//31
		"nibp_comm_stop", 			//32
		"nibp_module_error", 			//33
		"nsys_limit_error",				//34
		"ndia_limit_error",				//35
		"nmean_limit_error",			//36
		"nibp_over_range",				//37
		"nibp_error",					//38
		"temp_comm_error",			//39
		"temp_comm_stop",			//40
		"temp_module_error",			//41
		"t1_over_range",				//42
		"t2_over_range",				//43
		"t1_sensor_off",				//44
		"t2_sensor_off",				//45
		"t1_limit_error",				//46
		"t2_limit_error",				//47
		"td_limit_error",				//48
		"co2_comm_error",				//49
		"co2_comm_stop",				//50
		"co2_module_error",			//51
		"co2_over_range",				//52
		"etco2_limit_error",			//53
		"fico2_limit_error",				//54
		"awrr_limit_error",				//55
		"ibp1_comm_error",			//56
		"ibp1_comm_stop",				//57
		"ibp1_module_error",			//58
		"isys1_limit_error",				//59
		"idia1_limit_error",				//60
		"imean1_limit_error",			//61
		"ibp1_over_range",				//62
		"ibp1_sensor_off",				//63
		"ibp2_comm_error",			//64
		"ibp2_comm_stop",				//65
		"ibp2_module_error",			//66
		"isys2_limit_error",				//67
		"idia2_limit_error",				//68
		"imean2_limit_error",			//69
		"ibp2_over_range",				//70
		"ibp2_sensor_off",				//71
		"battery_low",					//72
		"sys_33vhigh",				//73
		"sys_33vlow",					//74
		"sys_5vhigh",					//75
		"sys_5vlow",					//76
		"selftest_watchdog",			//77
		"selftest_ad",					//78
		"selftest_ram",				//79
		"selftest_rom",				//80
		"selftest_cpu",				//81
		"selftest_ibp",					//82
		"selftest_nbp",				//83
		"selftest_spo2",				//84
		"selftest_temp",				//85
		"selftest_resp",				//86
		"selftest_ecg",				//87
		"power_abnormal",				//88
		"power_force",				//89
		"power_comm",				//90
		
	};	
		
	int i;	
	//��ȡ����������Ϣ
	for(i=0; i<ALM_TEC_INFOMAX; i++){
		memset(strTecAlmInfo[i], 0, ALM_TEC_INFOLEN);
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", sTecAlmInfoIndex[i], strTecAlmInfo[i], ALM_TEC_INFOLEN);
// 		if(i ==AT_SPO2_SENSOROFF) printf("Tec Alarm Info Index is %d --- %s.\n", AT_SPO2_SENSOROFF, sTecAlmInfoIndex[i]);
	}

	//��ȡ�������������Ϣ
	for(i=0; i<ALM_PHY_INFOMAX; i++){
		memset(strPhyAlmInfo[i], 0, ALM_PHY_INFOLEN);
		GetStringFromResFile(gsLanguageRes, "PHY_INFO", sPhyAlmInfoIndex[i], strPhyAlmInfo[i], ALM_PHY_INFOLEN);
 	//	if(i ==AP_ST1_HIGH) printf("Phy Alarm Info Index is %d --- %s.\n", AP_ST2_LOW, sPhyAlmInfoIndex[i]);
	}

	return 0;
}

		
//��������
static int CleanRect(HDC hdc, const PRECT prc)
{

	if(prc !=(PRECT)NULL){
		SetBrushColor(hdc, NONE_BKCOLOR);
		FillBox(hdc, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top);
		return 0;
	}
	else{
		return -1;
	}
}

//��������
static int CleanPanels(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	
	if(hWnd == (HWND)NULL) return -1;
	
	GetClientRect(hWnd, &rc);
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	SetBrushColor(hdc, NONE_BKCOLOR);
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

	
	ReleaseDC(hdc);
	return 0;
}

//��ʼ����ʾ���
static int InitAlmInfoPanels()
{
	
	int length = (SCREEN_RIGHT-105-120-100-2)/2;
	
	hWndPhyAlmInfo = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | INFOPANEL_BKMODE,
				      ID_PANEL_PHYINFO, SCREEN_LEFT+100+length+2, SCREEN_TOP, length, 30, ghWndMain, 0);
	
	hWndTecAlmInfo = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | INFOPANEL_BKMODE,
				      ID_PANEL_TECINFO, SCREEN_LEFT+100, SCREEN_TOP, length, 30, ghWndMain, 0);
	
	return 0;
}

/*
	��ʾ������������Ϣ����
	return: 0-OK, -1-Err
*/
static int ViewSinglePhyInfo(HWND hWnd, const B_ALM_INFO *info)
{
	int  bkColor;
	int  txtColor;
	HDC hdc; 
	RECT rc,rc_str;
	unsigned char strInfo[20] = {0};
	int i;
	if(hWnd == (HWND)NULL) return -1;
	
	GetClientRect(hWnd, &rc);
	rc_str.left = rc.left + 30;
	rc_str.right = rc.right-2;
	rc_str.top = rc.top;
	rc_str.bottom = rc.bottom;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;

		//Get BkColor
		switch(info->bClass){
			case ALARM_LOW:{bkColor = LOWALM_BKCOLOR;}break;
			case ALARM_MID:{bkColor = MIDALM_BKCOLOR;  }break;
			case ALARM_HIGH:{bkColor = HIGHALM_BKCOLOR; }break;
			default:{bkColor = INFO_BKCOLOR;}break;
		}
		txtColor = INFO_TXTCOLOR;
	
		SetBrushColor(hdc, bkColor);
		FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		SetBkColor(hdc, bkColor);
		SetTextColor(hdc, txtColor);
 		SelectFont(hdc, gFontSystem); 
		
		if(info !=NULL){
			//Class
			memset(strInfo, 0, sizeof strInfo);
			switch(info->bClass){
				case ALARM_LOW:{
					snprintf(strInfo, sizeof strInfo, " *");
				}break;	
				case ALARM_MID:{
					snprintf(strInfo, sizeof strInfo, " **");
				}break;	
				case ALARM_HIGH:{
					snprintf(strInfo, sizeof strInfo, " ***");
				}break;	
			}
			DrawText(hdc, strInfo, -1, &rc, DT_NOCLIP | DT_SINGLELINE| DT_LEFT| DT_VCENTER);
			//Info
	
//���8�����������СӢ�ı�����Ϣ��ʾ���£���Ҫ�����д���				
#if SCREEN_640
			if(gCfgSystem.bLanguage == LANGUAGE_ENGLISH&&strlen(strPhyAlmInfo[(info->bAlmID)])>22){
				DrawText(hdc, strPhyAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | ES_AUTOWRAP| DT_CENTER | DT_VCENTER);				
			}else{
				DrawText(hdc, strPhyAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | DT_SINGLELINE| DT_CENTER | DT_VCENTER);
			}
#else
			DrawText(hdc, strPhyAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | DT_SINGLELINE| DT_CENTER | DT_VCENTER);
#endif
		}
			

	
	ReleaseDC(hdc);
}

/*
	��ʾ��������������Ϣ����
	return: 0-OK, -1-Err
*/
static int ViewSingleTecInfo(HWND hWnd, const B_ALM_INFO *info)
{
	int  bkColor;
	int  txtColor;
	HDC hdc;
	RECT rc,rc_str;
	unsigned char strInfo[20] = {0};
			
	if(hWnd == (HWND)NULL) return -1;
	
	GetClientRect(hWnd, &rc);
	rc_str.left = rc.left + 30;
	rc_str.right = rc.right-2;
	rc_str.top = rc.top;
	rc_str.bottom = rc.bottom;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	//Get BkColor
	switch(info->bClass){
		case 1:{bkColor = LOWALM_BKCOLOR;}break;
		case 2:{bkColor = MIDALM_BKCOLOR;  }break;
		case 3:{bkColor = HIGHALM_BKCOLOR; }break;
		default:{bkColor = INFO_BKCOLOR;}break;
	}
	txtColor = INFO_TXTCOLOR;
	
	SetBrushColor(hdc, bkColor);
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	SetBkColor(hdc, bkColor);
	SetTextColor(hdc, txtColor);
	SelectFont(hdc, gFontSystem); 
	if(info !=NULL){
		//Class
		memset(strInfo, 0, sizeof strInfo);
		switch(info->bClass){
			case ALARM_LOW:{
				snprintf(strInfo, sizeof strInfo, " *");
			}break;	
			case ALARM_MID:{
				snprintf(strInfo, sizeof strInfo, " **");
			}break;	
			case ALARM_HIGH:{
				snprintf(strInfo, sizeof strInfo, " ***");
			}break;	
		}
		DrawText(hdc, strInfo, -1, &rc, DT_NOCLIP | DT_SINGLELINE| DT_LEFT| DT_VCENTER);
			
//���8�����������С��ʾ���£�ֻ��Ӣ�Ļ���ʾ����,��Ҫ�����д���				
#if SCREEN_640
			if(gCfgSystem.bLanguage == LANGUAGE_ENGLISH&&strlen(strTecAlmInfo[(info->bAlmID)])>22){
			
				DrawText(hdc, strTecAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | ES_AUTOWRAP| DT_CENTER | DT_VCENTER);				
			}else{
				DrawText(hdc, strTecAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | DT_SINGLELINE| DT_CENTER | DT_VCENTER);
			}
#else
			DrawText(hdc, strTecAlmInfo[(info->bAlmID)], -1, &rc_str, DT_NOCLIP | DT_SINGLELINE| DT_CENTER | DT_VCENTER);
#endif
		}
	

	ReleaseDC(hdc);
}

//------------------------ interface -----------------------
/*
	��ʼ��������ʾ
*/
int InitAlarmView()
{
	int res;
	
	//��ʼ����ʾ���
	InitAlmInfoPanels();
	
	//���ر�����Ϣ����
	ReadAlmInfoFromFile();
	
	return 0;
}

//��ʾ������������Ϣ����,(��������)
static int ViewSinglePhyInfoByIndex(int iIndex)
{
	B_ALM_INFO *pFindInfo=NULL;		//���ҵ��ļ�¼
	B_ALM_INFO FindInfo;				//���ҵ��ļ�¼
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	int res, i;
	
	//����ָ�������ļ�¼
	FindAddr = DLGetFirst(gDListAlmPhy);
	
	if(FindAddr != NULL){
		pthread_mutex_lock(&mtPhyAlm);
		for(i=0; i<iIndex; i++){
			FindAddr = FindAddr->Next;	
		}
		pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
		FindInfo = *pFindInfo;
		pthread_mutex_unlock(&mtPhyAlm);
					
		if(pFindInfo !=NULL){
			ViewSinglePhyInfo(hWndPhyAlmInfo, &FindInfo);
		}
	}
	else{
		if(B_PRINTF) printf("%s:%d No pyhinfo view, index:%d.\n", __FILE__, __LINE__, iIndex);
	}
	
	return iIndex;
}

//��ʾ��������Ϣ
int ViewPhyInfo()
{
	int iPhyAlmInfoNum = 0;			//������Ϣ����
	static int iViewIndex = 0;			//����ʾ�ı�����Ϣ������
	
	iPhyAlmInfoNum = DLCount((DLLIST *)gDListAlmPhy);
 	//if(B_PRINTF) printf("%s:%d iPhyAlmInfoNum is %d.\n", __FILE__, __LINE__, iPhyAlmInfoNum);
	
	if(iPhyAlmInfoNum<=0){
		iViewIndex = 0;
		CleanPanels(hWndPhyAlmInfo);
		return 0;
	}
	else{
		if(bResetView_Phy){
			bResetView_Phy = FALSE;
			//�����λ�����ͷ��ʾ
			iViewIndex = 0;
// 			if(B_PRINTF) printf("%s:%d phyinfo view reset.\n", __FILE__, __LINE__, iViewIndex);
		}
 		//if(B_PRINTF) printf("%s:%d View %d phyinfo.\n", __FILE__, __LINE__, iViewIndex);
		ViewSinglePhyInfoByIndex(iViewIndex);
		
		if(iViewIndex<(iPhyAlmInfoNum-1))
			iViewIndex +=1;
		else
			iViewIndex =0;
	}
	
	return 0;
	
	if(gbSuspending){
		ViewSinglePhyInfo(hWndPhyAlmInfo, NULL);
// 		sleep(1);
	}
		
	return 0;
}
//ɾ��
static int Clean_Alarm_info(int iIndex)
{
	B_ALM_INFO *pFindInfo=NULL;		//���ҵ��ļ�¼
	B_ALM_INFO FindInfo;				//���ҵ��ļ�¼
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	int res, i;
	
	//����ָ�������ļ�¼
	FindAddr = DLGetFirst(gDListAlmTec);
	
	if(FindAddr != NULL){
		pthread_mutex_lock(&mtTecAlm);
		for(i=0; i<iIndex; i++){
			FindAddr = FindAddr->Next;	
		}
		pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
		FindInfo = *pFindInfo;
		pthread_mutex_unlock(&mtTecAlm);
					
		if(pFindInfo !=NULL){
			ViewSingleTecInfo(hWndTecAlmInfo, &FindInfo);
			AlarmLifted_Tec(FindInfo);
		}
	}
	else{
		if(B_PRINTF) printf("%s:%d No tecinfo view, index:%d.\n", __FILE__, __LINE__, iIndex);
	}
	
	return iIndex;
}

//��ʾ��������������Ϣ����,(��������)
static int ViewSingleTecInfoByIndex(int iIndex)
{
	B_ALM_INFO *pFindInfo=NULL;		//���ҵ��ļ�¼
	B_ALM_INFO FindInfo;				//���ҵ��ļ�¼
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	int res, i;
	
	//����ָ�������ļ�¼
	FindAddr = DLGetFirst(gDListAlmTec);
	
	if(FindAddr != NULL){
		pthread_mutex_lock(&mtTecAlm);
		for(i=0; i<iIndex; i++){
			FindAddr = FindAddr->Next;	
		}
		pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
		FindInfo = *pFindInfo;
		pthread_mutex_unlock(&mtTecAlm);
					
		if(pFindInfo !=NULL){
			ViewSingleTecInfo(hWndTecAlmInfo, &FindInfo);
			
		}
	}
	else{
		if(B_PRINTF) printf("%s:%d No tecinfo view, index:%d.\n", __FILE__, __LINE__, iIndex);
	}
	
	return iIndex;
}

//��ʾ����������Ϣ
int ViewTecInfo()
{
	int iTecAlmInfoNum = 0;			//������Ϣ����
	static int iViewIndex = 0;			//����ʾ�ı�����Ϣ������
	int i;
	iTecAlmInfoNum = DLCount((DLLIST *)gDListAlmTec);
// 	if(B_PRINTF) printf("%s:%d iTecAlmInfoNum is %d.\n", __FILE__, __LINE__, iTecAlmInfoNum);
	
	if(iTecAlmInfoNum<=0){
		iViewIndex = 0;
		CleanPanels(hWndTecAlmInfo);
		return 0;
	}
	else{
		if(bResetView_Tec){
			bResetView_Tec = FALSE;
			//�����λ�����ͷ��ʾ
			iViewIndex = 0;
// 			if(B_PRINTF) printf("%s:%d phyinfo view reset.\n", __FILE__, __LINE__, iViewIndex);
		}
 	//	if(B_PRINTF) printf("%s:%d View %d Tecinfo.\n", __FILE__, __LINE__, iViewIndex);
		ViewSingleTecInfoByIndex(iViewIndex);
		
		if(iViewIndex<(iTecAlmInfoNum -1))
			iViewIndex +=1;
		else
			iViewIndex =0;
	}
	
//	return 0;
	
	if(gbSuspending){
		for(i=0;i<iTecAlmInfoNum;i++)
		Clean_Alarm_info(i);
// 		sleep(1);
	}
		
	return 0;
}

