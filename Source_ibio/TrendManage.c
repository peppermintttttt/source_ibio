/*************************************************************************** 
 *	Module Name:	TrendManage
 *
 *	Abstract:	�������ݹ������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-12 09:16:55
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "Global.h"


//�����ھ��
extern HWND ghWndMain;

#define TRENDFILE_NAME		TRENDPATH

//�����Զ��洢�����ڣ���λ����
#define AUTOSAVETREND_CYCLE	60	
// #define AUTOSAVETREND_CYCLE	1

//��������
CA_TRENDARRAY gTrendDatas;
CA_SHORTTREND gShortTrendDatas;

//�����ļ��Ƿ��Ѿ�����,���û�н���,��Ҫ��ʼ����������
BOOL gbTrendFileExist = FALSE;

//�Ƿ���Կ�ʼ���ݴ洢,�����ȴ����ݼ���
BOOL gbCanRecordTrend = FALSE;

//���ƴ洢�߳�
static pthread_t ptSaveTrend;	
//���ƴ洢������
static pthread_mutex_t mtSaveTrend;
extern sem_t semCalctime;


//��¼һ����������
int  RecOneTrendRecord(BYTE bLogFlag);
static int TrendArray_Add(const PTRENDDATA pData);
//���ɵ���ʹ�õ�����
static BOOL bDebugData = FALSE;

static TRENDDATA tmpTrend1;
int  AddNullRecord(BYTE bLogFlag)
{
	//������������
	//TRENDDATA tmpTrend;
	//��¼ʱ��
	S_TIME sTime;
	//ʱ��ƫ����
	long int liTimeOffset=0;
	

	
	//��ȡ�洢ʱ��
	GetSysTime(&sTime);
	
	//��ʼ����������
	memset(&tmpTrend1, 0, sizeof tmpTrend1);
	
	//����˴μ�¼��ʱ������ʼʱ���ƫ����, ��ȷ����
	//liTimeOffset = CompTime(&sTime, &gTrendDatas.sStartTime);

	tmpTrend1.liOffset +=1 ;
	tmpTrend1.sTime = sTime;
 		
 	//��¼ԭ��
 	tmpTrend1.bLogFlag = bLogFlag;
 	//������Ч��־
 	tmpTrend1.dwHaveFlag = 0;
 	

	
 	//HR
	if(bDebugData){
 		tmpTrend1.wHR = 0;
		tmpTrend1.dwHaveFlag |= HAVEHR;
	}
	else{
		if(gValueEcg.wHR !=0){
			tmpTrend1.wHR = 0; 
			tmpTrend1.dwHaveFlag |= HAVEHR;
		}
		else if(bLogFlag == TRENDNIBP){
			tmpTrend1.wHR =0; 
			tmpTrend1.dwHaveFlag |= HAVEHR;
			
			}
			
	}
 	
	//RR
	if(bDebugData){
		tmpTrend1.wRR = 0;
		tmpTrend1.dwHaveFlag |= HAVERESP;
	}
	else{
		if(!gValueResp.bOff && gValueResp.wRR!=0 ){
			tmpTrend1.wRR = 0;
			tmpTrend1.dwHaveFlag |= HAVERESP;
		}
	}
 	
 	//SpO2
	if(bDebugData){
		tmpTrend1.bSpO2 = 0;
		tmpTrend1.dwHaveFlag |= HAVESPO2;
	}
	else{
		if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
			tmpTrend1.bSpO2 = 0;
			tmpTrend1.dwHaveFlag |= HAVESPO2;
		}
	}
	
	//PR
	if(bDebugData){
		tmpTrend1.wPR = 0;
		tmpTrend1.dwHaveFlag |= HAVEPR;	
	}
	else{
		if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
			tmpTrend1.wPR = 0;	
			tmpTrend1.dwHaveFlag |= HAVEPR;
		}
	}
	
 	//Temp1
	if(!gValueTemp.bOff_T1){
		tmpTrend1.wTemps[0] = 0;
 		tmpTrend1.dwHaveFlag |= HAVET1;
 	}	
	//Temp2
	if(!gValueTemp.bOff_T2){
		tmpTrend1.wTemps[1] =0;
		tmpTrend1.dwHaveFlag |= HAVET2;
	}	
	
	//CO2
	if(gbHaveCo2 
		  && gValueCO2.wEtCO2 !=0 
		  && gValueCO2.bStatus ==CO2_STATUS_RUN ){
		tmpTrend1.sCO2.wEtCO2 =	0;
		tmpTrend1.sCO2.wFiCO2 = 	0;
		tmpTrend1.sCO2.wAwRR = 	0;
		tmpTrend1.dwHaveFlag |= HAVECO2;
	}	
	//IBP1
	if(gbHaveIbp && !gValueIbp1.bOff){
		tmpTrend1.sIBPs[0].iSys = 0;
		tmpTrend1.sIBPs[0].iDia = 0;
		tmpTrend1.sIBPs[0].iMean = 0;
		tmpTrend1.dwHaveFlag |= HAVEIBP1;

	}
	//IBP2
	if(gbHaveIbp && !gValueIbp2.bOff){
		tmpTrend1.sIBPs[1].iSys = 0;
		tmpTrend1.sIBPs[1].iDia = 0;
		tmpTrend1.sIBPs[1].iMean = 0;
		tmpTrend1.dwHaveFlag |= HAVEIBP2;

	}
	
 	//NIBP
 	if(bLogFlag == TRENDNIBP){
		if(bDebugData){
			tmpTrend1.sNIBP.wSys = 0;
			tmpTrend1.sNIBP.wDia = 0;
			tmpTrend1.sNIBP.wMean= 0;
			tmpTrend1.dwHaveFlag |= HAVENIBP;	
		}
		else{
			if(IS_RANGE(gValueNibp.wSys, NIBP_MIN+1, NIBP_MAX) 
				&& IS_RANGE(gValueNibp.wDia, NIBP_MIN+1, NIBP_MAX)
				&& IS_RANGE(gValueNibp.wMean, NIBP_MIN+1, NIBP_MAX)){
			
				tmpTrend1.sNIBP.wSys = 0;
				tmpTrend1.sNIBP.wDia =0;
				tmpTrend1.sNIBP.wMean=0;
				tmpTrend1.dwHaveFlag |= HAVENIBP;
				
			}
		}
 	}

	//��ӵ���������
	TrendArray_Add(&tmpTrend1);

	return 0;
}








/**
	���һ�����ݵ�����,��HeadIndex��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
*/
static int TrendArray_Add(const PTRENDDATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	

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
	
	gTrendDatas.wHeadIndex = wHead;
	gTrendDatas.wTailIndex = wTail;
	gTrendDatas.bLoop = bLoop;
	gTrendDatas.Datas[gTrendDatas.wHeadIndex] = *pData;
	//����ʵ�ʴ洢������
	if(gTrendDatas.wHeadIndex < gTrendDatas.wTailIndex){
		//�����Ѿ�����,����ת
		gTrendDatas.wCount = MAXTRENDCOUNT;
	}
	else{
		gTrendDatas.wCount +=1;
	}

	return wHead;
}

/**
	���һ�����ݵ�����������,��Head��������µ�����
	ע��:���˷��洢,δ��תǰ,��������[0]��������,��ȡ����ʱҪע��
*/
static int ShortTrendArray_Add(const PTRENDDATA pData)
{
	WORD wHead;
	BOOL bFull;
	
	if(pData == NULL) return -1;

	wHead = gShortTrendDatas.wHead;
	bFull = gShortTrendDatas.bFull;

	if(wHead < (MAXSHORTTREND-1)){
		wHead ++;
		
	}else{
		//�����Ѿ�����,��ת
		wHead = 0;
		bFull= TRUE;
	}
	
	gShortTrendDatas.wHead = wHead;
	gShortTrendDatas.bFull = bFull;
	gShortTrendDatas.Datas[wHead] = *pData;
	
}
/**
	�������Ƶ�FLash
	���˼·��
		1�������ƴ洢�Ĳ�������Ϊ�ļ�������
		2����׺�̶�Ϊ .ttt
*/


int SaveTrendToFlash(PCA_TRENDARRAY pTrendDatas)
{
	unsigned char strFileName[30] = {0};
	FILE *fpTrendData;
	int res;
	
	if(B_PRINTF)printf("%s:%d Start Save Trend Data.\n", __FILE__, __LINE__);
	if(pTrendDatas == NULL) return -1;
	//�����ļ���
	snprintf(strFileName, sizeof strFileName, TRENDFILE_NAME, pTrendDatas->sPatientInfo.sName);
	
	gbTrendFileWrite_OK = 0;
	SetIntValueToResFile(gFileSetup, "FLASH_TREND", "saveok",  gbTrendFileWrite_OK, 1);
	fpTrendData = fopen(strFileName, "w+");
	if(fpTrendData == NULL) return -1;
	
	//д�ļ�
	res= fwrite(pTrendDatas, sizeof(CA_TRENDARRAY), 1, fpTrendData); 
	
	fclose(fpTrendData);
	if(B_PRINTF)printf("%s:%d Save Trend to File %s  OK, size: %d.\n", __FILE__, __LINE__, strFileName, sizeof(CA_TRENDARRAY));
	//����һ��д�ɹ��ı�־��Ԥ��ͻȻ�ϵ�ʱ��û����ȫд���ļ��������ļ��𻵣��޷�����  2008-04-16 17:56:44
	gbTrendFileWrite_OK = 1;
	SetIntValueToResFile(gFileSetup, "FLASH_TREND", "saveok",  gbTrendFileWrite_OK, 1);
	
	return 0;
}

/**
	��������������
*/
static int LoadTrendToArray()
{
	int fd,ii;
	int InterValue=0,InterValues=0;
	struct stat fd_stat;	//�ļ���Ϣ
	PCA_TRENDARRAY pTrendDatas;
	CA_TRENDARRAY   TmpValue;
	S_TIME			TmpTime,TmpCurTime,TmpTimes;
				
	if(B_PRINTF)printf("%s:%d Loading Trend File (%s) ... ... \n", __FILE__, __LINE__, TRENDFILE_NAME);
	
	fd = open(TRENDFILE_NAME, O_RDONLY);
	if(fd<0){
		if(B_PRINTF)printf("(%s:%d)Read file %s failure.\n", __FILE__, __LINE__, TRENDFILE_NAME);
		return -1;
	}
		

	//ӳ���ڴ�
 	pTrendDatas = (PCA_TRENDARRAY)mmap(0, sizeof(CA_TRENDARRAY), PROT_READ, MAP_PRIVATE, fd, 0); 
	

			
	if(pTrendDatas == MAP_FAILED){
		if(B_PRINTF)printf("(%s:%d)MAP_FAILED.\n", __FILE__, __LINE__);
		close(fd);
		return -1;	
	} 
	memset(&TmpValue, 0, sizeof(CA_TRENDARRAY));
 	memcpy(&TmpValue, pTrendDatas, sizeof(CA_TRENDARRAY));
	TmpTime=TmpValue.Datas[TmpValue.wHeadIndex].sTime;
	printf("~~~%d  %d   %d  %d  %d  %d  \n " , TmpTime.wYear,TmpTime.bMonth ,TmpTime.bDay,TmpTime.bHour,TmpTime.bMin,TmpTime.bSec);
	GetSysTime(&TmpCurTime);
	TmpTimes=TmpCurTime;
	printf("~~~%d  %d   %d  %d  %d  %d  \n " , TmpCurTime.wYear,TmpCurTime.bMonth ,TmpCurTime.bDay,TmpCurTime.bHour,TmpCurTime.bMin,TmpCurTime.bSec);
	InterValue = ((int)CompTime(&TmpTimes , &TmpTime))/60;

	printf("~~~!!!%d~~!!!\n",InterValue);

	if(InterValue>60*24*5){
			memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));
			InitTrendArray();
			printf(" da  yu  san shi miao!!!!da  yu  san shi miao!!!!da  yu  san shi miao!!!!da  yu  san shi miao!!!!\n");
		}
	else{
	//�����ڴ�
			memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));	
 			memcpy(&gTrendDatas, pTrendDatas, sizeof(CA_TRENDARRAY));
			for(ii=1;ii<InterValue;ii++){
					AddNullRecord(0x01);
				}
			
			printf(" kao bei zhong  !!!!!!kao bei zhong  !!!!!! kao bei zhong  !!!!!!\n");


		}
	


	//�ͷ��ڴ�
 	munmap((PCA_TRENDARRAY)pTrendDatas, sizeof(CA_TRENDARRAY));

	
	close(fd);
	
 	if(B_PRINTF)printf("OK!\n");
	
	if(B_PRINTF)printf("%s:%d Trend Data -- Name %s, count :%d, %.2d-%.2d %.2d:%.2d:%.2d head:%d, tail:%d.\n",
	       __FILE__, __LINE__, 
	       gTrendDatas.sPatientInfo.sName,
	       gTrendDatas.wCount, 
	       gTrendDatas.sStartTime.bMonth, 
	       gTrendDatas.sStartTime.bDay, 
	       gTrendDatas.sStartTime.bHour, 
	       gTrendDatas.sStartTime.bMin, 
	       gTrendDatas.sStartTime.bSec,
	       gTrendDatas.wHeadIndex, 
	       gTrendDatas.wTailIndex);
	
	return 0;
}

/**
	��Flash������������
	��������ʱ, ���flash�����������ݣ�������������ݣ����ڴ˻���������µ���������
	����, �ó�ʼ����־, ��ʼ����������
	@return 0:�ɹ�, -1�ļ�������,����û����������

*/
static int LoadTrendFromFlash(PCA_TRENDARRAY pTrendDatas)
{
	FILE *fp = NULL;
	struct stat statConfig;
	
	if(pTrendDatas == NULL) return -1;
	
	
	gbCanRecordTrend = FALSE;
	
	//����ļ��Ƿ����
	fp = fopen(TRENDFILE_NAME, "r");
	
	if(fp == NULL){
	if(B_PRINTF)	printf("%s:%d File %s not exist.\n", __FILE__, __LINE__, TRENDFILE_NAME);
		gbTrendFileExist = FALSE;
		
		gbCanRecordTrend = TRUE;
		return -1;
	}else{
		
		//��ô洢�Ƿ�ɹ��ı�־����������ļ�
		GetIntValueFromEtcFile(gFileSetup, "FLASH_TREND", "saveok", &gbTrendFileWrite_OK);
		if(!gbTrendFileWrite_OK){
			//�ļ��洢���ɹ��������µĿհ��ļ�
		if(B_PRINTF)	printf("%s:%d The trend file may be Error.\n", __FILE__, __LINE__);
			gbTrendFileExist = FALSE;
			
			gbCanRecordTrend = TRUE;
			
			fclose(fp);
			return -1;
		}
		else{
			//�����ļ��Ĵ�С�ж��ļ��Ƿ���	
			fstat(fileno(fp), &statConfig);
	                //-- ����ļ�С��20Byte,���ж��ļ���
			if(statConfig.st_size <1){
				if(B_PRINTF)printf("%s:%d File %s, size %ld. Error!.\n", __FILE__, __LINE__, TRENDFILE_NAME, statConfig.st_size);
				gbTrendFileExist = FALSE;
			
				gbCanRecordTrend = TRUE;
			
				fclose(fp);
				return -1;
			}
			else{
				if(B_PRINTF)printf("%s:%d File %s OK!.\n", __FILE__, __LINE__, TRENDFILE_NAME);
				gbTrendFileExist = TRUE;
			}

		}
		
		fclose(fp);
	}

	//��������
	if(gbTrendFileExist)
	 	LoadTrendToArray();
	
	gbCanRecordTrend = TRUE;
	return 0;
}

/**
	�Զ����ƴ洢�߳�
	���˼·��
		1�����Ϊ1���ӵ�ѭ������ΪҪ��ѯ�Ƿ����ڹػ�
		2�����ü�������������
*/
//���Ե�Դ�ر�
extern BOOL gbTestPowerDown;
static void *ThreadProcSaveTrend(void *arg)
{
	struct timeval timeout;
	int delay;
	int iCount = AUTOSAVETREND_CYCLE,iCountTime=0;
	int temp;
		
	for(;;){
 		if(iCountTime>2*60){
			//�ѵ�ǰ���ƴ洢��Flash
  			SaveTrendToFlash(&gTrendDatas);
			iCountTime=1;
		}
		else{
			//�����ƴ洢���ڴ�
			if(iCount >=AUTOSAVETREND_CYCLE){
			//if(iCount >=2){
				//����
				pthread_mutex_lock(&mtSaveTrend);
				RecOneTrendRecord(TRENDAUTO);
				//����
				pthread_mutex_unlock(&mtSaveTrend);
				iCount = 1;
			}	
		}

		timeout.tv_sec = 1;
		timeout.tv_usec= 0;
		delay = select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
		iCount +=1;
		iCountTime+=1;
		
	}
}


//-------------------------- interface ------------------------------
/**
	��ʼ����������
 */
int InitTrendArray()
{
	S_TIME sTime;
	
	//����
	pthread_mutex_lock(&mtSaveTrend);
	
	//��ȡ��ʼʱ��
	GetSysTime(&sTime);
	gTrendDatas.sStartTime = sTime;
	//��ȡ������Ϣ
	gTrendDatas.sPatientInfo = gCfgPatient;
	//��ʼ����������
	gTrendDatas.wHeadIndex = 0;
	gTrendDatas.wTailIndex = 0;
	gTrendDatas.wCount = 0;
	gTrendDatas.bLoop  = FALSE;

	//��ʼ������������
	gShortTrendDatas.wHead= 0;
	gShortTrendDatas.bFull= FALSE;
	
	//����
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}

/**
	��¼һ����������
	bLogFlag:��¼��ԭ��
*/
//������������
static TRENDDATA tmpTrend;
int  RecOneTrendRecord(BYTE bLogFlag)
{
	//������������
	//TRENDDATA tmpTrend;
	//��¼ʱ��
	S_TIME sTime;
	//ʱ��ƫ����
	long int liTimeOffset=0;
	
	//�ж��Ƿ���Լ�������
	if(!gbCanRecordTrend) {
		if(B_PRINTF)printf("Cannt Record trend data.\n");
		return -1;
	}
	
	//��ȡ�洢ʱ��
	GetSysTime(&sTime);
	
	//��ʼ����������
	memset(&tmpTrend, 0, sizeof tmpTrend);
	
	//����˴μ�¼��ʱ������ʼʱ���ƫ����, ��ȷ����
	liTimeOffset = CompTime(&sTime, &gTrendDatas.sStartTime);
//	printf("###%d  %d   %d  %d  %d  %d###  \n " , sTime.wYear,sTime.bMonth ,sTime.bDay,sTime.bHour,sTime.bMin,sTime.bSec);
//	printf("###%d  %d   %d  %d  %d  %d###  \n " , gTrendDatas.sStartTime.wYear,gTrendDatas.sStartTime.bMonth ,
//		gTrendDatas.sStartTime.bDay,gTrendDatas.sStartTime.bHour,gTrendDatas.sStartTime.bMin,gTrendDatas.sStartTime.bSec);

	//(gTrendDatas.Datas[gTrendDatas.wHeadIndex].sTime)

	tmpTrend.liOffset = liTimeOffset;
	tmpTrend.sTime = sTime;
 		
 	//��¼ԭ��
 	tmpTrend.bLogFlag = bLogFlag;
 	//������Ч��־
 	tmpTrend.dwHaveFlag = 0;
 	
 	//��ȡ����
 	//HR
	if(bDebugData){
 		tmpTrend.wHR = 50+(long int)(50.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVEHR;
	}
	else{
		if(gValueEcg.wHR !=0){
			tmpTrend.wHR = gValueEcg.wHR; 
			tmpTrend.dwHaveFlag |= HAVEHR;
		}
		else if(bLogFlag == TRENDNIBP){
			tmpTrend.wHR = gValueNibp.wPR; 
			tmpTrend.dwHaveFlag |= HAVEHR;
			
			}
			
	}
 	
	//RR
	if(bDebugData){
		tmpTrend.wRR = 15+(long int)(10.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVERESP;
	}
	else{
		if(!gValueResp.bOff && gValueResp.wRR!=0 ){
			tmpTrend.wRR = gValueResp.wRR;
			tmpTrend.dwHaveFlag |= HAVERESP;
		}
	}
 	
 	//SpO2
	if(bDebugData){
		tmpTrend.bSpO2 = 95+(long int)(5.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVESPO2;
	}
	else{
		if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
			tmpTrend.bSpO2 = gValueSpO2.bSpO2;
			tmpTrend.dwHaveFlag |= HAVESPO2;
		}
	}
	
	//PR
	if(bDebugData){
		tmpTrend.wPR = 50+(long int)(50.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVEPR;	
	}
	else{
		if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
			tmpTrend.wPR = gValuePulse.wPR;	
			tmpTrend.dwHaveFlag |= HAVEPR;
		}
	}
	
 	//Temp1
	if(!gValueTemp.bOff_T1){
		tmpTrend.wTemps[0] = gValueTemp.wT1;
 		tmpTrend.dwHaveFlag |= HAVET1;
 	}	
	//Temp2
	if(!gValueTemp.bOff_T2){
		tmpTrend.wTemps[1] = gValueTemp.wT2;
		tmpTrend.dwHaveFlag |= HAVET2;
	}	
	
	//CO2
	if(gbHaveCo2 
		  && gValueCO2.wEtCO2 !=0 
		  && gValueCO2.bStatus ==CO2_STATUS_RUN ){
		tmpTrend.sCO2.wEtCO2 = gValueCO2.wEtCO2;
		tmpTrend.sCO2.wFiCO2 = gValueCO2.wFiCO2;
		tmpTrend.sCO2.wAwRR = gValueCO2.wAwRR;
		tmpTrend.dwHaveFlag |= HAVECO2;
	}	
	//IBP1
	if(gbHaveIbp && !gValueIbp1.bOff){
		tmpTrend.sIBPs[0].iSys = gValueIbp1.iSys;
		tmpTrend.sIBPs[0].iDia = gValueIbp1.iDia;
		tmpTrend.sIBPs[0].iMean = gValueIbp1.iMean;
		tmpTrend.dwHaveFlag |= HAVEIBP1;
// 		printf("Save Trend IBP1 : %d / %d   %d.\n ", gValueIbp1.iSys, gValueIbp1.iDia, gValueIbp1.iMean);
	}
	//IBP2
	if(gbHaveIbp && !gValueIbp2.bOff){
		tmpTrend.sIBPs[1].iSys = gValueIbp2.iSys;
		tmpTrend.sIBPs[1].iDia = gValueIbp2.iDia;
		tmpTrend.sIBPs[1].iMean = gValueIbp2.iMean;
		tmpTrend.dwHaveFlag |= HAVEIBP2;
// 		printf("Save Trend IBP2 : %d / %d   %d.\n ", gValueIbp2.iSys, gValueIbp2.iDia, gValueIbp2.iMean);
	}
	
 	//NIBP
 	if(bLogFlag == TRENDNIBP){
		if(bDebugData){
			tmpTrend.sNIBP.wSys = 70+(long int)(80.0*rand()/(RAND_MAX+1.0));
			tmpTrend.sNIBP.wDia = 30+(long int)(20.0*rand()/(RAND_MAX+1.0));
			tmpTrend.sNIBP.wMean= (tmpTrend.sNIBP.wSys + tmpTrend.sNIBP.wDia)/2;
			tmpTrend.dwHaveFlag |= HAVENIBP;	
		}
		else{
			if(IS_RANGE(gValueNibp.wSys, NIBP_MIN+1, NIBP_MAX) 
				&& IS_RANGE(gValueNibp.wDia, NIBP_MIN+1, NIBP_MAX)
				&& IS_RANGE(gValueNibp.wMean, NIBP_MIN+1, NIBP_MAX)){
			
				tmpTrend.sNIBP.wSys = gValueNibp.wSys;
				tmpTrend.sNIBP.wDia = gValueNibp.wDia;
				tmpTrend.sNIBP.wMean= gValueNibp.wMean;
				//if(gValueEcg.wHR == 0){
				//tmpTrend.wHR = gValueNibp.wPR; 
					//}
				tmpTrend.dwHaveFlag |= HAVENIBP;
				
			}
		}
 	}

	//��ӵ���������
	TrendArray_Add(&tmpTrend);
	 ShortTrendArray_Add(&tmpTrend);	
//  	printf("%s : %d Trend data start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
//  		 gTrendDatas.sStartTime.wYear, gTrendDatas.sStartTime.bMonth, gTrendDatas.sStartTime.bDay, 
//  		 gTrendDatas.sStartTime.bHour, gTrendDatas.sStartTime.bMin, gTrendDatas.sStartTime.bSec);
		
//   	printf("%s : %d Current Trend data time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d, Offset is %d. T1 is %d.\n", __FILE__, __LINE__, 
//   		 sTime.wYear, sTime.bMonth, sTime.bDay, 
// 		 sTime.bHour, sTime.bMin, sTime.bSec, liTimeOffset, tmpTrend.wTemps[0]);

// 	printf("%s:%d Save Trend Flag %d, Total %d records. Head:%d, Tail:%d.\n", __FILE__, __LINE__, 
// 	       bLogFlag, gTrendDatas.wCount, gTrendDatas.wHeadIndex, gTrendDatas.wTailIndex);
	
	return 0;
}

/**
	��ȡ�������ݵ�����
	�������絽���˳��ȡ������
*/
int TrendArray_Get()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(B_PRINTF)printf("--------%s:%d Get Time Offset(Up)head %d tail %d : ", __FILE__, __LINE__, wHead, wTail);
	if(wHead < wTail){
		printf(" Rollback \n");
		for(i=wTail; i<=(wSize-1); i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
		printf("\n");
		for(i=0; i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else if(bLoop){
		printf(" Spacial \n");
// 		for(i=wTail; i<=(wHead-1); i++){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
// 		}
// 		printf("\n");
// 		printf(" %d ", gTrendDatas.Datas[wHead].liOffset);
		for(i=wTail; i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else{
		printf(" Normal ");
		for(i=(wTail+1); i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	
	printf("\n");
	
	return 0;
}

/**
	��ȡ�������ݵ�����
	�����������˳��ȡ������
 */
int TrendArray_Read()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	printf("--------%s:%d Get Time Offset(Down)head %d tail %d : ", __FILE__, __LINE__, wHead, wTail);
	if(wHead < wTail){
		printf(" Rollback \n");
 		for(i=wHead; i>=0; i--){
  			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
 		}
		printf("\n");
 		for(i=(wSize-1); i>=wTail; i--){
 			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
 		}
	}
	else if(bLoop){
		printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else{
		printf(" Normal ");
		for(i=wHead; i>wTail; i--){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	
	printf("\n");
	
	return 0;
}

/**
	�Զ����ƴ洢����
*/
int StartAutoTrendSave()
{
	int res;
	
	//��ʼ��������
	pthread_mutex_init(&mtSaveTrend, NULL);
	
	//������������
	LoadTrendFromFlash(&gTrendDatas);
	
	//��ʼ����������,���û�н��������ļ�,���ʼ�����ݽṹ,����
	if(!gbTrendFileExist){
		InitTrendArray();
	}

	//Demo Data
	srand((long int)time(0));        
	
 	res = pthread_create(&ptSaveTrend, NULL, ThreadProcSaveTrend, NULL);
	
	if(res !=0){
		return -1;
	}
	
	return 0;
}

/**
	ȡ��ָ��ʱ�̵�����
	��ȡֵ��ʱ��,Ҫע����Щ��ɢ����������,��NIBP
	@param:sTime ָ��ʱ��
	@param:pData ȡ��������
	@return: 0-OK, -1-Failure
*/
int GetOneTrendRecord(S_TIME sTime, PTRENDDATA pData)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	S_TIME tmpTime;
	int i,j;
	int res;
	
	if(pData == NULL) return -1;
	
	//�� Head--->Tail �������ݿ�

	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(wHead < wTail){
// 		printf(" Rollback \n");
		for(i=wHead; i>=0; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//ȡ������
				*pData = gTrendDatas.Datas[i];
				
				//����Ƿ�����ɢ����(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//�����NIBP,���ټ���Ѱ��
					return 0;
				}else{
					//����ͬһʱ�̱��������
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j < 0) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}

				return 0;
			}
		}
// 		printf("\n");
		for(i=(wSize-1); i>=wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//ȡ������
				*pData = gTrendDatas.Datas[i];
				
				//����Ƿ�����ɢ����(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//�����NIBP,���ټ���Ѱ��
					return 0;
				}else{
					//����ͬһʱ�̱��������
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				
				return 0;
			}
 		}
	}
	else if(bLoop){
// 		printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//ȡ������
				*pData = gTrendDatas.Datas[i];
				
				//����Ƿ�����ɢ����(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//�����NIBP,���ټ���Ѱ��
					return 0;
				}else{
					//����ͬһʱ�̱��������
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				
				return 0;
			}
		}
	}
	else{
// 		printf(" Normal ");
		for(i=wHead; i>wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//ȡ������
				*pData = gTrendDatas.Datas[i];
				
				//����Ƿ�����ɢ����(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//�����NIBP,���ټ���Ѱ��
					return 0;
				}else{
					//����ͬһʱ�̱��������
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <=wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				return 0;
			}
		}
	}
	
// 	printf("\n");

	return -1;
}


/*
	������Ƽ�¼
*/
int ClearTrendData()
{
	//����
	pthread_mutex_lock(&mtSaveTrend);
	
	memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));
	//����
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}

/*
	�Լ�¼ʱ���������
	liOffset:ʱ��ƫ����,��λ:��
*/
int ReviseTrendTime(long int liOffset)
{
	//�µ���ʼʱ��
	S_TIME sNewStartTime;
	int i, iCount;
	
	printf("%s:%d time offset is %ld.\n", __FILE__, __LINE__, liOffset);
	
	printf("%s : %d Old start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
 		 gTrendDatas.sStartTime.wYear, gTrendDatas.sStartTime.bMonth, gTrendDatas.sStartTime.bDay, 
 		 gTrendDatas.sStartTime.bHour, gTrendDatas.sStartTime.bMin, gTrendDatas.sStartTime.bSec);

	//����
	pthread_mutex_lock(&mtSaveTrend);
	
	//�����µ�ʱ��
	NewCalcTime(&(gTrendDatas.sStartTime), &sNewStartTime, liOffset);
	
	printf("%s : %d New start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
	       sNewStartTime.wYear, sNewStartTime.bMonth, sNewStartTime.bDay, 
	       sNewStartTime.bHour, sNewStartTime.bMin, sNewStartTime.bSec);
	
	//������ʼʱ��
	gTrendDatas.sStartTime = sNewStartTime;
	
	//���ݼ�¼��offset, �������ݼ�¼��ʱ��
	for(i=0; i<gTrendDatas.wCount; i++){
		NewCalcTime(&(gTrendDatas.sStartTime), 
			      &(gTrendDatas.Datas[i].sTime), 
			      gTrendDatas.Datas[i].liOffset);
	}

	
	//����
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}		
