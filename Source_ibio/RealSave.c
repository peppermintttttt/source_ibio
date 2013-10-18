/*************************************************************************** 
 *	Module Name:	RealSave
 *
 *	Abstract:	ʵʱ�洢
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-08-03 10:09:15
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Global.h"

/*
	ʵʱ�洢���κͲ���,������ط�,�ͱ����洢�ȹ���ʹ��
	˼·: �ڴ��ڽ��ճ�����,ʵʱ�����������,��200��(1����,���ݲ����ʶ�)ʱ, д�뵱ʱ�Ĳ�����Ϣ,Ȼ�����һ��ʵʱ�洢����Ϣ,
*/

/*
	�����洢
	˼·: ��������ʱ, �����洢�߳�, Ȼ���ʵʱ������ȡ��ǰx����������뱨��������, ��ʼ��ʱ, �洢����x������ݺ�, �˳��߳�
	
	˼·2��2008-04-24 16:29:58
		����ʼʱ����һ��32��Ļ���������ʵʱ�洢���ݣ�
		����������ʱ�����ʱ�̣�Ȼ������洢16��󣬼��뱨��ʱ������
*/

//ʵʱ��������
CA_REALSAVEARRAY 	gRealDataArray;
//����ʵʱ����
CA_REALALMSAVEARRAY	gRealAlmDataArray;
//����ʵʱ����
REALSAVEDATA		gRealData;

//ʵʱECG����
CA_REALECGARRAY 	gRealEcgArray;
//����ʵʱECG����
REALECGDATA		gRealEcg;

//�������ݴ洢
CA_ALMDATAARRAY	gAlmDataArray;

//�洢�������ݵ��߳�
static pthread_t ptSaveAlarmData;
//�洢�������ݵĻ�����
pthread_mutex_t mtSaveAlarmData;                                             

static BOOL bDebug = TRUE;
//static BOOL bDebug = TRUE;

//��ӡ�����ź���
extern sem_t semReal_Print;
extern sem_t semCalctime;
/**
	���һ�����ݵ�����,��HeadIndex��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
 */
static int RealArray_Add(const PREALSAVEDATA pData, BOOL bFirst)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	
	if(pData == NULL) return -1;
	
	//����״�\�������, �������ʼʱ��
	if(bFirst){
		GetSysTime(&sTime);
		gRealDataArray.sStartTime = sTime;
	}
	
	wHead = gRealDataArray.wHeadIndex;
	wTail = gRealDataArray.wTailIndex;
	bLoop = gRealDataArray.bLoop;
	wSize = MAXREALSAVECOUNT;
	
	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//����Ƿ�ת�����
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//�����Ѿ�����,��ת
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	
	gRealDataArray.wHeadIndex = wHead;
	gRealDataArray.wTailIndex = wTail;
	gRealDataArray.bLoop = bLoop;
	gRealDataArray.Datas[gRealDataArray.wHeadIndex] = *pData;
	//����ʵ�ʴ洢������
	if(gRealDataArray.wHeadIndex < gRealDataArray.wTailIndex){
		//�����Ѿ�����,����ת
		gRealDataArray.wCount = MAXREALSAVECOUNT;
	}
	else{
		gRealDataArray.wCount +=1;
	}

	return wHead;
}



static int RealAlmArray_Add(const PREALSAVEDATA pData, BOOL bFirst)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	
	if(pData == NULL) return -1;
	
	//����״�\�������, �������ʼʱ��
	if(bFirst){
		GetSysTime(&sTime);
		gRealAlmDataArray.sStartTime = sTime;
	}
	
	wHead = gRealAlmDataArray.wHeadIndex;
	wTail = gRealAlmDataArray.wTailIndex;
	bLoop = gRealAlmDataArray.bLoop;
	wSize = MAXREALALARMSAVECOUNT;
	
	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//����Ƿ�ת�����
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//�����Ѿ�����,��ת
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	
	gRealAlmDataArray.wHeadIndex = wHead;
	gRealAlmDataArray.wTailIndex = wTail;
	gRealAlmDataArray.bLoop = bLoop;
	gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex] = *pData;
	//����ʵ�ʴ洢������
	if(gRealAlmDataArray.wHeadIndex < gRealAlmDataArray.wTailIndex){
		//�����Ѿ�����,����ת
		gRealAlmDataArray.wCount = MAXREALALARMSAVECOUNT;
	}
	else{
		gRealAlmDataArray.wCount +=1;
	}

	return wHead;
}


//----------------------------- interface -------------------------
/**
	��䵥��ʵʱ����, ���ǵ�Resp Spo2�Լ��������β�ͬ��
	��ECG�Ĳ�����Ϊ��׼�����д洢
*/
int FillOneRealSaveData()
{
	static int iCount = 199;
	//��¼ʱ��
	S_TIME sTime;
	//ʱ��ƫ����
	long int liTimeOffset=0;
	static BOOL bFirst = TRUE;			//�Ƿ��״���Ӽ�¼
	int i;
	
	if(iCount >=0 ){
		//������Ӳ���
		//Ecg
		if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){
			gRealData.waves[iCount].bEcg1 = gValueEcg.iWaves[ECGWAVE_I];
			gRealData.waves[iCount].bEcg2 = gValueEcg.iWaves[ECGWAVE_II];
			gRealData.waves[iCount].bEcg3 = gValueEcg.iWaves[ECGWAVE_V];
		}
		else{
			gRealData.waves[iCount].bEcg1 = gValueEcg.iWaves[ECGWAVE_I];
			gRealData.waves[iCount].bEcg2 = gValueEcg.iWaves[ECGWAVE_II];
			gRealData.waves[iCount].bEcg3 = gValueEcg.iWaves[ECGWAVE_III];
		}	
		
		//SpO2 
		gRealData.waves[iCount].bSpO2= gValueSpO2.bWave;
		
		//Resp
		gRealData.waves[iCount].bResp = gValueResp.iWave;	
		
		//CO2
		if(gbHaveCo2){
			gRealData.waves[iCount].bCO2 = gValueCO2.wWave;	
		}
		else{
			gRealData.waves[iCount].bCO2 = 0;	
		}
		if(gbHaveIbp){
			gRealData.waves[iCount].iIbp1= gValueIbp1.iWave;	
			gRealData.waves[iCount].iIbp2= gValueIbp2.iWave;	
		}
		else{
			gRealData.waves[iCount].iIbp1= 0;	
			gRealData.waves[iCount].iIbp2= 0;	
		}
		
		iCount --;
	}
	
	if(iCount <0){
		iCount = 199;
		//��ȡ�洢ʱ��
		GetSysTime(&sTime);
		//����˴μ�¼��ʱ������ʼʱ���ƫ����, ��ȷ����
		liTimeOffset = CompTime(&sTime, &(gRealDataArray.sStartTime));
		gRealData.paras.liOffset = liTimeOffset;
		gRealData.paras.sTime = sTime;
		
		//��¼ԭ��
		gRealData.paras.bLogFlag = TRENDMARK;
	 	//������Ч��־
		gRealData.paras.dwHaveFlag = 0;
		
		//��Ӳ�����Ϣ
		if(gValueEcg.wHR !=0){
			gRealData.paras.wHR = gValueEcg.wHR; 
			gRealData.paras.dwHaveFlag |= HAVEHR;
		}
		if(!gValueResp.bOff){
			gRealData.paras.wRR = gValueResp.wRR;
			gRealData.paras.dwHaveFlag |= HAVERESP;
		}
		//if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
		if(gValueSpO2.bSpO2 !=0 ){
			gRealData.paras.bSpO2 = gValueSpO2.bSpO2;
			gRealData.paras.dwHaveFlag |= HAVESPO2;
		}
		//if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
		if(gValuePulse.wPR !=0){
			gRealData.paras.wPR = gValuePulse.wPR;	
			gRealData.paras.dwHaveFlag |= HAVEPR;
		}
		//Temp1
		if(!gValueTemp.bOff_T1){
			gRealData.paras.wTemps[0] = gValueTemp.wT1;
			gRealData.paras.dwHaveFlag |= HAVET1;
		}	
		//Temp2
		if(!gValueTemp.bOff_T2){
			gRealData.paras.wTemps[1] = gValueTemp.wT2;
			gRealData.paras.dwHaveFlag |= HAVET2;
		}	
		//NIBP
		if(gValueNibp.wSys !=0 && gValueNibp.wDia !=0 && gValueNibp.wMean !=0){
			gRealData.paras.sNIBP.wSys = gValueNibp.wSys;
			gRealData.paras.sNIBP.wDia = gValueNibp.wDia;
			gRealData.paras.sNIBP.wMean = gValueNibp.wMean;
			gRealData.paras.dwHaveFlag |= HAVENIBP;
		}
		//CO2
		if(gbHaveCo2 
			&& gValueCO2.wEtCO2 !=0 
			 && gValueCO2.bStatus ==CO2_STATUS_RUN){
			
			gRealData.paras.sCO2.wEtCO2 = gValueCO2.wEtCO2;
			gRealData.paras.sCO2.wFiCO2 = gValueCO2.wFiCO2;
			gRealData.paras.dwHaveFlag |= HAVECO2;
		}
		//IBP1
		if(gbHaveIbp && !gValueIbp1.bOff){
			gRealData.paras.sIBPs[0].iSys = gValueIbp1.iSys;
			gRealData.paras.sIBPs[0].iDia = gValueIbp1.iDia;
			gRealData.paras.sIBPs[0].iMean = gValueIbp1.iMean;
			gRealData.paras.dwHaveFlag |= HAVEIBP1;
		}
		//IBP2
		if(gbHaveIbp && !gValueIbp2.bOff){
			gRealData.paras.sIBPs[1].iSys = gValueIbp2.iSys;
			gRealData.paras.sIBPs[1].iDia = gValueIbp2.iDia;
			gRealData.paras.sIBPs[1].iMean = gValueIbp2.iMean;
			gRealData.paras.dwHaveFlag |= HAVEIBP2;
		}
	
		//����¼��ӵ�������
		RealArray_Add(&gRealData, bFirst);
		RealAlmArray_Add(&gRealData, bFirst);
		bFirst = FALSE;
		
		
//  		printf("%s:%d RealSave. Total %d records. Head %d, Tail %d. Time:%.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
//  		       gRealDataArray.wCount, gRealDataArray.wHeadIndex, gRealDataArray.wTailIndex, 
//  		       gRealData.paras.sTime.bHour, gRealData.paras.sTime.bMin, gRealData.paras.sTime.bSec);
		       
// 		       gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras.sTime.bMin,
// 		       gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras.sTime.bSec);
	}
	
	return 0;
}

//-------------------------------  Alarm data Save Thread --------------------------------
/**
	���һ�����ݵ�����,��HeadIndex��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
 */
static int AlarmDataArray_Add(const PALARMDATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gAlmDataArray.wHeadIndex;
	wTail = gAlmDataArray.wTailIndex;
	bLoop = gAlmDataArray.bLoop;
	wSize = MAXALARMDATACOUNT;
	
	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//����Ƿ�ת�����
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//�����Ѿ�����,��ת
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	
	gAlmDataArray.wHeadIndex = wHead;
	gAlmDataArray.wTailIndex = wTail;
	gAlmDataArray.bLoop = bLoop;
	gAlmDataArray.Datas[gAlmDataArray.wHeadIndex] = *pData;
	
	if(B_PRINTF)printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, pData->bAlmID, pData->bAlmClass, 
						pData->paras.sTime.bHour, pData->paras.sTime.bMin, pData->paras.sTime.bSec, 
						pData->paras.bSpO2); 
						
	//����ʵ�ʴ洢������
	if(gAlmDataArray.wHeadIndex < gAlmDataArray.wTailIndex){
		//�����Ѿ�����,����ת
		gAlmDataArray.wCount = MAXALARMDATACOUNT;
	}
	else{
		gAlmDataArray.wCount +=1;
	}
	
	if(B_PRINTF)printf("Add one Alarm Data ID is %d, Total Count is %d.\n ", gAlmDataArray.Datas[wHead].bAlmID, gAlmDataArray.wCount);
	
	return wHead;
}
//������ӡ
int Print_Alm_Data()
{
	Printer_PrintAlarmData(&gPrintAlarmData);
	return 0;
}	

//���������������ͣ16�룬Ȼ��ȡ����ʱ��  gRealAlmDataArray ���ݣ����뱨���¼�����
//������������,��������߳����棬����ͬʱ���洢�̣߳���Ҫ���߳̽���֮���ڿ����´δ洢�߳�
static ALARMDATA sAlmData;
static BOOL FinishedSaveAlarmData = TRUE;
static void *NewThreadProcSaveAlarmData(void *arg)
{

	//ALARMDATA sAlmData;
	B_ALM_INFO sAlarm = *(PB_ALM_INFO)arg;

	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
	int iCount_Print;
	
	int iTimeCount = 0;
	int iTimeCount_Print = 0;
	BOOL bPrint = FALSE;	//�Ƿ��Ѿ���ӡ
	BOOL bSaveParas = FALSE;	//�Ƿ��Ѿ��洢������Ϣ
	
	FinishedSaveAlarmData = FALSE;
	if(sAlarm.bClass == ALARM_OFF || sAlarm.bClass >=ALARM_INFO){
		if(!B_PRINTF)printf("%s:%d Alarm is turned off. Stopped saving alarm data.\n ", __FILE__, __LINE__);
		FinishedSaveAlarmData = TRUE;
		pthread_exit("");
	}
		
	//��ʼ����������
	memset(&sAlmData, 0, sizeof sAlmData);
	memset(&gPrintAlarmData, 0, sizeof gPrintAlarmData);
printf("adfafadfadsf====gRealAlmDataArray.wCount=%d\n",gRealAlmDataArray.wCount);		
	while(gRealAlmDataArray.wCount == 0);

	//��¼��ʱ�ı��������Լ�������Ϣ
	sAlmData.bAlmType = ALARM_PHYSIOLOGICAL;		//������
	sAlmData.bAlmID = sAlarm.bAlmID;
	sAlmData.bAlmClass = sAlarm.bClass;
// 	//��ò�������
// 	sAlmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
// 	//��õ�ǰʱ��
// 	GetSysTime(&(sAlmData.paras.sTime));
	//��ȡ�������ݵĲ�����Ϣ
	gPrintAlarmData.bAlmType = ALARM_PHYSIOLOGICAL;		//������
	gPrintAlarmData.bAlmID = sAlarm.bAlmID;
	gPrintAlarmData.bAlmClass = sAlarm.bClass;
// 	gPrintAlarmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
// 	GetSysTime(&(gPrintAlarmData.paras.sTime));
	for(;;){
		//�ȴ�16��(MAXALARMWAVELENGTH/2)��Ȼ��洢
		if(iTimeCount < MAXALARMWAVELENGTH/2){
			sleep(1);
			iTimeCount ++;
			iTimeCount_Print ++;
			
			//�洢һ�β�������,֮�����ӳ�����Ϊ�洢��ʵ�ʵ�ֵ��ʱ���ϵ��ӳ�
			if((!bSaveParas)  && (iTimeCount==2)){
				bSaveParas = TRUE;
				//ʱ��
				GetSysTime(&(sAlmData.paras.sTime));
				
				sAlmData.paras = gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
			
				sAlmData.paras.wHR = gValueEcg.wHR;
				sAlmData.paras.wPR = gValuePulse.wPR;
				sAlmData.paras.wRR = gValueResp.wRR;
				sAlmData.paras.iSTs[0] = gValueEcg.iST1;
				sAlmData.paras.iSTs[1] = gValueEcg.iST2;
				sAlmData.paras.bSpO2 = gValueSpO2.bSpO2;
				sAlmData.paras.sNIBP.wSys = gValueNibp.wSys;
				sAlmData.paras.sNIBP.wMean= gValueNibp.wMean;
				sAlmData.paras.sNIBP.wDia= gValueNibp.wDia;
				sAlmData.paras.sIBPs[0].iSys = gValueIbp1.iSys;
				sAlmData.paras.sIBPs[0].iMean = gValueIbp1.iMean;
				sAlmData.paras.sIBPs[0].iDia = gValueIbp1.iDia;
				sAlmData.paras.sIBPs[1].iSys = gValueIbp2.iSys;
				sAlmData.paras.sIBPs[1].iMean = gValueIbp2.iMean;
				sAlmData.paras.sIBPs[1].iDia = gValueIbp2.iDia;
				sAlmData.paras.wTemps[0] = gValueTemp.wT1;
				sAlmData.paras.wTemps[1] = gValueTemp.wT2;
				sAlmData.paras.sCO2.wEtCO2   = gValueCO2.wEtCO2;
				sAlmData.paras.sCO2.wFiCO2   = gValueCO2.wFiCO2;
				sAlmData.paras.sCO2.wAwRR= gValueCO2.wAwRR;

				if(!B_PRINTF)printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d. HR is %d    REALARRY: %d   %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
						sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
						sAlmData.paras.bSpO2, sAlmData.paras.wHR, 
						 gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras.bSpO2,
						 gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras.wHR); 
				//gPrintAlarmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
				gPrintAlarmData.paras = sAlmData.paras;
				//GetSysTime(&(gPrintAlarmData.paras.sTime));
			}

			if(bDebug) printf("Alarm Time count :%d.\n", iTimeCount);
			//�ȴ�x��(x<16)��ȡ�����Σ���ʼ��ӡ
			if(iTimeCount_Print < (gCfgAlarm.bPrintWaveLength/2 +1) ){
				if(bDebug) printf("alarm print count %d\n", iTimeCount_Print);
			}
			else if(!bPrint){
				//���ӵ�ǰ wHeadIndex ȡ֮ǰ x������, ���� wHeadIndex(����ǰ������)
				wHead = gRealAlmDataArray.wHeadIndex;
				wTail = gRealAlmDataArray.wTailIndex;
				bLoop = gRealAlmDataArray.bLoop;
				wSize = MAXREALALARMSAVECOUNT;
				
				iCount_Print = gCfgAlarm.bPrintWaveLength;
				
				if(bDebug){
					printf("print Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount_Print);
				}
				if(wHead < wTail){
					if(bDebug)
						printf("print Real Data Rollback \n");
					for(i=wHead; i>=0; i--, iCount_Print--){
						if(iCount_Print>=0){
					//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
					if(bDebug)
						printf("\n");
					for(i=(wSize-1); i>=wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
						//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}	
					}
				}
				else if(bLoop){
					if(bDebug)
						printf("print Real Data Spacial \n");
					for(i=wHead; i>=wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
						//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
				}
				else{
					if(bDebug)
						printf("print Real Data  Normal: ");
					for(i=wHead; i>wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
							//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
				}
				if(bDebug)
					printf("\n");
					
				//��ӡ��������
				
		
				if(gCfgAlarm.bPrint == SWITCH_ON && sAlarm.bPrint == ALARM_PRINT_ON){
					//Printer_PrintAlarmData(&gPrintAlarmData);
					gPrintingType=PRN_WORK_ALMPRINT;
					sem_post(&semReal_Print);
				}
				
				bPrint = TRUE;
			}
		}
		else{
			
			//������������
			//���ӵ�ǰ wHeadIndex ȡ֮ǰ x������, ���� wHeadIndex(����ǰ������)
			wHead = gRealAlmDataArray.wHeadIndex;
			wTail = gRealAlmDataArray.wTailIndex;
			bLoop = gRealAlmDataArray.bLoop;
			wSize = MAXREALALARMSAVECOUNT;
			
			//��ȡHead-Tail�Ĳ�������
			iCount = MAXALARMWAVELENGTH-1;
			if(bDebug){
				printf("Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount);
			}
			
			if(wHead < wTail){
				if(bDebug)
					printf("Real Data Rollback \n");
				for(i=wHead; i>=0; i--, iCount--){
					if(iCount>=0){
					//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
				if(bDebug)
					printf("\n");
				for(i=(wSize-1); i>=wTail; i--, iCount--){
					if(iCount>=0){
						//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}	
				}
			}
			else if(bLoop){
				if(bDebug)
					printf("Real Data Spacial \n");
				for(i=wHead; i>=wTail; i--, iCount--){
					if(iCount>=0){
				//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
			}
			else{
				if(bDebug)
					printf("Real Data  Normal: ");
				for(i=wHead; i>wTail; i--, iCount--){
					if(iCount>=0){
				//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
			}
			if(bDebug)
				printf("\n");
			
			
			
// 			for(i=0; i<MAXALARMWAVELENGTH; i++){
// 				memcpy((PTRENDWAVE)(sAlmData.waves[i]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
// 			}
// 		
			//����
			pthread_mutex_lock(&mtSaveAlarmData);
			AlarmDataArray_Add(&sAlmData);
			//����
			pthread_mutex_unlock(&mtSaveAlarmData);
			
			break;
		}
	}
printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
						sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
						sAlmData.paras.bSpO2); 
	printf("%s:%d  Cancel  Save Alarm Data thread.\n", __FILE__, __LINE__);
	FinishedSaveAlarmData = TRUE;
	pthread_exit("Cancel Save Alarm Data thread.\n");
}
		
static void *ThreadProcSaveAlarmData(void *arg)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i,j,k; 
	int iCount;				//�������ݼ�����
	int iTimeCount;			//ʱ�������
	int iCount_Print;			//�������ݼ�����
	int iTimeCount_Print;	//ʱ�������
	
	
	//������������
	ALARMDATA sAlmData;
  	B_ALM_INFO sAlarm = *(PB_ALM_INFO)arg;
	BOOL bSaveParas = FALSE;	//�Ƿ��Ѿ��洢������Ϣ
	BOOL bPrint = FALSE;	//�Ƿ��Ѿ���ӡ
		
	if(bDebug){
		printf("%s:%d Start Save Alarm Data thread.\n", __FILE__, __LINE__);
		printf("--------------%s:%d Alarm ID is %d, class is %d.\n", __FILE__, __LINE__, sAlarm.bAlmID, sAlarm.bClass);
	}
	
	
	if(sAlarm.bClass == ALARM_OFF || sAlarm.bClass >=ALARM_INFO){
		return ;
	}
	
	//��ʼ����������
	memset(&sAlmData, 0, sizeof sAlmData);
	memset(&gPrintAlarmData, 0, sizeof gPrintAlarmData);
	
	//����֮ǰ��X�������, ��ΪRealData�ǰ��������洢��,���Բ�����������
	//���ӵ�ǰ wHeadIndex ȡ֮ǰ x������, ���� wHeadIndex(����ǰ������)
	wHead = gRealDataArray.wHeadIndex;
	wTail = gRealDataArray.wTailIndex;
	bLoop = gRealDataArray.bLoop;
	wSize = MAXREALSAVECOUNT;
	
	while(gRealDataArray.wCount == 0);
	
	//��¼��ʱ�ı��������Լ�������Ϣ
	sAlmData.bAlmType = ALARM_PHYSIOLOGICAL;		//������
	sAlmData.bAlmID = sAlarm.bAlmID;
	sAlmData.bAlmClass = sAlarm.bClass;
	//��ӡ������
	gPrintAlarmData.bAlmType = ALARM_PHYSIOLOGICAL;		//������
	gPrintAlarmData.bAlmID = sAlmData.bAlmID;
	gPrintAlarmData.bAlmClass = sAlmData.bAlmClass;
	
	
// 	//TODO:�˴����ص����ݿ����ͺ�
//  	sAlmData.paras = gRealDataArray.Datas[wHead].paras;
// 	//ʱ��
// 	GetSysTime(&(sAlmData.paras.sTime));
	
// 	if(bDebug)
// 		printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, Head is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
// 		       sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
// 		       gRealDataArray.Datas[wHead].paras.liOffset);
/*	
	printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
	       sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
	       sAlmData.paras.bSpO2);              */
	
	//��ȡǰ16��Ĳ�������(Head֮ǰ��16������)
	iCount = MAXALARMWAVELENGTH/2;
	if(bDebug){
		printf("Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount);
	}
	if(wHead < wTail){
		if(bDebug)
			printf("Real Data Rollback \n");
		for(i=wHead; i>=0; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
		if(bDebug)
			printf("\n");
		for(i=(wSize-1); i>=wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}	
		}
	}
	else if(bLoop){
		if(bDebug)
			printf("Real Data Spacial \n");
		for(i=wHead; i>=wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
	}
	else{
		if(bDebug)
			printf("Real Data  Normal: ");
		for(i=wHead; i>wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
 				if(bDebug)
 					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
	}
	if(bDebug)
		printf("\n");
	
	
	
	//��ȡ��16��Ĳ�������(�������ӵļ����ȡHead��������)
	iCount = MAXALARMWAVELENGTH/2+1;
	
// 	iCount_Print = (gCfgAlarm.bPrintWaveLength+1) /2 +1;	
	
	//��ʱʵ�ֱ���ǰx�벨�εĴ�ӡ����Ϊ�����ӡ֮��Ĳ��Σ������ӳ�
	//��ȡ������������
	for(j=iCount-gCfgAlarm.bPrintWaveLength, k=0; j<=iCount; j++,k++){
		printf("Copy Alarm wave  %d\n", j);
		memcpy((PTRENDWAVE)(gPrintAlarmData.waves[k]), (PTRENDWAVE)(sAlmData.waves[j]), SAMPLING_RATE * sizeof(TRENDWAVE));	
	}
	
	//��ȡ�������ݵĲ�����Ϣ
	gPrintAlarmData.paras = gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras;
	GetSysTime(&(gPrintAlarmData.paras.sTime));
	//��ӡ��������
	Printer_PrintAlarmData(&gPrintAlarmData);
	
	for(i=MAXALARMWAVELENGTH/2; i<MAXALARMWAVELENGTH; i++, iCount ++, iCount_Print ++){
		wHead = gRealDataArray.wHeadIndex;
		if(iCount < MAXALARMWAVELENGTH){
 			sleep(1);
			//�洢һ�β�������,֮�����ӳ�����Ϊ�洢��ʵ�ʵ�ֵ��ʱ���ϵ��ӳ�
			if(!bSaveParas && iCount == MAXALARMWAVELENGTH/2+4){
				bSaveParas = TRUE;
				sAlmData.paras = gRealDataArray.Datas[wHead].paras;
				//ʱ��
				GetSysTime(&(sAlmData.paras.sTime));
			}

			//copy Wave Data
			memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[wHead].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
			/*
			if(iCount_Print < (gCfgAlarm.bPrintWaveLength+1) ){
// 				memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealDataArray.Datas[wHead].waves), SAMPLING_RATE * sizeof(TRENDWAVE));	
				printf("alarm  count %d\n", iCount_Print);
			}
			else if(!bPrint){
				//��ȡ������������
				for(j=iCount-gCfgAlarm.bPrintWaveLength, k=0; j<=iCount; j++,k++){
					printf("Copy Alarm wave  %d\n", j);
					memcpy((PTRENDWAVE)(gPrintAlarmData.waves[k]), (PTRENDWAVE)(sAlmData.waves[j]), SAMPLING_RATE * sizeof(TRENDWAVE));	
				}
// 				gPrintAlarmData =  sAlmData;
				//��ȡ�������ݵĲ�����Ϣ
				gPrintAlarmData.paras = gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras;
				GetSysTime(&(gPrintAlarmData.paras.sTime));
				//��ӡ��������
				Printer_PrintAlarmData(&gPrintAlarmData);
				bPrint = TRUE; 
			}
			*/
			if(bDebug)
				printf("count %d\n", iCount);
		}
	}
	
	//����
	pthread_mutex_lock(&mtSaveAlarmData);
	AlarmDataArray_Add(&sAlmData);
	//����
	pthread_mutex_unlock(&mtSaveAlarmData);
		
	printf("%s:%d Cancel Save Alarm Data thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel Save Alarm Data thread.\n");
}

//��¼һ����������
int SaveOneAlarmData(B_ALM_INFO sInfo)
{
	B_ALM_INFO sAlm;
	int res = 0;
	
	sAlm = sInfo;
       
	if(sAlm.bClass == ALARM_OFF) return -1;
	printf("------------%d\n",FinishedSaveAlarmData);
//   	res = pthread_create(&ptSaveAlarmData, NULL, ThreadProcSaveAlarmData, &sAlm);
	if(FinishedSaveAlarmData) {
		usleep(20000);		//��ʱ�ȴ��߳��˳�
		res = pthread_create(&ptSaveAlarmData, NULL, NewThreadProcSaveAlarmData, &sAlm);
		usleep(20000);		//��ʱ�ȴ��̴߳�����
	}
	else{
		printf("----------------------- %s:%d  Cann't create thread, saving Alarm Data .\n", __FILE__, __LINE__);
	}
	
	return res;
}


///////////////////////// �ĵ�洢�ͷ���  2008-08-04 10:41:26//////////////////
//ʵʱECG����  I II III   V1~V6
typedef struct {
	BYTE bWave[4];		
}REAL_ECG_DATA, *PREAL_ECG_DATA;
REAL_ECG_DATA gRealEcg_Data;


//ʵʱECG���δ洢, ��λ:  �� * SAMPLING_RATE
#define MAXREALECG_COUNT	12000  		 //60*200
//#define MAXREALECG_COUNT		120000   	//60*200*X 
typedef struct {
	long int 		wHeadIndex;				//ѭ��������ָ��
	long int 		wTailIndex;				//ѭ������βָ��
	BOOL 		bLoop;					//���鷭ת���
	long int 		wCount;					//ʵ�ʴ洢������
	REAL_ECG_DATA		Datas[MAXREALECG_COUNT];
}REAL_ECGARRAY, *PREAL_ECGARRAY;
REAL_ECGARRAY gRealEcg_Array;

//�Ƿ�洢ECG����
BOOL gbSaveRealEcgData = FALSE;
BOOL gbSaveRealEcgDataOK = FALSE;

//�����ļ��洢��ECG���ݽṹ
typedef struct {
 	S_TIME 				sStartTime;			//��ʼ�洢���ݵ�ʱ��	
	REAL_ECG_DATA		Datas[MAXREALECG_COUNT];
}REALSAVEDATA_ECG, *PREALSAVEDATA_ECG;


/**
	���һ�����ݵ�����,��HeadIndex��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
 */
static int RealEcgArray_Add(const PREAL_ECG_DATA pData)
{
	long int wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gRealEcg_Array.wHeadIndex;
	wTail = gRealEcg_Array.wTailIndex;
	bLoop = gRealEcg_Array.bLoop;
	wSize = MAXREALECG_COUNT;
	
	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//����Ƿ�ת�����
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//�����Ѿ�����,��ת
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
		gbSaveRealEcgDataOK = TRUE;
	}
	
	gRealEcg_Array.wHeadIndex = wHead;
	gRealEcg_Array.wTailIndex = wTail;
	gRealEcg_Array.bLoop = bLoop;
	gRealEcg_Array.Datas[gRealEcg_Array.wHeadIndex] = *pData;
	//����ʵ�ʴ洢������
	if(gRealEcg_Array.wHeadIndex < gRealEcg_Array.wTailIndex){
		//�����Ѿ�����,����ת
		gRealEcg_Array.wCount = MAXREALECG_COUNT;
	}
	else{
		gRealEcg_Array.wCount +=1;
	}

	return wHead;
}

/**
	��䵥��ʵʱECG����
	��ECG�Ĳ�����Ϊ��׼�����д洢
 */
int FillOneRealEcg_Data()
{
	/*
	static int iCount = 199;
	int i;
	
	if(iCount >=0 ){
		gRealEcg_Data.bWave[0][iCount]  = gValueEcg.iWaves[0];
		gRealEcg_Data.bWave[1][iCount]  = gValueEcg.iWaves[1];
		gRealEcg_Data.bWave[2][iCount]  = gValueEcg.iWaves[2];
		iCount --;
	}
	
	if(iCount <0){                             
		iCount = 199;
		//����¼��ӵ�������
		RealEcgArray_Add(&gRealEcg_Data);

		printf("%s:%d RealECGSave. Total %d records. Head %d, Tail %d.\n", __FILE__, __LINE__, 
		       gRealEcg_Array.wCount, gRealEcg_Array.wHeadIndex, gRealEcg_Array.wTailIndex);
	}
	*/
	
	static int iCount = 199;
	
	gRealEcg_Data.bWave[0]  = gValueEcg.iWaves[ECGWAVE_I];
	gRealEcg_Data.bWave[1]  = gValueEcg.iWaves[ECGWAVE_II];
	gRealEcg_Data.bWave[2]  = gValueEcg.iWaves[ECGWAVE_III];
	gRealEcg_Data.bWave[3]  = gValueEcg.iWaves[ECGWAVE_V];
	
	
	//����¼��ӵ�������
	RealEcgArray_Add(&gRealEcg_Data);
	
	iCount --;
	
	if(iCount <0){                             
		iCount = 199;
		printf("%s:%d RealECGSave. Total %ld records. Head %ld, Tail %ld.\n", __FILE__, __LINE__, 
		       gRealEcg_Array.wCount, gRealEcg_Array.wHeadIndex, gRealEcg_Array.wTailIndex);
	}
	
	return 0;
}

int SaveEcgDataToFile()
{
	long int wHead, wTail, wSize;
	BOOL bLoop;
	long int  i, iCount;
	REALSAVEDATA_ECG  tmpDatas;
// 	REAL_ECG_DATA		tmpDatas[MAXREALECG_COUNT];

	wHead = gRealEcg_Array.wHeadIndex;
	wTail = gRealEcg_Array.wTailIndex;
	bLoop = gRealEcg_Array.bLoop;
	wSize = MAXREALECG_COUNT;
	
	//��ô洢ʱ��
 	GetSysTime(&(tmpDatas.sStartTime));
	
	iCount =0;
	//������ǰ����(�����)
	/*
	if(wHead < wTail){
		for(i=wHead; i>=0; i--, iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
		for(i=(wSize-1); i>=wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else{
		for(i=wHead; i>wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	*/
	
	if(wHead < wTail){
		for(i=wTail; i<wSize; i++,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
		for(i=0; i<=wHead; i++, iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else if(bLoop){
		for(i=wTail; i<=wHead; i++,iCount++){	
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else{
		for(i=wTail+1; i<=wHead; i++,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}

	
	//������д���ļ�
	FILE *fp;
	fp  = fopen("ecg_records.dat", "w+");
	fwrite(&tmpDatas, sizeof (REALSAVEDATA_ECG), 1, fp); 
	fclose(fp);
	
	printf("%s:%d --------------------Save %ld ECG datas to File. (wHead %ld   wTail %ld)\n", __FILE__, __LINE__, iCount, wHead, wTail);
	
	return 0;
	
}


