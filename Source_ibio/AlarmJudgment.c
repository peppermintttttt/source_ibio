/*************************************************************************** 
 *	Module Name:	AlarmJudgment
 *
 *	Abstract:		�����ж�
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-03 09:50:53
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
//#include "arr.h"

//���峬�ޱ���״̬
#define NORMAL		0
#define LOWER		1
#define HIGHER		2
#define LOWEST		3	

//�������䱨����״̬
#define SENSOR_ON	0
#define SENSOR_OFF	1

//����������󱨾�״̬
#define MEASURE_OK	0
#define MEASURE_ERROR	1

//�����������״̬
#define POWER_LOWEST	BATTERY_EMPTY
#define POWER_NORMAL	BATTERY_Q1

//���������������ʾ֮�����ϵ����
extern BOOL gbAlarmNibp_Sys;
extern BOOL gbAlarmNibp_Dia;
extern BOOL gbAlarmNibp_Mean;
extern BOOL gbAlarmTemp_T1;
extern BOOL gbAlarmTemp_T2;
extern BOOL gbAlarmTemp_TD;
extern BOOL gbAlarmEcg_HR;
extern BOOL gbAlarmEcg_ST;
extern BOOL gbAlarmSpO2_Data;
extern BOOL gbAlarmPr_Data;
extern BOOL gbAlarmResp_Data;
extern BOOL gbAlarmCo2_EtCo2;
extern BOOL gbAlarmCo2_FiCo2;
extern BOOL gbAlarmIbp1_Sys;
extern BOOL gbAlarmIbp1_Dia;
extern BOOL gbAlarmIbp1_Mean;
extern BOOL gbAlarmIbp2_Sys;
extern BOOL gbAlarmIbp2_Dia;
extern BOOL gbAlarmIbp2_Mean;

//�Ƿ���뱨����ͣ״̬
extern BOOL gbSuspending;

//-------------------------------- �ж����±��� ------------------------------
//�ж����²�������
int IsAlm_T1(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wT1;
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff_T1 || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmTemp_T1 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_T1)){
			iAlmStatus = LOWER;
			gbAlarmTemp_T1 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_T1)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_T1 = TRUE;
		
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_T1 = FALSE;
		}
	}
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_T1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_T1_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_T1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_Temp1_High();	
// 					if(B_PRINTF) printf("Add High Alarm   --- T1.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_T1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_Temp1_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- T1.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_T2(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wT2;

// 	if(B_PRINTF) printf("T1 value is %d, High linit is %d, Low Limit is %d.\n", iValue, pCfg->wHigh_T1, pCfg->wLow_T1);
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff_T2 || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmTemp_T2 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_T2)){
			iAlmStatus = LOWER;
			gbAlarmTemp_T2 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_T2)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_T2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_T2 = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_T2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_T2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_T2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_Temp2_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- T2.\n");
				}break;                                     
				case LOWER:{
					sAlmInfo.bAlmID = AP_T2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_Temp2_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- T2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж����±���
int IsAlm_TD(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue1, iValue2, iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue1 = pValue->wT1;
	iValue2 = pValue->wT2;	
	iValue = abs(iValue1 - iValue2);
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff_T2 || pValue->bOff_T1){
		iAlmStatus = NORMAL;
		gbAlarmTemp_TD = FALSE;
	}
	else{
		if(iValue > (pCfg->wHigh_TD)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_TD = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_TD = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("TD value is %d, High linit is %d. iPreAlmStatus is %d, iAlmStatus is %d\n", iValue, pCfg->wHigh_TD, 
// 	      iPreAlmStatus, iAlmStatus);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_TD_HIGH;
		AlarmLifted_Phy(sAlmInfo);	

		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_TD_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_TD_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- TD.\n");
				}break;                                     
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж����¼�������
int IsOff_T1(const PS_TEMP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected_T1 = FALSE;
	//�õ���ǰ�ı���״̬
  	if(pValue->bOff_T1&& pValue->bConnected_T1){
//  	if(pValue->bOff_T1){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected_T1){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_TEMP_T1SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
// 		if(pValue->bConnected_T1)
// 			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Temp1_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_T2(const PS_TEMP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected_T2 = FALSE;
		
	//�õ���ǰ�ı���״̬
 	if(pValue->bOff_T2 && pValue->bConnected_T2){
// 	if(pValue->bOff_T2){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected_T2){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_TEMP_T2SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected_T2)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Temp2_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

//------------------------   
//�ж�SpO2��������
int IsAlm_SpO2(const PS_SPO2_VALUE pValue, const PS_SPO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->bSpO2;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmSpO2_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->bLow_SpO2)){
			iAlmStatus = LOWER;
			gbAlarmSpO2_Data = TRUE;
			
		}
		else if(iValue > (pCfg->bHigh_SpO2)){
			iAlmStatus = HIGHER;
			gbAlarmSpO2_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmSpO2_Data = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_SPO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_SPO2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_SPO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_SpO2_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- SpO2.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_SPO2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_SpO2_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- SpO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�SpO2��������
int IsAlmLowest_SpO2(const PS_SPO2_VALUE pValue, const PS_SPO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->bSpO2;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
	}
	else{
		if(iValue < (pCfg->bLowest_SpO2)){
			iAlmStatus = LOWEST;
		}
		else{
			iAlmStatus = NORMAL;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_SPO2_LOWEST;
		AlarmLifted_Phy(sAlmInfo);	
	
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case LOWEST:{
					sAlmInfo.bAlmID = AP_SPO2_LOWEST;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = ALARM_HIGH;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
// 					if(B_PRINTF) printf("Add Lowest Alarm   --- SpO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�SpO2�Ƿ�����
int IsOff_SpO2(const PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//�õ���ǰ�ı���״̬
 	if(pValue->bOff && pValue->bConnected){
// 	if(pValue->bOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_SPO2_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		
// 		if( pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_SpO2_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}
		
//�ж�SpO2�Ƿ���ֲ�������
int isErr_SpO2(const PS_SPO2_VALUE pValue)
{
	
	if(pValue == NULL) return -1;
	

	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	//��ǰ��״̬
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
		
	//�õ���ǰ�ı���״̬
	if(pValue->bOff || pValue->bErrCode ==0){
		iAlmStatus = MEASURE_OK;
	}
	else{//���ݴ�������ж��Ƿ���ֲ�������
		iAlmStatus = MEASURE_ERROR;	
	}
	
	if(gbHaveNellSpO2) iAlmStatus = MEASURE_OK;
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_SPO2_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		//���ݲ�ͬ�Ĳ������󣬶��屨������
		if((pValue->bErrCode) & SPO2_INTERFERE){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Interfere.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_SEARCHPULSE){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Search Pulse.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_HIGHLIGHT){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm High Light.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_WEAKSIGNAL){
			sAlmInfo.bClass = ALARM_LOW;	
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Weak Signal.\n", __FILE__, __LINE__);	
		}else if((pValue->bErrCode) & SPO2_SIGNALBAD){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Signal Bad.\n", __FILE__, __LINE__);
		}else{
			sAlmInfo.bClass = ALARM_INFO;		
		}

		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�NIBP��������
static int IsAlm_NSys(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wSys;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Sys = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_NSYS_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NSYS_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NSYS_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NSys_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NSYS_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NSys_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�NIBP��������
static int IsAlm_NDia(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wDia;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Dia = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_NDIA_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NDIA_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NDIA_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NDia_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NDIA_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_NDia_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�NIBP��������
static int IsAlm_NMean(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wMean;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Mean = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("NIBP Mean is %d. High is %d, Low is %d.\n", iValue, pCfg->wHigh_Mean, pCfg->wLow_Mean);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_NMEAN_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NMEAN_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NMEAN_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NMean_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NMEAN_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_NMean_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//���ı䱨��Դʱʹ��
static int ClearNibpAlarm()
{
	B_ALM_INFO sAlmInfo;
	
	//���ԭ�ȵ�״̬
	sAlmInfo.bAlmID = AP_NSYS_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NSYS_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Sys = FALSE;
	sAlmInfo.bAlmID = AP_NDIA_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NDIA_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Dia = FALSE;
	sAlmInfo.bAlmID = AP_NMEAN_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NMEAN_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Mean = FALSE;

	return 0;
}

/*
	�ж�NIBP�����Ƿ񱨾�
*/
int IsAlm_NIBP(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearNibpAlarm();
		bChanged = TRUE;
	}

	//���ݲ�ͬ�ı���Դ�����ж�               
	if(B_PRINTF) printf("%s:%d NIBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgNibp.bAlmSource);
	switch(gCfgNibp.bAlmSource){
		case NIBP_ALM_SRC_S:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_M:{
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_D:{
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_SM:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_MD:{
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_SD:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		default:{//NIBP_ALM_SRC_ALL
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
	}
	
	return 0;
}

extern BOOL gbNibpProtect;
//�ж�NIBP�Ƿ���ֲ�������
int isErr_Nibp(const PS_NIBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	//��ǰ��״̬
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
		
	//�õ���ǰ�ı���״̬
	if((pValue->bCanceled || pValue->wErrCode ==0) && (!gbNibpProtect)){
		iAlmStatus = MEASURE_OK;
	}
	else{//���ݴ�������ж��Ƿ���ֲ�������
		iAlmStatus = MEASURE_ERROR;	
	}
	
	if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X gbNibpProtect:%d.\n", __FILE__, __LINE__, pValue->wErrCode, gbNibpProtect);
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
		if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X   status:%d.\n", __FILE__, __LINE__, pValue->wErrCode, iAlmStatus);
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_NIBP_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		//���ݲ�ͬ�Ĳ������󣬶��屨������
		if((pValue->wErrCode) & NIBP_INFO_EXCESSIVE_MOTION){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_CUFF_MISTAKE){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_CUFF_TOOLAX){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_WEAK_SIGNAL){
			sAlmInfo.bClass = ALARM_LOW;	
		}else if((pValue->wErrCode) & NIBP_INFO_INTERFERENCE){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_OVER_RANGE){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_OVERPRESSURE){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_TIMEOUT){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_ERROR){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_SIGNAL_SATURATION){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_MODULE_FAULT){
			sAlmInfo.bClass = ALARM_HIGH;		
		}
		else{
			sAlmInfo.bClass = ALARM_INFO;		
		}

		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�NIBP�Ƿ���ֲ�������
int isErr_iBioNibp(const PS_NIBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	//��ǰ��״̬
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;

	switch(pValue->wFDB){
		case NBP_FDB_MAN_START_ABT:
		case NBP_FDB_AUTO_START_ABT:
		case NBP_FDB_STAT_START_ABT:
		case NBP_FDB_STC_START_ABT:
		case NBP_FDB_LEAK_START_ABT:
		case NBP_FDB_PVT_START_ABT:
		case NBP_FDB_OVP_START_ABT:
		case NBP_FDB_CAL_START_ABT:
		break;	
			
	}
		
	//�õ���ǰ�ı���״̬
	if((pValue->bCanceled || pValue->wFDB==0) && (!gbNibpProtect)){
		iAlmStatus = MEASURE_OK;
	}
	else{//���ݴ�������ж��Ƿ���ֲ�������
		iAlmStatus = MEASURE_ERROR;	
	}
	
	//if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X gbNibpProtect:%d.\n", __FILE__, __LINE__, pValue->wErrCode, gbNibpProtect);
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
//		if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X   status:%d.\n", __FILE__, __LINE__, pValue->wErrCode, iAlmStatus);
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_NIBP_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		
		
		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}
//�ж�PR�Ƿ񱨾�
int IsAlm_PR(const PS_PULSE_VALUE pValue, const PS_PULSE_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wPR;
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmPr_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_PR)){
			iAlmStatus = LOWER;
			gbAlarmPr_Data = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_PR)){
			iAlmStatus = HIGHER;
			gbAlarmPr_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmPr_Data = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_PR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_PR_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_PR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_PR_High();
// 					if(B_PRINTF) printf("----------------------Add High Alarm   --- PR.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_PR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_PR_Low();
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�ECG���������Ƿ񱨾�
int IsAlm_HR(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wHR;

// 	if(B_PRINTF) printf("-----------------HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//�жϲ���ֵ�Ƿ�Խ��
  	if(pValue->bRAOff || iValue<=0){
//	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_HR = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_HR)){
			iAlmStatus = LOWER;
			gbAlarmEcg_HR = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_HR)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_HR = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_HR = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_HR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_HR_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_HR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_HR_High();
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_HR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_HR_Low();	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�жϺ�����������
int IsAlm_RR(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wRR;
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff || gCfgResp.bSwitch == SWITCH_OFF){
		iAlmStatus = NORMAL;
		gbAlarmResp_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_RR)){
			iAlmStatus = LOWER;
			gbAlarmResp_Data = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_RR)){
			iAlmStatus = HIGHER;
			gbAlarmResp_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmResp_Data = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_RR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_RR_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_RR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_RR_High();
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_RR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_RR_Low();
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


//�ж�ECG���䱨��
int IsOff_LA(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//�õ���ǰ�ı���״̬
 	if(pValue->bLAOff && pValue->bConnected){
// 	if(pValue->bLAOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_LA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_LA_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- LA Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- LA Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_LL(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//�õ���ǰ�ı���״̬
 	if(pValue->bLLOff && pValue->bConnected){
// 	if(pValue->bLLOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_LL;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_LL_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- LL Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- LL Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_RA(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//�õ���ǰ�ı���״̬
 	if(pValue->bRAOff && pValue->bConnected){
// 	if(pValue->bRAOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_RA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_RA_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- RA Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- RA Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_V(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//�õ���ǰ�ı���״̬
 	if(pValue->bVOff && pValue->bConnected){
// 	if(pValue->bVOff ){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_V;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_V_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- V Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- V Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

extern BOOL gbApneaTimeChanged;
//�ж���Ϣ����,��1s�Ķ�ʱ������
int IsAlm_Apnea(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	//��Ϣ��ʱ��
	static int iTimerCount = 0;
	
	iValue = pValue->wRR;
//   	if(B_PRINTF) printf("(%s:%d) bOff:%d,,bchanged:%d,,apneatime=%d,,switch=%d.\n", __FILE__, __LINE__,pValue->bOff,bChanged,gCfgResp.wApneaTime,gCfgResp.bSwitch );
	//�����������,���߸ı�ʱ��, ���߹ر���Ϣ����(ʱ��Ϊ0),��λ��ʱ��,�����ǰ�Ǳ���״̬,��ȡ������״̬
	if(pValue->bOff 
		  || bChanged 
		  || gCfgResp.bApnea ==0 
		  || gCfgResp.bSwitch == SWITCH_OFF){

		if(bChanged) gbApneaTimeChanged = FALSE;
		
		iTimerCount = 0;
		if(iPreAlmStatus != NORMAL){
			sAlmInfo.bAlmID = AP_APNEA;
			AlarmLifted_Phy(sAlmInfo);	
			iPreAlmStatus = NORMAL;
// 			if(B_PRINTF) printf("%s:%d Lifted Apnea.\n", __FILE__, __LINE__);
		}
	}
	else if(iValue>0){
		//��������ʲ�Ϊ0,��λ��ʱ��,�����ǰ�Ǳ���״̬,��ȡ������״̬
		iTimerCount = 0;
		if(iPreAlmStatus != NORMAL){
			sAlmInfo.bAlmID = AP_APNEA;
			AlarmLifted_Phy(sAlmInfo);	
			iPreAlmStatus = NORMAL;
// 			if(B_PRINTF) printf("%s:%d Lifted Apnea.\n", __FILE__, __LINE__);
		}
	}
	else{
 //		if(B_PRINTF) printf("%s:%d Debug Apnea iTimerCount:%d,,%d.\n", __FILE__, __LINE__, iTimerCount,,gValueResp.bApneaAlm);
		//����δ����, �Һ�����Ϊ0�����
		//��ʱ����ʱ,�����ǰ���Ǳ���״̬,����ӱ���״̬
		if(iTimerCount >=gCfgResp.wApneaTime&&gValueResp.bApneaAlm!=0){
			if(iPreAlmStatus == NORMAL){
				sAlmInfo.bAlmID = AP_APNEA;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				//TODO:�˴�����Ҫ����������Ϣ�����ļ���
				sAlmInfo.bClass = pCfg->bAlmControl;
				sAlmInfo.bPrint = pCfg->bPrnControl;
				AddAlarm_Phy(sAlmInfo);	
 //				if(B_PRINTF) printf("%s:%d Add Apnea.\n", __FILE__, __LINE__);
				iPreAlmStatus = LOWEST;
				NetSend_Alm_RR_Apnea();
			}
		}
		else{
			iTimerCount =(iTimerCount +1)%100;
		}
	}
	
	return 0;
}
//�ж��Ƿ�CVA����,���ĵ��źŵ����ں������ϵĸ��ţ�1:���ĵ����
int IsAlm_CVA(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	//��Ϣ��ʱ��
	static int iTimerCount = 0;
	
	iValue = pValue->bCVAAlm;
//CVA����ʱ������Ϊ��Чֵ��-100��
	if(!pValue->bOff&&pValue->bInval&&iValue>0){
		iAlmStatus = LOWER;
	}else{
		iAlmStatus = NORMAL;
	}

	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_CVA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_MID;
		if(iAlmStatus == LOWER){
			//Add	
			AddAlarm_Tec(sAlmInfo);
 			if(B_PRINTF) printf("Add Tec Alarm   --- RESP CVA.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
 			if(B_PRINTF) printf("Lift Tec Alarm   --- LA Off.\n");
		}
	
	}
	
	iPreAlmStatus = iAlmStatus;

	return 0;
}


//�жϵ͵�����������
int IsLow_Power(const PPOWER_STATUS pValue)
{
	//ǰһ�εı���״̬
	static int iPreAlmStatus = POWER_LOWEST;
	//��ǰ��״̬
	int iAlmStatus=POWER_LOWEST;
	B_ALM_INFO sAlmInfo;
	
	//�õ���ǰ�ı���״̬
	//�������ʹ�ý����磬����Ե͵�������
	if(pValue->bBatteryStatus == BATTERY_EMPTY && pValue->bPowerType == POWER_DC){
		iAlmStatus = POWER_LOWEST;
	}
	else{
		iAlmStatus = POWER_NORMAL;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_BATTERY_LOW;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_HIGH;
		if(iAlmStatus == POWER_LOWEST){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Battery_Low();
// 			if(B_PRINTF) printf("%s:%d Add Power Lowest Alarm.\n", __FILE__, __LINE__);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("%s:%d Lift Power Lowest Alarm.\n", __FILE__, __LINE__);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsAlm_EtCO2(const PS_CO2_VALUE pValue, const PS_CO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wEtCO2;
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmCo2_EtCo2 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_EtCO2)){
			iAlmStatus = LOWER;
			gbAlarmCo2_EtCo2 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_EtCO2)){
			iAlmStatus = HIGHER;
			gbAlarmCo2_EtCo2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmCo2_EtCo2 = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ETCO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ETCO2_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ETCO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_EtCO2_High();
					if(B_PRINTF) printf("Add High Alarm   --- EtCO2.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ETCO2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_EtCO2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- EtCO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_FiCO2(const PS_CO2_VALUE pValue, const PS_CO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wFiCO2;
	
	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmCo2_FiCo2 = FALSE;
	}
	else{
		if(iValue > (pCfg->wHigh_FiCO2)){
			iAlmStatus = HIGHER;
			gbAlarmCo2_FiCo2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmCo2_FiCo2 = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_FICO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	

		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_FICO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_FiCO2_High();
 					if(B_PRINTF) printf("Add High Alarm   --- FiCO2.\n");
				}break;                                     
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//�ж�IBP��������
static int IsAlm_ISys1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iSys;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Sys = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ISYS1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ISYS1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ISYS1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_ISys1_High();
 					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ISYS1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_ISys1_Low();
 					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IDia1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iDia;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Dia = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_IDIA1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IDIA1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IDIA1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IDia1_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IDIA1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IDia1_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IMean1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iMean;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Mean = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IMEAN1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IMean1_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IMEAN1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IMean1_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

//���ı䱨��Դʱʹ��
static int ClearIbp1Alarm()
{
	B_ALM_INFO sAlmInfo;
	
	//���ԭ�ȵ�״̬
	sAlmInfo.bAlmID = AP_ISYS1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_ISYS1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Sys = FALSE;
	sAlmInfo.bAlmID = AP_IDIA1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IDIA1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Dia = FALSE;
	sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IMEAN1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Mean = FALSE;

	return 0;
}

/*
	�ж�IBP1�����Ƿ񱨾�
*/
int IsAlm_IBP1(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearIbp1Alarm();
		bChanged = TRUE;
	}

	//���ݲ�ͬ�ı���Դ�����ж�               
// 	if(B_PRINTF) printf("%s:%d IBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgIbp1.bAlmSource);
	switch(gCfgIbp1.bAlmSource){
		case IBP_ALM_SRC_S:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_M:{
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_D:{
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_SM:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_MD:{
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_SD:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		default:{//IBP_ALM_SRC_ALL
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
	}
	
	return 0;
}

//�ж�IBP��������
static int IsAlm_ISys2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iSys;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Sys = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ISYS2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ISYS2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ISYS2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_ISys2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ISYS2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_ISys2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IDia2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iDia;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Dia = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_IDIA2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IDIA2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IDIA2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IDia2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IDIA2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IDia2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IMean2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iMean;

	//�жϲ���ֵ�Ƿ�Խ��
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Mean = FALSE;
		}
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IMEAN2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
			
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IMean2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IMEAN2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IMean2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

//���ı䱨��Դʱʹ��
static int ClearIbp2Alarm()
{
	B_ALM_INFO sAlmInfo;
	
	//���ԭ�ȵ�״̬
	sAlmInfo.bAlmID = AP_ISYS2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_ISYS2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Sys = FALSE;
	sAlmInfo.bAlmID = AP_IDIA2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IDIA2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Dia = FALSE;
	sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IMEAN2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Mean = FALSE;

	return 0;
}

/*
	�ж�IBP2�����Ƿ񱨾�
*/
int IsAlm_IBP2(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearIbp2Alarm();
		bChanged = TRUE;
	}

	//���ݲ�ͬ�ı���Դ�����ж�               
// 	if(B_PRINTF) printf("%s:%d IBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgIbp1.bAlmSource);
	switch(gCfgIbp2.bAlmSource){
		case IBP_ALM_SRC_S:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_M:{
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_D:{
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_SM:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_MD:{
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_SD:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		default:{//IBP_ALM_SRC_ALL
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
	}

	return 0;
}

int IsOff_IBP1(const PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//�õ���ǰ�ı���״̬
	if(pValue->bOff && pValue->bConnected){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_IBP1_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_IBP2(const PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = SENSOR_OFF;
	//��ǰ��״̬
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//���������ͣ����ȡ�����䱨��
	if(gbSuspending) pValue->bConnected = FALSE;

	//�õ���ǰ�ı���״̬
	if(pValue->bOff && pValue->bConnected){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//����ǰ��״̬�Ƚ�,�����ͬ,��ֱ�ӷ���,
	//������� or �������
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_IBP2_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}



//�ж�ECG���������Ƿ񱨾�
int IsAlm_ST1(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iST1;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//�жϲ���ֵ�Ƿ�Խ��
// 	if(pValue->bRAOff || iValue ==0){
	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_ST=FALSE;
	}
	else{
		if(iValue < (pCfg->iSt1_Low)){
			iAlmStatus = LOWER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_Low();	
		}
		else if(iValue > (pCfg->iSt1_High)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_High();
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_ST=FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ST1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ST1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ST1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ST1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


int IsAlm_ST2(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iST2;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//�жϲ���ֵ�Ƿ�Խ��
// 	if(pValue->bRAOff || iValue ==0){
	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_ST=FALSE;
	}
	else{
		if(iValue < (pCfg->iSt2_Low)){
			iAlmStatus = LOWER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_Low();	
		}
		else if(iValue > (pCfg->iSt2_High)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_High();
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_ST=FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ST2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ST2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ST2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ST2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_Arr(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = NORMAL,iold_arr_value;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->lARR;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//�жϲ���ֵ�Ƿ�Խ��
 	if(iValue ==0||iValue ==0x8000000||(gCfgEcg.bArrAnalyse==0)){
//	if(pValue->bRAOff ){
		iAlmStatus = NORMAL;
	}
	else{
		if(iValue > 0){
			iAlmStatus = HIGHER;
		}	
		else{
			iAlmStatus = NORMAL;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//���Ƚ��ԭ�ȵ�״̬
		sAlmInfo.bAlmID = AP_ARR;
		AlarmLifted_Phy(sAlmInfo);	
		
		//�����µ�״̬
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ARR;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//ϵͳ״̬�������ѹ3.3v �Լ�5.5v�Ƿ�����
int IsAlm_Power(const unsigned char *pValue,BOOL bChanged)
{

	if(pValue == NULL ) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	int abnormal,force,comm;
	B_ALM_INFO sAlmInfo;

	abnormal = *pValue&0x01<<4;
	force = *pValue&0x01<<3;
	comm = *pValue&0x01<<2;

	if(*pValue&0x07<<2){
	//	if(B_PRINTF)printf("1=============\n");
		iAlmStatus = MEASURE_ERROR;
	}
	else{
	//	if(B_PRINTF)printf("2~~~~~~~~~~~~~~\n");
		iAlmStatus = MEASURE_OK;
	}
	
	if(gbSuspending){
		if(abnormal){
	//		if(B_PRINTF)printf("11111111111111\n");
			sAlmInfo.bAlmID = AT_POWER_ABNORMAL;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(3);
			
		}
		if(force){
	//		if(B_PRINTF)printf("2222222222222\n");
			sAlmInfo.bAlmID = AT_POWER_FORCE;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(2);
		}
		if(comm){
		//	if(B_PRINTF)printf("33333333\n");
			sAlmInfo.bAlmID = AT_POWER_COMM;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(1);
		}
		return 0;
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
//	if(B_PRINTF)	printf("-----return \n");
		return -1;
	}
	else{
		if(abnormal){
			sAlmInfo.bAlmID = AT_POWER_ABNORMAL;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		if(force){
			sAlmInfo.bAlmID = AT_POWER_FORCE;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		if(comm){
			sAlmInfo.bAlmID = AT_POWER_COMM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}
//ϵͳ״̬�������ѹ3.3v �Լ�5.5v�Ƿ�����
int IsAlm_Voltage(const PSYS_STATUS pValue,BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	int iValue33,iValue5;
	B_ALM_INFO sAlmInfo;
	
	iValue33 = pValue->bState33V;
	iValue5 = pValue->bState5V;

	if(pValue->bState33V>0||pValue->bState5V>0){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		switch(iValue33){
			case 0:
				sAlmInfo.bAlmID = AT_BOARD33V_HIGH;
				AlarmLifted_Tec(sAlmInfo);
				sAlmInfo.bAlmID = AT_BOARD33V_LOW;
				AlarmLifted_Tec(sAlmInfo);
			break;
			case 1:
				sAlmInfo.bAlmID = AT_BOARD33V_HIGH;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			case 2:
				sAlmInfo.bAlmID = AT_BOARD33V_LOW;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			default:
			break;
			}
			switch(iValue5){
			case 0:
				sAlmInfo.bAlmID = AT_BOARD5V_HIGH;
				AlarmLifted_Tec(sAlmInfo);
				sAlmInfo.bAlmID = AT_BOARD5V_LOW;
				AlarmLifted_Tec(sAlmInfo);
			break;
			case 1:
				sAlmInfo.bAlmID = AT_BOARD5V_HIGH;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			case 2:
				sAlmInfo.bAlmID = AT_BOARD5V_LOW;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			default:
			break;
			}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//ϵͳ�Լ�������
int IsAlm_SelfTest(const PSYS_STATUS pValue,BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
	

	if(pValue->bSelWatchdog>0||pValue->bSelAD>0||pValue->bSelRAM>0
		||pValue->bSelROM>0||pValue->bSelCPU>0||pValue->bSelIBP>0
		||pValue->bSelNBP>0||pValue->bSelSPO2>0||pValue->bSelTEMP>0
		||pValue->bSelRESP>0||pValue->bSelECG>0){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//watchdog
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_WATCHDOG;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_WATCHDOG;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//AD
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_AD;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_AD;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//RAM
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_RAM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_RAM;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ROM
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_ROM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_ROM;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//CPU
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_CPU;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_CPU;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//IBP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_IBP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_IBP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//NIBP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_NBP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_NBP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//SPO2
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_SPO2;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_SPO2;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//TEMP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_TEMP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_TEMP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//RESP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_RESP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_RESP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ECG
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_ECG;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_ECG;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


//Ecgͨ�����ر���
int IsAlm_ECGOverload(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//ǰһ�εı���״̬
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
	

	if(pValue->bOverLoad1||pValue->bOverLoad2||pValue->bOverLoad3){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//�����֮ǰ��״̬��ͬ����ֱ�ӷ��أ�������ݲ�ͬ״̬�ж�
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//ͨ��1����
		if(pValue->bOverLoad1){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD1;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD1;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ͨ��2����
		if(pValue->bOverLoad2){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD2;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD2;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ͨ��3����
		if(pValue->bOverLoad3){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD3;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD3;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


