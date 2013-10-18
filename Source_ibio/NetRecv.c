/*************************************************************************** 
 *	Module Name:	NetRecv
 *
 *	Abstract:		���紫�����
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2008-10-21 14:21:47
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "Central.h"

#define STR_START 	9		//������Ϣ��ʼλ��
#define STR_END		29		//������Ϣ����λ��
#define LENGTH_STR_MAX		9
//��������ַ (���ݱ��Ľ�����ȷ���õ�ַ�Ƿ�Ϊ�������ַ)
char gsServerIP[16] ;


//��������������ʹ�õĶ˿�
int giServerPort = 0;
BYTE gbIPNum=0;
//�������ַ, ��ʼΪCENTRALIP_START
char gsCentralIP[16] ;
//����������ӵ�״̬
BOOL gbCentralStatus = FALSE;		//FALSE:δ�����������TRUE���Ѿ����������
//������Ļ�����
int giCentralMacNO = 0;				//241~254


//Ԥ����������������ʹ�õĶ˿�
int giOtherPort = 0;
//�������ַ, ��ʼΪCENTRALIP_START
char gsOtherCentralIP[16] ;
//����������ӵ�״̬
BOOL gbOtherCentralStatus = FALSE;		//FALSE:δ�����������TRUE���Ѿ����������
//������Ļ�����
int giOtherCentralMacNO = 0;				//241~254


//����ͨѶ������
int giOBMacNO = 0;				///1~240
BOOL gbOB_Status = FALSE;		//FALSE:δ�����������TRUE���Ѿ����������
char gsOB_IP[16]={0};

//���ջ�����
BYTE  gbRecvBuff[NETRECVBUFF_MAX_SIZE] = {0};

//��������߳�
pthread_t ptID_NetRecv;

//������ͨѶ״̬  0-offline  1-online
BYTE gbOtherBedStatus[240] = {0};

//�׽���
extern int gfdMonitorSocket;
extern int gbOtherBedIndex;
extern int giCentralPort_Data;
extern S_OB_INFO ob_bedlist[MAX_OB_IP];
extern BYTE gOB_RecvIPNum;

//�ַ����Ƚ�
int cmp_str(char * str0, char * str1)
{
    int i;
    for(i=0;str0[i]!=0 && str1[i]!=0 && str0[i]==str1[i];i++);
    return str0[i]-str1[i];
}

//��ȡ��Ϣ
int Get_msg_from_net(BYTE *pPack,BYTE *strMsg){
	int i;
	int msg_count=0;
	
	for(i=STR_START;i<STR_END;i++){
		if(pPack[i] == 0) break;
		printf("%.2x ",pPack[i]);
		strMsg[msg_count]=pPack[i];
		msg_count++;
	}
	printf("\n");
	return msg_count;
}

static int AnalysisPack(BYTE *pPack, int iLength)
{
	int i;
	BYTE bCode;
//	WORD wInfoCode = 0;		//������������Ϣ���
//	WORD wInfoValue = 0;		//��������õ���Ϣ����
	unsigned int  wInfoCode = 0;		//������������Ϣ���
	unsigned int  wInfoValue = 0;		//��������õ���Ϣ����
	BYTE strMsg[20]={0};
	int msg_count=0;
	if(pPack ==NULL) return -1;
	
	//too short 
	if(iLength < 4){
		if(B_PRINTF) printf("pPack too short  %d. \n", iLength);
		return -1;
	} 
	

//	if(B_PRINTF) printf("Central data.  MacNO:%d. Length :%d,giServerPort=%d.\n", pPack[3], iLength,giServerPort);
//	if(B_PRINTF) printf("Net Recv from %s:%d :: ", gsCentralIP, giServerPort);
	for(i=0; i<iLength; i++){
		if(B_PRINTF) printf(" 0x%.2X", pPack[i]);
	}
	if(B_PRINTF) printf(" \n");
	
	
	//������
	bCode = pPack[6];
	
	switch(bCode){
		case NCMD_BROADCASE:{
			
	//		for(i=0; i<iLength; i++)
	//			if(B_PRINTF) printf(" 0x%.2X", pPack[i]);
	//		printf("\n");	
		
		}break;
		case NCMD_ANSWER_PACK:{//�������ӱ���
		if(pPack[2]==NMACHINE_CENTER){
			//������ڵ�ͨѶ״̬Ϊ���У���ͬ����֮����
			if(!gbCentralStatus){
				//��������IP
				memset(gsCentralIP, 0, sizeof gsCentralIP);
				strcpy(gsCentralIP, gsServerIP);
				//��������������
				giCentralMacNO = pPack[3];

			//	giCentralPort_Data=pPack[8]*256+pPack[7];
			
				if(B_PRINTF) printf("************Request: Central IP :%s. Mac No :%d\n", gsCentralIP, giCentralMacNO);
				//����Ӧ������
				NetSend_Connect(TRUE);		//OK
				gbCentralStatus = TRUE;
				
				
			}	
			else{
				NetSend_Connect(FALSE);		//Refuse
			}
		}else if(pPack[2]==NMACHINE_OTHER){
			//������ڵ�ͨѶ״̬Ϊ���У���ͬ����֮����
			if(!gbOtherCentralStatus){
				//��������IP
				memset(gsOtherCentralIP, 0, sizeof gsOtherCentralIP);
				strcpy(gsOtherCentralIP, gsOtherCentralIP);
				//��������������
				giOtherCentralMacNO = pPack[3];
			
				if(B_PRINTF) printf("************Request: Other Central IP :%s. Mac No :%d\n", gsOtherCentralIP, giOtherCentralMacNO);
				//����Ӧ������
				NetSend_Connect_Other(TRUE);		//OK
				gbOtherCentralStatus = TRUE;
				
				
			}	
			else{
				NetSend_Connect_Other(FALSE);		//Refuse
			}

		}

			
		}break;
		case NCMD_STOP_COMM:{//��ֹ����
			if(pPack[2]==NMACHINE_CENTER){
				if(B_PRINTF) printf("----------------------------Recv disconnect code.\n");
				memset(gsCentralIP, 0, sizeof gsCentralIP);
				giCentralMacNO = 0;
				gbCentralStatus = FALSE;
			}else if(pPack[2]==NMACHINE_OTHER){
				if(B_PRINTF) printf("----------------------------Recv disconnect code.\n");
				memset(gsOtherCentralIP, 0, sizeof gsOtherCentralIP);
				giOtherCentralMacNO = 0;
				gbOtherCentralStatus = FALSE;
			}
			
		}break;
		case NCMD_ASK_RESEND:{//���������ط�
			BYTE resend_num;
			resend_num = pPack[7];
			NetSend_RESEND(resend_num);
			
			
			if(B_PRINTF)printf("-=-=-=-=-ask resend\n");		
		}break;
		case NCMD_ANSWER_NIBP:{//Ѫѹ����
			if(pPack[7] ==1){
				//����
				NIBP_Start();
			}
			else{
				//ֹͣ
				if(B_PRINTF) printf("---------stop NIBP.\n");
				NIBP_Cancel();
			}
		}break;
		case NCMD_SET_CFG:{//���� or ���ò���������Ϣ 
			wInfoCode = pPack[8]*256 + pPack[7];
			wInfoValue = pPack[10]*256 + pPack[9];
 			if(B_PRINTF) printf("Info Code is %d.  Value is %.8x.\n", wInfoCode, wInfoValue);
			
			switch(wInfoCode){
				/*************patient info***************/	
				case NET_PAT_ALL:{//0 ask Patient Info
					if(B_PRINTF) printf("%s,%d,ask Patient Info\n",__FILE__,__LINE__);
 					NetSend_PatientInfo();
				}break;
				case NET_PAT_DEPT:{///1
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sRoom, sizeof(gCfgPatient.sRoom),"%s" ,strMsg);
					}
					
					SetValueToEtcFile(gFileSetup, "PatientSetup", "room",  gCfgPatient.sRoom);
					NetSend_PatientInfo_Room();
					if(B_PRINTF) printf("1----set patient info :DEPT.%s \n",gCfgPatient.sRoom);
					
				}break;
				case NET_PAT_ID:{///2
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sID, sizeof(gCfgPatient.sID),"%s" ,strMsg);
					}
					SetValueToEtcFile(gFileSetup, "PatientSetup", "id",  gCfgPatient.sID);
					NetSend_PatientInfo_PatID();
					if(B_PRINTF) printf("2----set patient info :PAT NO.  %s \n",gCfgPatient.sID);
					
				}break;
				case NET_PAT_BEDNO:{///3
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sBed, sizeof(gCfgPatient.sBed),"%s" ,strMsg);
					}
					NetSend_PatientInfo_Bedno();
					if(B_PRINTF) printf("3----set patient info :BED NO %s \n",gCfgPatient.sBed);
					
				}break;
				case NET_PAT_DOCTOR:{///4
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sDoctor, sizeof(gCfgPatient.sDoctor),"%s" ,strMsg);
					}
					SetValueToEtcFile(gFileSetup, "PatientSetup", "doctor",  gCfgPatient.sDoctor);
					NetSend_PatientInfo_Doctor();
					if(B_PRINTF) printf("4----set patient info :DOCTOR  %s \n",gCfgPatient.sDoctor);
					
				}break;
				case NET_PAT_NAME:{//5
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sName, sizeof(gCfgPatient.sName),"%s" ,strMsg);
					}

					SetValueToEtcFile(gFileSetup, "PatientSetup", "name",  gCfgPatient.sName);
					NetSend_PatientInfo_Name();
					if(B_PRINTF) printf("5----set patient info :NAME  %s\n",gCfgPatient.sName);
					
				}break;
				case NET_PAT_SEX:{//6
					BYTE male[] = "MALE";
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						if(cmp_str(strMsg,male) == 0)
							gCfgPatient.bSex = 0;	//female
						else
							gCfgPatient.bSex = 1;	//male
						
					}
					SetIntValueToResFile(gFileSetup, "PatientSetup", "sex",  gCfgPatient.bSex, 1);
					NetSend_PatientInfo_Sex();
					if(B_PRINTF) printf("6----set patient info :SEX(0:MALE, 1:FEMALE) %dn",gCfgPatient.bSex);
				}break;
				case NET_PAT_TYPE:{//7
					BYTE adult[] = "ADU";
					BYTE baby[] = "BABY";
					BYTE neo[] = "NEO";
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						if(cmp_str(strMsg,adult) == 0) gCfgPatient.bObject = 0;		
						else if(cmp_str(strMsg,neo) == 0) gCfgPatient.bObject = 2;
						else	gCfgPatient.bObject = 1;
					}
					SetIntValueToResFile(gFileSetup, "PatientSetup", "sex",  gCfgPatient.bSex, 1);
					NetSend_PatientInfo_Type();
					if(B_PRINTF) printf("7----set patient info :PAT TYPE(0:ADU, 1:PED, 2:BABY, 3:NEO) %d \n", gCfgPatient.bObject );
				}break;
				case NET_PAT_ADMIT:{//8
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sAdmit, sizeof(gCfgPatient.sAdmit),"%s" ,strMsg);
					}
					SetValueToEtcFile(gFileSetup, "PatientSetup", "admit",  gCfgPatient.sAdmit);
					NetSend_PatientInfo_Admit();
					if(B_PRINTF) printf("8----set patient info :ADMIT(20081007)  %s # %s #%d\n",gCfgPatient.sAdmit,strMsg,msg_count);
				}break;
				case NET_PAT_BIRTH:{//9
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						snprintf(gCfgPatient.sBirth, sizeof(gCfgPatient.sBirth),"%s" ,strMsg);
					}
					SetValueToEtcFile(gFileSetup, "PatientSetup", "birth",  gCfgPatient.sBirth);
					NetSend_PatientInfo_Birth();
					if(B_PRINTF) printf("9----set patient info :BIRTH(20081007) %s \n",gCfgPatient.sBirth);
				}break;
				case NET_PAT_HEIGHT:{//10
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						gCfgPatient.wHeight = atoi(strMsg);
					}
					SetIntValueToResFile(gFileSetup, "PatientSetup", "height",  gCfgPatient.wHeight, 3);
					NetSend_PatientInfo_Height();
					if(B_PRINTF) printf("10----set patient info :HEIGHT %d \n",gCfgPatient.wHeight );
				}break;
				case NET_PAT_WEIGHT:{//11
					
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						gCfgPatient.wWeight = atoi(strMsg);
					}
					SetIntValueToResFile(gFileSetup, "PatientSetup", "weight",  gCfgPatient.wWeight, 3);
					NetSend_PatientInfo_Weight();
					if(B_PRINTF) printf("11----set patient info :WEIGHT  %d\n",gCfgPatient.wWeight );
				}break;
				case NET_PAT_BLOOD:{//12
					BYTE blood_a[]="A";
					BYTE blood_b[]="B";
					BYTE blood_o[]="O";
					BYTE blood_ab[]="AB";
					msg_count=Get_msg_from_net(pPack,strMsg);
					if(msg_count<LENGTH_STR_MAX){
						if(cmp_str(strMsg,blood_a) == 0) gCfgPatient.bBlood = 0;		
						else if(cmp_str(strMsg,blood_b) == 0) gCfgPatient.bBlood = 1;
						else	 if(cmp_str(strMsg,blood_o) == 0) gCfgPatient.bBlood = 2;
						else	 if(cmp_str(strMsg,blood_ab) == 0) gCfgPatient.bBlood = 3;
						else	gCfgPatient.bBlood = 4;
					}
					SetIntValueToResFile(gFileSetup, "PatientSetup", "blood",  gCfgPatient.bBlood, 1);
					NetSend_PatientInfo_Blood();
					if(B_PRINTF) printf("12----set patient info :BLOOD(0:A, 1:B, 2:O, 3:AB, 4 N/A) %d \n",gCfgPatient.bBlood);
				}break;
				/*************ECG info***************/	
				case NET_ECG_ALL:{//100 ask ECG Info
					NetSend_EcgInfo();
				}break;
				case NET_ECG_ALMLEV:{//101 Set ECG Alarm Class
					switch(wInfoValue){
						case 0: gCfgEcg.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgEcg.bAlmControl = ALARM_MID;break;
						case 2: gCfgEcg.bAlmControl = ALARM_LOW;break;
						default: gCfgEcg.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
					
					IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
					IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
					IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
					
					//Send To Central
					NetSend_EcgInfo();
				}break;
				case NET_ECG_HR_HI:{//102 Set High Alarm Limit of HR
					if(wInfoValue<=gCfgEcg.wLow_HR) wInfoValue = gCfgEcg.wLow_HR+1;
					if(wInfoValue>HR_MAX) wInfoValue = HR_MAX;
					gCfgEcg.wHigh_HR = wInfoValue;	
					//Write To File
					SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_high",  gCfgEcg.wHigh_HR, 3);
					//Send To Central
					NetSend_EcgInfo();
				}break;
				case NET_ECG_HR_LOW:{//103 Set Low Alarm Limit of HR
					if(wInfoValue < HR_MIN) wInfoValue = HR_MIN;
					if(wInfoValue>= gCfgEcg.wHigh_HR) wInfoValue = gCfgEcg.wHigh_HR-1;
					gCfgEcg.wLow_HR = wInfoValue;	
					//Write To File
					SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_low",  gCfgEcg.wLow_HR, 3);
					//Send To Central
					NetSend_EcgInfo();
				}break;
				case NET_ECG_LEADTYPE:{//104 Set ECG Lead Type  TODO:
					//Send To Central
					printf("Ecglead type :%d\n",wInfoValue);
					//Set_ECGLeadType();
					NetSend_EcgInfo();
				}break;
				case NET_ECG_MONTYPE:{//105 Set the type of monitoring ECG
				/*
					switch(wInfoValue){
						case 0:{
							gCfgEcg.bMode = ECGMODE_OPE;
							gCfgEcg.bFilterAC = TRUE;	
							gCfgEcg.bFilterDrift = TRUE;
						//	SetEcgACFilter();
						//	SetEcgDriftFilter();
							
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_ac",  gCfgEcg.bFilterAC, 1);
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_drift",  gCfgEcg.bFilterDrift, 1);
						}  break;
						case 1:{
							gCfgEcg.bMode = ECGMODE_MON; 
							gCfgEcg.bFilterAC = TRUE;	
							gCfgEcg.bFilterDrift = TRUE;
						//	SetEcgACFilter();
						//	SetEcgDriftFilter();
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_ac",  gCfgEcg.bFilterAC, 1);
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_drift",  gCfgEcg.bFilterDrift, 1);
						}break;
						default: gCfgEcg.bMode = ECGMODE_DIAG; break;
						
						
					}
					*/
					
					//����ECG��ⷽʽ
					SetiBioECGMode();
					//Write To File
					SetIntValueToResFile(gFileSetup, "ECGSetup", "mode",  gCfgEcg.bMode, 1);
					//Send To Central
					NetSend_EcgInfo();
				}break;
				/*************spo2 info***************/	
				case NET_SPO2_ALL:{//200 ask SpO2 Info
					NetSend_SpO2Info();
				}break;
				case NET_SPO2_ALMLEV:{//201 Set Alarm Class of SpO2
					switch(wInfoValue){
						case 0: gCfgSpO2.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgSpO2.bAlmControl = ALARM_MID;break;
						case 2: gCfgSpO2.bAlmControl = ALARM_LOW;break;
						default:gCfgSpO2.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_control",  gCfgSpO2.bAlmControl, 1);
					//Send To Central
					NetSend_SpO2Info();
				}break;
				case NET_SPO2_HI:{//202 Set SpO2 HighLimit
					if(wInfoValue <= gCfgSpO2.bLow_SpO2) wInfoValue = gCfgSpO2.bLow_SpO2+1;
					if(wInfoValue >SPO2_MAX) wInfoValue = SPO2_MAX;
					gCfgSpO2.bHigh_SpO2 = wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_high",  gCfgSpO2.bHigh_SpO2, 3);
					//Send To Central
					NetSend_SpO2Info();
				}break;
				case NET_SPO2_LOW:{//203 Set SpO2 Low Limit
					if(wInfoValue < SPO2_MIN) wInfoValue = SPO2_MIN;
					if(wInfoValue >=gCfgSpO2.bHigh_SpO2) wInfoValue = gCfgSpO2.bHigh_SpO2-1;
					gCfgSpO2.bLow_SpO2 = wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_low",  gCfgSpO2.bLow_SpO2, 3);
					//Send To Central
					NetSend_SpO2Info();
				}break;
				case NET_SPO2_PR_HI:{//204 Set PR HighLimit
					if(wInfoValue <= gCfgPulse.wLow_PR) wInfoValue = gCfgPulse.wLow_PR+1;
					if(wInfoValue >PR_MAX) wInfoValue = PR_MAX;
					gCfgPulse.wHigh_PR =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_high",  gCfgPulse.wHigh_PR, 3);
					//Send To Central
					NetSend_SpO2Info();
				}break;
				case NET_SPO2_PR_LOW:{//205 Set PR Low Limit
					if(wInfoValue < PR_MIN) wInfoValue = PR_MIN;
					if(wInfoValue >=gCfgPulse.wHigh_PR) wInfoValue = gCfgPulse.wHigh_PR-1;
					gCfgPulse.wLow_PR = wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_low",  gCfgPulse.wLow_PR, 3);
					//Send To Central
					NetSend_SpO2Info();
				}break;
				/*************resp info***************/	
				case NET_RESP_ALL:{//300 sk Resp Info
					NetSend_RespInfo();
				}break;
				case NET_RESP_ALMLEV:{//301 Set Alarm Class of Resp
					switch(wInfoValue){
						case 0: gCfgResp.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgResp.bAlmControl = ALARM_MID;break;
						case 2: gCfgResp.bAlmControl = ALARM_LOW;break;
						default:gCfgResp.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_control",  gCfgResp.bAlmControl, 1);
					//Send To Central
					NetSend_RespInfo();
				}break;
				case NET_RESP_RR_HI:{//302 Set RR High Limit
					if(wInfoValue <= gCfgResp.wLow_RR) wInfoValue = gCfgResp.wLow_RR +1;
					if(wInfoValue >RR_MAX) wInfoValue = RR_MAX;
					gCfgResp.wHigh_RR =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_high",  gCfgResp.wHigh_RR, 3);
					//Send To Central
					NetSend_RespInfo();
				}break;
				case NET_RESP_RR_LOW:{//303 Set RR Low Limit
					if(wInfoValue >= gCfgResp.wHigh_RR) wInfoValue = gCfgResp.wHigh_RR-1;
					if(wInfoValue <RR_MIN) wInfoValue = RR_MIN;
					gCfgResp.wLow_RR =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_low",  gCfgResp.wLow_RR, 3);
					//Send To Central
					NetSend_RespInfo();
				}break;
				case NET_RESP_APNEA:{//304 Apnea Time  TODO
					//Send To Central
					NetSend_RespInfo();
				}break;
				/*************nibp info***************/		
				case NET_NIBP_ALL:{//400 ask NIBP Info
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_ALMLEV:{//401 Set Alarm Class of NIBP
					switch(wInfoValue){
						case 0: gCfgNibp.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgNibp.bAlmControl = ALARM_MID;break;
						case 2: gCfgNibp.bAlmControl = ALARM_LOW;break;
						default:gCfgNibp.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_SYS_HI:{//402 Set SYS High Limit
					if(wInfoValue <= gCfgNibp.wLow_Sys) wInfoValue = gCfgNibp.wLow_Sys+1;
					if(wInfoValue >NIBP_MAX) wInfoValue = NIBP_MAX;
					gCfgNibp.wHigh_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_high",  gCfgNibp.wHigh_Sys, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_SYS_LOW:{//403 Set SYS Low Limit
					if(wInfoValue < NIBP_MIN) wInfoValue = NIBP_MIN;
					if(wInfoValue >=gCfgNibp.wHigh_Sys) wInfoValue = gCfgNibp.wHigh_Sys-1;
					gCfgNibp.wLow_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_low",  gCfgNibp.wLow_Sys, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_MAP_HI:{//404 Set MAP High Limit
					if(wInfoValue <= gCfgNibp.wLow_Mean) wInfoValue = gCfgNibp.wLow_Mean+1;
					if(wInfoValue >NIBP_MAX) wInfoValue = NIBP_MAX;
					gCfgNibp.wHigh_Mean =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "mean_high",  gCfgNibp.wHigh_Mean, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_MAP_LOW:{//405 Set MAP Low Limit
					if(wInfoValue < NIBP_MIN) wInfoValue = NIBP_MIN;
					if(wInfoValue >=gCfgNibp.wHigh_Mean) wInfoValue = gCfgNibp.wHigh_Mean-1;
					gCfgNibp.wLow_Mean =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "mean_low",  gCfgNibp.wLow_Mean, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_DIA_HI:{//406 Set Dia High Limit
					if(wInfoValue <= gCfgNibp.wLow_Dia) wInfoValue = gCfgNibp.wLow_Dia+1;
					if(wInfoValue >NIBP_MAX) wInfoValue = NIBP_MAX;
					gCfgNibp.wHigh_Dia =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_high",  gCfgNibp.wHigh_Dia, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_DIA_LOW:{//407 Set DIA Low Limit
					if(wInfoValue < NIBP_MIN) wInfoValue = NIBP_MIN;
					if(wInfoValue >=gCfgNibp.wHigh_Dia) wInfoValue = gCfgNibp.wHigh_Dia-1;
					gCfgNibp.wLow_Dia=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_low",  gCfgNibp.wLow_Dia, 4);
					//Send To Central
					NetSend_NibpInfo();
				}break;
				case NET_NIBP_INTERVAL:{//408 Inter of  NIBP Auto Testing  TODO
					//Send To Central
					NetSend_NibpInfo();
				}break;
				
				/*************temp1 info***************/	
				case NET_TEMP1_ALL:{//500 ask TEMP1 Info
					NetSend_Temp1Info();
				}break;
				case NET_TEMP1_ALMLEV:{//501 Set Alarm Class of Temp1
					switch(wInfoValue){
						case 0: gCfgTemp.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgTemp.bAlmControl = ALARM_MID;break;
						case 2: gCfgTemp.bAlmControl = ALARM_LOW;break;
						default:gCfgTemp.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
					//Send To Central
					NetSend_Temp1Info();
				}break;
				case NET_TEMP1_HI:{//502 Set Temp1 High Limit
					if(wInfoValue <= gCfgTemp.wLow_T1) wInfoValue = gCfgTemp.wLow_T1+1;
					if(wInfoValue >TEMP_MAX) wInfoValue = TEMP_MAX;
					gCfgTemp.wHigh_T1 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_high",  gCfgTemp.wHigh_T1, 3);
					//Send To Central
					NetSend_Temp1Info();
				}break;
				case NET_TEMP1_LOW:{//503 Set Temp1 Low Limit
					if(wInfoValue >= gCfgTemp.wHigh_T1) wInfoValue = gCfgTemp.wHigh_T1-1;
					if(wInfoValue <TEMP_MIN) wInfoValue = TEMP_MIN;
					gCfgTemp.wLow_T1 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_low",  gCfgTemp.wLow_T1, 3);
					//Send To Central
					NetSend_Temp1Info();
				}break;

				/*************temp2 info***************/	
				case NET_TEMP2_ALL:{//600 ask TEMP2 Info
					NetSend_Temp2Info();
				}break;
				case NET_TEMP2_ALMLEV:{//601 Set Alarm Class of Temp2
					switch(wInfoValue){
						case 0: gCfgTemp.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgTemp.bAlmControl = ALARM_MID;break;
						case 2: gCfgTemp.bAlmControl = ALARM_LOW;break;
						default:gCfgTemp.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
					//Send To Central
					NetSend_Temp2Info();
				}break;
				case NET_TEMP2_HI:{//602 Set Temp2 High Limit
					if(wInfoValue <= gCfgTemp.wLow_T2) wInfoValue = gCfgTemp.wLow_T2+1;
					if(wInfoValue >TEMP_MAX) wInfoValue = TEMP_MAX;
					gCfgTemp.wHigh_T2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_high",  gCfgTemp.wHigh_T2, 3);
					//Send To Central
					NetSend_Temp2Info();
				}break;
				case NET_TEMP2_LOW:{//603 Set Temp2 Low Limit
					if(wInfoValue >= gCfgTemp.wHigh_T2) wInfoValue = gCfgTemp.wHigh_T2-1;
					if(wInfoValue <TEMP_MIN) wInfoValue = TEMP_MIN;
					gCfgTemp.wLow_T2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_low",  gCfgTemp.wLow_T2, 3);
					//Send To Central
					NetSend_Temp2Info();
				}break;
				/*************co2 info***************/	
				case NET_CO2_ALL:{//700 ask CO2 Info
					NetSend_CO2Info();
				}break;
				case NET_CO2_ALMLEV:{//701 Set Alarm Class of CO2
					switch(wInfoValue){
						case 0: gCfgCO2.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgCO2.bAlmControl = ALARM_MID;break;
						case 2: gCfgCO2.bAlmControl = ALARM_LOW;break;
						default:gCfgCO2.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_control",  gCfgCO2.bAlmControl, 1);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_ETCO2_HI:{//702 Set EtCo2  High Limit
					wInfoValue = wInfoValue/10;
					if(wInfoValue <= gCfgCO2.wLow_EtCO2) wInfoValue = gCfgCO2.wLow_EtCO2+1;
					if(wInfoValue >CO2_MAX) wInfoValue = CO2_MAX;
					gCfgCO2.wHigh_EtCO2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "etco2_high",  gCfgCO2.wHigh_EtCO2, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_ETCO2_LOW:{//703 Set EtCo2  Low Limit
					wInfoValue = wInfoValue/10;
					if(wInfoValue >= gCfgCO2.wHigh_EtCO2 ) wInfoValue = gCfgCO2.wHigh_EtCO2-1;
					if(wInfoValue <CO2_MIN) wInfoValue = CO2_MIN;
					gCfgCO2.wLow_EtCO2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "etco2_low",  gCfgCO2.wLow_EtCO2, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_FICO2_HI:{//704 Set FiCo2  High Limit
					wInfoValue = wInfoValue/10;
					if(wInfoValue < CO2_MIN) wInfoValue = CO2_MIN;
					if(wInfoValue >CO2_MAX) wInfoValue = CO2_MAX;
					gCfgCO2.wHigh_FiCO2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "fico2_high",  gCfgCO2.wHigh_FiCO2, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_FICO2_LOW:{//705 Set FiCo2  low Limit
					wInfoValue = wInfoValue/10;
					if(wInfoValue < CO2_MIN) wInfoValue = CO2_MIN;
					if(wInfoValue >CO2_MAX) wInfoValue = CO2_MAX;
					gCfgCO2.wLow_FiCO2 =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "fico2_low",  gCfgCO2.wLow_FiCO2, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_AWRR_HI:{//706 Set awRR  High Limit
					if(wInfoValue <= gCfgCO2.wLow_awRR) wInfoValue = gCfgCO2.wLow_awRR+1;
					if(wInfoValue >RR_MAX) wInfoValue = RR_MAX;
					gCfgCO2.wHigh_awRR=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "awrr_high",  gCfgCO2.wHigh_awRR, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				case NET_CO2_AWRR_LOW:{//707 Set awRR  Low Limit
					if(wInfoValue >= gCfgCO2.wHigh_awRR) wInfoValue = gCfgCO2.wHigh_awRR-1;
					if(wInfoValue <RR_MIN) wInfoValue = RR_MIN;
					gCfgCO2.wLow_awRR=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "CO2Setup", "awrr_low",  gCfgCO2.wHigh_awRR, 4);
					//Send To Central
					NetSend_CO2Info();
				}break;
				/*************IBP1 info***************/	
				case NET_IBP1_ALL:{//800 ask IBP1 Info
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_ALMLEV:{//801 Set Alarm Class of IBP1
					switch(wInfoValue){
						case 0: gCfgIbp1.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgIbp1.bAlmControl = ALARM_MID;break;
						case 2: gCfgIbp1.bAlmControl = ALARM_LOW;break;
						default:gCfgIbp1.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_control",  gCfgIbp1.bAlmControl, 1);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_SYS_HI:{//802 Set SYS High Limit   TODO �˴��������ܳ�����ͬ
					if(wInfoValue <= gCfgIbp1.iLow_Sys) wInfoValue = gCfgIbp1.iLow_Sys+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp1.iHigh_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_high",  gCfgIbp1.iHigh_Sys, 4);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_SYS_LOW:{//803 Set SYS Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp1.iHigh_Sys) wInfoValue = gCfgIbp1.iHigh_Sys-1;
					gCfgIbp1.iLow_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_low",  gCfgIbp1.iLow_Sys, 4);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_MAP_HI:{//804 Set MAP High Limit
					if(wInfoValue <= gCfgIbp1.iLow_Mean) wInfoValue = gCfgIbp1.iLow_Mean+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp1.iHigh_Mean=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_high",  gCfgIbp1.iHigh_Mean, 4);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_MAP_LOW:{//805 Set MAP Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp1.iHigh_Mean) wInfoValue = gCfgIbp1.iHigh_Mean-1;
					gCfgIbp1.iLow_Mean=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_low",  gCfgIbp1.iLow_Mean, 4);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_DIA_HI:{//806 Set Dia High Limit
					if(wInfoValue <= gCfgIbp1.iLow_Dia) wInfoValue = gCfgIbp1.iLow_Dia+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp1.iHigh_Dia =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_high",  gCfgIbp1.iHigh_Dia, 4);
					//Send To Central
					NetSend_Ibp1Info();
				}break;
				case NET_IBP1_DIA_LOW:{//807 Set DIA Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp1.iHigh_Dia) wInfoValue = gCfgIbp1.iHigh_Dia-1;
					gCfgIbp1.iLow_Dia=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_low",  gCfgIbp1.iLow_Dia, 4);
					//Send To Central
					NetSend_Ibp1Info();
					
				}break;
				/*************IBP2 info***************/
				case NET_IBP2_ALL:{//900 ask IBP2 Info
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_ALMLEV:{//901 Set Alarm Class of IBP2
					switch(wInfoValue){
						case 0: gCfgIbp2.bAlmControl = ALARM_HIGH;break;
						case 1: gCfgIbp2.bAlmControl = ALARM_MID;break;
						case 2: gCfgIbp2.bAlmControl = ALARM_LOW;break;
						default:gCfgIbp2.bAlmControl = ALARM_OFF;break;
					}
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "alarm_control",  gCfgIbp2.bAlmControl, 1);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_SYS_HI:{//902 Set SYS High Limit   TODO �˴��������ܳ�����ͬ
					if(wInfoValue <= gCfgIbp2.iLow_Sys) wInfoValue = gCfgIbp2.iLow_Sys+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp2.iHigh_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "sys_high",  gCfgIbp2.iHigh_Sys, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_SYS_LOW:{//903 Set SYS Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp2.iHigh_Sys) wInfoValue = gCfgIbp2.iHigh_Sys-1;
					gCfgIbp2.iLow_Sys =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "sys_low",  gCfgIbp2.iLow_Sys, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_MAP_HI:{//904 Set MAP High Limit
					if(wInfoValue <= gCfgIbp2.iLow_Mean) wInfoValue = gCfgIbp2.iLow_Mean+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp2.iHigh_Mean=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "mean_high",  gCfgIbp2.iHigh_Mean, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_MAP_LOW:{//905 Set MAP Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp2.iHigh_Mean) wInfoValue = gCfgIbp2.iHigh_Mean-1;
					gCfgIbp2.iLow_Mean=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "mean_low",  gCfgIbp2.iLow_Mean, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_DIA_HI:{//906 Set Dia High Limit
					if(wInfoValue <= gCfgIbp2.iLow_Dia) wInfoValue = gCfgIbp2.iLow_Dia+1;
					if(wInfoValue >IBP_MAX) wInfoValue = IBP_MAX;
					gCfgIbp2.iHigh_Dia =  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "dia_high",  gCfgIbp2.iHigh_Dia, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				case NET_IBP2_DIA_LOW:{//907 Set DIA Low Limit
					if(wInfoValue < IBP_MIN) wInfoValue = IBP_MIN;
					if(wInfoValue >=gCfgIbp2.iHigh_Dia) wInfoValue = gCfgIbp2.iHigh_Dia-1;
					gCfgIbp2.iLow_Dia=  wInfoValue;
					//Write To File
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "dia_low",  gCfgIbp2.iLow_Dia, 4);
					//Send To Central
					NetSend_Ibp2Info();
				}break;
				default:{
				}break;
			}
			
		}break;
		case NCMD_CHANGE_MODE:{//�л��໤ģʽ
			U8 uMode = pPack[7];
			
			switch(uMode){
				case 0:{
					gCfgEcg.bMode = ECGMODE_OPE;
					gCfgEcg.bFilterAC = TRUE;	
					gCfgEcg.bFilterDrift = TRUE;
				//	SetEcgACFilter();
				//	SetEcgDriftFilter();
					SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_ac",  gCfgEcg.bFilterAC, 1);
					SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_drift",  gCfgEcg.bFilterDrift, 1);
				}  break;
				case 1:{
					gCfgEcg.bMode = ECGMODE_MON; 
					gCfgEcg.bFilterAC = TRUE;	
					gCfgEcg.bFilterDrift = TRUE;
				//	SetEcgACFilter();
				//	SetEcgDriftFilter();
					SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_ac",  gCfgEcg.bFilterAC, 1);
					SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_drift",  gCfgEcg.bFilterDrift, 1);
				}break;
				default: gCfgEcg.bMode = ECGMODE_DIAG; break;
			}
			//����ECG��ⷽʽ
		//	SetECGMode();
			//Write To File
			SetIntValueToResFile(gFileSetup, "ECGSetup", "mode",  gCfgEcg.bMode, 1);
			//Send To Central
			NetSend_EcgInfo();
		}break;
		case NCMD_CHANGE_PAT:{//��������  TODO
		
		}break;
	}
	
	return 0;
}



//�����ж�������Ƿ�����
BOOL gbCentralConnetced = FALSE;

/*
	��������߳�
*/
static void  *ptProc_NetRecv(void *arg)
{
	int iLen; 
	fd_set readfs;
	struct timeval Timeout;	
	int fd ;
	int i;
	static int iNetError = 0;	
	static int OB_timeout[MAX_OB_IP]={0};
	BYTE bTmpIndex = 0;
	
	//ͨ���жϽ���������Ĺ㲥�����Ƿ�ʱ�������������״̬
	static struct timeval start_tv;
	struct timeval end_tv;
	static BOOL flag_time = TRUE;        //����start_tv
	static struct timeval start_tv_Bed;		//�������´��߻�״̬
	static struct timeval start_tv_Other;		//�������´��߻�״̬

	//��ʼ�������ṹ������
	for(i=0;i<MAX_OB_IP;i++){
		OB_Info[i].BedNo=0;
		OB_Info[i].FLAG=FALSE;
		memset(OB_Info[i].IPaddr, 0, sizeof OB_Info[i].IPaddr);
		OB_Info[i].timeout=0;
		OB_Info[i].MacNo=0;
		OB_Info[i].Status=0;
	}
	
	fd = gfdMonitorSocket;
	while(fd >0){
		FD_SET(fd, &readfs);
		Timeout.tv_sec =0;
		Timeout.tv_usec = 100;
	//	printf("select =%d\n",select(fd+1, &readfs, NULL, NULL, &Timeout) );
		if(select(fd+1, &readfs, NULL, NULL, &Timeout) > 0){
			
			memset(gsServerIP, 0, sizeof gsServerIP);
			giServerPort =0;
			memset(gbRecvBuff, 0, sizeof gbRecvBuff);
			iLen = NetRecvBuff(fd, gbRecvBuff, &giServerPort, gsServerIP,&gbIPNum);
	
			if(iLen>0){
    				if(B_PRINTF) printf("Recv from %s:%d.\n", gsServerIP, giServerPort);

				if(flag_time){
					gettimeofday(&start_tv, NULL);
					gettimeofday(&start_tv_Other, NULL);
					gettimeofday(&start_tv_Bed, NULL);
					flag_time = FALSE;
				}
				gettimeofday(&end_tv, NULL);
				
				//�˴��ж�����������Ƿ�ʱ	
				if((abs(end_tv.tv_sec - start_tv_Other.tv_sec))>3){
  				//	if(B_PRINTF) printf("----------------Net Error!   timeout %d,end_tv.tv_sec:%d,start_tv.tv_sec:%d.\n", abs(end_tv.tv_sec - start_tv.tv_sec),end_tv.tv_sec,start_tv.tv_sec);
					giOtherCentralMacNO = 0;
					gbOtherCentralStatus = FALSE;
				}
				//�˴��ж�����������Ƿ�ʱ	
				if((abs(end_tv.tv_sec - start_tv.tv_sec))>3){
  				//	if(B_PRINTF) printf("----------------Net Error!   timeout %d,end_tv.tv_sec:%d,start_tv.tv_sec:%d.\n", abs(end_tv.tv_sec - start_tv.tv_sec),end_tv.tv_sec,start_tv.tv_sec);
					giCentralMacNO = 0;
					gbCentralStatus = FALSE;
				}
				
			//�ж������������������3sδ���ӣ��ж�Ϊ���ߣ���λ
			    if((abs(end_tv.tv_sec - start_tv_Bed.tv_sec))>3){
					memset( gbOtherBedStatus, 0, sizeof gbOtherBedStatus);
					
					for(i=0;i<MAX_OB_IP;i++){
						if(OB_Info[i].FLAG){
						//	printf("-------- ip%s,timeout=%d,oldtimeout[i]=%d\n",OB_Info[i].IPaddr,OB_Info[i].timeout,OB_timeout[i]);
							if(OB_timeout[i]==OB_Info[i].timeout){
						//		printf("``````111111111111111111```clear time out ob ip%s,gsOB_IP:%s\n",OB_Info[i].IPaddr,gsOB_IP);
								if(strcmp(OB_Info[i].IPaddr,gsOB_IP)==0){
									gbOB_Status=FALSE;
									giOBMacNO = 0;
								//	gbOtherBedIndex=0;
								//	OB_ResetDraw();
								//	printf("11111 clean timout ip:%s gbOB_Status=%d\n",OB_Info[i].IPaddr,gbOB_Status);
								}
								if(OB_Info[i].MacNo==ob_bedlist[gbOtherBedIndex].MacNo){
									OB_Info[i].BedNo=0;
							//		OB_Info[i].FLAG=FALSE;
							//		memset(OB_Info[i].IPaddr, 0, sizeof OB_Info[i].IPaddr);
							//		OB_Info[i].timeout=0;
									OB_Info[i].Status=0;
									OB_ResetDraw();
									gbOtherBedIndex=0;
									gOB_RecvIPNum=0;
									
							//		giOBMacNO = 0;
							//	printf("22222 clean timout ip:%s gbOB_Status=%d\n",OB_Info[i].IPaddr,gbOB_Status);

								}
								OB_Info[i].FLAG=FALSE;
							}
								
						}
						OB_timeout[i]=OB_Info[i].timeout;
					}
					
					start_tv_Bed = end_tv;
				}
		
				//����Դ�������ж��Ƿ�Ϊ�����
				
				//ip:241~250  ����� 
		//		if(gbIPNum >240 && gbIPNum<=250) {
				if(gbRecvBuff[2]==NMACHINE_CENTER){
					gbCentralConnetced = TRUE;
 					iNetError = 0;
					//��������
					AnalysisPack(gbRecvBuff, iLen);	

       					start_tv = end_tv;
  					if(B_PRINTF) printf("----------------Central  %d.  Code %d.\n", gbRecvBuff[3], gbRecvBuff[6]);
				}
				//ip:other
				if(gbRecvBuff[2]==NMACHINE_OTHER){
					
					//��������
					AnalysisPack(gbRecvBuff, iLen);	

       					start_tv_Other= end_tv;
  				//	if(B_PRINTF) printf("----------------Central  %d.  Code %d.\n", gbRecvBuff[3], gbRecvBuff[6]);
				}
				
				//	ip :1~240  �໤�ǣ��㲥������
			//	if(gbIPNum>0&&gbIPNum<=240){
				if(gbRecvBuff[2]==NMACHINE_MULTI_PARA){
				//����������߻���״̬      

					 OB_Info[gbIPNum].BedNo=gbRecvBuff[11];
					 OB_Info[gbIPNum].FLAG=TRUE;
					 strcpy(OB_Info[gbIPNum].IPaddr,gsServerIP);
					 OB_Info[gbIPNum].MacNo=gbIPNum;
					 OB_Info[gbIPNum].timeout=(OB_Info[gbIPNum].timeout+1)%100; 
					OB_Info[gbIPNum].Status=gbRecvBuff[8];

				//	 if(B_PRINTF) printf("-----------------%d--BedNo:%d,Status=%d.\n",   OB_Info[gbIPNum].timeout,OB_Info[gbIPNum].BedNo,OB_Info[gbIPNum].Status);
	                		
				}	

				
			}
			else{
				if(B_PRINTF) printf("Net Recv failure!\n");
			}
		}
	}
	
	pthread_exit("Bye");
}

///////////////////////////// interface //////////////////////////////////
int CreatNetRecvThread()
{
	
	pthread_create(&ptID_NetRecv, NULL, ptProc_NetRecv, NULL);
	return 0;
}
