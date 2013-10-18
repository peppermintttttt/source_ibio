/*************************************************************************** 
 *	Module Name:	NetRecv_Bed
 *
 *	Abstract:		�������ճ���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2009-10-20 10:16:09
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "Central.h"
//#include "OtherBed.h"

//������Դ��ַ (���ݱ��Ľ���)
char gsSourceIP[16] = "127.0.0.1";



//�������ݽ����߳�
pthread_t ptID_NetRecv_Bed;
 //���������׽���
extern int gfdSocket_Bed;
 //�������ݽ��ն˿�
extern int giOtherBedPort_Data;


//��������
CENTERDATA gOtherBedData;

//��Ҫ���ݵ���������  (255��ʾ��ǰû����Ҫ��������)
BYTE  gbOtherBedIndex = 0;
BOOL gbChangeBedIndex = TRUE;

BOOL Recevie_Status=FALSE;
//������ͨѶ״̬  0-offline  1-online
extern BYTE gbOtherBedStatus[240];
BYTE gOB_RecvIPNum=0;



extern char gsOB_IP[16];
extern BOOL gbOB_Status;
extern int giOBMacNO;
extern S_OB_INFO ob_bedlist[MAX_OB_IP];
static int NetRecvBuff_Bed(int iSocketFd, BYTE *pRecvBuff,char *sCentralIP,BYTE *IPNum)
{
	struct sockaddr_in recv_addr;
	fd_set readfs;
	struct timeval Timeout;	
	int fd ;
	int addr_len = sizeof(struct sockaddr_in);
	int iCount=-1;
	
	if(iSocketFd<=0) return -1;
	FD_ZERO(&readfs);
	FD_SET(iSocketFd, &readfs);
	
	Timeout.tv_sec = 1;
	Timeout.tv_usec = 100;
		
	if(select(iSocketFd+1, &readfs, NULL, NULL, &Timeout) > 0){
		iCount = recvfrom(iSocketFd, pRecvBuff, NETRECVBUFF_MAX_SIZE, 0, (struct sockaddr *)&recv_addr, (socklen_t*)&addr_len);
		
		strcpy(sCentralIP, inet_ntoa(recv_addr.sin_addr));
		
		*IPNum=((inet_addr(inet_ntoa(recv_addr.sin_addr)))>>24)&0xff;
		
	}
	
	return iCount;
}

/*
	�ְ�����
*/
static int RepackWaveData(const CENTERDATA *netdata)
{
	int i,j;
	static BYTE OldSpo2Value,Spo2Value;
	static int OldRespValue,RespValue;
	S_ECG_VALUE ecgValue;
	S_SPO2_VALUE spo2Value;
	S_RESP_VALUE respValue;

	if(!(netdata->ECG_lead_sta&0x01)){
		//ECG Wave  (��ʱֻȡ����)
		for(i=0; i<ECG_WAVE_PER_SEC*3; i++)
		{
			ecgValue.iWaves[ECGWAVE_I] = netdata->ECG_wave[i];
			ecgValue.iWaves[ECGWAVE_II] = netdata->ECG_wave[i+1];
			ecgValue.iWaves[ECGWAVE_V] = netdata->ECG_wave[i+2];
			
			//III=LL-LA=II-Is
			ecgValue.iWaves[ECGWAVE_III] = ecgValue.iWaves[ECGWAVE_II] - ecgValue.iWaves[ECGWAVE_I] + 128;
			//aVR = -(I+II)/2
			ecgValue.iWaves[ECGWAVE_AVR] = 256 - (ecgValue.iWaves[ECGWAVE_I] + ecgValue.iWaves[ECGWAVE_II])/2;
			//aVL=(I-III)/2
			ecgValue.iWaves[ECGWAVE_AVL] = (ecgValue.iWaves[ECGWAVE_I] - ecgValue.iWaves[ECGWAVE_III])/2 + 128;
			//aVF=(II+III)/2
			ecgValue.iWaves[ECGWAVE_AVF] = (ecgValue.iWaves[ECGWAVE_II] + ecgValue.iWaves[ECGWAVE_III])/2;	

		//	printf("I:%d,II:%d,V:%d\n",ecgValue.iWaves[ECGWAVE_I],ecgValue.iWaves[ECGWAVE_II],ecgValue.iWaves[ECGWAVE_V]);
		//	if(B_PRINTF) printf("%d ",i);
		//	if(i%10==0) if(B_PRINTF) printf("\n");
			i=i+2;
			NetReceivingData_Ecg1(&ecgValue);
			if((i/3)%5==0){
				NetReceivingData_Ecg1(&ecgValue);
			}
		}
	}else{
		for(i=0; i<ECG_WAVE_PER_SEC*3; i++)
		{
			ecgValue.iWaves[ECGWAVE_I] = netdata->ECG_wave[i];
			ecgValue.iWaves[ECGWAVE_II] = netdata->ECG_wave[i+1];
			ecgValue.iWaves[ECGWAVE_III] = netdata->ECG_wave[i+2];
		
			i=i+2;
			NetReceivingData_Ecg1(&ecgValue);
			if((i/3)%5==0){
				NetReceivingData_Ecg1(&ecgValue);
			}
		}
	}
		#if 1
	//SPo2 Wave  
	for(i=0; i<SPO2_WAVE_PER_SEC; i++)
	{
	#if 1
		//���в�ֵ��������4��ֵ
		Spo2Value=(netdata->SPO2_wave[i]*256)/100;
		for(j=0;j<5;j++){
			spo2Value.bWave=OldSpo2Value+((Spo2Value-OldSpo2Value)/4)*j;
			NetReceivingData_SpO2(&spo2Value);
		}
		OldSpo2Value=Spo2Value;
	#else
		spo2Value.bWave=netdata->SPO2_wave[i];
		NetReceivingData_SpO2(&spo2Value);
		NetReceivingData_SpO2(&spo2Value);
		
		if(i%5==0){
			NetReceivingData_SpO2(&spo2Value);
			NetReceivingData_SpO2(&spo2Value);
		
		}
	#endif
	}
	
	//RESP Wave  
	for(i=0; i<RESP_WAVE_PER_SEC; i++)
	{
	#if 1
		//���в�ֵ��������4��ֵ
		RespValue=(netdata->RESP_wave[i]*256)/100;
		for(j=0;j<5;j++){
			respValue.iWave=OldRespValue+((RespValue-OldRespValue)/4)*j;
			NetReceivingData_Resp(&respValue);
		}
		OldRespValue=RespValue;
	#else
		respValue.iWave=netdata->RESP_wave[i];
		NetReceivingData_Resp(&respValue);
		NetReceivingData_Resp(&respValue);
		if(i%5==0){
			NetReceivingData_Resp(&respValue);
			NetReceivingData_Resp(&respValue);
		
		}
	#endif
	}
#endif
	return 0;
}

static int AnalysisPack_bed(BYTE *pPack,char *SourceIP,int OB_IPNum, int iLength)
{
	int i;
	int ilen;
	unsigned int localIP;
	BYTE bCode;
//	WORD wInfoCode = 0;		//������������Ϣ���
//	WORD wInfoValue = 0;		//��������õ���Ϣ����
	unsigned int  wInfoCode = 0;		//������������Ϣ���
	unsigned int  wInfoValue = 0;		//��������õ���Ϣ����
	
	if(pPack ==NULL) {
		if(B_PRINTF) printf("pPack=NULL. \n");
		return -1;
	}
	
	//too short 
	if(iLength < 4){
		if(B_PRINTF) printf("pPack too short  %d. \n", iLength);
		return -1;
	} 
	

	//������
	bCode = pPack[6];

	switch(bCode){
		case NCMD_ANSWER_PACK:{//�������ӱ���
			//������ڵ�ͨѶ״̬Ϊ���У���ͬ����֮����
			
			if(!gbOB_Status){
				
				ilen=OB_NetSend_Connect(TRUE,SourceIP);
				
				if(ilen>0){
					if(OB_IPNum>0&&OB_IPNum<=240)
						giOBMacNO = OB_IPNum;
					gbOB_Status = TRUE;
					strcpy(gsOB_IP,SourceIP);
					if(B_PRINTF)printf("ok connect ,gsSourceIP:%s,OB_IPNum=%d\n",SourceIP,OB_IPNum);
				}
				
			}else{
				ilen=OB_NetSend_Connect(FALSE,SourceIP);
				gbOB_Status=FALSE;
				if(B_PRINTF)printf("no connect \n");
			}
				
			
		}break;
		case NCMD_OB_ANSWER_PACK:{
			if(pPack[7]){//��Ӧ���ӳɹ�
				OB_ResetDraw();
				Recevie_Status=TRUE;
			}else{//��Ӧ����ʧ��
				OB_ResetDraw();
				Recevie_Status=FALSE;
			}
		}break;
		case NCMD_OB_STOP_COMM:{
			
			localIP=GetOwnIPAddr(gCfgNet.cIP);
			if(B_PRINTF) printf("----------------------------Recv disconnect code%d,localIP=%d.\n",OB_IPNum,localIP);
		
			if(localIP!=OB_IPNum){
				memset(gsOB_IP, 0, sizeof gsOB_IP);
				giOBMacNO = 0;
				gbOB_Status = FALSE;
			}
		}break;
		case NCMD_DATA_PACK:{
			if(B_PRINTF)printf("data pack gbOtherBedIndex=%d,macno=%d,,ipnum=%d,Recevie_Status=%d\n",gbOtherBedIndex,ob_bedlist[gbOtherBedIndex].MacNo,OB_IPNum,Recevie_Status);
		//	 if(gbOtherBedIndex>0 && ob_bedlist[gbOtherBedIndex].MacNo==OB_IPNum&&Recevie_Status){
			
			 if(Recevie_Status){	
			 	gOB_RecvIPNum=OB_IPNum;
				memcpy(&gOtherBedData, pPack, sizeof gOtherBedData);
				RepackWaveData(&gOtherBedData);
			 }
		}break;
		
	}
	
	return 0;	
}
static void  *ptProc_NetRecv_Bed(void *arg)
{
	int iLen; 	
	int fd ;
	BYTE RecvBuff_Bed[NETRECVBUFF_MAX_SIZE]={0};
	char SourceIP[16]={0};
	BYTE OB_IPNum=0;
	
	fd = gfdSocket_Bed;

	while(fd >0){

		iLen = NetRecvBuff_Bed(fd, RecvBuff_Bed, SourceIP,&OB_IPNum);

		if(iLen>0){
	//		if(B_PRINTF)printf("----------------Other Bed  %d.  Code %d.  SN: %d. Length (0x%.2X 0x%.2X )%d\n", RecvBuff_Bed[3], RecvBuff_Bed[6], RecvBuff_Bed[7], 
	//					RecvBuff_Bed[8], RecvBuff_Bed[9], RecvBuff_Bed[9]*256 + RecvBuff_Bed[8]);
			AnalysisPack_bed(RecvBuff_Bed,SourceIP, OB_IPNum,iLen);	
 
		}

	}
	
	pthread_exit("Bye");
}
//�������������߳�
int CreatNetRecvThread_Bed()
{
	memset(&gOtherBedData, 0, sizeof gOtherBedData);
	memset(&gbOtherBedStatus, 0, sizeof gbOtherBedStatus);
	gbOtherBedIndex = 0;
	pthread_create(&ptID_NetRecv_Bed, NULL, ptProc_NetRecv_Bed, NULL);
	return 0;
}




