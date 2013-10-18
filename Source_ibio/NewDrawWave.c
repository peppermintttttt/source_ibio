/*************************************************************************** 
 *	Module Name:	NewDrawWave
 *
 *	Abstract:	�µĻ�ͼ������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-01-18 16:25:48
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "NewDrawWave.h"
#include "DrawWave.h"


//�û��Բ�����������, ������:  Normal, Short Trend, Nibp List
WAVE_PANEL_CFG gWaveCfg_User[WAVEPANEL_MAX_COUNT];


//��ͼNormal�Ĳ�������
NWAVE_CFG gNWaveCfg_Normal[NWAVEPANEL_MAX_COUNT];
//��ͼ ShortTrend ��������
NWAVE_CFG gNWaveCfg_ShortTrend[NWAVEPANEL_MAX_COUNT];
//��ͼ Multi ECG ��������
NWAVE_CFG gNWaveCfg_MultiEcg[NWAVEPANEL_MAX_COUNT];
//��ͼ7lead ECG ��������
NWAVE_CFG gNWaveCfg_7LeadEcg[NWAVEPANEL_MAX_COUNT];
//��ͼ Big Character ��������
NWAVE_CFG gNWaveCfg_BigChar[NWAVEPANEL_MAX_COUNT];
//��ͼ oxyCrg ��������
NWAVE_CFG gNWaveCfg_OxyCrg[NWAVEPANEL_MAX_COUNT];
//��ͼ NIBP List ��������
NWAVE_CFG gNWaveCfg_NibpList[NWAVEPANEL_MAX_COUNT];
//��ͼ 12lead ��������
NWAVE_CFG gNWaveCfg_12Lead[NWAVEPANEL_MAX_COUNT];
//��ͼ other bed ��������
NWAVE_CFG gNWaveCfg_OhterBed[NWAVEPANEL_MAX_COUNT];
//��ͼ IBP ��������
NWAVE_CFG gNWaveCfg_IBP[NWAVEPANEL_MAX_COUNT];

//�������
NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];
//������������
int giPanelCount = 0;
//������������(�����û������������ɵ�����,�ݴ����򴴽����) 
WAVE_PANEL gWavePanels[WAVEPANEL_MAX_COUNT];

//���ε���ʾ����(����ڲ�������Ϣ����ʾ����, �ڸ������ڣ������������ʾ����)
RECT gNRcWaves;
RECT gNRcWavesAll;	//����Short Trend, oxyCRG, etc.

//�Ƿ��⵽���ʺ�����
extern BOOL gbHaveBeep_QRS;
extern BOOL gbHaveBeep_Pulse;
//����ͼ����ʾ����
// RECT gNRCEcg_Icon={760, 36, 800, 52};
RECT gNRCEcg_Icon;

///1mm��Ӧ���صĵ���
float gfNMMToPixel = 3.2;

//���Ʋ��λ��Ƶ��ź���
sem_t	semNDrawWave;

//���θ�λ����
BOOL gbResetWaves = TRUE;

//�����Ƿ���Խ��л��Ƶı���
BOOL gbNCanDrawWave = FALSE;
//�����Ƿ���Խ��л��Ƶı���
BOOL gbNCanDrawWave_OB = FALSE;

//���Ƶڼ���������־λ
extern int gbViewPara;
//��ʱ���߳�
static pthread_t 	ptNewDrawTimer;	
BOOL gbHasCreatedTimer = FALSE;

//�Ƿ���뱨����ͣ״̬
extern BOOL gbSuspending;
//������ͣ״̬����ʱ
extern int giSuspendTime;
//��ͼ�߳��л�������
pthread_mutex_t 	mtDrawWave;	

//ECG����
float gfMMToPixel=3.2;				//1mm=3.2pixel
float gfLSB_Ecg = 0.0131579;		//ECG AD�����������Чλ 1/gSamplesPerMV, (��δʹ��)
// static float gSamplesPerMV = 76;			//1mv��Ӧ�Ĳ�������
float gSamplesPerMV = 62;		///1mv��Ӧ�Ĳ�������

//���������ļ���ʼ���û��Ĳ������� gNWaveCfg_Normal
static int InitUsrWaveConfig(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	int i;
	
	if(pCfgFiles == NULL) return -1;
		
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_Normal[i].bID = NWID_NONE;
		gNWaveCfg_Normal[i].bSwitch = SWITCH_OFF;
	}
	
	//Wave 0	 ECG1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[0].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[0].bSwitch = iCfgValue;
	//Wave 1 ECG 2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[1].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[1].bSwitch = iCfgValue;
	//Wave 2 Pleth
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[2].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[2].bSwitch = iCfgValue;
	
	//Wave 3 RESP
	if(gbHaveCo2){
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_id", &iCfgValue);
		if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
		gNWaveCfg_Normal[3].bID = iCfgValue;
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_switch", &iCfgValue);
		if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
		gNWaveCfg_Normal[3].bSwitch = iCfgValue;

	} else{
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_id", &iCfgValue);
		if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
		gNWaveCfg_Normal[3].bID = iCfgValue;
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_switch", &iCfgValue);
		if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
		gNWaveCfg_Normal[3].bSwitch = iCfgValue;
	}
	
	/*
	//Wave 4 IBP1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[4].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[4].bSwitch = iCfgValue;
	if(!gbHaveIbp) gNWaveCfg_Normal[4].bSwitch = SWITCH_OFF;

	//Wave 5 IBP2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[5].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[5].bSwitch = iCfgValue;
	if(!gbHaveIbp) gNWaveCfg_Normal[5].bSwitch = SWITCH_OFF;
	/*
	//Wave 6 CO2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[6].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[6].bSwitch = iCfgValue;
	if(!gbHaveCo2) gNWaveCfg_Normal[6].bSwitch = SWITCH_OFF;
	*/
	return 0;
}

/*
	��ʼ������������Ϣ
	Normal: �����û������ö���
	ShortTrend: ͬNormal
	MultiECG: ECG + SpO2 + Resp/CO2
	NIBP List: ECG1 + SpO2 + Resp / CO2
	oxyCRG: ECG1 + SpO2 + Resp / CO2
	BigChar: ECG1 + SpO2 
*/
static int InitWaveConfig()
{
	int i;
		
	// Normal�������û����ö���
	InitUsrWaveConfig(gFileSetup);
	
	//ShortTrend ͬNormal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_ShortTrend[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_ShortTrend[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
	//MultiECG:  ECG  + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_MultiEcg[i].bID = NWID_NONE;
		gNWaveCfg_MultiEcg[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_MultiEcg[0].bID = NWID_MULTIECG;
	gNWaveCfg_MultiEcg[0].bSwitch = SWITCH_ON;
	gNWaveCfg_MultiEcg[1].bID = NWID_SPO2;
 	gNWaveCfg_MultiEcg[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_MultiEcg[2].bID = NWID_CO2;
		gNWaveCfg_MultiEcg[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_MultiEcg[2].bID = NWID_RESP;
		gNWaveCfg_MultiEcg[2].bSwitch = SWITCH_ON;
	}
	
	//7LeadECG:  ECG 
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_7LeadEcg[i].bID = NWID_NONE;
		gNWaveCfg_7LeadEcg[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_7LeadEcg[0].bID = NWID_MULTIECG;
	gNWaveCfg_7LeadEcg[0].bSwitch = SWITCH_ON;
 	
	//NIBP List: ECG1 + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_NibpList[i].bID = NWID_NONE;
		gNWaveCfg_NibpList[i].bSwitch = SWITCH_OFF;
	}
	//NIBP List ͬNormal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_NibpList[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_NibpList[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
	//oxyCRG: ECG1 + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OxyCrg[i].bID = NWID_NONE;
		gNWaveCfg_OxyCrg[i].bSwitch = SWITCH_OFF;
	}
		// oxyCRG ͬNormal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OxyCrg[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_OxyCrg[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	/*
	gNWaveCfg_OxyCrg[0].bID = NWID_ECG1;
	gNWaveCfg_OxyCrg[0].bSwitch = SWITCH_ON;
	gNWaveCfg_OxyCrg[1].bID = NWID_SPO2;
	gNWaveCfg_OxyCrg[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_OxyCrg[2].bID = NWID_CO2;
		gNWaveCfg_OxyCrg[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_OxyCrg[2].bID = NWID_RESP;
		gNWaveCfg_OxyCrg[2].bSwitch = SWITCH_ON;
	}
	*/
	//BigChar: ECG1 + SpO2 
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_BigChar[i].bID = NWID_NONE;
		gNWaveCfg_BigChar[i].bSwitch = SWITCH_OFF;
	}
		// BigChar ͬNormal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_BigChar[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_BigChar[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
		//IBP: ECG1 + SpO2 +RESP+IBP1+IBP2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_IBP[i].bID = NWID_NONE;
		gNWaveCfg_IBP[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_IBP[0].bID = NWID_ECG1;
	gNWaveCfg_IBP[0].bSwitch = SWITCH_ON;
	gNWaveCfg_IBP[1].bID = NWID_SPO2;
	gNWaveCfg_IBP[1].bSwitch = SWITCH_ON;	
	
	if(gbHaveCo2) {
		gNWaveCfg_IBP[2].bID = NWID_CO2;
		gNWaveCfg_IBP[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_IBP[2].bID = NWID_RESP;
		gNWaveCfg_IBP[2].bSwitch = SWITCH_ON;
	}
	gNWaveCfg_IBP[3].bID = NWID_IBP1;
	gNWaveCfg_IBP[3].bSwitch = SWITCH_ON;
	gNWaveCfg_IBP[4].bID = NWID_IBP2;
	gNWaveCfg_IBP[4].bSwitch = SWITCH_ON;
	
	/*
	gNWaveCfg_BigChar[0].bID = NWID_ECG1;
	gNWaveCfg_BigChar[0].bSwitch = SWITCH_ON;
	gNWaveCfg_BigChar[1].bID = NWID_SPO2;
	gNWaveCfg_BigChar[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_BigChar[2].bID = NWID_CO2;
		gNWaveCfg_BigChar[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_BigChar[2].bID = NWID_RESP;
		gNWaveCfg_BigChar[2].bSwitch = SWITCH_ON;
	}
	*/
	//12Lead:  12 ECG waves
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_12Lead[i].bID = NWID_NONE;
		gNWaveCfg_12Lead[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_12Lead[0].bID = NWID_12LEAD;
	gNWaveCfg_12Lead[0].bSwitch = SWITCH_ON;
 	
	// Otherbed ͬNormal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OhterBed[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_OhterBed[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	return 0;
}
/*
	ͨ�����εķ�Χ���㲨����������ϵ��(Y���ϵı���ϵ��)
*/
static int SetWaveY(PWAVE_PANEL pPanel)
{
	int iValueMax;
	int iValueMin;
	float fYAxis=0.0;		//y�����ϵ��
	RECT rcPanel;		//�������Ĵ�С
	
	if(pPanel == NULL) return -1;
	
	//ȡ�ò����ò��εķ�Χ
	switch(pPanel->bID){
		case WID_ECG1:{ 
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break; 
		case WID_ECG2:{ 
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break; 
		case WID_MULTIECG:{
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break;
		case WID_SPO2:{
			iValueMax = NWAVEMAX_SPO2;
			iValueMin = 0; 
		}break;
		case WID_RESP:{
			iValueMax = NWAVEMAX_RESP;
			iValueMin = 0; 
		}break;
		case WID_CO2:{
			switch(gCfgCO2.bRange){
				case CO2_RANGE_MIDDLE:{
					iValueMax = 75;			
				}break;
				case CO2_RANGE_SMALL:{
					iValueMax = 99;
				}break;
				default:{
					iValueMax = 50;
				}break;
			}
			iValueMin = 0; 
		}break;
		case WID_IBP1:{
			iValueMax = gCfgIbp1.iScaleTop;
			iValueMin = gCfgIbp1.iScaleBottom; 
		}break;
		case WID_IBP2:{
			iValueMax = gCfgIbp2.iScaleTop;
			iValueMin = gCfgIbp2.iScaleBottom; 
		}break;
		
	}
	pPanel->iYMax = iValueMax;
	pPanel->iYMin = iValueMin;

	//���ݲ��εķ�Χ������ʵ�ʴ�С�����㲨����Y���ϵ�����ϵ��
	if(pPanel->hWnd == (HWND)NULL ) return -1;
	GetClientRect(pPanel->hWnd, &rcPanel);
		
	fYAxis = (float)((float)(iValueMax-iValueMin) / (float)((float)(rcPanel.bottom-rcPanel.top)/(float)pPanel->bWaveCount));	
	pPanel->fYAxis = fYAxis;
			
 	printf("(%s:%d)MAX: %d  MIN:%d    RECT: H-%d    Y Axis: %f  \n", __FILE__, __LINE__,
 	       pPanel->iYMax, pPanel->iYMin,  (rcPanel.bottom-rcPanel.top)/pPanel->bWaveCount, pPanel->fYAxis);
	
	pPanel->bReset = TRUE;
	
	return 0;
}

/*
    ==================================================
	Description:	������ϵ��������
			
   	Parameters:								method of calling:(r/w/rw/other)
   		hdc : ��ͼ����									rw
		ViewOrg_X, ViewOrg_Y: ��ͼԭ��;                   r
		ViewExt_X, ViewExt_Y:  ��ͼ��Χ;                   r
		WindOrg_X, WindOrg_Y: ����ԭ��;                 r
		WindExt_X, WindExt_Y: ���ڷ�Χ                   r
				
	return:0-OK, -1-Error
    ===================================================
*/
int ChangeCoord(HDC hdc, int ViewOrg_X, int ViewOrg_Y, int ViewExt_X, int ViewExt_Y, int WindOrg_X, int WindOrg_Y, int WindExt_X, int WindExt_Y)
{
	POINT Pt_ViewOrg;
	POINT Pt_ViewExt;
	POINT Pt_WindExt;
	POINT Pt_WindOrg;

	Pt_ViewOrg.x = ViewOrg_X;
	Pt_ViewOrg.y = ViewOrg_Y;
	Pt_ViewExt.x = ViewExt_X;
	Pt_ViewExt.y = ViewExt_Y;
	Pt_WindOrg.x = WindOrg_X;
	Pt_WindOrg.y = WindOrg_Y;
	Pt_WindExt.x = WindExt_X;
	Pt_WindExt.y = WindExt_Y;
	
	SetMapMode(hdc, MM_ANISOTROPIC);
	SetViewportOrg(hdc, &Pt_ViewOrg);
	SetViewportExt(hdc, &Pt_ViewExt);         
	SetWindowExt(hdc, &Pt_WindExt);
	SetWindowOrg(hdc, &Pt_WindOrg );
	
	return 0;
}

/*
	�����������ã�ID����������X��Y�� ��ϵ��
*/
int SetWavePanelXY(PNWAVE_PANEL pWavePanel)
{
	//X
	float ftmpX;
	float fSpeeds[4] = {6.25, 12.5, 25.0, 50.0};
	float ftmpSpeed;
	//Y
	int iMaxY, iMinY;
	float ftmpY;
	
	if(pWavePanel == NULL) return -1;
	
	//���ݲ���Ƶ�ʺ��ٶȼ���X��ϵ��
	switch(pWavePanel->bWaveID){
		case NWID_ECG1:
		case NWID_ECG2:
		case NWID_MULTIECG:{
			ftmpSpeed = fSpeeds[gCfgEcg.bScanSpeed];
			iMaxY =  NWAVEMAX_ECG;
			iMinY = 0;
		}break;
		case NWID_SPO2:{
			ftmpSpeed = fSpeeds[gCfgSpO2.bScanSpeed];	
			iMaxY =  NWAVEMAX_SPO2;
			iMinY = 0;
		}break;
		case NWID_RESP:{
			ftmpSpeed = fSpeeds[gCfgResp.bScanSpeed];	
			iMaxY =  NWAVEMAX_RESP;
			iMinY = 0;
		}break;
		case NWID_CO2:{
			ftmpSpeed = fSpeeds[gCfgCO2.bScanSpeed];	
			switch(gCfgCO2.bRange){
				case CO2_RANGE_MIDDLE:{iMaxY = 75;}break;
				case CO2_RANGE_SMALL:{iMaxY = 100;}break;
				default:{iMaxY = 50;}break;
			}
			iMinY = 0;
		}break;			
		case NWID_IBP1:{
			ftmpSpeed = fSpeeds[gCfgIbp1.bScanSpeed];	
			iMaxY = gCfgIbp1.iScaleTop;
			iMinY = gCfgIbp1.iScaleBottom; 
		}break;
		case NWID_IBP2:{
			ftmpSpeed = fSpeeds[gCfgIbp2.bScanSpeed];	
			iMaxY = gCfgIbp2.iScaleTop;
			iMinY = gCfgIbp2.iScaleBottom; 
		}break;
		default:{
			ftmpSpeed = 25.0;
			iMaxY = RECTH(pWavePanel->rcWave);
			iMinY = 0; 
		}break;
	}
	
	//Xϵ����Hz/mm ---  SampleRate / (speed * mmToPixel)
	ftmpX =(float)( (float)NSAMPLERATE / (ftmpSpeed * gfNMMToPixel));
	
	//Yϵ����(Max-Min) / (RECTH / WaveCount)
	ftmpY = (float)((float)(iMaxY - iMinY) / (float)((float)(RECTH(pWavePanel->rcWave))/(float)pWavePanel->bWaveCount));
	/*
	if(B_PRINTF) printf("%s:%d Panel ID : %d, Speed: %.1f, gfNMMToPixel:%f,X_Axis: %.1f.\n", __FILE__, __LINE__, 
	       pWavePanel->bWaveID, ftmpSpeed,gfNMMToPixel, ftmpX);
	
	if(B_PRINTF) printf("YMax: %d, YMin: %d, RECTH: %d,bWaveCount: %1f, Y_Axis: %.1f.\n", 
	       iMaxY, iMinY, RECTH(pWavePanel->rcWave), pWavePanel->bWaveCount,ftmpY);
	*/	
	//����XYϵ��
	pWavePanel->fXAxis = ftmpX;
	pWavePanel->fYAxis = ftmpY;
	
	return 0;
}

/*
	�����������ò��λ������ ��ͼ���� 
*/
static int SetWavePanelsArea(int iPanelCount, RECT rcWaveArea)
{
	int iWaveCount;		//���ε�����	
	int i;
	
	//���Ķ�������
	int iPanelTop, iPanelLeft,iPanelRight;
	//�����
	int iPanelWidth, iPanelHeight;	

	//ͳ�Ʋ��ε�����(��������)
  	iWaveCount = 0;
  	for(i=0; i<iPanelCount; i++){
 		if(gNWavePanels[i].bWaveID != NWID_NONE){
 			iWaveCount += gNWavePanels[i].bWaveCount;
 		}
 	}
 	printf("%s:%d Total %d waves.\n", __FILE__, __LINE__, iWaveCount);
	
	//���ݲ��������ͻ�ͼ����ȷ�������Ļ�ͼ����
	//�õ����Ŀ��
	iPanelWidth = RECTW(rcWaveArea);
	//���ݲ�������������������
	iPanelHeight = (RECTH(rcWaveArea)) / iWaveCount - PANELAREA_INTER;	
	
	//���ø�����������
	iPanelTop = rcWaveArea.top;
	iPanelLeft = rcWaveArea.left;
	iPanelRight=rcWaveArea.right;
	for(i=0; i<iPanelCount; i++){
		//Para
		SetRect(&(gNWavePanels[i].rcPara), 
			  iPanelRight-PARAOFWAVE_WIDTH, 
			  iPanelTop, 
			  iPanelRight, 
			  iPanelTop + iPanelHeight*(gNWavePanels[i].bWaveCount));
		//Wave
		SetRect(&(gNWavePanels[i].rcWave), 
			  iPanelLeft,  
			  iPanelTop, 
			  iPanelLeft + iPanelWidth-5, 
			  iPanelTop + iPanelHeight*(gNWavePanels[i].bWaveCount));
		iPanelTop += (iPanelHeight*(gNWavePanels[i].bWaveCount)+PANELAREA_INTER);
	}
	
//  	FillWaveArea(gNRcWavesAll,  iPanelCount);
	
	return iPanelCount;
}

/*
	���ò������
	����ʵ�ʻ��ƵĲ����������
*/
static int SetWavePanels(const PNWAVE_CFG pCfg, RECT rcWaveArea)
{
	int  iPanelCount;		//��������
	int i;
	
	if(pCfg == NULL) return -1;
	
	//��ʼ�����
	for(i=0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWavePanels[i].bWaveID = NWID_NONE;
		gNWavePanels[i].bCascade = CASCADE_OFF;
		gNWavePanels[i].bWaveCount = 1;
	}
	
	//�������,������ϵͳ����	
	iPanelCount = 0;
	for(i=0; i<NWAVEPANEL_MAX_COUNT; i++){
		if(pCfg[i].bID != NWID_NONE && pCfg[i].bSwitch != SWITCH_OFF){
			gNWavePanels[iPanelCount].bWaveID = pCfg[i].bID;
			//�ж��Ƿ���Ҫ����(ECG)
			if(pCfg[i].bID == NWID_ECG1 ){
				gNWavePanels[iPanelCount].bCascade = gCfgEcg.bCascade;
				if(gNWavePanels[iPanelCount].bCascade == CASCADE_ON) gNWavePanels[iPanelCount].bWaveCount = 2;
			}
			else{
				//�������β���Ҫ����
				gNWavePanels[iPanelCount].bCascade = CASCADE_OFF;
				//7�ĵ�
				//���ڲ�����ֻ֧����ͨ������7����ͬ������ʱ��Ϊ6����ͬ��

				if(pCfg[i].bID == NWID_MULTIECG) gNWavePanels[iPanelCount].bWaveCount = 7;
				if(pCfg[i].bID == NWID_12LEAD) gNWavePanels[iPanelCount].bWaveCount = 12;
			}
			iPanelCount ++;
		}
	}
	
	if(B_PRINTF) printf("%s:%d Total %d waveform panel:\n", __FILE__, __LINE__, iPanelCount);
	
	//û�в��Σ��˳�
	if(iPanelCount<=0) return -1;

	//�����������
 	SetWavePanelsArea(iPanelCount, rcWaveArea);
	
	//�����������ã�ID����������X��Y�� ��ϵ��
	for(i=0; i<iPanelCount; i++){
		SetWavePanelXY(&(gNWavePanels[i]));
	}
	
	return iPanelCount;
}

/*
	��ʼ�����λ������
	����ʵ�ʻ��ƵĲ����������	
*/
static int InitWavePanels()
{
	int i;
	int iPanelCount = 0;
	NWAVE_CFG tmpWaveCfg[NWAVEPANEL_MAX_COUNT];
	
	SetRect(&gNRcWavesAll, 
		 RC_NORMAL_LEFT, 
		 RC_NORMAL_TOP, 
		 RC_NORMAL_RIGHT+3, 
		 RC_NORMAL_BOTTOM+2);
	
	//������ͼ����ȷ����ǰ�Ĳ�������,�ͻ�ͼ����
	switch(gCfgSystem.bInterface){
		case SCREEN_SHORTTREND:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_ShortTrend[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_ShortTrend[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_SHORTTREND_LEFT, 
				 RC_SHORTTREND_TOP, 
				 RC_SHORTTREND_RIGHT, 
				 RC_SHORTTREND_BOTTOM);
			
		}break;
		case SCREEN_BIGCHAR:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_BigChar[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_BigChar[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_BIGCHAR_LEFT, 
				 RC_BIGCHAR_TOP, 
				 RC_BIGCHAR_RIGHT, 
				 RC_BIGCHAR_BOTTOM);
		}break;
		case SCREEN_NIBPLIST:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_NibpList[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_NibpList[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_NIBPLIST_LEFT, 
				 RC_NIBPLIST_TOP, 
				 RC_NIBPLIST_RIGHT, 
				 RC_NIBPLIST_BOTTOM);
		}break;
		case SCREEN_MULTIECG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_MultiEcg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_MultiEcg[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_MECG_LEFT, 
				 RC_MECG_TOP, 
				 RC_MECG_RIGHT, 
				 RC_MECG_BOTTOM);
		}break;
		case SCREEN_7LEADECG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_7LeadEcg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_7LeadEcg[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_MECG_LEFT, 
				 RC_MECG_TOP, 
				 RC_MECG_RIGHT, 
				 RC_MECG_BOTTOM);
		}break;
		case SCREEN_OXYCRG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_OxyCrg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_OxyCrg[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_OXYCRG_LEFT, 
				 RC_OXYCRG_TOP, 
				 RC_OXYCRG_RIGHT, 
				 RC_OXYCRG_BOTTOM);
		}break;
		case SCREEN_12LEAD:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_12Lead[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_12Lead[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_12LEAD_LEFT, 
				 RC_12LEAD_TOP, 
				 RC_12LEAD_RIGHT, 
				 RC_12LEAD_BOTTOM);
		}break;
		case SCREEN_OTHERBED:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_OhterBed[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_OhterBed[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_OTHERBED_LEFT, 
				 RC_OTHERBED_TOP, 
				 RC_OTHERBED_RIGHT, 
				 RC_OTHERBED_BOTTOM);
		}break;
		case SCREEN_IBP:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_IBP[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_IBP[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_IBP_LEFT, 
				 RC_IBP_TOP, 
				 RC_IBP_RIGHT, 
				 RC_IBP_BOTTOM);
		}break;
		default:{//Normal
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_Normal[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
			}
			//ȷ����ͼ����
			SetRect(&gNRcWaves, 
				 RC_NORMAL_LEFT, 
				 RC_NORMAL_TOP, 
				 RC_NORMAL_RIGHT, 
				 RC_NORMAL_BOTTOM);
		}break;
	}
			
	//�������ó�ʼ�����λ������
 	iPanelCount = SetWavePanels((PNWAVE_CFG)&tmpWaveCfg, gNRcWaves);
	
	//������
	if(iPanelCount <=0) return -1;
	
	return iPanelCount;
}

/*
	��ʾ����ͼ��
*/
extern HWND ghWndPanel_Heart;
int HRIconDisplay(BOOL bDisplay)
{

	HDC hdc = (HDC)NULL;
	RECT rc; 
	
	hdc = GetClientDC(ghWndPanel_Heart);
	GetClientRect(ghWndPanel_Heart, &gNRCEcg_Icon);
	if(hdc != (HDC)NULL){
		if(bDisplay){ 
			FillBoxWithBitmap(hdc, gNRCEcg_Icon.left,gNRCEcg_Icon.top, 24, 24, &gIconHR[0]);     
		}
		else{ //����ͼ��                               
			SetBrushColor(hdc, COLOR_black);
			FillBox(hdc, gNRCEcg_Icon.left, gNRCEcg_Icon.top, 24, 24); 
		} 
		
		ReleaseDC(hdc);
		hdc = (HDC)NULL;
	}
	
	
	return 0;
}
 int DisplayHeartIcon()
{
	static int iHrState =0;	//״̬ 0����ʼ״̬ 1��ͼ����ʾ״̬ 2��ͼ��ر�״̬
	static int iHrCount =0;
	
	switch(iHrState){
		case 1:{
			//iHrCount=(iHrCount+1)%10;
			iHrCount=(iHrCount+1)%2;
			if(iHrCount==0)
				iHrState = 2;
		
		}break;	
		case 2:{
			iHrCount=(iHrCount+1)%2;
			if(iHrCount==0){
				HRIconDisplay(FALSE);
				iHrState = 0;
			}
		}break;
		default:{
			//TODO:�˴�Ҫ������ʷ����������ʷ���
			switch(gCfgEcg.bAlmSource){
				case HR_ALM_SRC_HR:{
					//����ECG����
					if(gbHaveBeep_QRS){
						Sound_Beep();
						HRIconDisplay(TRUE);
						gbHaveBeep_QRS = FALSE;
						iHrCount = 0;
						iHrState = 1;
					}	
				}break;
				case HR_ALM_SRC_PR:{
					
					//����SpO2����	
					if(gbHaveBeep_Pulse){
						Sound_Beep();
						HRIconDisplay(TRUE);
						gbHaveBeep_Pulse = FALSE;
						iHrCount = 0;
						iHrState = 1;
					}	
					
				}break;
				default:{
					//Auto
					//�����ж�ECG�Ƿ�����
					if(gValueEcg.bRAOff){
						//����SpO2����	
						if(gbHaveBeep_Pulse){
							Sound_Beep();
							HRIconDisplay(TRUE);
							gbHaveBeep_Pulse = FALSE;
							iHrCount = 0;
							iHrState = 1;
						}	
					}
					else{
						//����ECG����
						if(gbHaveBeep_QRS){
							Sound_Beep();
							HRIconDisplay(TRUE);
							gbHaveBeep_QRS = FALSE;
							iHrCount = 0;
							iHrState = 1;
						}
					}
				}break;
			}
			
		}break;
	}
	return 0;
}


//�Ƿ����ڻ��Ʋ���
extern BOOL gbParasViewing;

extern BOOL gbApneaTimeChanged;
/////////////////////////��ʱ�� ///////////////////////
/*
	��ʱ���߳�
*/
extern BOOL gbTestPowerDown;	//���Թػ�
static void *NewThreadProcDrawTimer(void *arg)
{
	struct timeval	timeout;
	int delay;
	
	//ʱ�������
	int iTimer_1s = 0;
	int iTimer_500ms = 0;
	int iTimer_1m = 0;
	
	int iHrState =0;	//״̬ 0����ʼ״̬ 1��ͼ����ʾ״̬ 2��ͼ��ر�״̬
	int iHrCount =0;
		
	for(;;)
	{
		//-- ����Select��ʱ --
		timeout.tv_sec = 0;
		timeout.tv_usec= TIMER_DRAW;

		delay = select(FD_SETSIZE, NULL, NULL, NULL, &timeout);

		//ι������Ϊ��ͼ�߳����ȼ��ϸ�
		WDT_Feed();
		
		//��Դ���ڹر�ʱ������ʾ
		if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
			break;
		}

		//���ܼ���ѯ����
		DealFunKey();
		//���»�ͼ
		UpdateDrawWave();
		//����������ͼ
		if(gCfgSystem.bInterface==SCREEN_OTHERBED){
			NewUpdateOtherBedWave();
		}
		DisplayHeartIcon();
		//���²�����ʾ
		
		
		

		
		//������1
		if(iTimer_1s<((TIMER_1S/TIMER_DRAW)-1)){    
			
			iTimer_1s ++;   
		}
		else{
			iTimer_1s =0;
			
			//�������ڲ�������ʱ
			UpdateNibpAutoTimer(FALSE);
			//����������������ʱ
			UpdateNibpStatTimer();
			//�ж��Ƿ�����Ϣ����
			if(gbApneaTimeChanged)
				IsAlm_Apnea(&gValueResp, &gCfgResp, TRUE);
			else
				IsAlm_Apnea(&gValueResp, &gCfgResp, FALSE);

			
			//���±�������
			UpdateAlarmSound();
			//������ڱ�����ͣ״̬��,�򵹼�ʱ
			if(gbSuspending){
				if(giSuspendTime >0){
					giSuspendTime --;
				}
				else{
					giSuspendTime = gCfgAlarm.bPauseTime*10;
					gbSuspending = FALSE;
				}
			} 
			
			//--- ��õ�Դ״̬ ---old
		//	GetPowerStatus();
			//send_net_data_test();	
			
		}
		
		//0.5s�Ķ�ʱ��
		if(iTimer_500ms <((TIMER_500MS/TIMER_DRAW)-1)){    
			iTimer_500ms ++;   
		}
		else{
			iTimer_500ms =0;
			
			//���±����ƹ�
			UpdateAlarmLight();
 	//		if(gbViewDemoData && gbNCanDrawWave)
 	//		DrawDemo(NULL, NULL);
		}
		
	}
}

/*
	������ͼ��ʱ��
	return: 0-OK, -1:Failure
*/
static int NewCreateDrawWaveTimer()
{
	int res;
	
	//��ʼ����ͼ��ʱ���ź���
	res = sem_init(&semNDrawWave, 0, 0);
	
	//������ͼ��ʱ���߳�
	res = pthread_create(&ptNewDrawTimer, NULL, NewThreadProcDrawTimer, NULL);
		
	gbHasCreatedTimer = TRUE;
	return res;
}

//��ղ��λ�����
static int ClearWaveBuffer()
{
	ClearData_Ecg1();
	ClearData_Ecg2();
	ClearData_SpO2();
	ClearData_Resp();
	ClearData_CO2();
	ClearData_Ibp1();
	ClearData_Ibp2();
	printf("%s:%d Clear wave buffer.\n", __FILE__, __LINE__);
	return 0;
}

//////////////////////////////////// interface ///////////////////
/*
	��ʼ�����λ���
*/
int NewInitWaveDraw()
{
	int res;
	int i;
	
	//��ֹ��ͼ
	gbNCanDrawWave = FALSE;
	
//	usleep(200000);
	
#if SCREEN_1024
	//15" 1024
	gfNMMToPixel  = 3.33;	
#elif SCREEN_640
	//12.1" 800x600
	gfNMMToPixel  = 3.2;	
#else
	//12.1" 800x600
	gfNMMToPixel  = 3.2;		
#endif
	
	FillWaveArea(gNRcWavesAll);
	
	//��ʼ����������
	res = InitWaveConfig();
		
	//������ͼ�Ͳ������ó�ʼ�����λ������
	giPanelCount  = InitWavePanels();
	printf("%s:%d Waveform Config:\n", __FILE__, __LINE__, res);
	for(i=0; i<NWAVEPANEL_MAX_COUNT;  i++){
		printf("Index: %d, WaevID: %d\n", i, gNWavePanels[i].bWaveID);
	}
			
 	gbResetWaves = TRUE;
		
	if(!gbHasCreatedTimer)
		NewCreateDrawWaveTimer();
	
	//��ղ��λ�����
	ClearWaveBuffer();
			
	if(gCfgSystem.bInterface == SCREEN_SHORTTREND)
		NewInitShortTrend();
	else if(gCfgSystem.bInterface == SCREEN_OXYCRG)
		NewInitCrg();
	else if(gCfgSystem.bInterface == SCREEN_OTHERBED)
		NewInitOtherBed();
	
	FillWaveArea(gNRcWavesAll);
	usleep(200000);
	//�����ͼ
	gbNCanDrawWave = TRUE;
	
	if(gCfgSystem.bInterface == SCREEN_OTHERBED)
		OtherBedShowBtn();
	if(gCfgSystem.bInterface == SCREEN_IBP)
		UpdateIbp();
	
	return 0;
}

/*
	���²��λ���
*/
int UpdateDrawWave()
{
	if(gbNCanDrawWave){
		sem_post(&semNDrawWave);
	} 
	else{
		return -1;
	} 
	
	return 0;
}

/*
	�ı䲨���������߽磨���жԻ��򵯳�ʱ�������ƶ����Σ�
	prcDlg : NULL��û�жԻ���other���Ի�����ռ������ 
*/
int ChangeWavePanelsLeft(PRECT prcDlg)
{
 	return 0;
	
	static int iLeft = 0;
	
	//��ֹ��ͼ
	gbNCanDrawWave = FALSE;
	
// 	FillWaveArea(gNRcWavesAll, giPanelCount);
	FillWaveArea(gNRcWavesAll);
		
	if(prcDlg !=NULL){
		gNRcWaves.left = prcDlg->right +10;
	}
	else{
		if(gCfgSystem.bInterface != SCREEN_SHORTTREND)
			gNRcWaves.left =  RC_NORMAL_LEFT;
		else
			gNRcWaves.left =  RC_SHORTTREND_LEFT;
	}
		
 	SetWavePanelsArea(giPanelCount,gNRcWaves);
	gbResetWaves = TRUE;
		
	//�����ͼ
	gbNCanDrawWave = TRUE;
	
	//����oxyCRG
	if(gCfgSystem.bInterface == SCREEN_OXYCRG){
		NewUpdateCrg();
	}	
	
	return 0;
}

//////////////////һЩ��������/////////////
/*
	�ı�ECGɨ���ٶ�
*/
int ChangeWaveSpeed_Ecg()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_ECG1 
				 ||gNWavePanels[i].bWaveID == NWID_ECG2 
				 ||gNWavePanels[i].bWaveID == NWID_MULTIECG)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	�ı�SpO2ɨ���ٶ�
*/
int ChangeWaveSpeed_SpO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_SPO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
	
}

/*
	�ı�Respɨ���ٶ�
*/
int ChangeWaveSpeed_Resp()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_RESP)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	�ı�CO2ɨ���ٶ�
*/
int ChangeWaveSpeed_CO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_CO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	�ı�CO2ɨ���ٶ�
*/
int ChangeWaveGain_CO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_CO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveGain_Ibp1()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP1)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveGain_Ibp2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveSpeed_Ibp1()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP1)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveSpeed_Ibp2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}
int ChangeFilter_Ibp1()
{
	iBioCMD_IBP_FILTER(gCfgIbp1.bFilterMode);
	gbResetWaves = TRUE;
}

int ChangeFilter_Ibp2()
{
	iBioCMD_IBP_FILTER(gCfgIbp2.bFilterMode);
	gbResetWaves = TRUE;
}


/*
	�л�������ʾ
*/
int SwitchView()
{
	static BYTE bOldInterface;
	ReturnMainView();
	//����
	SetBrushColor(HDC_SCREEN, COLOR_black);
	FillBox(HDC_SCREEN, 
		gNRcWaves.left, 
		gNRcWaves.top, 
		RECTW(gNRcWaves), 
		RECTH(gNRcWaves));
	bOldInterface=gCfgSystem.bInterface;
	
	gCfgSystem.bInterface=(gCfgSystem.bInterface+1)%SCREEN_12LEAD;
	
	
	SetIntValueToResFile(gFileSetup, "SystemSetup", "interface", gCfgSystem.bInterface, 1);	
	printf("gCfgSystem.bInterface=%d\n",gCfgSystem.bInterface);
							
	//�µ���ͼ
	NewDestroyShortTrend();
	NewDestroyCrg();
	NewDestroyOtherBed();
	SwitchParasView(bOldInterface);
	NewInitWaveDraw();
	
	ChangeECGLeadChannel();
	bOldInterface=gCfgSystem.bInterface;
	
	return 0;
}

//���Შ��
int FreezeWaves()
{
	gbNCanDrawWave = FALSE;
	
	//������Ļ�ط�
	StartScreenReview();
	
	return 0;
}

//�ⶳ����
int UnfreezeWaves()
{
	//�ص�������
	ReturnMainView();
	
	gbNCanDrawWave = TRUE;
	
	return 0;
}
/*
	�ı�Y���ϵ�ϵ��,������
*/
int ChangeWaveY()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL){
			SetWaveY(&gWavePanels[i]);
		}
	}	
	
	return 0;
}


/*
	�ı�Ibp1�Ĳ��η�Χ
*/
int ChangeWaveRange_Ibp1()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL 
				 && gWavePanels[i].bID == WID_IBP1){
			SetWaveY(&gWavePanels[i]);
		 }
	}	
	
	return 0;
}

/*
	�ı�Ibp2�Ĳ��η�Χ
*/
int ChangeWaveRange_Ibp2()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL 
				 && gWavePanels[i].bID == WID_IBP2){
			SetWaveY(&gWavePanels[i]);
				 }
	}	
	
	return 0;
}

