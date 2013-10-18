/*************************************************************************** 
 *	Module Name:	DataManage
 *
 *	Abstract:	���ݹ������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-23 10:46:39
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "DataManage.h"
#include "Global.h"


//���ƶ��ж�д���ź���
static sem_t semQueEcg1;				
static sem_t semQueEcg2;
static sem_t semQueSpO2;	
static sem_t semQueResp;
static sem_t semQueCO2;
static sem_t semQueIbp1;
static sem_t semQueIbp2;
//��������
static sem_t semNetQueEcg1;
static sem_t semNetQueSpO2;	
static sem_t semNetQueResp;
//��������
static sem_t semOBQueEcg1;
extern sem_t semCalctime;
//���ݶ���		
QUEUE_SPO2 gQueSpO2;	
QUEUE_ECG gQueEcg1;		//Channel1
QUEUE_ECG gQueEcg2;		//Channel2
QUEUE_RESP gQueResp;	
QUEUE_CO2 gQueCO2;		
QUEUE_IBP gQueIbp1;			
QUEUE_IBP gQueIbp2;

QUEUE_ECG gNetQueEcg1;
QUEUE_SPO2 gNetQueSpO2;	
QUEUE_RESP gNetQueResp;

QUEUE_ECG gOBQueEcg1;


/*
	��ʼ�����ж�д�ź���
*/
static int InitSemForQueue()
{
	int res;
		
	res = sem_init(&semQueEcg1, 0, 1);
	res = sem_init(&semQueEcg2, 0, 1);
	res = sem_init(&semQueSpO2, 0, 1);
	res = sem_init(&semQueResp, 0, 1);
	res = sem_init(&semQueCO2, 0, 1);
	res = sem_init(&semQueIbp1, 0, 1);
	res = sem_init(&semQueIbp2, 0, 1);
	
	res = sem_init(&semNetQueEcg1, 0, 1);
	res = sem_init(&semNetQueSpO2, 0, 1);
	res = sem_init(&semNetQueResp, 0, 1);

	res = sem_init(&semOBQueEcg1, 0, 1);

	
	res=  sem_init(&semCalctime,0,1);
	return 0;
}


/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_Ecg1(QUEUE_ECG *pQueue, size_t size, S_ECG_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueEcg1);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semQueEcg1); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semQueEcg1);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_Ecg1(QUEUE_ECG *pQueue, S_ECG_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Ecg1(pQueue, size, NULL);
	}
	sem_wait(&semQueEcg1);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueEcg1);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_Ecg1(QUEUE_ECG *pQueue, size_t size)
{
	sem_wait(&semQueEcg1);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueEcg1);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_Ecg2(QUEUE_ECG *pQueue, size_t size, S_ECG_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueEcg2);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semQueEcg2); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semQueEcg2);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_Ecg2(QUEUE_ECG *pQueue, S_ECG_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Ecg2(pQueue, size, NULL);
	}
	sem_wait(&semQueEcg2);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueEcg2);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_Ecg2(QUEUE_ECG *pQueue, size_t size)
{
	sem_wait(&semQueEcg2);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueEcg2);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_SpO2(QUEUE_SPO2 *pQueue, size_t size, S_SPO2_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueSpO2);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_SPO2_VALUE));
		end = pQueue->tail;
		sem_post(&semQueSpO2); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_SPO2_VALUE));
		end = pQueue->tail;
		sem_post(&semQueSpO2);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_SpO2(QUEUE_SPO2 *pQueue, S_SPO2_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_SpO2(pQueue, size, NULL);
	}
	sem_wait(&semQueSpO2);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueSpO2);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_SpO2(QUEUE_SPO2 *pQueue, size_t size)
{
	sem_wait(&semQueSpO2);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueSpO2);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_Resp(QUEUE_RESP *pQueue, size_t size, S_RESP_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueResp);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_RESP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueResp); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_RESP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueResp);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_Resp(QUEUE_RESP *pQueue, S_RESP_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Resp(pQueue, size, NULL);
	}
	sem_wait(&semQueResp);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueResp);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_Resp(QUEUE_RESP *pQueue, size_t size)
{
	sem_wait(&semQueResp);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueResp);
}


/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_CO2(QUEUE_CO2 *pQueue, size_t size, S_CO2_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueCO2);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_CO2_VALUE));
		end = pQueue->tail;
		sem_post(&semQueCO2); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_CO2_VALUE));
		end = pQueue->tail;
		sem_post(&semQueCO2);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_CO2(QUEUE_CO2 *pQueue, S_CO2_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_CO2(pQueue, size, NULL);
	}
	sem_wait(&semQueCO2);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueCO2);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_CO2(QUEUE_CO2 *pQueue, size_t size)
{
	sem_wait(&semQueCO2);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueCO2);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_Ibp1(QUEUE_IBP *pQueue, size_t size, S_IBP_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueIbp1);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_IBP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueIbp1); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_IBP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueIbp1);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_Ibp1(QUEUE_IBP *pQueue, S_IBP_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Ibp1(pQueue, size, NULL);
	}
	sem_wait(&semQueIbp1);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueIbp1);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_Ibp1(QUEUE_IBP *pQueue, size_t size)
{
	sem_wait(&semQueIbp1);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueIbp1);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL QueDel_Ibp2(QUEUE_IBP *pQueue, size_t size, S_IBP_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semQueIbp2);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_IBP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueIbp2); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_IBP_VALUE));
		end = pQueue->tail;
		sem_post(&semQueIbp2);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int QueAdd_Ibp2(QUEUE_IBP *pQueue, S_IBP_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Ibp1(pQueue, size, NULL);
	}
	sem_wait(&semQueIbp2);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semQueIbp2);
	return pQueue->tail;
}

/*
	��ն���
*/
static void QueClean_Ibp2(QUEUE_IBP *pQueue, size_t size)
{
	sem_wait(&semQueIbp2);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semQueIbp2);
}

/*
	��ʼ������
*/
static int InitQueue()
{
	QueClean_Ecg1(&gQueEcg1, QUEUE_MAX_SERIAL);
	QueClean_Ecg2(&gQueEcg2, QUEUE_MAX_SERIAL);
	QueClean_SpO2(&gQueSpO2, QUEUE_MAX_SERIAL);
	QueClean_Resp(&gQueResp, QUEUE_MAX_SERIAL);
	QueClean_CO2(&gQueCO2, QUEUE_MAX_SERIAL);
	QueClean_Ibp1(&gQueIbp1, QUEUE_MAX_SERIAL);
	QueClean_Ibp2(&gQueIbp2, QUEUE_MAX_SERIAL);
	
	return 0;
}


//--------------- interface ------------------
/*
	��ʼ�����ݹ���	
*/
int InitDataManage()
{
	//��ʼ�����ж�д�ź���
	InitSemForQueue();
	
	return 0;
}

/*
	����Ecg����
*/
int ReceivingData_Ecg1(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	QueAdd_Ecg1(&gQueEcg1, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ecg����
*/
BOOL RetrieveData_Ecg1(PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_Ecg1(&gQueEcg1, QUEUE_MAX_SERIAL, pValue);
}

/*
	���ECG����
*/
int ClearData_Ecg1()
{
	QueClean_Ecg1(&gQueEcg1, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����Ecg����
*/
int ReceivingData_Ecg2(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	QueAdd_Ecg2(&gQueEcg2, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ecg����
*/
BOOL RetrieveData_Ecg2(PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_Ecg2(&gQueEcg2, QUEUE_MAX_SERIAL, pValue);
}

/*
	���ECG����
*/
int ClearData_Ecg2()
{
	QueClean_Ecg2(&gQueEcg2, QUEUE_MAX_SERIAL);
	
	return 0;
}


/*
	����SpO2����
*/
int ReceivingData_SpO2(const PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	QueAdd_SpO2(&gQueSpO2, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��SpO2����
*/
BOOL RetrieveData_SpO2(PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_SpO2(&gQueSpO2, QUEUE_MAX_SERIAL, pValue);
}

/*
	���SpO2����
*/
int ClearData_SpO2()
{
	QueClean_SpO2(&gQueSpO2, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����Resp����
*/
int ReceivingData_Resp(const PS_RESP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	QueAdd_Resp(&gQueResp, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Resp����
*/
BOOL RetrieveData_Resp(PS_RESP_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_Resp(&gQueResp, QUEUE_MAX_SERIAL, pValue);
}

/*
	���Resp����
*/
int ClearData_Resp()
{
	QueClean_Resp(&gQueResp, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����CO2����
*/
int ReceivingData_CO2(const PS_CO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	QueAdd_CO2(&gQueCO2, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��CO2����
*/
BOOL RetrieveData_CO2(PS_CO2_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_CO2(&gQueCO2, QUEUE_MAX_SERIAL, pValue);
}

/*
	���CO2����
*/
int ClearData_CO2()
{
	QueClean_CO2(&gQueCO2, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����Ibp1����
*/
int ReceivingData_Ibp1(const PS_IBP_VALUE pValue)
{

	if(pValue == NULL) return -1;
	
	QueAdd_Ibp1(&gQueIbp1, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ibp1����
*/
BOOL RetrieveData_Ibp1(PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_Ibp1(&gQueIbp1, QUEUE_MAX_SERIAL, pValue);
}

/*
	���Ibp1����
*/
int ClearData_Ibp1()
{
	QueClean_Ibp1(&gQueIbp1, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����Ibp2����
*/
int ReceivingData_Ibp2(const PS_IBP_VALUE pValue)
{
	
	if(pValue == NULL) return -1;
	
	QueAdd_Ibp2(&gQueIbp2, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ibp2����
*/
BOOL RetrieveData_Ibp2(PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return QueDel_Ibp2(&gQueIbp2, QUEUE_MAX_SERIAL, pValue);
}

/*
	���Ibp2����
*/
int ClearData_Ibp2()
{
	QueClean_Ibp2(&gQueIbp2, QUEUE_MAX_SERIAL);
	
	return 0;
}


static BOOL NetQueDel_Ecg1(QUEUE_ECG *pQueue, size_t size, S_ECG_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semNetQueEcg1);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueEcg1); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueEcg1);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int NetQueAdd_Ecg1(QUEUE_ECG *pQueue, S_ECG_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		NetQueDel_Ecg1(pQueue, size, NULL);
	}
	sem_wait(&semNetQueEcg1);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semNetQueEcg1);
	return pQueue->tail;
}

/*
	��ն���
*/
static void NetQueClean_Ecg1(QUEUE_ECG *pQueue, size_t size)
{
	sem_wait(&semNetQueEcg1);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semNetQueEcg1);
}
/*
	�ӻ������ͷȡ������
*/
static BOOL NetQueDel_SpO2(QUEUE_SPO2 *pQueue, size_t size, S_SPO2_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semNetQueSpO2);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_SPO2_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueSpO2); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_SPO2_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueSpO2);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int NetQueAdd_SpO2(QUEUE_SPO2 *pQueue, S_SPO2_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_SpO2(pQueue, size, NULL);
	}
	sem_wait(&semNetQueSpO2);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semNetQueSpO2);
	return pQueue->tail;
}

/*
	��ն���
*/
static void NetQueClean_SpO2(QUEUE_SPO2 *pQueue, size_t size)
{
	sem_wait(&semNetQueSpO2);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semNetQueSpO2);
}

/*
	�ӻ������ͷȡ������
*/
static BOOL NetQueDel_Resp(QUEUE_RESP *pQueue, size_t size, S_RESP_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semNetQueResp);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_RESP_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueResp); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_RESP_VALUE));
		end = pQueue->tail;
		sem_post(&semNetQueResp);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int NetQueAdd_Resp(QUEUE_RESP *pQueue, S_RESP_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		QueDel_Resp(pQueue, size, NULL);
	}
	sem_wait(&semNetQueResp);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semNetQueResp);
	return pQueue->tail;
}

/*
	��ն���
*/
static void NetQueClean_Resp(QUEUE_RESP *pQueue, size_t size)
{
	sem_wait(&semNetQueResp);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semNetQueResp);
}

 /*
	��������ECG����
 */
int NetReceivingData_Ecg1(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	NetQueAdd_Ecg1(&gNetQueEcg1, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ecg����
*/
BOOL NetRetrieveData_Ecg1(PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return NetQueDel_Ecg1(&gNetQueEcg1, QUEUE_MAX_SERIAL, pValue);
}

/*
	���ECG����
*/
int NetClearData_Ecg1()
{
	NetQueClean_Ecg1(&gNetQueEcg1, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����SpO2����
*/
int NetReceivingData_SpO2(const PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	NetQueAdd_SpO2(&gNetQueSpO2, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��SpO2����
*/
BOOL NetRetrieveData_SpO2(PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return NetQueDel_SpO2(&gNetQueSpO2, QUEUE_MAX_SERIAL, pValue);
}

/*
	���SpO2����
*/
int NetClearData_SpO2()
{
	NetQueClean_SpO2(&gNetQueSpO2, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	����Resp����
*/
int NetReceivingData_Resp(const PS_RESP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	NetQueAdd_Resp(&gNetQueResp, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Resp����
*/
BOOL NetRetrieveData_Resp(PS_RESP_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return NetQueDel_Resp(&gNetQueResp, QUEUE_MAX_SERIAL, pValue);
}

/*
	���Resp����
*/
int NetClearData_Resp()
{
	NetQueClean_Resp(&gNetQueResp, QUEUE_MAX_SERIAL);
	
	return 0;
}

//other bed----------------
static BOOL OBQueDel_Ecg1(QUEUE_ECG *pQueue, size_t size, S_ECG_VALUE *pvalue)
{
	int start, end;
	
	start = pQueue->tail;
	
	if((pQueue->head+1)%size == pQueue->tail){//queue is empty
		return FALSE;
	}
	sem_wait(&semOBQueEcg1);
	if(size-1 == pQueue->head){
		pQueue->head = 0; 	//loop back
		if(pvalue !=NULL)	
			memcpy(pvalue, &(pQueue->values[size-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semOBQueEcg1); 
		return TRUE;
	} 
	else{
		pQueue->head ++;
		if(pvalue !=NULL)
			memcpy(pvalue, &(pQueue->values[pQueue->head-1]), sizeof(S_ECG_VALUE));
		end = pQueue->tail;
		sem_post(&semOBQueEcg1);
		return TRUE;
	}
}

/*
	������ݵ��������β
*/
static int OBQueAdd_Ecg1(QUEUE_ECG *pQueue, S_ECG_VALUE value, size_t size)
{
	int start, end;
		 
	if((pQueue->tail+1)%size == pQueue->head){
		NetQueDel_Ecg1(pQueue, size, NULL);
	}
	sem_wait(&semOBQueEcg1);
	start = pQueue->head;
	pQueue->values[pQueue->tail] = value;
	pQueue->tail ++;
	if(pQueue->tail == size) pQueue->tail = 0;	//loop back
	end = pQueue->head;
	sem_post(&semOBQueEcg1);
	return pQueue->tail;
}

/*
	��ն���
*/
static void OBQueClean_Ecg1(QUEUE_ECG *pQueue, size_t size)
{
	sem_wait(&semOBQueEcg1);
	if((pQueue->tail-1)<0) pQueue->head =size-1;
	else pQueue->head = pQueue->tail -1;	
	sem_post(&semOBQueEcg1);
}

 /*
	��������ECG����
 */
int OBReceivingData_Ecg1(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	OBQueAdd_Ecg1(&gOBQueEcg1, *pValue, QUEUE_MAX_SERIAL);
	
	return 0;
}

/*
	ȡ��Ecg����
*/
BOOL OBRetrieveData_Ecg1(PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return FALSE;
	
	return OBQueDel_Ecg1(&gOBQueEcg1, QUEUE_MAX_SERIAL, pValue);
}

/*
	���ECG����
*/
int OBClearData_Ecg1()
{
	OBQueClean_Ecg1(&gOBQueEcg1, QUEUE_MAX_SERIAL);
	
	return 0;
}


