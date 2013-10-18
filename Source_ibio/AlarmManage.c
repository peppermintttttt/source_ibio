/*************************************************************************** 
 *	Module Name:	AlarmManage
 *
 *	Abstract:		������Ϣ����
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-27 14:28:42
 ***************************************************************************/
/*
˼·��
	
	������
	a. ���汨����Ϣ��ԭ��
		1�����ֱ�����������������ʾ����
		2��ʹ���������棬��������ɾ��
		3���·���(�ı伶��Ҳ�����·���)�ı������жϵ�ǰ����ʾ˳��������ʾ
		4����1��2Hz��Ƶ��ѭ����ʾ���еı�����Ϣ

	b. ���������Ľӿں���
		1��AddAlarm()		����µı���		Lock-->��ѯ�����Ƿ��д˼�¼(N)-->����µļ�¼-->Unlock-->֪ͨ��ʾReset
		2��AlarmLifted()   	������� 		Lock-->��ѯ�����Ƿ��д˼�¼(Y)-->ɾ���˼�¼-->Unlock-->֪ͨ��ʾReset	
		3��ChangeAlarm()     	�ı䱨��		Lock-->��ѯ�����Ƿ��д˼�¼(Y)-->ɾ���˼�¼-->��¼���ĺ�ļ�����"�ر�"(Y)-->����¼�¼-->Unlock-->֪ͨ��ʾReset
*/
#include "assert.h"
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "dllist.h"
#include "AlarmManage.h"

//����������ݳɹ�
#define DL_FIND_OK		1	
//�����������ʧ��
#define DL_FIND_ERR	0	

//������������̻߳�����
pthread_mutex_t mtPhyAlm;	
//�������������̻߳�����
pthread_mutex_t mtTecAlm;	

//����������
DLLIST  *gDListAlmPhy;		
//������������
DLLIST  *gDListAlmTec;		

//�Ƿ���Ҫ������ʾ������Ϣ
BOOL bResetView_Phy = FALSE;
BOOL bResetView_Tec = FALSE;

//�Ƿ���뱨����ͣ״̬
extern BOOL gbSuspending;

//�Ƿ���״̬
extern BOOL gbSilence;


//------------------------------------ ���������һЩ�������� -----------------------------------------
/*
	��ӡ����������Ŀ
*/
static int ProcPrintAlmItems(int Tag, void *Memory, void *Args)
{

	B_ALM_INFO *source = Memory;
			
	assert(Tag == 0);
	
	if(source !=NULL){
//  		printf("(%s:%d)Read AlmID:%d,  Status:%d,  Class:%d  \n", __FILE__, __LINE__, 
//  			 source->bAlmID, source->bStatus, source->bClass);
	}
	else{
// 		printf("(%s:%d)source is NULL ........\n", __FILE__, __LINE__);
	}
	
	return 0;
}

/*
	�������в���ָ����¼
*/
static int ProcFindAlmItem(int Tag, void *Memory, void *Args)
{
	B_ALM_INFO *source = Memory;
	B_ALM_INFO *target = Args;
			
	assert(Tag == 0);
	
	if(source !=NULL){
		if(target !=NULL){
			if(source->bAlmID == target->bAlmID){
//    				printf("\n** Get AlmID:%d,  Status:%d,  Class:%d  \n\n", 
//   					 source->bAlmID, source->bStatus, source->bClass);
				return DL_FIND_OK;
			}	
		}
		else{
			if(B_PRINTF) printf("(%s:%d)target is NULL ........\n", __FILE__, __LINE__);
		}
	}
	else{
		if(B_PRINTF) printf("(%s:%d)source is NULL ........\n", __FILE__, __LINE__);
	}
	
	return DL_FIND_ERR;
}
//--------------------------------------------------------------------------------------------------------------------------
/*
	����һ���������������¼
*/
static int AddPhyAlarm(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	

	if(info.bAlmID == AP_NONE){
// 		if(B_PRINTF) printf("%s:%d Append Unkonwn Type Alarm.\n", __FILE__, __LINE__);
		return 0;	
	} 
		
	//����
	pthread_mutex_lock(&mtPhyAlm);

	//���ü�¼�Ƿ��Ѿ�����
	res = DLWalk((DLLIST *)gDListAlmPhy, &FindAddr, ProcFindAlmItem, &info);
	
	//����ü�¼������,������µļ�¼,���򷵻�
	if(res == DL_FIND_ERR){
		if(DL_SUCCESS != DLPrepend((DLLIST **)&gDListAlmPhy, 0, &info, sizeof(B_ALM_INFO))){
// 		if(DL_SUCCESS != DLAppend((DLLIST **)&gDListAlmPhy, 0, &info, sizeof(B_ALM_INFO))){
			if(B_PRINTF) printf("(%s:%d)Couldn't allocate enough memory.\n", __FILE__, __LINE__);
		}	
		else{
 			if(B_PRINTF) printf("(%s:%d)Add record at end of list.\n", __FILE__, __LINE__);	
		}
	}
	else{
 		if(B_PRINTF) printf("%s:%d The record has been in existence.\n", __FILE__, __LINE__);
		//����
		pthread_mutex_unlock(&mtPhyAlm);
		return -1;
	}

      //��ӡ����
//     	iListNum = DLCount((DLLIST *)gDListAlmPhy);
//     	printf("(%s:%d)List  has  %d Infos.\n\n", __FILE__, __LINE__, iListNum);		
//     	if(iListNum>0) res = DLWalk((DLLIST *)gDListAlmPhy, NULL, ProcPrintAlmItems, NULL);
		
	//����
	pthread_mutex_unlock(&mtPhyAlm);
	
	//������ʾ
	bResetView_Phy = TRUE;
	
	if(B_PRINTF) printf("%s:%d Add One PhyAlm Info.\n", __FILE__, __LINE__);
	
	return 0;
}

/*
	���һ��������
*/
static int PhyAlarmLifted(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *DelAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	DLLIST *SaveList;				//����������ʼλ��
	B_ALM_INFO *DelData=NULL;		//���ҵ��ļ�¼
	
	//����
	pthread_mutex_lock(&mtPhyAlm);
	
	//���ü�¼�Ƿ����
	res = DLWalk((DLLIST *)gDListAlmPhy, &DelAddr, ProcFindAlmItem, &info);
	
	//�������,ɾ���ü�¼,���򷵻�
	if(res == DL_FIND_OK){
		if(DelAddr != NULL){
			DelData = (B_ALM_INFO *)DLGetData(DelAddr, NULL, NULL);
			if(DelData != NULL){
				//���������ַ
				if(DelAddr->Prev !=NULL){
					gDListAlmPhy = DelAddr->Prev;
				}
				else if(DelAddr->Next !=NULL)
				{
					gDListAlmPhy = DelAddr->Next;
				} 
				else{
					gDListAlmPhy = NULL;
				}
				//ɾ��ԭ�е���Ϣ
				DLDelete((DLLIST *)DelAddr);
			}
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The Phy records (%d) do not exist.\n", __FILE__, __LINE__, info.bAlmID);
		//����
		pthread_mutex_unlock(&mtPhyAlm);
		return -1;
	}
	
	//��ӡ����
// 	iListNum = DLCount((DLLIST *)gDListAlmPhy);
// 	if(B_PRINTF) printf("(%s:%d)Del::List  has  %d Infos.\n", __FILE__, __LINE__, iListNum);		
// 	res = DLWalk((DLLIST *)gDListAlmPhy, NULL, ProcPrintAlmItems, NULL);
	
	//����
	pthread_mutex_unlock(&mtPhyAlm);
	
	//--- ������ʾ��־ ---
	bResetView_Phy = TRUE;

// 	if(B_PRINTF) printf("%s:%d Del One PhyAlm Info.\n", __FILE__, __LINE__);
	return 0;
}

/*
	�ı�һ���������ļ���
*/
static int ChangePhyAlarm(B_ALM_INFO info)
{
	int res;
	
	//ɾ��ָ���ļ�¼
	res = PhyAlarmLifted(info);
	
	//�����¼������(ɾ�����ɹ�)ֱ�ӷ���
	if(res != 0){
		return -1;
	}
	
	//���ɾ���ɹ�,�����Ŀǰ�ı������������µı�����¼
	//�����������ǹر�,ֱ�ӷ���
	if(info.bClass == ALARM_OFF){
		return -1;
	}
	
	//����µļ�¼
	AddPhyAlarm(info);
	
	return 0;
}

/*
	���ҵ�ǰ����������߼���
	return -1: error
	pInfo: ��ű�����¼��ϸ����
*/
static int GetPhyAlarmClass(PB_ALM_INFO pInfo)
{
	int res;
	int iListNum = 0;
	int iClass = -1;
	int i;
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	B_ALM_INFO *pFindInfo=NULL;		//���ҵ��ļ�¼
	B_ALM_INFO FindInfo;				//���ҵ��ļ�¼
		
	iListNum = DLCount((DLLIST *)gDListAlmPhy);
	
	if(iListNum <=0) iClass = -1;
	//��������,���Ҽ�����ߵļ�¼
	FindAddr = DLGetFirst(gDListAlmPhy);
	for(i=0; i<iListNum;i++){
		if(FindAddr != NULL){
// 			pthread_mutex_lock(&mtPhyAlm);
			pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
			FindInfo = *pFindInfo;
			FindAddr = FindAddr->Next;
// 			pthread_mutex_unlock(&mtPhyAlm);
			if(pFindInfo !=NULL){
				if(FindInfo.bClass == ALARM_INFO) FindInfo.bClass = ALARM_OFF;
				if(iClass < FindInfo.bClass){
					iClass = FindInfo.bClass;
					*pInfo = *pFindInfo;
				}
				//����鵽��һ���߼������ļ�¼,�Ͳ����ٲ���
				if(iClass >= ALARM_HIGH && iClass != ALARM_INFO){
					break;	
				}
			}
		}
		else{
			if(B_PRINTF) printf("%s:%d No Data Found.\n", __FILE__, __LINE__);
		}
				
	}
	
	return iClass;
}


/*
	����һ����������������¼
*/
static int AddTecAlarm(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ

	if(info.bAlmID == AT_NONE){
// 		if(B_PRINTF) printf("%s:%d Append Unkonwn Type Alarm.\n", __FILE__, __LINE__);
		return 0;	
	} 
		
	//����
	pthread_mutex_lock(&mtTecAlm);

	//���ü�¼�Ƿ��Ѿ�����
	res = DLWalk((DLLIST *)gDListAlmTec, &FindAddr, ProcFindAlmItem, &info);
	
	//����ü�¼������,������µļ�¼,���򷵻�
	if(res == DL_FIND_ERR){
		if(DL_SUCCESS != DLPrepend((DLLIST **)&gDListAlmTec, 0, &info, sizeof(B_ALM_INFO))){
//  			if(B_PRINTF) printf("(%s:%d)Couldn't allocate enough memory.\n", __FILE__, __LINE__);
		}	
		else{
// 			if(B_PRINTF) printf("(%s:%d)Add record at end of list.\n", __FILE__, __LINE__);	
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The record has been in existence.\n", __FILE__, __LINE__);
		//����
		pthread_mutex_unlock(&mtTecAlm);
		return -1;
	}
	

//		��ӡ����
// 	iListNum = DLCount((DLLIST *)gDListAlmTec);
// 	if(B_PRINTF) printf("(%s:%d)List  has  %d Infos.\n\n", __FILE__, __LINE__, iListNum);		
// 	if(iListNum>0) res = DLWalk((DLLIST *)gDListAlmTec, NULL, ProcPrintAlmItems, NULL);
		
	//����
	pthread_mutex_unlock(&mtTecAlm);
	
	//������ʾ
	bResetView_Tec = TRUE;
	
	if(B_PRINTF) printf("%s:%d Add One TecAlm Info.\n", __FILE__, __LINE__);
	
	return 0;
}

/*
	���һ����������
*/
static int TecAlarmLifted(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *DelAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	DLLIST *SaveList;				//����������ʼλ��
	B_ALM_INFO *DelData=NULL;		//���ҵ��ļ�¼
	
	//����
	pthread_mutex_lock(&mtTecAlm);
	
	//���ü�¼�Ƿ����
	res = DLWalk((DLLIST *)gDListAlmTec, &DelAddr, ProcFindAlmItem, &info);
	
	//�������,ɾ���ü�¼,���򷵻�
	if(res == DL_FIND_OK){
		if(DelAddr != NULL){
			DelData = (B_ALM_INFO *)DLGetData(DelAddr, NULL, NULL);
			if(DelData != NULL){
				//���������ַ
				if(DelAddr->Prev !=NULL){
					gDListAlmTec = DelAddr->Prev;
				}
				else if(DelAddr->Next !=NULL) {
					gDListAlmTec = DelAddr->Next;
				} 
				else{
					gDListAlmTec = NULL;
				}
				//ɾ��ԭ�е���Ϣ
				DLDelete((DLLIST *)DelAddr);
			}
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The Tec records (%d) do not exist.\n", __FILE__, __LINE__, info.bAlmID);
		//����
		pthread_mutex_unlock(&mtTecAlm);
		return -1;
	}
	
	//��ӡ����
// 	iListNum = DLCount((DLLIST *)gDListAlmTec);
// 	if(B_PRINTF) printf("(%s:%d)Del::List  has  %d Infos.\n", __FILE__, __LINE__, iListNum);		
// 	res = DLWalk((DLLIST *)gDListAlmTec, NULL, ProcPrintAlmItems, NULL);
	
	//����
	pthread_mutex_unlock(&mtTecAlm);
	
	//--- ������ʾ��־ ---
	bResetView_Tec = TRUE;

// 	if(B_PRINTF) printf("%s:%d Del One PhyAlm Info.\n", __FILE__, __LINE__);
	return 0;
}

/*
	�ı�һ�����������ļ���
*/

static int ChangeTecAlarm(B_ALM_INFO info)
{
	int res;
	
	//ɾ��ָ���ļ�¼
	res = TecAlarmLifted(info);
	
	//�����¼������(ɾ�����ɹ�)ֱ�ӷ���
	if(res != 0){
		return -1;
	}
	
	//���ɾ���ɹ�,�����Ŀǰ�ı������������µı�����¼
	//�����������ǹر�,ֱ�ӷ���
	if(info.bClass == ALARM_OFF){
		return -1;
	}
	
	//����µļ�¼
	AddTecAlarm(info);
	
	return 0;
}

/*
	���ҵ�ǰ������������߼���
	return -1: error
	pInfo: ��ű�����¼��ϸ����
*/
static int GetTecAlarmClass(PB_ALM_INFO pInfo)
{
	int res;
	int iListNum = 0;
	int iClass = -1;
	int i;
	DLLIST *FindAddr=NULL;			//���ҵ��ļ�¼�ĵ�ַ
	B_ALM_INFO *pFindInfo=NULL;		//���ҵ��ļ�¼
	B_ALM_INFO FindInfo;				//���ҵ��ļ�¼
		
	iListNum = DLCount((DLLIST *)gDListAlmTec);
	
	if(iListNum <=0) iClass = -1;
	
	//��������,���Ҽ�����ߵļ�¼
	FindAddr = DLGetFirst(gDListAlmTec);
	for(i=0; i<iListNum;i++){
		if(FindAddr != NULL){
// 			pthread_mutex_lock(&mtPhyAlm);
			pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
			FindInfo = *pFindInfo;
			FindAddr = FindAddr->Next;
// 			pthread_mutex_unlock(&mtPhyAlm);
			if(pFindInfo !=NULL){
				if(FindInfo.bClass == ALARM_INFO) FindInfo.bClass = ALARM_OFF;
				if(iClass < FindInfo.bClass){
					iClass = FindInfo.bClass;
					*pInfo = *pFindInfo;
				}
				//����鵽��һ���߼������ļ�¼,�Ͳ����ٲ���
				if(iClass >= ALARM_HIGH && iClass !=ALARM_INFO){
					break;	
				}
			}
		}
		else{
			if(B_PRINTF) printf("%s:%d No Data Found.\n", __FILE__, __LINE__);
		}
				
	}
	
	return iClass;
}


static int Test()
{
	int res;
	B_ALM_INFO testInfo;
	
	if(B_PRINTF) printf("\n---------------------------------Add Debug 00\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 01\n");
	testInfo.bAlmID = AP_HR_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_MID;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 02\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 03\n");
	testInfo.bAlmID = AP_TD_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 04\n");
	testInfo.bAlmID = AP_NSYS_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 05\n");
	testInfo.bAlmID = AP_NDIA_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 00\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_HIGH;
	res = PhyAlarmLifted(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 01\n");
	testInfo.bAlmID = AP_T1_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_HIGH;
	res = PhyAlarmLifted(testInfo);
//  	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 02\n");
	testInfo.bAlmID = AP_TD_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_LOW;
	res = PhyAlarmLifted(testInfo);
//  	sleep(1);
		
	res = GetPhyAlarmClass(&testInfo);
	if(B_PRINTF) printf("%s:%d  At present, the most alarm record is : Class(%d)  -----  ID-%d, Class-%d, Status-%d.\n", __FILE__, __LINE__, 
	       res, testInfo.bAlmID, testInfo.bClass, testInfo.bStatus);
	
	if(B_PRINTF) printf("\n---------------------------------Add Debug 00\n");
	testInfo.bAlmID = AT_ECGOFF_V;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 01\n");
	testInfo.bAlmID = AT_RESP_OVERRANGE;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_MID;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 02\n");
	testInfo.bAlmID = AT_SPO2_MODULEINITERR;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 03\n");
	testInfo.bAlmID = AT_SPO2_OVERRANGE;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddTecAlarm(testInfo);
// 	sleep(1);

	
	return 0;
}


//---------------------------- interface ----------------------------------
/*
	��ʼ����������
*/
int InitAlarmManage()
{
	int res;
	
	//��ʼ��������
	res = pthread_mutex_init(&mtPhyAlm, NULL);
	res = pthread_mutex_init(&mtTecAlm, NULL);
 	
	//��ʼ��������ʾ
 	InitAlarmView();
	
	//��ʼ����������
	InitAlarmSound();
	
	//��ʼ�������ƹ�
	InitAlarmLight();
	
// 	Test();
			
	return 0;
}

/*
	���һ����������Ϣ
*/
int AddAlarm_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
	//return 0;
	
	res = AddPhyAlarm(InfoAlm);

	if(res ==0){
		//TODO:����ɹ����ӵĻ�,�˴������ӱ����洢
		if(B_PRINTF) printf("%s:%d Add one alarm info. AlmID is %d, Class is %d. Print is %d.\n", __FILE__, __LINE__, 
		       InfoAlm.bAlmID, InfoAlm.bClass, InfoAlm.bPrint);
		SaveOneAlarmData(InfoAlm);
		
		//������ڱ�����ͣ״̬��,���Զ����������ͣ
		gbSuspending = FALSE;
		
		//������ھ���״̬����ȡ������
		gbSilence = FALSE;
	}

	return res;
}



/*
	ɾ��һ����������Ϣ
*/
int AlarmLifted_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
//  	return -1;
	
	res = PhyAlarmLifted(InfoAlm);
	
	return res;
}

/*
	�ı�һ���������ļ���
*/
int ChangeAlarm_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
	ChangePhyAlarm(InfoAlm);
			
	return -1;	
}

		
/*
	�õ���ǰ��������߼���ı���
	���ؼ���
*/
int GetHighestAlarm_Phy(PB_ALM_INFO pInfoAlm)
{
	int res;
	
	if(pInfoAlm == NULL) return -1;
	
	res = GetPhyAlarmClass(pInfoAlm);
	
	return res;
}

/*
	���һ������������Ϣ
*/
int AddAlarm_Tec(B_ALM_INFO InfoAlm)
{
	int res;
	
  	//return -1;
	
	res = AddTecAlarm(InfoAlm);

	//������ڱ�����ͣ״̬��,���Զ����������ͣ
	gbSuspending = FALSE;
	
	//������ھ���״̬����ȡ������
	gbSilence = FALSE;

	return res;
}

/*
	ɾ��һ������������Ϣ
*/
int AlarmLifted_Tec(B_ALM_INFO InfoAlm)
{
	int res;
	
// 	return -1;
	
	res = TecAlarmLifted(InfoAlm);
	
	return res;
}

/*
	�ı�һ���������ļ���
*/
int ChangeAlarm_Tec(B_ALM_INFO InfoAlm)
{
	int res;
		
	ChangeTecAlarm(InfoAlm);
			
	return -1;	
}


/*
	�õ���ǰ����������߼���ı���
	���ؼ���
*/
int GetHighestAlarm_Tec(PB_ALM_INFO pInfoAlm)
{
	int res;
	
	if(pInfoAlm == NULL) return -1;
	
	res = GetTecAlarmClass(pInfoAlm);
	
	return res;
}

/*
	�õ���ǰ����&����������߼���ı���
	���ؼ���
*/
int GetHighestAlarm()
// int GetHighestAlarm(PB_ALM_INFO pInfoAlm)
{
	int res, res_phy, res_tec;
	B_ALM_INFO info_phy;
	B_ALM_INFO info_tec;
	
	//�õ���߼����������
	res_phy = GetPhyAlarmClass(&info_phy);
// 	if(res_phy == ALARM_INFO) res_phy = ALARM_OFF;
	//�õ���߼���ļ�������
	res_tec = GetTecAlarmClass(&info_tec);
// 	if(res_tec == ALARM_INFO) res_tec = ALARM_OFF;
	//�Ƚ���������
	res = MAX(res_phy, res_tec);
	
// 	if(B_PRINTF) printf("%s:%d Current Highest Alarm Class is %d.\n", __FILE__, __LINE__, res);
	
	return res;
}
//����pause��ʱ���һ���Ա�����־λ
int Clean_Alarm()
{
	unsigned char buf;
	buf |= 0x07<<2;
	IsAlm_Power(&buf,FALSE);
//	printf("---------------clean alarm\n");
	return 0;
}

