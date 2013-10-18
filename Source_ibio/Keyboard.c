/*************************************************************************** 
 *	Module Name:	Keyboard
 *
 *	Abstract:	
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-09 09:13:30
 ***************************************************************************/
#include "IncludeFiles.h"
#include "Global.h"
#include "General_Funcs.h"
#include "linux/input.h"

//-- KernelKey ioctl para --
#define IO_ENTER				28
#define IO_KEY_RIGHTSHIFT		54
#define IO_KEY_LEFTSHIFT		42
#define IO_KEY_SPACE			57
#define IO_KEY_LEFT				105
#define IO_KEY_RIGHT			106
#define IO_KEY_UP				103
#define IO_KEY_DOWN			108
#define IO_KEY_PAGEDOWN		109
#define IO_KEY_PAGEUP			104
#define IO_KEY_TAB				15
#define IO_KEY_ESC				1
#define IO_KEY_G				34
#define IO_KEY_H				35
#define IO_KEY_SHIFTTAB		249
#define IO_KEY_CLRSHIFT			120

#define S1 	30
#define S2 	46
#define S3 	32
#define S4 	48
#define S5 	33
#define S6 	18
//�����豸
extern int gfdKey;
extern int gfdRotor;

//������
extern HWND ghWndMain;

//������ϵ��(���̿�������ƽ��ֵ����ϵ��)
//#define CALC_COUNT	5	
static BYTE bCalcCount = 4;		//6

//ĳЩ�賤���Ĺ��ܼ��ļ���ϵ��
static BYTE bLongKey = 12;

//������Ϣ���
static BOOL  bDebug = FALSE;

//NIBP ����ֹͣ��ť
BOOL gbNibpKey_Start = TRUE;

//����ͽⶳ����
BOOL gbFreezeKey = FALSE;

//�Ƿ���뱨����ͣ״̬
extern BOOL gbSuspending;
//������ͣ״̬����ʱ
extern int giSuspendTime;

//�Ƿ���
extern BOOL gbSilence;

//��ӡ��״̬
extern int gPrinterStatus;

//�Ƿ�洢ECG����
extern BOOL gbSaveRealEcgData;
//�Ƿ��Ѿ��洢�㹻��ECG����
extern BOOL gbSaveRealEcgDataOK;

/*
	��ť�ĺ�����װ
				case KEY_LR:{//Right
				ioctl(gfdKey, MYKEY_IOC_RIGHT);
}break;
			case KEY_TAB:{//TAB
				ioctl(gfdKey, MYKEY_IOC_TAB);
}break;
			case KEY_UD:{//Down
				ioctl(gfdKey, MYKEY_IOC_DOWN);
}break;

*/

static void recive_rotor_handler(int signum)
{
	static char preValue=100 ; //�����ϴε�ֵ�������ж�rotor�ķ���ϵͳ���ϵ�ʱΪ100
	char cValue = 0;	
	int Key=0,shiftKey=0;
	static int iCount=0,left=0,right=0;
	
	if( gfdRotor < 0 ) return;
	if(read(gfdRotor,&cValue,1) <0)
	{
		printf("read /dev/rotor0 fail may be copy_to_user() return fail\n");
	}
	else
	{
	/*
		if(preValue<cValue){
			left++;
			iCount++;
			printf("***left***left=%d,icount=%d\n",left,iCount);
		}else if(preValue>cValue){
			right++;
			iCount++;
			printf("###right###right=%d,icount=%d\n",cValue,iCount);

		}
		
		int tab=KEY_TAB;
		if(iCount>=3){
			*/
			if(preValue>cValue){//left
		
		//		printf("left ,gbKeyType=%d,tab=%d\n",gbKeyType,tab);

				switch(gbKeyType){
					case KEY_LR:{//left
						if(B_PRINTF)printf("---------left\n");
						
						Key=IO_KEY_LEFT;
						write(gfdKey,&Key,1);	
					}break;
					case 1:{//shift TAB
						if(B_PRINTF)printf("!!!!!!!!!!shift tab!\n");
					//	Key=IO_KEY_SHIFTTAB;
						Key=IO_KEY_LEFT;
						write(gfdKey,&Key,1);
					}break;
					case KEY_UD:{//up
						if(B_PRINTF)printf("#######up\n");
						
						Key=IO_KEY_UP;
						write(gfdKey,&Key,1);
					}break;
					case KEY_AD:{//a
						if(B_PRINTF)printf("+++++++++A\n");
						Key=IO_KEY_PAGEUP;
						write(gfdKey,&Key,1);
					}break;
					default:
						
					break;
				}
			}else{//right
			
			//	printf("right,gbKeyType=%d,tab=%d\n",gbKeyType,tab);
				switch(gbKeyType){
					case KEY_LR:{//Right
						if(B_PRINTF)printf("---------right\n");
						Key=IO_KEY_RIGHT;
						write(gfdKey,&Key,1);
					}break;
					case 1:{//TAB
						if(B_PRINTF)printf("!!!!!!!!!!!tab\n");
					//	Key=IO_KEY_TAB;
						Key=IO_KEY_RIGHT;
						write(gfdKey,&Key,1);
					}break;
					case KEY_UD:{//Down
						if(B_PRINTF)printf("#######down\n");
						Key=IO_KEY_DOWN;
						write(gfdKey,&Key,1);
					}break;
					case KEY_AD:{
						if(B_PRINTF)printf("+++++++++D\n");
						Key=IO_KEY_PAGEDOWN;
						write(gfdKey,&Key,1);
					}break;
					default:
					break;
				}
			}
			
		//	iCount=0;
	//		left=0;
	//		right=0;
	//	}
		
		preValue=cValue;
	//	printf("***the rotor  is %d\n",value);
	}
}

int DealFunKey()
{
	unsigned char key;
	key=GetFunKeysValue();
	switch(key){
	case S1://menu
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_M, 0);
		if(B_PRINTF)printf("S1 key=%d\n",key);
	break;
	case S2://freeze
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_F, 0);
		if(B_PRINTF)printf("S2 key=%d\n",key);
	break;
	case S3://pause
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_A, 0);
		if(B_PRINTF)printf("S3 key=%d\n",key);
	break;
	case S4://print
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_P, 0);
		if(B_PRINTF)printf("S4 key=%d\n",key);
	break;
	case S5://nibp
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_Z, 0);
		if(B_PRINTF)printf("S5 key=%d\n",key);
	break;
	case S6://silence
		PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_S, 0);
		if(B_PRINTF)printf("S6 key=%d\n",key);
	break;
	}
		
	return 0;
}

//���������Ϣ
int  Deal_Rotor()
{
	unsigned char iKbdValue=0;
	static unsigned char iRotorVal=0;
	static unsigned char iRotorBtnVal=0;
	char RotorBtn_value[2] = {0};	
	int oflags = 0;
//	int fd=0;
	int keyboard=0;
	struct input_event myevent;
	fd_set rdfds; /* ������һ�� fd_set ��������������Ҫ���� socket��� */
	struct timeval tv; /* ����һ��ʱ�����������ʱ�� */
	int ret; /* ���淵��ֵ */	

//rotor-------------------------------------	
	signal(SIGIO,recive_rotor_handler);/* ����SIGIO�ź�*/
	
	fcntl(gfdRotor,F_SETOWN,getpid());
	
	oflags = fcntl(gfdRotor,F_GETFL);
	
	fcntl(gfdRotor,F_SETFL,oflags |FASYNC);

	return 0;
}


