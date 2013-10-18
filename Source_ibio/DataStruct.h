/*************************************************************************** 
 *	Module Name:	DataStruct.h	
 *
 *	Abstract:	������������ݽṹ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-22 10:27:24
 ***************************************************************************/
#ifndef __DATASTRUCT_H__
#define __DATASTRUCT_H__

//������Ļ����
#define SCREEN_1024			0

#define SCREEN_640			0

//�����۲칦��
#define OTHERBED_FUNC		1


//#define HAVE_PRINTER	1

//�˴��޸ĳ��ұ�־
#if SCREEN_1024
	#define VERSION_SOFT		"L-2.00"
#elif SCREEN_640
	#define VERSION_SOFT		"S-2.00"
#else 
	#define VERSION_SOFT		"M-2.00"
#endif	


//�������
#define	DEBUGINFO()		printf("%s-%s:%d\n", __FILE__, __func__, __LINE__)

#define  B_PRINTF 	1

//��ʾ��Ļ
#if SCREEN_1024
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		1024
#define SCREEN_BOTTOM		768
#elif  SCREEN_640
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		640
#define SCREEN_BOTTOM		480
#else
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		800
#define SCREEN_BOTTOM		600
#endif

//������ʾ��������ұ߽磨�����ַ���ͼ�⣩
#if SCREEN_1024	
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 290)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM)
#elif  SCREEN_640
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 200)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM )
#else
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 245)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM )
#endif
//��������(����)
#define DEV_SERIAL0		0
#define DEV_SERIAL1		1
#define DEV_SERIAL2		2
#define DEV_SERIAL3		3
#define DEV_SERIAL4		4
#define DEV_SERIAL5		5
#define DEV_SERIAL6		6
#define DEV_SERIAL7		7


//����ģ������
#define MODULE_MAX_COUNT	10
#define MOD_NONE			0
#define MOD_ECG				1
#define MOD_NIBP			2
#define MOD_SPO2			3
#define MOD_PULSE			4
#define MOD_RESP			5
#define MOD_TEMP			6
#define MOD_IBP1				7
#define MOD_IBP2				8
#define MOD_CO2				9

//ģ�鲻ʹ��	
#define MOD_DISABLE	0	
//ģ����ʹ��
#define MOD_ENABLE	1	
typedef struct {
	BYTE bID;			//ģ��ID: MOD_X
	BYTE bSerial;		//ģ��ʹ�õĴ���: DEV_SERIALX	
	BYTE bStatus;		//ģ��״̬: MOD_DISABLE Or MOD_ENABLE
}S_MODULE, *PS_MODULE;

typedef struct {
	BYTE bCount;		//����(��Ч��)ģ�������
	S_MODULE s_Modules[MODULE_MAX_COUNT];	//����ģ����Ϣ
}S_MODULES, *PS_MODULES;

//ϵͳʱ��ṹ                             
typedef struct {
	WORD wYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bHour;
	BYTE bMin;
	BYTE bSec;
}S_TIME, *PS_TIME;

//����
#define BASELINE_ECG		2047
#define BASELINE_SPO2		50
#define BASELINE_RESP		1027

//-------------- ��ģ������ݽṹ ------------------
//����
#define	VOLUME_OFF		0
#define	VOLUME_1		1
#define	VOLUME_2		2
#define	VOLUME_3		3
#define	VOLUME_4		4
#define	VOLUME_5		5
#define	VOLUME_6		6
#define	VOLUME_7		7
#define	VOLUME_8		8
#define	VOLUME_9		9
#define	VOLUME_10		10

//ɨ���ٶ�
#define SCANSPEED_625		0
#define SCANSPEED_125		1
#define SCANSPEED_25		2
#define SCANSPEED_50		3
//����Ƶ��
#define AC_50HZ			0
#define AC_60HZ			1
//��������
#define ALARM_OFF		0
#define ALARM_LOW		1
#define ALARM_MID		2
#define ALARM_HIGH		3
#define ALARM_INFO		4	//һ����ʾ��Ϣ

//��������
#define ALARM_TECHNOLOGY   	0
#define ALARM_PHYSIOLOGICAL	1
//������ӡ
#define ALARM_PRINT_OFF		0
#define ALARM_PRINT_ON			1
//�����ع˴洢
#define ALARM_REVIEW_OFF		0
#define ALARM_REVIEW_ON		1
//����״̬
#define SWITCH_OFF		0
#define SWITCH_ON		1
//ģ��״̬
#define MODULE_OFF		1
#define MODULE_ON		0
//ģ��ͨѶ״̬
#define COMMUNICATION_ERR	0
#define COMMUNICATION_OK	1
//������Ƿ�ʽ
#define DRAWMODE_LINE		0
#define DRAWMODE_FILL		1

//---- ECG����ض��� ----
//���ʲ�����Χ(bpm)
#define HR_MAX	350
#define HR_MIN	0

#define  ST_MAX		(100)
#define  ST_MIN		(-100)

//��������
#define ECGWAVE_COUNT		7	//7������
#define ECGWAVE_I		0
#define ECGWAVE_II		1
#define ECGWAVE_III		2
#define ECGWAVE_AVR		3
#define ECGWAVE_AVL		4
#define ECGWAVE_AVF		5
#define ECGWAVE_V		6
//12��������
#define ECG12WAVE_COUNT		12	//7������
#define ECG12WAVE_I		0
#define ECG12WAVE_II		1
#define ECG12WAVE_III		2
#define ECG12WAVE_AVR		3
#define ECG12WAVE_AVL		4
#define ECG12WAVE_AVF		5
#define ECG12WAVE_V1		6
#define ECG12WAVE_V2		7
#define ECG12WAVE_V3		8
#define ECG12WAVE_V4		9
#define ECG12WAVE_V5		10
#define ECG12WAVE_V6		11


//���α�ʾ
#define WAVEFORM_OFF		0
#define WAVEFORM_ECG1		1
#define WAVEFORM_CASCADE	2
#define WAVEFORM_I  			3
#define WAVEFORM_II			4
#define WAVEFORM_III			5
#define WAVEFORM_AVL  		6
#define WAVEFORM_AVR		7
#define WAVEFORM_AVF 		8
#define WAVEFORM_V  			9
#define WAVEFORM_SPO2		10
#define WAVEFORM_RESP 		11
#define WAVEFORM_IBP1  		12
#define WAVEFORM_IBP2 		13
#define WAVEFORM_CO2  		14
#define WAVEFORM_MAX		15


//����ģʽ
#define ECGMODE_DIAG		0
#define ECGMODE_MON		1
#define ECGMODE_OPE		2
//��������
#define ECGTYPE_5LEAD	0
#define ECGTYPE_3LEAD	1
//��������
#define ECGGAIN_025X	0
#define ECGGAIN_05X	1
#define ECGGAIN_1X	2
#define ECGGAIN_2X	3
//������Դ(HR Or PR)
#define HR_ALM_SRC_AUTO	0
#define HR_ALM_SRC_HR	1
#define HR_ALM_SRC_PR	2
//�����˲�����
#define ECGFILTER_AC_OFF	0
#define ECGFILTER_AC_ON		1
//����Ư���˲�
#define ECGFILTER_DRIFT_OFF	0
#define ECGFILTER_DRIFT_ON	1

//�����˲�����
#define ECGFILTER_HUM_OFF	0
#define ECGFILTER_HUM_ON	1

//��
#define PACEMAKER_OFF	0
#define PACEMAKER_MODE1	1
#define PACEMAKER_MODE2		1
//���μ���
#define CASCADE_OFF		0
#define CASCADE_ON		1
///1mvУ׼�ź�����
#define CAL1MV_OFF		0
#define CAL1MV_ON		1
//ECG������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bSwitch;		//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;			//����������ɫ
	BYTE bLeadType;		//��������: ECGTYPE_5LEAD or ECGTYPE_3LEAD
	BYTE bScanSpeed;	//ɨ���ٶ�: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	BYTE bPacemaker;	//�Ƿ���: PACEMAKER_OFF or PACEMAKER_ON, �벡����Ϣ������Ҫһ��	
	BYTE bChannel1;		//��ʾͨ��1: ECGWAVE_X�����ܹرգ�
	BYTE bChannel2;		//��ʾͨ��2: ECGWAVE_X��������ʱ��Ч,�ر���ʾ��
	BOOL bCascade;		//���μ���: CASCADE_OFF, CASCADE_ON
	BYTE bMode;			//����ģʽ: ECGMODE_DIAG����ϣ�ECGMODE_MON���໤��ECGMODE_OPE������
	BYTE bFilterAC;		//�����˲�: ECGFILTER_AC_OFF or ECGFILTER_AC_ON
	BYTE bFilterDrift;		//����Ư���˲�: ECGFILTER_DRIFT_OFF, ECGFILTER_DRIFT_ON
	BYTE bFilterHum;		//�����˲�: OFF or ON
	BYTE bGain;			//��������: ECGGAIN_XX
	WORD wHigh_HR;		//��������
	WORD wLow_HR;		//��������
	BYTE bBeepVolume;	//��������: VOLUME_XX
	BYTE bAlmSource;	//���ʱ�����Դ: HR_ALM_SRC_AUTO, HR_ALM_SRC_HR, HR_ALM_SRC_PR
	BYTE bAlmControl;	//��������: ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH
	BYTE bPrnControl;	//����������ӡ: ALARM_PRINT_OFF, ALARM_PRINT_ON
	BYTE b1mvCal;	///1mvУ׼�ź�����CAL1MV_OFF CAL1MV_ON
	int iAdjust;		//У׼ϵ��(�ٷֱ�)
	BYTE bISO;			//ISO
	BYTE bST;			//ST
	BYTE bArrAnalyse;	//����ʧ����������
	BYTE bPVCs;			//pvcs��ʾ����
	int iMax_HR;			//�������еı�����ֵ
	int iMin_HR;			//�������еı����Сֵ
	int iSt1_High;		//x100
	int iSt1_Low;		//x100
	int iSt2_High;		//x100
	int iSt2_Low;		//x100
}S_ECG_CFG, *PS_ECG_CFG;
//ECG������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BOOL bConnected;		//�Ƿ�ӹ����壬FALSE��δ�ӹ���TRUE���Ѿ��ӹ�
	BYTE bCommStatus;		//ͨѶ״̬��COMMUNICATION_OK, COMMUNICATION_ERR
	BOOL bAdjust;			//ECGУ׼��FALSE������������TRUE������У׼ 
	BYTE bLeadType;			//�������ͣ�ECGTYPE_5LEAD or ECGTYPE_3LEAD
	int wHR;			//����
	BOOL bBeep;			//Beep��FALSE���ޣ�TRUE����
	BYTE bPacingpulse;		//�����壬0���ޣ�1����
	int iWaves[ECGWAVE_COUNT];	//����
	BOOL bRAOff;			//����RA����: TRUE-Off, FALSE-ON
	BOOL bRLOff;			//����RL����: TRUE-Off, FALSE-ON
	BOOL bLAOff;			//����LA����
	BOOL bLLOff;			//����LL����
	BOOL bVOff;			//����V����
	BOOL bHRInval;			//HR��Чֵ��־λ-100
	BOOL bOverLoad1;			//ECGͨ��1������ϢTRUE-���أ�FALSE-����
	BOOL bOverLoad2;			//ECGͨ��2������Ϣ
	BOOL bOverLoad3;			//ECGͨ��3������Ϣ
	BYTE bPVC;					//PVC
	unsigned long lARR;					//ARR	
	int iST1;			//ST1��10x
	int iST2;			//ST2��10x
	int iST[ECGWAVE_COUNT];		//ST I~V
}S_ECG_VALUE, *PS_ECG_VALUE;

//---- RESP����ض��� ----
//�����ʲ�����Χ(rpm)
#define RR_MAX			180
#define RR_MIN			0
//������������
#define RESPGAIN_025X		0
#define RESPGAIN_05X		1
#define RESPGAIN_1X		2
#define RESPGAIN_2X		3
#define RESPGAIN_4X		4
#define RESPGAIN_8X		5
#define RESPGAIN_16X		6
#define RESPGAIN_32X		7
//��Ϣʱ��(Sec)
#define APNEATIME_OFF		0
#define APNEATIME_MAX		45
//RR��Դ
#define RR_SRC_ECG		0
#define RR_SRC_CO2		1
//����I/II��������
#define RR_LEADTYPE_OFF	0
#define RR_LEADTYPE1		1
#define RR_LEADTYPE2		2
//RESP������Ϣ
typedef struct {
	BYTE bID;		//ģ��ID
	BYTE bSwitch;		//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;			//��ɫ
	BYTE bSource;		//�����ʱ�����Դ:RR_SRC_ECG, RR_SRC_CO2
	BYTE bGain;			//��������: RESPGAIN_05X, RESPGAIN_1X, RESPGAIN_2X, RESPGAIN_4X
	BYTE bScanSpeed;	//ɨ���ٶ�: SCANSPEED_625, SCANSPEED_125, SCANSPEED_25
	WORD wHigh_RR;		//����������
	WORD wLow_RR;		//����������
	WORD wApneaTime;	//��Ϣ����ʱ��0-Off	, x*10s(10s, 20s, .... 120s), ��λ:10s//����
	BYTE bApnea;		//��Ϣ����ʱ��0-Off	, 1~7  10s 15s 20s 25s 30s 35s 40s
	BYTE bAlmControl;	//��������: ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH
	BYTE bPrnControl;	//����������ӡ: ALARM_PRINT_OFF, ALARM_PRINT_ON
	BYTE bLeadType;		 //����I/II��������RR_LEADTYPE1 RR_LEADTYPE2
	BYTE bMax_RR;		//�������еı�����ֵ
	BYTE bMin_RR;		//�������еı����Сֵ
}S_RESP_CFG, *PS_RESP_CFG;
//RESP������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BOOL bConnected;	//�Ƿ�ӹ�����:FALSE��δ�ӹ���TRUE���Ѿ��ӹ�
	BYTE bCommStatus;	//ͨѶ״̬��COMMUNICATION_OK, COMMUNICATION_ERR
	BOOL bApneaAlm;		//��Ϣ����
	BOOL bCVAAlm;		//�Ķ����ţ��ĵ��źŵ����ں������ϵĸ���
	int wRR;				//������
	int iWave;			//��������
	BOOL bInval;			// ��Чֵ��־λ
	BOOL bartifact;		//����α��
	BOOL bBaselineErr;	//RESP���߳�����ֵ
	BOOL bOff;			//��������: TRUE-Off, FALSE-ON
}S_RESP_VALUE, *PS_RESP_VALUE;


//---- SpO2����ض��� ----
//Ѫ��������Χ(%)
#define SPO2_MAX		100
#define SPO2_MIN		0
//��Ӧ������
#define SPO2_FAST		0
#define SPO2_NORMAL		1
#define SPO2_SLOW		2
//��������
#define SPO2GAIN_AUTO	0 
#define SPO2GAIN_05X	1
#define SPO2GAIN_1X		2
#define SPO2GAIN_2X		3
//�˲�
#define SPO2FILTER_OFF	0
#define SPO2FILTER_ON	1
//SpO2������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bSwitch;		//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;			//��ɫ
	BYTE bResponse;		//��Ӧ�ٶ�: SPO2_FAST, SPO2_NORMAL, SPO2_SLOW
	BYTE bGain;			//��������: SPO2GAIN_AUTO, SPO2GAIN_05X, SPO2GAIN_1X, SPO2GAIN_2X
	BYTE bScanSpeed;	//ɨ���ٶ�: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	BYTE bFilter;		//�����˲�: SPO2FILTER_OFF, SPO2FILTER_ON
	BYTE bDrawMode;		//������Ʒ�ʽ: DRAWMODE_LINE or DRAWMODE_FILL
	BYTE bHigh_SpO2;	//SpO2����
	BYTE bLow_SpO2;		//SpO2����
	BYTE bLowest_SpO2;	//�ͱ��Ͷȼ���, ���ڴ˼��޺�߼�����
	BYTE bAlmControl;	//��������
	BYTE bPrnControl;	//����������ӡ
	int iAdjust;		//У׼ϵ��(+-)
	BYTE bMax_SPO2;	//�������еı�����ֵ
	BYTE bMin_SPO2;		//�������еı����Сֵ
}S_SPO2_CFG, *PS_SPO2_CFG;
//SpO2������Ϣ
//Ҫ����ģ�鱾��Ĵ���Ͳ��������г��ֵĴ���
#define SPO2_INTERFERE		BIT0
#define SPO2_SEARCHPULSE	BIT1
#define SPO2_HIGHLIGHT		BIT2
#define SPO2_WEAKSIGNAL		BIT3
#define SPO2_SIGNALBAD		BIT4
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bConnected;		//�Ƿ�ӹ�����
	BYTE bSpO2;			//SpO2 value -100����Чֵ
	BOOL bInvalSpo2;		//SpO2 value  ��Чֵ��־
	BYTE bWave;			//SpO2 wave
	BOOL bBeep;			//Beep��FALSE������������TRUE����������
	BYTE bStrength;			//�ź�ǿ��
	BOOL bComError;			//ͨѶ����	
	BOOL bComStop;			//ͨѶֹͣ
	BOOL bModuleInitError;		//ģ���ʼ������
	BOOL bModuleError;		//ģ�����
	BOOL bLimitError;		//�����޴���
	BOOL bOverrange;		//��������
	BOOL bNoSensor;			//δ�Ӵ�����
	BOOL bSensorError;		//����������
	BOOL bUnknownSensor;		//δ֪������
	BOOL bIncompatible;		//�����ݵĴ�����
	BOOL bSearchPulse;		//����������־��1-��ʾ������������
	BOOL bSearchTimeout;			//����ʱ��̫����־��1��ʾ����������ʱ��>15��
	BOOL bSpo2drop;			//�����Ͷ��½���־��1 ��ʾ�����Ͷ��½�
	BOOL bOff;			//̽ͷ����
	BYTE bErrCode;			//������룬0-OK, Bit0:Interfere, Bit1:Search Pulse, Bit2:HighLight, Bit3:Weak Signal, Bit4: Signal Bad
}S_SPO2_VALUE, *PS_SPO2_VALUE;


//---- Pulse����ض��� ----
//���ʲ�����Χ(bpm)
#define PR_MAX	300
#define PR_MIN	0
//Pulse������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bSwitch;			//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;			//��ɫ
	BYTE bBeepVolume;		//��������: VOLUME_XX
	WORD wHigh_PR;			//��������
	WORD wLow_PR;			//��������
	BYTE bAlmSource;		//���ʱ�����Դ: HR_ALM_SRC_AUTO, HR_ALM_SRC_HR, HR_ALM_SRC_PR
	BYTE bAlmControl;		//��������
	BYTE bPrnControl;		//����������ӡ
}S_PULSE_CFG, *PS_PULSE_CFG;
//Pulse������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bConnected;		//�Ƿ�ӹ�����
	BYTE bCommStatus;		//ͨѶ״̬
	BOOL bOff;			//����״̬: TRUE-Off, FALSE-On
	BOOL bInvalPR;		//PR��Чֵ��־
	BYTE bStrength;		//SPO2 ��ͼ(��ʾ�����������),0��15
	WORD wPR;			//����	
}S_PULSE_VALUE, *PS_PULSE_VALUE;

//---- TEMP����ض��� ----
//���²�����Χ(C*10)
#define TEMP_MAX	500
#define TEMP_MIN	0
//��ʾ��λ
#define TEMP_UNIT_C	0
#define TEMP_UNIT_F		1
//temp sensor type
#define TEMP_YSI		0
#define TEMP_CYF		1
//TEMP������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bSwitch;			//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;			//��ɫ
	BYTE bUnit;			//��ʾ��λ: TEMP_UNIT_C, TEMP_UNIT_F
	WORD wHigh_T1;			//����ֵ���ޣ�10x
	WORD wLow_T1;			//����ֵ���ޣ�10x
	WORD wHigh_T2;			//����ֵ���ޣ�10x
	WORD wLow_T2;			//����ֵ���ޣ�10x
	WORD wHigh_TD;			//���²����
	BYTE bAlmControl;		//��������
	BYTE bPrnControl;		//����������ӡ
	BYTE bTempType;		//����̽ͷ��������0-YSI 1-CY-F
	int iAdjust_T1;			//У׼ϵ��(+-)
	int iAdjust_T2;			//У׼ϵ��(+-)
	int iMax_TEMP;		//�������еı�����ֵ
	int iMin_TEMP;		//�������еı����Сֵ	
}S_TEMP_CFG, *PS_TEMP_CFG;

//TEMP������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bConnected_T1;		//�Ƿ�ӹ�����, FALSE��δ�ӹ���TRUE���Ѿ��ӹ�
	BYTE bCommStatus_T1;		//ͨѶ״̬
	BOOL bOverRange_T1;		//�������磬FALSE��normal��TRUE������
	BOOL bOff_T1;			//������״̬��FALSE��normal��TRUE������������
	BYTE bConnected_T2;		//�Ƿ�ӹ����壬FALSE��δ�ӹ���TRUE���Ѿ��ӹ�
	BYTE bCommStatus_T2;		//ͨѶ״̬
	BOOL bOverRange_T2;		//�������磬FALSE��normal��TRUE������
	BOOL bOff_T2;			//������״̬��FALSE��normal��TRUE������������

	WORD wT1;			//����ֵ��10x
	WORD wT2;			//����ֵ��10x
	WORD wTD;			//����ֵ��10x(�²����ֵ)
}S_TEMP_VALUE, *PS_TEMP_VALUE;


//---- NIBP����ض��� ----
//NIBP������Χ(mmHg)
#define NIBP_MAX	300
#define NIBP_MIN	0
//NIBP��������
#define NIBP_OBJECT_ADULT	0
#define NIBP_OBJECT_CHILD	1
#define NIBP_OBJECT_BABY	2
//NIBP��ʾ��λ
#define NIBP_UNIT_MMHG		0
#define NIBP_UNIT_KPA		1
//NIBP������Դ
#define NIBP_ALM_SRC_ALL	0
#define NIBP_ALM_SRC_S		1
#define NIBP_ALM_SRC_M		2
#define NIBP_ALM_SRC_D		3
#define NIBP_ALM_SRC_SM		4
#define NIBP_ALM_SRC_MD		5
#define NIBP_ALM_SRC_SD		6

/****************CUFF**************************/
#define NIBP_MANUAL				0
#define NIBP_AUTO				1
#define NIBP_STAT				2
#define NIBP_STATIC_VEN			3
#define NIBP_LEAK_TEST			4
#define NIBP_P_V_TEST			5
#define NIBP_OVER_PRE_TESE		6
#define NIBP_PRE_CAL			7

#define NIBP_UPDATE_PATIENT_INFO		1
#define NIBP_AUTO_DETECT_CUFF_TYPE	0
#define NIBP_ADULT_CUFF_TYPE			1
#define NIBP_PEDIATRIC_CUFF_TYPE		2
#define NIBP_NEONATE_CUFF_TYPE		3
/*****************************************/
#define  NIBP_SYSTEM_RUNNING		1
#define  NIBP_SYSTEM_IDLE			0
/******************************************/

/*******************FDB***********************/
#define NBP_FDB_MAN_START_OK		0x01	//
#define NBP_FDB_MAN_START_ABT	0x02	//
#define NBP_FDB_AUTO_START_OK	0x03	//
#define NBP_FDB_AUTO_START_ABT	0x04	//
#define NBP_FDB_STAT_START_OK		0x05	//
#define NBP_FDB_STAT_START_ABT	0x06	//
#define NBP_FDB_STC_START_OK		0x07	//
#define NBP_FDB_STC_START_ABT		0x08	//
#define NBP_FDB_LEAK_START_OK		0x09	//
#define NBP_FDB_LEAK_START_ABT	0x0a	//
#define NBP_FDB_PVT_START_OK		0x0b	//
#define NBP_FDB_PVT_START_ABT		0x0c	//
#define NBP_FDB_OVP_START_OK		0x0d	//
#define NBP_FDB_OVP_START_ABT		0x0e	//
#define NBP_FDB_CAL_START_OK		0x0f	//

#define NBP_FDB_CAL_START_ABT		0x10	
#define NBP_FDB_CAL_EXPRS_ERR	0x11	//NIBP calibration external pressure value Error
#define NBP_FDB_CAL_SMC_OT		0x12	//NIBP calibration Safety MCU init overtime
#define NBP_FDB_CAL_ZERO_OT		0x13	//NIBP calibration zeroing overtime
#define NBP_FDB_CAL_E200_OT		0x14	//NIBP calibration external 200mmHg overtime
#define NBP_FDB_CAL_SPAN_OT		0x15	//NIBP calibration calculate Span overtime
#define NBP_FDB_CAL_SAVE_OT		0x16	//NIBP calibration data save overtime
#define NBP_FDB_CAL_DONE_OK		0x17	//NIBP calibration finished
#define NBP_FDB_CAL_DATA_ERR		0x18	//NIBP calibration data error

#define NBP_FDB_TAG_PRS_ERR		0x20	//NIBP target inflate pressure set ERROR
#define NBP_FDB_PNEU_LEAK			0x21	//NIBP measurement pneumatic leak
#define NBP_FDB_INFL_FAIL			0x22	//NIBP measurement inflation fail ( cuff/hose not connected )
#define NBP_FDB_DEFLT_OT			0x23	//NIBP measurement deflate overtime
#define NBP_FDB_MEASURE_OT		0x24	//NIBP measurement  overtime
#define NBP_FDB_PULSE_WEAK		0x25	//NIBP measurement  pulse signal too weak
#define NBP_FDB_EXCS_PRS			0x26	//NIBP measurement pressure out of range

#define NBP_FDB_SIS_CTI_OT			0x30	//NIBP Static Venipuncture Cuff Type Inflation Overtime
#define NBP_FDB_SIS_CTC_OT			0x31	//NIBP Static Venipuncture Cuff Type Check Overtime
#define NBP_FDB_SIS_CTW_OT			0x32	//NIBP Static Venipuncture Cuff Type Wait Overtime
#define NBP_FDB_SIS_CTD_OT			0x33	//NIBP Static Venipuncture Cuff Type Deflation Overtime
#define NBP_FDB_SIS_CTR_OT			0x34	//NIBP Static Venipuncture Cuff Type Resolution Overtime
#define NBP_FDB_SIS_PRO_OT			0x35	//NIBP Static Venipuncture Process Overtime

#define NBP_FDB_LEAK_YES			0x40	//NIBP Pneumatic Leak
#define NBP_FDB_LEAK_NO			0x41	//NIBP Pneumatic No Leak
#define NBP_FDB_LEAK_PUMPERR		0x42	//NIBP Pneumatic pump error

#define NBP_FDB_OVP_NEO_OK		0x50	//NIBP Neonate /Pediactric Overpressue protection ok
#define NBP_FDB_OVP_NEO_FAIL		0x51	//NIBP Neonate /Pediactric Overpressue protection fail
#define NBP_FDB_OVP_ADU_OK		0x52	//NIBP Adult Overpressue protection ok
#define NBP_FDB_OVP_ADU_FAIL		0x53	//NIBP Adult Overpressue protection fail
#define NBP_FDB_OVP_PNEU_ERR		0x54	//NIBP Overpressue test fail, pneumatic error

#define NBP_FDB_PVT_PUMP_ON		0x60	//NIBP Pump On
#define NBP_FDB_PVT_PUMP_OFF		0x61	//NIBP Pump Off
#define NBP_FDB_PVT_DEF_OPEN		0x62	//NIBP Deflate[Fast] Valve Open
#define NBP_FDB_PVT_DEF_CLOSE		0x63	//NIBP Deflate[Fast] Valve Close
#define NBP_FDB_PVT_DUMP_OPEN	0x64	//NIBP Dump[Slow] Valve Open
#define NBP_FDB_PVT_DUMP_CLOSE	0x65	//NIBP Dump[Slow] Valve Close
#define NBP_FDB_PVT_OT				0x66	//NIBP Pump&Valve Test Overtime

#define NBP_FDB_FAIL_01			0x70
#define NBP_FDB_FAIL_02			0x71
#define NBP_FDB_FAIL_03			0x72
#define NBP_FDB_FAIL_04			0x73
#define NBP_FDB_FAIL_05			0x74
#define NBP_FDB_FAIL_06			0x75
#define NBP_FDB_FAIL_07			0x76
#define NBP_FDB_FAIL_08			0x77
#define NBP_FDB_FAIL_09			0x78
#define NBP_FDB_FAIL_10			0x79
#define NBP_FDB_FAIL_11			0x7a
#define NBP_FDB_FAIL_12			0x7b
#define NBP_FDB_FAIL_13			0x7c
#define NBP_FDB_FAIL_14			0x7d
#define NBP_FDB_FAIL_15			0x7e
#define NBP_FDB_FAIL_16			0x7f

/******************************************/

#define NIBP_STATUS_STOP				0	//δ������ȡ��NBP��λ���
#define NIBP_STATUS_MANUAL			1	//���ڲ����ֶ�����
#define NIBP_STATUS_AUTO				2	//���ڲ����Զ�����
#define NIBP_STATUS_SERIES				3	//���ڲ�����������
#define NIBP_STATUS_CALIBRATION		4		//���ڱ궨
#define NIBP_STATUS_DETECTION		5		//���ڼ��(©��..)
#define NIBP_STATUS_RESET			6		//NBP��λ
#define NIBP_STATUS_SYSERROR		10		//ϵͳ����
//NIBP��������
#define NIBP_TYPE_MANUAL				1	//���ֶ�������ʽ��
#define NIBP_TYPE_AUTO				2		//���Զ�������ʽ��
#define NIBP_TYPE_STAT				3		//��STAT������ʽ��
#define NIBP_TYPE_CALIBRATION		4		//��У׼��ʽ��
#define NIBP_TYPE_DETECTION			5		//��©�������
#define NIBP_TYPE_SYSERROR			10		//ϵͳ���󣬾��������Ϣ��NBP״̬��

//NIBP���ڲ������ʱ��(min)
#define NIBPMODE_MANUAL				0	//�ֶ�����ģʽ TODO:����
#define NIBP_INTERVAL_MIN			1
#define NIBP_INTERVAL_MAX			480
//��������ѹ��ֵ(��ʼѹ��)
#define NIBP_V_PRESSURE_MIN			20
#define NIBP_V_PRESSURE_MAX			120
//NIBP������ʽ(NEW)
#define NIBP_MODE_MANUAL			0	//����
#define NIBP_MODE_AUTO				1	//�Զ�
#define NIBP_MODE_SERIES				2	//����
#define NIBP_MODE_PUNCTURE			3	//����
//����������ʱ�� �� 5����
#define NIBP_STAT_TIME		300	
//���������ļ�� ��
#define NIBP_STAT_INTER		5	
//�������̱���ѹ����ʱ�� ��
#define NIBP_PUNCTURE_TIME_ADULT	170	
#define NIBP_PUNCTURE_TIME_BABY		80	
//NIBP������Ϣ
typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bSwitch;			//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;				//��ɫ
	BYTE bUnit;				//��ʾ��λ: NIBP_UNIT_MMHG, NIBP_UNIT_KPA
	BYTE bObject;			//��������: NIBP_OBJECT_ADULT, NIBP_OBJECT_CHILD, NIBP_OBJECT_BABY
	BYTE bMode;				//������ʽ: NIBP_MODE_XXX
	WORD wInterval;			//AUTO����ʱ�ļ��min, NIBP_INTERVAL_MIN~NIBP_INTERVAL_MAX, Ϊ0ʱ�ֶ�
	WORD wInitPres;		//��ʼѹ��, 
	WORD wVP;			//��������ѹ��, NIBP_V_PRESSURE_MIN ~ NIBP_V_PRESSURE_MAX
	WORD wHigh_Sys;			//����ѹ����
	WORD wLow_Sys;			//����ѹ����
	WORD wHigh_Dia;			//����ѹ����
	WORD wLow_Dia;			//����ѹ����
	WORD wHigh_Mean;		//ƽ��ѹ����
	WORD wLow_Mean;			//ƽ��ѹ����
	WORD wVPressure;
	BYTE bAlmSource;		//������Դ: NIBP_ALM_SRC_X
	BYTE bAlmControl;		//��������
	BYTE bPrnControl;		//����������ӡ
	int iAdjust_Sys;		//У׼ϵ��(�ٷֱ�)
	int iAdjust_Dia;		//У׼ϵ��(�ٷֱ�)
	int iAdjust_Mean;		//У׼ϵ��(�ٷֱ�)
	WORD wMax_NIBP;	//�������еı�����ֵ
	WORD wMin_NIBP;		//�������еı����Сֵ
}S_NIBP_CFG, *PS_NIBP_CFG;
//NIBP������Ϣ
//NIBP������Ϣ(��������) 
#define NIBP_INFO_EXCESSIVE_MOTION	BIT0		//�˶�����
#define NIBP_INFO_CUFF_MISTAKE		BIT1		//������ʹ�
#define NIBP_INFO_CUFF_TOOLAX		BIT2		//���̫��
#define NIBP_INFO_WEAK_SIGNAL		BIT3		//΢���ź�
#define NIBP_INFO_INTERFERENCE		BIT4		//����̫��
#define NIBP_INFO_OVER_RANGE		BIT5		//����������Χ
#define NIBP_INFO_OVERPRESSURE		BIT6		//��ѹ����
#define NIBP_INFO_TIMEOUT			BIT7		//��ʱ
#define NIBP_INFO_ERROR				BIT8		//��������
#define NIBP_INFO_SIGNAL_SATURATION	BIT9		//�źű���
#define NIBP_INFO_MODULE_FAULT		BIT10		//ģ�����
//(new)hejp
#define NIBP_ERR_CUFF_TOOLAX			BIT0		//������� 
#define NIBP_ERR_CUFF_LEAK				BIT1		//©�� 
#define NIBP_ERR_PRESSURE				BIT2		//��ѹ���� 
#define NIBP_ERR_WEAK_SIGNAL			BIT3		//΢���ź�
#define NIBP_ERR_OVER_RANGE			BIT4		//����Χ
#define NIBP_ERR_EXCESSIVE_MOTION	BIT5		//�����˶�
#define NIBP_ERR_OVERPRESSURE			BIT6		//��ѹ 
#define NIBP_ERR_SIGNAL_SATURATION	BIT7		//�źű��� 
#define NIBP_ERR_CHECKLEAK				BIT8		//©�����ʧ��
#define NIBP_ERR_MODULE_FAULT			BIT9		//ϵͳ���� 
#define NIBP_ERR_TIMEOUT				BIT10	//��ʱ 
#define NIBP_ERR_CUFF_MISTAKE			BIT11	//������ʹ�

typedef struct {
	BYTE bID;			//ģ��ID
	BYTE bConnected;	//�Ƿ�ӹ�����
	BYTE bObject;		//������Ϣ00-���ˣ�01-��ͯ��02-������
	BYTE bStatus;		//����״̬: NIBP_STATUS_XX
	BYTE bEndType;		//�������ͽ���:NIBP_STATUS_XX
	BYTE bMeasureType;		//��������:NIBP_STATUS_XX
	BOOL bMeasureOK;	//������ɵı�־, TRUE-OK
	BOOL bCanceled;		//�Ƿ�ȡ��,TRUE:yes	
	WORD wSys;			//����ѹ
	WORD wDia;			//����ѹ
	WORD wMean;			//ƽ��ѹ
	WORD wPR;			//����
	WORD wPressure;		//���ѹ
	BOOL bInvalPre; 	//���ѹ��Чֵ��־
	S_TIME sTime;		//����ʱ��
	WORD wRemTime;		//ʣ��ʱ��:s��
	WORD bPeriod;			//��������
	WORD wCalPreValue;
	//Error
	BOOL bComError;		//ͨѶ����	
	BOOL bComStop;		//ͨѶֹͣ
	BOOL bModuleError;	//ģ�����
	BOOL bLimitError_Sys;	//�����޴���
	BOOL bLimitError_Dia;	//�����޴���
	BOOL bLimitError_Mean;	//�����޴���
	BOOL bOverrange;		//��������
	unsigned int  wFDB;		//������Ϣ  NIBP_INFO_XX
	unsigned int  wErrCode;		//������Ϣ  NIBP_INFO_XX
}S_NIBP_VALUE, *PS_NIBP_VALUE;

//---- IBP����ض��� ----
//������Χ(mmHg)
#define IBP_MAX		400
#define IBP_MIN		-100
//��ʾ��λ
#define IBPUNIT_MMHG	0
#define IBPUNIT_KPA	1
#define IBPUNIT_CMH2O	2
//����
#define	IBPLAB_ART		0	//����Ѫѹ
#define 	IBPLAB_PA		1	//����ѹ	
#define	IBPLAB_CVP		2	//���ľ���ѹ
#define	IBPLAB_LAP		3	//��ѹ
#define	IBPLAB_RAP		4	//�ҷ�ѹ 
#define	IBPLAB_ICP		5	//­��ѹ
#define	IBPLAB_P1		6
#define	IBPLAB_P2		7
#define 	IBPLAB_AO		8  	//������ѹ
#define	IBPLAB_UAP		9	//�궯��ѹ
#define	IBPLAB_BAP		10	//�Ŷ���ѹ
#define	IBPLAB_FAP		11	//�ɶ���ѹ
#define	IBPLAB_UVP		12	//�꾲��ѹ

//������ʾ��ʽ
#define 	IBPVIEW_ALL	0
#define 	IBPVIEW_M	  	1
//����Դ
#define IBP_ALM_SRC_ALL		0
#define IBP_ALM_SRC_S		1
#define IBP_ALM_SRC_M		2
#define IBP_ALM_SRC_D		3
#define IBP_ALM_SRC_SM		4
#define IBP_ALM_SRC_MD		5
#define IBP_ALM_SRC_SD		6
//����״̬
#define IBP_STATUS_NORMAL	0
#define IBP_STATUS_ZERO		1	//����У׼
//�˲�ģʽ
#define IBP_FILTER_SMOOTH	0	//ƽ���˲�
#define IBP_FILTER_NORMAL	1	//�����˲�

//IBP������Ϣ
typedef struct {
	BYTE bID;					//ģ��ID
	BYTE bSwitch;				//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;					//��ɫ
	BYTE bUnit;					//��ʾ��λ: IBPUNIT_MMHG, IBPUNIT_KPA, IBPUNIT_CMH2O
	BYTE bLabel;				//ѡ��໤ѹ������: IBPLAB_XX
	BYTE bViewMode;				//������ʾģʽ: IBPVIEW_ALL, IBPVIEW_M
	BYTE bScanSpeed;			//ɨ���ٶ�: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	int iScaleTop;				//ѹ��������ʾ�̶ȸ��ޣ��ߵ����������10mmHg
	int iScaleBottom;			//ѹ��������ʾ�̶ȵ���	
	int iScaleMiddle;			//ѹ��������ʾ�̶�����
	int iHigh_Sys;				//����ѹ����
	int iLow_Sys;				//����ѹ����
	int iHigh_Dia;				//����ѹ����
	int iLow_Dia;				//����ѹ����
	int iHigh_Mean;				//ƽ��ѹ����
	int iLow_Mean;				//ƽ��ѹ����
	int iCheckValue;			//У׼ֵ��Ϊ���ֵ0�� ��ģ��ɼ�����ֵ��ȥ��ֵ��������ȷ��ֵ
	BYTE bAlmSource;			//����Դ: IBP_ALM_SRC_XX, ����ʾ��ʽΪMeanʱ������Դ��̶�ΪMean
	BYTE bAlmControl;			//�������ƣ�0���رգ�1��LOW��2��MID��3��HIGH
	BYTE bPrnControl;			//����������ӡ��0���رգ�1����	
	BYTE bFilterMode;			//IBP�˲�ģʽIBP_FILTER_SMOOTH��IBP_FILTER_NORMAL
}S_IBP_CFG, *PS_IBP_CFG;
//IBP������Ϣ
typedef struct {
	BYTE bID;		//ģ��ID
	BYTE bConnected;	//�Ƿ�ӹ�����
	BYTE bCommStatus;	//ͨѶ״̬
	WORD wHR;		//����ֵ
	BOOL bOff;		//̽ͷ״̬��FALSE��normal��TRUE������
	BYTE bStatus;		//����״̬��IBP_STATUS_NORMAL, IBP_STATUS_ZERO
	BYTE bCalResult;	//У׼���
	BYTE bZeroResult;  	//У����
	S_TIME sCalTime;		//����ʱ��
	int iSys;		//����ѹ
	int iDia;		//����ѹ
	int iMean;		//ƽ��ѹ
	int iWave;		//ѹ������
	BYTE Calviewmode; //�ı������ʾ����У�㻹��У׼
}S_IBP_VALUE, *PS_IBP_VALUE;

//---- CO2����ض��� ----
//CO2������Χ(mmHg)
#define CO2_MAX		99
#define CO2_MIN		0
//��ʾ��λ
#define CO2UNIT_MMHG		0
#define CO2UNIT_PER		1
#define CO2UNIT_KPA		2
//��������
#define CO2RATE_50		0
#define CO2RATE_75		1
#define CO2RATE_100		2
#define CO2RATE_150		3
#define CO2RATE_200		4
//����״̬
#define CO2_STATUS_STOP		0	//����
#define CO2_STATUS_RUN		1	//����	
//������ʾ����
#define CO2_RANGE_BIG		0	//��0~50mmHg
#define CO2_RANGE_MIDDLE	1	//�У�0~75mmHg
#define CO2_RANGE_SMALL		2	//С��0~CO2_MAX
//CO2������Ϣ
typedef struct {
	BYTE bID;		//ģ��ID
	BYTE bSwitch;		//ģ�鿪��: SWITCH_OFF�� SWITCH_ON
	int iColor;		//��ɫ
	BYTE bUnit;		//��ʾ��λ: CO2UNIT_MMHG, CO2UNIT_PER, CO2UNIT_KPA
	BYTE bScanSpeed;	//ɨ���ٶ�: SCANSPEED_625, SCANSPEED_125, SCANSPEED_25
	int iScaleTop;		//��������: ���ε��ϱ��(mmHg)
	BYTE bRate;		//�����������: CO2RATE_XX
	BYTE bRange;		//������ʾ��Χ: CO2_RANGE_BIG, CO2_RANGE_MIDDLE, CO2_RANGE_SMALL
	int iCaliValue;		//У׼ֵ
	WORD wHigh_EtCO2;	//EtCo2����
	WORD wLow_EtCO2;	//EtCo2����
	WORD wHigh_FiCO2;	//FiCo2����
	WORD wLow_FiCO2;	//FiCo2����	(��ʹ��)
	WORD wHigh_awRR;	//awRR����	(��ʹ��)
	WORD wLow_awRR;		//awRR����	(��ʹ��)
	WORD wApneaTime;	//��Ϣ����ʱ��1~60s	(��ʹ��)
	BYTE bAlmControl;	//��������
	BYTE bPrnControl;	//����������ӡ
}S_CO2_CFG, *PS_CO2_CFG;
//CO2������Ϣ
typedef struct {
	BYTE bID;		//ģ��ID
	BYTE bConnected;	//�Ƿ�ӹ�����
	BYTE bCommStatus;	//ͨѶ״̬
	BYTE bStatus;		//����״̬: CO2_STATUS_STOP, CO2_STATUS_RUN
	BOOL bOff;		//������״̬��FALSE��normal��TRUE������
	BYTE bRate;		//�����������: CO2RATE_XX
	WORD wAwRR;		//����������
	WORD wEtCO2;		//��ĩ������̼
	WORD wFiCO2;		//���������̼
	WORD wInsCO2;		//˲ʱ������̼(У׼ʱʹ��)
	WORD wWave;		//CO2����
	WORD wBaro;		//����ѹ��   (CO2�� �� (CO2(mmHg)*100 / wBaro) )
}S_CO2_VALUE, *PS_CO2_VALUE;

//---- ���˵���ض��� ----
//�����ִ��ķ�Χ
#define PATIENT_NAME_MAX	9
#define PATIENT_DATE_MAX	9
#define PATIENT_BED_MAX		PATIENT_NAME_MAX
#define PATIENT_ROOM_MAX	PATIENT_NAME_MAX
#define PATIENT_ID_MAX		PATIENT_NAME_MAX
#define PATIENT_BIRTH_MAX	9
//��������
#define ADULT	0
#define CHILD	1
#define BABY		2
//�����Ա�
#define MALE		0
#define FEMALE	1
//����
#define AGE_MIN		0
#define AGE_MAX		150
//���(cm)
#define HEIGHT_MIN	10
#define HEIGHT_MAX	250
//����(kg)
#define WEIGHT_MIN	1
#define WEIGHT_MAX	200
//Ѫ��
#define BLOOD_A			0
#define BLOOD_B			1
#define BLOOD_O			2
#define BLOOD_AB		3
#define BLOOD_UNKNOW	4

typedef struct {

	BYTE sRoom[PATIENT_ROOM_MAX];	//���ҷ����
	BYTE sID[PATIENT_ID_MAX];			//����ID
	BYTE sBed[PATIENT_BED_MAX];		//��λ	
	BYTE sDoctor[PATIENT_NAME_MAX];	//ҽ��	
	BYTE sName[PATIENT_NAME_MAX];		//����
	BYTE bSex;							//�Ա�: MALE, FEMALE	
	BYTE bObject;						//��������: ADULT, CHILD, BABY
	BYTE sAdmit[PATIENT_BIRTH_MAX];	//��Ժ����
	BYTE sBirth[PATIENT_BIRTH_MAX];		//������
	BYTE bAge;							//����
	WORD wHeight;						//���cm
	WORD wWeight;						//����Kg
	BYTE bBlood;						//Ѫ��	0A, 1:B, 2:O, 3:AB, 4 :N/A
	BYTE bPacemaker;					//�Ƿ���: PACEMAKER_OFF or PACEMAKER_ON	
	
}S_PATIENT_CFG, *PS_PATIENT_CFG;

//---- ϵͳ����ض��� ----
//ϵͳ�����ļ�
#define SETUP_RECENT			0
#define SETUP_ADULT			1
#define SETUP_CHILD			2
#define SETUP_BABY			3
#define SETUP_USER1			4
#define SETUP_USER2			5
#define SETUP_USER3			6
//ϵͳ����
#define LANGUAGE_ENGLISH		0
//#define LANGUAGE_OTHER		1
#define LANGUAGE_CHINESE		1
#define LANGUAGE_CHT			2
#define LANGUAGE_TURKISH		3
#define LANGUAGE_RUSSIAN		4
#define LANGUAGE_ROMANIA		5
#define LANGUAGE_SPANISH		6
#define LANGUAGE_ITALIAN		7



//��������
#define SCREEN_NORMAL			0
#define SCREEN_NIBPLIST			1
#define SCREEN_BIGCHAR			2
#define SCREEN_SHORTTREND		3
#define SCREEN_7LEADECG		4
#define SCREEN_MULTIECG		5
#define SCREEN_OXYCRG			6
#define SCREEN_OTHERBED		7
#define SCREEN_IBP				8
#define SCREEN_12LEAD			9

//���̷���
#define KEYVOLUME_OFF	0
#define KEYVOLUME_ON	1
//ECG��������
#define ECGLEAD_AHA		0
#define ECGLEAD_IEC		1
//Һ������
#define LCDTYPE_800_104		0	//800x600-10.4"
#define LCDTYPE_800_121		1	//800x600-12.1"
#define LCDTYPE_1024_121	2	//1024x768-12.1"
#define LCDTYPE_1024_15		3	//1024x768-15"
//��������
#define NETTYPE_ETH			0
#define NETTYPE_485			1
//���ܼ�����
#define FUNCBT_EXIT			0	//�˻�����Ļ
#define FUNCBT_SWITCHVIEW		1	//�л���ͼ
//�߶εĻ��Ʒ�ʽ
#define LINE_WIDE			0	//������(multi Pixels)
#define LINE_NORMAL			1	//��������(1Pixel)
#define LINE_ANTI			2	//����������

//ϵͳ������Ϣ
typedef struct {
	BYTE bLanguage;			//ϵͳ����: 0-English, 1-Other
	BYTE bInterface;		//��������
	BYTE bKeyVolume;		//���̲�������: KEYVOLUME_OFF, KEYVOLUME_ON
	BYTE bACFreq;			//����Ƶ��: AC_50HZ, AC_60HZ
	BYTE bECGLeadDef;		//ECG��������: ECGLEAD_AHA, ECGLEAD_IEC
	BYTE bLCDType;			//Һ������: LCDTYPE_121, LCDTYPE_104
	BYTE bNetType;			//��������: NETTYPE_ETH, NETTYPE_485
	BYTE bShortTrendScale;		//������ʱ��,0-30min, 1-60min, 2-120Min, other=120Min
	BYTE bOxyCrgScale;		//OxyCrgʱ��, 0-1Min, 1-2Min, 2-5Min, 3-10Min, 4-15Min, 5-30Min,other-30Min  
	BYTE bFuncButton;		//���ܼ�����
	BYTE bLineType;			//�߶εĻ��Ʒ�ʽ
	BYTE bWDTEnable;		//���ÿ��Ź�
}S_SYSTEM_CFG, *PS_SYSTEM_CFG;


#define ALARMPAUSE_1MIN		0
#define ALARMPAUSE_2MIN		1
#define ALARMPAUSE_3MIN		2
//������������
typedef struct {
	BYTE bVolume;			//0:Off, 1~10
	BYTE bPrint;			//SWITCH_OFF, SWITCH_ON
	BYTE bPauseTime;		//��ͣʱ��:ALARMPAUSE_xMIN
	BOOL bViewLimit;		//�Ƿ���ʾ����������
	BYTE bPrintWaveLength;		//��¼�Ĳ��γ��ȣ� 4��8��16��, ������ MAXALARMWAVELENGTH
}S_ALARM_CFG, *PS_ALARM_CFG;

//һ�������ɵ�������
#define WAVEPANEL_MAX_COUNT	10

//�������ͱ�ʶ
#define WID_NONE			0
#define WID_ECG1			1
#define WID_ECG2			2
#define WID_SPO2				3
#define WID_RESP			4
#define WID_IBP1				5
#define WID_IBP2				6
#define WID_CO2				7
#define WID_MULTIECG		8
#define WID_MAX		WID_MULTIECG

//���������������,�û�����
typedef struct {
	BYTE  bID;		//�������ͱ�ʶ
	BOOL bSwitch;		//��ʾ���Ŀ��� SWITCH_OFF, SWITHC_ON 
}WAVE_PANEL_CFG, *PWAVE_PANEL_CFG;

//-------------- ���ݴ�������ݽṹ ------------------
/*
ѭ������Ľṹ
typedef struct {
	WORD wCount;	
	WORD wHeadIndex;	//ѭ��������ָ��
	WORD wTailIndex;	//ѭ������βָ��
	BOOL bLoop;		//���鷭ת���
	int iValue[MAXVALUECOUNT];
}CA_XX;
*/

//---- ���ƴ洢�Ķ��� ----
//����һЩ������ʾ���Ƶ���Ϣ����ʾ���е�ĳ��ģ�����Ϣ�Ƿ���Ч,���ĳ��ģ�������ˣ�����ϢҲ����Ч��
//DWORD       dwHaveFlag;
#define HAVEHR		0x00000001
#define HAVESPO2	0x00000002
#define HAVENIBP	0x00000004
#define HAVEPR		0x00000008
#define HAVERESP	0x00000010
#define HAVET1		0x00000020
#define HAVET2		0x00000040
#define HAVEST		0x00000080
#define HAVECO2		0x00000100
#define HAVEIBP1		0x00000200
#define HAVEIBP2		0x00000400
#define HAVEAA		0x00000800

//�������Ƽ�¼ԭ��ı�� 
//BYTE        bLogFlag
#define TRENDAUTO	0x01    //�����Զ���¼
#define TRENDNIBP	0x02    //NIBP������¼
#define TRENDMARK	0x04    //�û���ǵ��¼�
#define TRENDALARM	0x08    //����

//NIBP �������ݽṹ
typedef struct{
	WORD wSys;
	WORD wMean;
	WORD wDia;
}NIBPDATA, *PNIBPDATA;
//IBP �������ݽṹ
typedef struct{
	int iSys;
	int iMean;
	int iDia;
}IBPDATA, *PIBPDATA;
//CO2 �������ݽṹ
typedef struct {
	WORD wEtCO2;	
	WORD wFiCO2;
	WORD wAwRR;	
}CO2DATA, *PCO2DATA;
//ͬ���Ͳ���������
#define ST_COUNT	2
#define TEMP_COUNT	2
#define IBP_COUNT	2

//������������	
typedef struct {
	long int	liOffset;	//�����ƴ洢��ʼʱ���ƫ����,��λ����, ���������洢ʱ��
	S_TIME		sTime;		//�洢�������ݵ�ʱ��(����ƫ��������)
	BYTE      	bLogFlag; 	//��¼ԭ�� ��ǣ�NIBP����������������
	DWORD		dwHaveFlag;		//��¼�������Ƿ�����  HR ST1 ST2 ST3 NIBP SPO2 PULSE T1 T2 CO2 IBP1 IBP2 
	int		wHR;
	int		wPR;
	int		wRR;    
	short int	iSTs[ST_COUNT];
	BYTE		bSpO2;
	NIBPDATA	sNIBP;
	IBPDATA		sIBPs[IBP_COUNT];
	WORD		wTemps[TEMP_COUNT]; 
	CO2DATA		sCO2;
	//TODO:�˴��Ժ�Ҫ���AA�Ȳ���
}TRENDDATA, *PTRENDDATA;

//����洢���ε�����, ���ǵ�Resp Spo2�Լ��������β�ͬ��, �����������±�־
#define REALSAVE_ECG	0
#define REALSAVE_SPO2	1
#define REALSAVE_RESP	2
//���洢�Ĳ���, 8λ����
typedef struct {
	//����ECG����,����3/5����,5����ʱ:Ecg1-I, Ecg2-II, Ecg3-V, ������ʱ: Ecg1-I, Ecg2--II, Ecg3-III
	int bEcg1;
	int bEcg2;
	int bEcg3;
	BYTE bSpO2;
	BYTE bResp;
	BYTE bCO2;
	int iIbp1;
	int iIbp2;
}TRENDWAVE, *PTRENDWAVE;

//�����������������洢����,��λ:����
#define MAXSHORTTREND		120	///2Сʱ	
//��������������
typedef struct {
	WORD		wHead;	//ѭ��������ָ��
	BOOL		bFull;		//���鷭ת���
	TRENDDATA	Datas[MAXSHORTTREND];		//����������
}CA_SHORTTREND, *PCA_SHORTTREND;


//���������������洢����,��λ:����
#define MAXTRENDCOUNT		7200	//5��	
//������������
typedef struct {
	S_PATIENT_CFG	sPatientInfo;	//������Ϣ	
	S_TIME		sStartTime;	//��ʼ�洢�������ݵ�ʱ��
	WORD		wHeadIndex;	//ѭ��������ָ��
	WORD		wTailIndex;	//ѭ������βָ��
	BOOL		bLoop;		//���鷭ת���
	WORD		wCount;		//ʵ�ʴ洢������
	TRENDDATA	Datas[MAXTRENDCOUNT];		//��������
}CA_TRENDARRAY, *PCA_TRENDARRAY;


//ʵʱ�洢�ĵ�������,��������,��1����Ϊ��λ
#define SAMPLING_RATE	200		//������
typedef struct {
	TRENDDATA	paras;
	TRENDWAVE	waves[SAMPLING_RATE];
}REALSAVEDATA, *PREALSAVEDATA;

//ʵʱ����ѭ������,��λ ��
#define MAXREALSAVECOUNT	60
typedef struct {
	S_TIME 			sStartTime;			//��ʼ�洢���ݵ�ʱ��
	WORD 			wHeadIndex;			//ѭ��������ָ��
	WORD 			wTailIndex;			//ѭ������βָ��
	BOOL 				bLoop;			//���鷭ת���
	WORD 			wCount;				//ʵ�ʴ洢������
	REALSAVEDATA		Datas[MAXREALSAVECOUNT];	//��������
}CA_REALSAVEARRAY, *PCA_REALSAVEARRAY;


//�Ƿ�洢ʵʱECG����
// #define SAVEREALECG				1

//ʵʱECG����(ֻ��һ��,Channel1)
typedef struct {
	BYTE bLead;				//������
	BYTE bWave[SAMPLING_RATE];		//����
}REALECGDATA, *PREALECGDATA;

//ʵʱECG���δ洢, ��λ: ��
#define MAXREALECGCOUNT	3600
typedef struct {
	WORD 			wHeadIndex;				//ѭ��������ָ��
	WORD 			wTailIndex;				//ѭ������βָ��
	BOOL 			bLoop;					//���鷭ת���
	WORD 			wCount;					//ʵ�ʴ洢������
	REALECGDATA		Datas[MAXREALECGCOUNT];
}CA_REALECGARRAY, *PCA_REALECGARRAY;

//��������ʵʱ�洢���� second
#define MAXREALALARMSAVECOUNT	33		
typedef struct {
	S_TIME 		sStartTime;				//��ʼ�洢���ݵ�ʱ��
	WORD 			wHeadIndex;			//ѭ��������ָ��
	WORD 			wTailIndex;			//ѭ������βָ��
	BOOL 			bLoop;				//���鷭ת���
	WORD 			wCount;				//ʵ�ʴ洢������
	REALSAVEDATA		Datas[MAXREALALARMSAVECOUNT];	//��������
}CA_REALALMSAVEARRAY, *PCA_REALALMSAVEARRAY;


//����ʱ���εĴ洢��ʱ�� ��
#define MAXALARMWAVELENGTH	MAXREALALARMSAVECOUNT		//ǰ��16��
//������������(���ݼӲ���)
typedef struct {
	BYTE 		bAlmType;				//��������, ALARM_TECHNOLOGY, ALARM_PHYSIOLOGICAL	
	BYTE 		bAlmID;					//����ID, (������AlarmManger.h)
	BYTE 		bAlmClass;       			//��������, ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH, ALARM_INFO	
	TRENDDATA	paras;					//����ʱ�̵�����
	TRENDWAVE	waves[MAXALARMWAVELENGTH][SAMPLING_RATE];           //����ʱ��ǰ��X��Ĳ���
}ALARMDATA, *PALARMDATA;

//�����洢������, ��λ:��
//#define MAXALARMDATACOUNT			150	//60
#define MAXALARMDATACOUNT			60
typedef struct {
	WORD 			wHeadIndex;			//ѭ��������ָ��
	WORD 			wTailIndex;			//ѭ������βָ��
	BOOL 			bLoop;				//���鷭ת���
	WORD 			wCount;				//ʵ�ʴ洢������
	ALARMDATA		Datas[MAXALARMDATACOUNT];	//��������
}CA_ALMDATAARRAY, *PCA_ALMDATAARRAY;


//ϵͳ��Դ״̬
#define POWER_SHUTDOWN		0
#define POWER_WORKING			1
#define POWER_DC				0
#define POWER_AC				1
#define POWER_NOT_CHARGE		0
#define POWER_CHARGING		1
#define POWER_NOBAT			0
#define POWER_HAVEBAT			1
#define BATTERY_EMPTY			0
#define BATTERY_Q1				1
#define BATTERY_Q2				2
#define BATTERY_Q3				3
#define BATTERY_Q4				4
#define BATTERY_FULL			5
typedef struct {
	BYTE  bShutDown;
	BYTE  bPowerType;
	BYTE  bHaveBat;
	BYTE  bCharge;
	BYTE  bBatteryStatus;
	int iSoft_Rev;
	int iHard_Rev;
}POWER_STATUS, *PPOWER_STATUS;


//---- ���ڣ�>��ͼ������ ----
//�������ݻ�����г���
#define QUEUE_MAX_SERIAL	500	//200

//-- ECG ������� --
typedef struct {
	int head;
	int tail;
	S_ECG_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_ECG;
//-- RESP ������� --
typedef struct {
	int head;
	int tail;
	S_RESP_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_RESP;	
//-- Spo2 ������� --
typedef struct {
	int head;         
	int tail;
	S_SPO2_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_SPO2;		
//CO2 �������
typedef struct {
	int head;
	int tail;
	S_CO2_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_CO2;
//IBP �������
typedef struct {
	int head;
	int tail;
	S_IBP_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_IBP;

//�����������ͺ�����
//��ʾ����
#define PASSTYPE_DEMO		0		
#define PASS_DEMO		2888
//�û�ά��                                 	
#define PASSTYPE_USER		1
#define PASS_USER		2899    
//����ά��
#define PASSTYPE_FACTORY		2
#define PASS_FACTORY		125689
//����ά��
#define PASSTYPE_MACHINE		3
#define PASS_MACHINE		1789

//��ӡ���Ĺ���״̬
#define PRNSTATUS_ERR			0		//û�д�ӡ�������߹���
#define PRNSTATUS_IDLE			1		//��ӡ������
#define PRNSTATUS_PRINTING		2		//��ӡ�����ڴ�ӡ
#define PRNSTATUS_NOPAPER		3		//��ӡ��ȱֽ     (��ʱû��)

//New-��ӡ��״̬
#define PRINT_STATUS_NOPRN		0		//û�д�ӡ�������߹���
#define PRINT_STATUS_BUSY		1		//��ӡ��æµ�����ڴ�ӡ
#define PRINT_STATUS_ASK		2		//��ӡ�����У����Դ�ӡ
#define PRINT_STATUS_ERR		3		//��ӡ�����󣬹��Ȼ���ֽ
#define PRINT_STATUS_NOERR		4		//��ӡ��״̬���������Դ�ӡ

//��ӡ�����ڽ��еĹ�������
#define PRNWORK_NONE		0		//û�д�ӡ����
#define PRNWORK_REAL		1		//���ڽ���ʵʱ��ӡ
#define PRNWORK_COPY		2		//���ڽ��п�����ӡ
#define PRNWORK_OTHER		3		//���ڽ��м�¼��ӡ


//new��ӡ����ӡ�߳��д�ӡ����
#define PRN_WORK_NONE		0		//û�д�ӡ����
#define PRN_WORK_REAL		1		//ʵʱ������Ϣ��ӡ
#define PRN_WORK_ALM		2		//�����ع˴�ӡ
#define PRN_WORK_TREND	3		//�����б��ӡ
#define PRN_WORK_NIBP		4		//nibp�б��ӡ
#define PRN_WORK_ALMPRINT		5		//nibp�б��ӡ
//��ӡ�ٶ�
#define	PRINTSPEED_125		0
#define	PRINTSPEED_25		1
#define	PRINTSPEED_50		2

//define print recorder text print macro
#define MAX_ROW_COUNT				14  			//����ӡ�ִ�������
#define MAX_TEXT_LENGTH				200		//�ִ��ĳ���

//������ʱ�� ��
#define REALPRINTTIME_MAX	60
//����Զ���¼��� ��
#define REALPRINTAUTO_MAX	480

//��ӡ������
typedef struct {
	BYTE  bWave1;		//WID_xxx
	BYTE  bWave2;
	BYTE  bWave3;		
	BYTE bSpeed;		//PRINTSPEED_xx		
	int  iDuration;		//����ʱ��  0 - ������ other-��λ����
	int  iInter;		//�Զ�ʵʱ��¼�ļ��ʱ��, 0-�ر�	 
}S_PRINT_CFG, *PS_PRINT_CFG;

//��������
typedef struct {
	WORD	wBedNO;
	unsigned char	cIP[16];
	unsigned char 	cMask[16];
	unsigned char  cGW[16];
}S_NET_CFG, *PS_NET_CFG;

//�б��ѡ����
typedef struct {
	BYTE bID;	 //��ǰѡ�е�ID��
	unsigned char *strIndex[15];//��ǰ�б����ʾ��ѡ��
	BYTE Count; //ѡ���
}NWAVEFORM_COB, *PNWAVEFORM_COB;
NWAVEFORM_COB	WaveformCob[10];

//���ι���ѡ��ṹ��
typedef struct {
	BYTE bID;	 
	unsigned char *strWaveform;//ѡ��������
	BOOL bSwitch; //�Ƿ�ѡ��
}NWAVEFORM_CFG, *PNWAVEFORM_CFG;
NWAVEFORM_CFG tmpWaveform[15];

//New ϵͳ״̬
typedef struct {
	BYTE bState33V;	 ///3.3v��ѹ״̬0-���� 1-̫�� 2-̫��
	BYTE bState5V;	 	///5v��ѹ״̬0-���� 1-̫�� 2-̫��
	BYTE bSelWatchdog;	//watchdog�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelAD;		//A/D�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelRAM;		//RAM�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelROM;		//ROM�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelCPU;		//CPU�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelIBP;		//IBP�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelNBP;		//NBP�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelSPO2;		//SPO2�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelTEMP;		//TEMP�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelRESP;		//RESP�Լ���0-�Լ���ȷ1-�Լ����
	BYTE bSelECG;		//ECG�Լ���0-�Լ���ȷ1-�Լ����
	
	WORD wVersion;		//�汾��
	WORD wModuleSta1;		//ģ���ʾ��1
	WORD wModuleSta2;		//ģ���ʾ��2
	WORD wModuleSta3;		//ģ���ʾ��3
	
	WORD wModuleSV;
	WORD wModuleHV;
	unsigned long lModuleUnitID;
	unsigned long  wModuleSN;
	WORD wPowerSV;
	WORD wPowerHV;	
}SYS_STATUS, *PSYS_STATUS;
SYS_STATUS SystemStatus;

//����ͼ����
typedef struct {
	BYTE  bPara1;		//����ͼ1
	BYTE  bPara2;
	BYTE  bPara3;		
	BYTE  bScale;		//���ʱ��0~9:1h 2h 4h 6h 8h  10h 12h 24h 48h 72h	
	 
}TRENDGRAPH_CFG, *P_TRENDGRAPH_CFG;
TRENDGRAPH_CFG TrendGraphCfg;

#define MAX_OB_IP		 250

//��������
typedef struct {
	char IPaddr[16]; 	//IP��ַ
	BYTE BedNo;		//����	
	BYTE MacNo;		//mac������	
	BOOL FLAG;		//�㲥��flag
	BYTE Status;		//����״̬flag
	int timeout;		//�������״̬������
}S_OB_INFO, *PS_OB_INFO;

S_OB_INFO OB_Info[MAX_OB_IP];


#endif	//__DATASTRUCT_H__

//����ͨ�Ź�Լ
#define NMACHINE_CENTER 		0x01 	//�����
#define NMACHINE_MULTI_PARA 	0x02	//������໤��
#define NMACHINE_MATERNAL 		0x03	//ĸӤ�໤��
#define NMACHINE_FETAL 			0x04	//̥���໤��
#define NMACHINE_DEFIBRILLATE	0x05	//̥���໤��
#define NMACHINE_OTHER			0x06	//Ԥ���豸

#define NCMD_BROADCASE		0x00	//�㲥����
#define NCMD_ASK_PACK			0x01	//�������ݱ�����->��
#define NCMD_ANSWER_PACK		0x01	//Ӧ���������ݱ���
#define NCMD_DATA_PACK		0x02	//���ݱ���
#define NCMD_STOP_COMM		0x02	//��ֹͨ��
#define NCMD_ASK_RESEND		0x03	//���������ط�
#define NCMD_NO_RESENDDATA	0x03	//Ҫ���ط��ı��Ĳ����� 
#define NCMD_ASK_NIBP			0x04	//�����ӻ���Ѫѹ����
#define NCMD_ANSWER_NIBP		0x04	//Ӧ��������Ѫѹ��������
#define NCMD_ASK_CFG			0x05	//����ӻ��Ĳ���������Ϣ
#define NCMD_SET_PATINFO		0x05	//���ôӻ��Ĳ�����Ϣ
#define NCMD_SET_CFG			0x05	//���ôӻ������ò�����Ϣ 
#define NCMD_UPLOAD_PATINFO	0x05	//�ϴ�������Ϣ������� 
#define NCMD_UPLOAD_CFG		0x05	//�ϴ����ò�����Ϣ������� 
#define NCMD_CHANGE_MODE		0x06	//�໤ģʽ�л�
#define NCMD_CHANGE_PAT		0x07	//��������
#define NCMD_ALARM				0x10	//����

#define NCMD_OB_STOP_COMM	0x08	//����ֹͣͨѶ
#define NCMD_OB_ANSWER_PACK	0x09	//��������

#define NMODULE_ECG			0x01	//ECG ģ��
#define NMODULE_SPO2			0x02	//SPO2 ģ��
#define NMODULE_RESP			0x03	//RESP ģ��
#define NMODULE_NIBP			0x04	//NIBP ģ��
#define NMODULE_TEMP1			0x05	//TEMP1 ģ��
#define NMODULE_TEMP2			0x06	//TEMP2ģ��
#define NMODULE_CO2			0x07	//CO2 ģ��
#define NMODULE_IBP1			0x08	//IBP1 ģ��
#define NMODULE_IBP2			0x09	//IBP2 ģ��
#define NMODULE_ARR			0x0A	//ARR ģ��

//0x05 ����������
#define NET_PAT_ALL 				0		//���в�����Ϣ		
#define NET_PAT_DEPT 			1		//���ҷ���	
#define NET_PAT_ID	 			2		//���˱��
#define NET_PAT_BEDNO 			3		//���˴���
#define NET_PAT_DOCTOR 		4		//����ҽ��
#define NET_PAT_NAME			5		//��������
#define NET_PAT_SEX 			6		//�Ա�
#define NET_PAT_TYPE 			7		//����
#define NET_PAT_ADMIT 			8		//��Ժ��������
#define NET_PAT_BIRTH 			9		//��������
#define NET_PAT_HEIGHT 			10		//���
#define NET_PAT_WEIGHT 			11		//����
#define NET_PAT_BLOOD 			12		//Ѫ��

#define NET_ECG_ALL				100		//�����ĵ������Ϣ
#define NET_ECG_ALMLEV			101		//�����ȼ�
#define NET_ECG_HR_HI			102		//HR ��������
#define NET_ECG_HR_LOW		103		//HR ��������
#define NET_ECG_LEADTYPE		104		//��������
#define NET_ECG_MONTYPE		105		//�໤ģʽ
		
#define NET_SPO2_ALL			200		//����Ѫ��������Ϣ
#define NET_SPO2_ALMLEV		201		//�����ȼ�
#define NET_SPO2_HI				202		//spo2 ��������
#define NET_SPO2_LOW			203		//spo2 ��������
#define NET_SPO2_PR_HI			204		//PR ��������
#define NET_SPO2_PR_LOW		205		//PR ��������
	
#define NET_RESP_ALL			300		//���к�������
#define NET_RESP_ALMLEV		301		//�����ȼ�
#define NET_RESP_RR_HI			302		//RR ��������
#define NET_RESP_RR_LOW		303		//RR ��������
#define NET_RESP_APNEA			304		//RR ��������

#define NET_NIBP_ALL			400		//�����޴�Ѫѹ����
#define NET_NIBP_ALMLEV		401		//�����ȼ�
#define NET_NIBP_SYS_HI			402		//����ѹ����
#define NET_NIBP_SYS_LOW		403		//����ѹ����
#define NET_NIBP_MAP_HI			404		//ƽ��ѹ����
#define NET_NIBP_MAP_LOW		405		//ƽ��ѹ����
#define NET_NIBP_DIA_HI			406		//����ѹ����
#define NET_NIBP_DIA_LOW		407		//����ѹ����
#define NET_NIBP_INTERVAL		408		//��������

#define NET_TEMP1_ALL			500		//��������1����
#define NET_TEMP1_ALMLEV		501		//�����ȼ�
#define NET_TEMP1_HI			502		//temp1 ��������
#define NET_TEMP1_LOW			503		//temp1 ��������

#define NET_TEMP2_ALL			600		//��������1����
#define NET_TEMP2_ALMLEV		601		//�����ȼ�
#define NET_TEMP2_HI			602		//temp1 ��������
#define NET_TEMP2_LOW			603		//temp1 ��������

#define NET_CO2_ALL				700		//����co2����
#define NET_CO2_ALMLEV			701		//�����ȼ�
#define NET_CO2_ETCO2_HI		702		//ETCO2 ��������
#define NET_CO2_ETCO2_LOW		703		//ETCO2 ��������
#define NET_CO2_FICO2_HI		704		//FICO2 ��������
#define NET_CO2_FICO2_LOW		705		//FICO2 ��������
#define NET_CO2_AWRR_HI		706		//AWRR ��������
#define NET_CO2_AWRR_LOW		707		//AWRR ��������

#define NET_IBP1_ALL				800		//����IBP2����
#define NET_IBP1_ALMLEV			801		//�����ȼ�
#define NET_IBP1_SYS_HI			802		//����ѹ����
#define NET_IBP1_SYS_LOW		803		//����ѹ����
#define NET_IBP1_MAP_HI			804		//ƽ��ѹ����
#define NET_IBP1_MAP_LOW		805		//ƽ��ѹ����
#define NET_IBP1_DIA_HI			806		//����ѹ����
#define NET_IBP1_DIA_LOW		807		//����ѹ����

#define NET_IBP2_ALL				900		//����IBP2����
#define NET_IBP2_ALMLEV			901		//�����ȼ�
#define NET_IBP2_SYS_HI			902		//����ѹ����
#define NET_IBP2_SYS_LOW		903		//����ѹ����
#define NET_IBP2_MAP_HI			904		//ƽ��ѹ����
#define NET_IBP2_MAP_LOW		905		//ƽ��ѹ����
#define NET_IBP2_DIA_HI			906		//����ѹ����
#define NET_IBP2_DIA_LOW		907		//����ѹ����


















