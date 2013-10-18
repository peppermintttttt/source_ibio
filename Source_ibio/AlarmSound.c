/*************************************************************************** 
 *	Module Name:	AlarmSound
 *
 *	Abstract:		����������ʾ
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-04 09:52:35
 ***************************************************************************/
/*
˼·:
	1����ʱ����(1s),��ѯ��ǰ�Ƿ񱨾��ͱ�������߼���
	2���Ա�������߼�����Ϊ�����͵ƹ������
*/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "AlarmManage.h"


//�������������߳�
static pthread_t ptAlarmSound;
//�����߳��ź���
static sem_t semAlarmSound;

//�߼�������� 10+4S 
#define TIMER_ALARMSOUND_HIGH		14
//�м�������� 25S
#define TIMER_ALARMSOUND_MIDDLE	25
//�ͼ�������� 25S
#define TIMER_ALARMSOUND_LOW		25

//�Ƿ���뱨����ͣ״̬
extern BOOL gbSuspending;

//���������߳�
static void *ThreadProcAlarmSound(void *arg)
{
	//֮ǰ�ı�������
	BYTE bPrevAlarmClass = ALARM_OFF;
	//��ǰ�ı�������
	BYTE bCurrentAlarmClass = ALARM_OFF;
	//�������������
	int iSoundCount = 0;
	int iSoundCountMax = 0;
	
	for(;;){
		sem_wait(&semAlarmSound);	
				
		//�õ���ǰ��������߼���
		bCurrentAlarmClass = GetHighestAlarm();
		
		//��֮ǰ�ļ���Ƚ�,�����ͬ,�����Ӽ�����,Ȼ����ݼ����������Ƿ���
		//����,�������յ�ǰ������,Ȼ�����ü�����
		if(bPrevAlarmClass != bCurrentAlarmClass){
			//���ü�����
			iSoundCount = 0;
			//���յ�ǰ������
			AlarmSound(bCurrentAlarmClass);
//  			if(B_PRINTF) printf("%s:%d Pre Class is %d. Current Class is %d. Sound\n", __FILE__, __LINE__, bPrevAlarmClass, bCurrentAlarmClass);
		}
		else{
			switch(bCurrentAlarmClass){
				case ALARM_HIGH:{
					iSoundCountMax = TIMER_ALARMSOUND_HIGH;
				}break;
				case ALARM_MID:{
					iSoundCountMax = TIMER_ALARMSOUND_MIDDLE;
				}break;
				case ALARM_LOW:{
					iSoundCountMax = TIMER_ALARMSOUND_LOW;
				}break;
				default:{
					iSoundCountMax = TIMER_ALARMSOUND_LOW;
				}break;
			}
			
			//�����������ʱ,���ռ�����,���ü�����,���������++
			if(iSoundCount >=iSoundCountMax){
				//���յ�ǰ������
				if(!gbSuspending)
					AlarmSound(bCurrentAlarmClass);
// 				if(B_PRINTF) printf("%s:%d Class is %d. Timer Count is %d. Sound\n", __FILE__, __LINE__, bCurrentAlarmClass, iSoundCount);		
				iSoundCount = 0;
			}
			else{
				iSoundCount ++;
			}
		}
		
		bPrevAlarmClass = bCurrentAlarmClass;
	}
}

//----------------- interface --------------------
/*
	�������������߳�
*/
int InitAlarmSound()
{
	int res;
	
	res = sem_init(&semAlarmSound, 0, 0);
	
	res = pthread_create(&ptAlarmSound, NULL, ThreadProcAlarmSound, NULL);
	
	return res;
}


/*
	���·����߳�(��ʱ������)
*/
int UpdateAlarmSound()
{
	sem_post(&semAlarmSound);
	return 0;
}
