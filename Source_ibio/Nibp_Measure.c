/*************************************************************************** 
 *	Module Name:	Nibp_Measure
 *
 *	Abstract:	NIBP��������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-27 11:09:40
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Nibp_Measure.h"
	
#define START_MAUNAL			0
#define START_AUTO				1
#define START_ATAT				2
#define START_STATIC			3
#define START_LEAK				4
#define START_PUMP				5
#define START_OVERPRESSURE	6
#define START_CAL				7


//���ڲ����߳�
pthread_t 	ptNibpAuto;
//���������߳�
pthread_t 	ptNibpStat;
//���������߳�
pthread_t 	ptNibpPuncture;
	
//���ڲ�����ʱ���ź���
static sem_t semNibpAuto;
//����������ʱ���ź���
static sem_t semNibpStat;
//���������ź���
static sem_t semNibpPuncture;

//�����Ƿ�ȡ��AUTO�����߳�
static BOOL bNibpAutoCancel = TRUE;
//�����Ƿ�ȡ��STAT�����߳�
static BOOL bNibpStatCancel = TRUE;
//�����Ƿ�ȡ�����̲����߳�
static BOOL bNibpPunctureCancel = TRUE;
	
//��λ�Զ�������ʱ
static BOOL bNibpAutoReset = TRUE;
//�Ƿ��Ѿ��������ڲ���
static BOOL bNibpAutoHasStart = FALSE;
	
//���ڲ���ʱ�� ��ʾͼ��ʱʹ��
static unsigned char *strAutoTime[]={"1min", "2min", "3min", "4min", "5min", "10min",
	"15min", "30min", "60min", "90min", "120min", "180min", "240min", "480min"};	
	
//������
extern HWND ghWndMain;
	
//��ʾ����
extern RECT gRCNibp_InterTime;

/**
	���Ƶ���ʱ��ͼ�̶Ⱥͽ���
	��λ:��
	bType: 0-Auto, 1-Stat, 2-Puncture
*/
int DrawNibpCycleGraph(int iTime, int iMax, BYTE bType)
{
	int iStep;
	
	return 0;
	
	if(iMax >0){
		iStep = (iTime * (gRCNibp_InterTime.right - gRCNibp_InterTime.left) )/ iMax;
		//���Ϊ�ѹ�ʱ��
		SetBrushColor(HDC_SCREEN, COLOR_darkblue);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, iStep, gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
		//�ұ�Ϊʣ��ʱ��
		SetBrushColor(HDC_SCREEN, COLOR_black);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left+iStep, gRCNibp_InterTime.top, gRCNibp_InterTime.right - (gRCNibp_InterTime.left+iStep), gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
		
		//ʣ��ʱ��
		SetTextColor(HDC_SCREEN, gCfgNibp.iColor);
		SetBkColor(HDC_SCREEN, COLOR_black);
		SelectFont(HDC_SCREEN, gFontSystem);
		
		if(bType ==1){
			//Stat
			DrawText(HDC_SCREEN, "5min", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if(bType ==2) {
	//	if(bType ==2) {
			//Puncture
			if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
				DrawText(HDC_SCREEN, "80s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
			else
				DrawText(HDC_SCREEN, "170s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
		}
		else{
			//Auto or STAT
			DrawText(HDC_SCREEN, strAutoTime[TransformTimeToIndex(gCfgNibp.wInterval)], -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		
		//���̶� : |__|__|__|__|
		SetPenColor(HDC_SCREEN, gCfgNibp.iColor);
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
				
		//Center
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.top);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		Rectangle(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
	}
	else{
		//����
		SetBrushColor(HDC_SCREEN, COLOR_black);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right - gRCNibp_InterTime.left+1, gRCNibp_InterTime.bottom - gRCNibp_InterTime.top+1);
	}
	return 0;
}

/**
	��ʼһ��NIBP����
 */
static int StartSingleNibpMeasure()
{
	//ȡ��״̬Ϊ��
	gValueNibp.bCanceled = FALSE;
	//��õ�ǰ������ʱ��
	GetSysTime(&(gValueNibp.sTime));
	//��ʼ����
	
//	StartiBioNibpMeasure();
	StartNibpMeasure(START_MAUNAL);
	return 0;
}
/**
	��ʼһ���Զ�����
 */
static int StartSingleAutoNibpMeasure()
{
	//ȡ��״̬Ϊ��
	gValueNibp.bCanceled = FALSE;
	//��õ�ǰ������ʱ��
	GetSysTime(&(gValueNibp.sTime));
	//��ʼ����
	
//	StartiBioNibpMeasure();
	StartNibpMeasure(START_AUTO);
	return 0;
}

/**
	ȡ������
 */
static int CancelSingleNibpMeasure()
{
	printf("stop ++++++++++\n");
	//ֹͣ����
	//CancelNibpMeasure();
//	StopiBioNibpMeasure();
	StopNibpMeasure();
	//ȡ������Ϊ��
	gValueNibp.bCanceled = TRUE;
	//����������
	gValueNibp.wErrCode = 0;
//	gValueNibp.bMeasureType=NIBP_TYPE_MANUAL;
	return 0;
}

/**
	��ʼ��������
*/
static int StartPuncture()
{
//	NibpLeakTest(gCfgNibp.wInitPres);

	return 0;
}

/**
	ȡ����������
*/
static int CancelPuncture()
{
//	CancelNibpMeasure();
	return 0;
}


//���ڲ�����ʱ�߳�
int giNibpTimeCount_Auto = 0;	//��ʱ��
static void *ThreadProcNibpAuto(void *arg)
{
// 	int iTimeCount = 0;	//��ʱ��
	int iTimeMax = 0;		//��ʱ����

	for(;;){
		sem_wait(&semNibpAuto);
				
		//ȡ���߳�
		if(bNibpAutoCancel){
			giNibpTimeCount_Auto = 0;
			iTimeMax = gCfgNibp.wInterval*60;
			DrawNibpCycleGraph(giNibpTimeCount_Auto, iTimeMax, 0);
			if(B_PRINTF) printf("(%s:%d)Cnacel AUTO Test.\n", __FILE__, __LINE__);
			break;
		} 
		
		//��λ��ʱ��
		if(bNibpAutoReset){
			giNibpTimeCount_Auto = 0;
			iTimeMax = gCfgNibp.wInterval*60;
			bNibpAutoReset = FALSE;
//  			continue;
		}
		else{
			if(giNibpTimeCount_Auto < iTimeMax){
				giNibpTimeCount_Auto ++;
//  				if(B_PRINTF) printf("%s:%d NIBP Auto Count is %d.\n", __FILE__, __LINE__, iTimeCount);
			}
			else{
				//��ʼһ��NIBP����
				StartSingleNibpMeasure();
				giNibpTimeCount_Auto = 0;
			}
		}
		DrawNibpCycleGraph(giNibpTimeCount_Auto, iTimeMax, 0);
// 		if(B_PRINTF) printf("(%s:%d)Running AUTO Test, MAX is %d, TimeCount is %d.\n", __FILE__, __LINE__, iTimeMax, giNibpTimeCount_Auto);
	}
	
	giNibpTimeCount_Auto = 0;
	sem_destroy(&semNibpAuto);
	if(B_PRINTF) printf("%s:%d Cancel NIBP Auto Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP Auto Measure thread.\n");
}


/**
	��ʼNIBP�Զ�����
*/
static int StartAutoNibpMeasure()
{
	int res;
	
	//��ʼһ��NIBP����
 	StartSingleAutoNibpMeasure();
	
	bNibpAutoCancel = FALSE; 
	res = sem_init(&semNibpAuto, 0, 0);
	res = pthread_create(&ptNibpAuto, NULL, ThreadProcNibpAuto, NULL);
	UpdateNibpAutoTimer(TRUE);
	
	bNibpAutoHasStart = TRUE;
	
	return 0;
}

 int StopNibpAutoMeasure_Error()
{
	if(bNibpAutoHasStart){
		bNibpAutoCancel = TRUE;
		sem_post(&semNibpAuto);
		bNibpAutoHasStart = FALSE;
	}
	return 0;
}

/**
	ȡ��NIBP�Զ�����
*/
static int CancelNibpAutoMeasure()
{

	if(B_PRINTF) printf("----------------Cancel NIBP .\n");
	bNibpAutoCancel = TRUE;
	sem_post(&semNibpAuto);
	//ȡ����ǰ�Ĳ���
	CancelSingleNibpMeasure();
			
	bNibpAutoHasStart = FALSE;
	return 0;
}

/**
	���������߳�
*/
int giNibpTimeCount_Stat = 0;	//��ʱ��
static void *ThreadProcNibpStat(void *arg)
{
// 	int iTimeCount = 0;	//��ʱ��
	int iTimeMax = NIBP_STAT_TIME;	//��ʱ����
	int iTimeInter = 0;	//���β���֮��ļ��
	
	//����һ��NIBP����
	StartSingleAutoNibpMeasure();	
	
	//��������λ
	giNibpTimeCount_Stat = 0;
	
	for(;;){
		sem_wait(&semNibpStat);
	
		//ȡ���߳�
		if(bNibpStatCancel){
			giNibpTimeCount_Stat = 0;
			iTimeMax = NIBP_STAT_TIME;
		//	iTimeMax = gCfgNibp.wInterval*60;
			DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax, 1);
			break;
		}
		if(giNibpTimeCount_Stat < iTimeMax){
			giNibpTimeCount_Stat ++;
 		//	if(B_PRINTF) printf("%s:%d NIBP Stat Count is %d.\n", __FILE__, __LINE__, iTimeCount);
			//�ж��ϴβ����Ƿ����,�������,����X�� �����´β���,�����������
		
			if(gValueNibp.bStatus== NIBP_SYSTEM_IDLE){
				//����ϴβ�������������ֹͣ����
				if(gValueNibp.wErrCode !=0) {
					if(B_PRINTF) printf("%s:%d NIBP Stat Error is %d.\n", __FILE__, __LINE__, gValueNibp.wErrCode);
					break;
				}
				iTimeInter ++;
				//���������
				if(iTimeInter > NIBP_STAT_INTER){
					iTimeInter =0;
					//�����´β���
					StartSingleAutoNibpMeasure();	
					if(B_PRINTF) printf("Next NIBP measuring.\n");
				}	
				else{
					if(B_PRINTF) printf("Time Inter is %d\n", iTimeInter);
				}
			}
			else{
				iTimeInter = 0;
			}
		}
		else{
			//�ж��ϴβ����Ƿ����,�����������ȡ���߳�,����,�ȴ��ôβ���������,ȡ���߳�
			while(gValueNibp.bStatus!= NIBP_SYSTEM_IDLE){
				//if(B_PRINTF) printf("NIBP still running.\n");	
			}
			DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax,1);
			break;
		}
		
		DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax,1);
	}

	//�������
	DrawNibpCycleGraph(0, -1, 0);
	
	giNibpTimeCount_Stat = 0;
			
	sem_destroy(&semNibpStat);
	if(B_PRINTF) printf("%s:%d Cancel NIBP STAT Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP STAT Measure thread.\n");
}


/**
	��ʼNIBP��������
 */
static int StartStatNibpMeasure()
{
	int res;
	bNibpStatCancel = FALSE; 
	if(B_PRINTF) printf("start star nibp measure~~bNibpStatCancel=%d~~~~~~~~~~~\n",bNibpStatCancel);
	
	res = sem_init(&semNibpStat, 0, 0);
	res = pthread_create(&ptNibpStat, NULL, ThreadProcNibpStat, NULL);
	
	return 0;
}

/**
	ȡ��NIBP�Զ�����
 */
static int CancelNibpStatMeasure()
{

	bNibpStatCancel= TRUE;
	sem_post(&semNibpStat);
	//ȡ����ǰ�Ĳ���
	CancelSingleNibpMeasure();
	
	return 0;
}

/**
	���������߳�
 */
int  giNibpTimeCount_Punc = 0;
static void *ThreadProcNibpPuncture(void *arg)
{
// 	int iTimeCount = 0;	//��ʱ��
	int iTimeMax = NIBP_PUNCTURE_TIME_ADULT;	//��ʱ����
	int iTimeInter = 0;	//���β���֮��ļ��
	
	if(gCfgNibp.bObject == NIBP_OBJECT_BABY){
		iTimeMax = NIBP_PUNCTURE_TIME_BABY;
	}
	else{
		iTimeMax = NIBP_PUNCTURE_TIME_ADULT;
	}
	
	
	//��������
//	StartPuncture();	
	
	//��������λ
	giNibpTimeCount_Punc = 0;
	
	for(;;){
		sem_wait(&semNibpPuncture);
		
		//ȡ���߳�
		if(bNibpPunctureCancel){
			giNibpTimeCount_Punc = 0;
			DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax, 2);
			break;
		}
				
		if(giNibpTimeCount_Punc < iTimeMax){
			giNibpTimeCount_Punc ++;
// 			if(B_PRINTF) printf("Puncture Timer %d.\n", giNibpTimeCount_Punc);
		}
		else{
			//ȡ������
			CancelPuncture();
// 			if(B_PRINTF) printf("Puncture Timeout %d.\n", iTimeCount);
			DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax,2);
			break;
		}
		
		DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax,2);
	}

	//�������
	DrawNibpCycleGraph(0, -1, 0);
	
	giNibpTimeCount_Punc = 0;
			 
	sem_destroy(&semNibpPuncture);
	if(B_PRINTF) printf("%s:%d Cancel NIBP Puncture Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP Puncture Measure thread.\n");
}

/**
	��ʼNIBP����
 */
static int StartPunctureNibpMeasure()
{
	int res;
	
	bNibpPunctureCancel = FALSE; 
	res = sem_init(&semNibpPuncture, 0, 0);
	res = pthread_create(&ptNibpPuncture, NULL, ThreadProcNibpPuncture, NULL);
	
	return 0;
}

/**
	ȡ��NIBP�Զ�����
 */
static int CancelNibpPunctureMeasure()
{

	bNibpPunctureCancel= TRUE;
	sem_post(&semNibpPuncture);
	//ȡ����ǰ�Ĳ���
	CancelPuncture();
	
	return 0;
}


//--------------------------------------- interface ----------------------------------------
/**
	��ʱ��֪ͨ�̸߳���
	bReset : �Ƿ���Ҫ��λ
*/
int UpdateNibpAutoTimer(BOOL bReset)
{
	if(bNibpAutoCancel) return -1;
	
	bNibpAutoReset = bReset;
	sem_post(&semNibpAuto);
	
	return 0;
}

/**
	��ʱ��֪ͨ�̸߳���
 */
int UpdateNibpStatTimer()
{
	if(bNibpStatCancel) return -1;
	
	sem_post(&semNibpStat);
	
	return 0;
}

/**
	��ʱ��֪ͨ�̸߳���
 */
int UpdateNibpPunctureTimer()
{
	if(bNibpPunctureCancel) return -1;
	
	sem_post(&semNibpPuncture);
	
	return 0;
}

//�����������NIBP����
int NIBP_Start()
{
	//�жϵ�ǰ��״̬
	switch(gValueNibp.bStatus){
		case NIBP_SYSTEM_IDLE:{
			switch(gCfgNibp.bMode){
				case NIBP_MODE_AUTO:{
					//�ж��Ƿ��Ѿ��������ڲ���,�����,���������β���,��λ������
					if(bNibpAutoHasStart){
						//���β���
						StartSingleAutoNibpMeasure();
						//��λ������
						UpdateNibpAutoTimer(TRUE);
					}
					else{
						//����,�������ڲ���
						StartAutoNibpMeasure();
					}
				}break;
				case NIBP_MODE_SERIES:{
					//����Ѿ���ʼ�Զ�����,��ȡ���Զ�����
					if(bNibpAutoHasStart){
						//ȡ���Զ�����
						CancelNibpAutoMeasure();
					}
					//������������
					StartStatNibpMeasure();
				}break;
				case NIBP_MODE_PUNCTURE:{
					if(bNibpAutoHasStart){
						//ȡ���Զ�����
						CancelNibpAutoMeasure();
					}
					//��������
					StartPunctureNibpMeasure();	
				}break;
				default:{
					//���β���
					StartSingleNibpMeasure();
				}break;
			}
		}break;
		default:{
			//���ڴӲ�����
		}break;
	}
	
	return 0;
}

//�������ȡ������
int NIBP_Cancel()
{
	
	//�жϵ�ǰ��״̬
	switch(gValueNibp.bStatus){
		printf("111111\n");
		case NIBP_SYSTEM_IDLE:{
			//�ж��Ƿ��Ѿ��������ڲ���
			if(bNibpAutoHasStart){
				//ȡ���Զ�����
				CancelNibpAutoMeasure();
			}	
			CancelNibpAutoMeasure();
		}break;
		default:{
				printf("12222222\n");
			//�ж�����
			switch(gCfgNibp.bMode){
				case NIBP_MODE_AUTO:{
					//ȡ���Զ�����
					CancelNibpAutoMeasure();
				}break;
				case NIBP_MODE_SERIES:{
					//ȡ����������
					CancelNibpStatMeasure();
				}break;
				case NIBP_MODE_PUNCTURE:{
					//ȡ������
					CancelNibpPunctureMeasure();
				}break;
				default:{
					//ȡ�����β���
					CancelSingleNibpMeasure();	
				}break;
			}
		}break;
	}
	
	return 0;
}

