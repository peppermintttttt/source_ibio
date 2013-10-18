/*************************************************************************** 
 *	Module Name:	ExtDevice_Funcs
 *
 *	Abstract:	��չ��Ӳ���Ĳ�������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-28 09:48:38
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "ExtDevice_Funcs.h"


#define DEVICE_FUNCBTN 			"/dev/input/event0"
#define DEVICE_ROTOR 			"/dev/rotor0"
#define DEVICE_KEY				"/dev/keyboard0"
#define DEVICE_LED				"/dev/cpld0"
#define DEVICE_INTERFACE 		"/dev/interface" 
#define DEVICE_PWM 				"/dev/pwm_device" 
#define DEVICE_WDT				"/dev/watchdog" 


//-- ioctl para --
#define INTERFACE_IOC_MAGIC 	'j'

#define IOCTL_CMD_MAGIC 'c'
//led����˸Ƶ��
#define 	IO_LED_FREQ 	
/*������ɫ������,�Լ�������˸Ƶ��*/
#define WCE2400_CPLD_ENABLE_RED_LED	_IOW(IOCTL_CMD_MAGIC, 0xc6, unsigned char)	
#define WCE2400_CPLD_DISABLE_RED_LED	_IO(IOCTL_CMD_MAGIC, 0xc7)	
/*������ɫ������,�Լ�������˸Ƶ��*/
#define WCE2400_CPLD_ENABLE_YELLOW_LED	_IOW(IOCTL_CMD_MAGIC, 0xc8, unsigned char)
#define WCE2400_CPLD_DISABLE_YELLOW_LED	_IO(IOCTL_CMD_MAGIC, 0xc9)	
/*������ɫ������,�Լ�������˸Ƶ��*/
#define WCE2400_CPLD_ENABLE_GREEN_LED	_IOW(IOCTL_CMD_MAGIC, 0xca, unsigned char)
#define WCE2400_CPLD_DISABLE_GREEN_LED	_IO(IOCTL_CMD_MAGIC, 0xcb)	

/*���˷�(�췧)��*/
#define PUMP_IOC_QUICK_ON		_IO(INTERFACE_IOC_MAGIC, 5)
/*���˷�(�췧)��*/
#define PUMP_IOC_QUICK_OFF		_IO(INTERFACE_IOC_MAGIC, 6)
/*��ͯ��(�з�)��*/
#define PUMP_IOC_MIDDLE_ON		_IO(INTERFACE_IOC_MAGIC, 7)
/*��ͯ��(�з�)��*/
#define PUMP_IOC_MIDDLE_OFF		_IO(INTERFACE_IOC_MAGIC, 8)
/*��û�������״̬��1������������0��16��󽫹رյ�Դ*/
#define IOC_GETPOWERSTATUS		_IO(INTERFACE_IOC_MAGIC, 9)
/*��õ�Դ���ͣ�1��ʹ������Դ��0��ʹ�����õ��*/
#define IOC_GETPOWERTYPE		_IO(INTERFACE_IOC_MAGIC, 10)
/*��õ�ص�����*/
#define IOC_GETBATTERYSTATUS	_IO(INTERFACE_IOC_MAGIC, 11)

//-- ioctl para --
#define PWMDEV_IOC_MAGIC 	'k'

/*�ı�ռ�ձ�*/
#define PWMDEV_IOC_CHANGEDUTY	_IO(PWMDEV_IOC_MAGIC, 1)
/*�ı䶨ʱֵ�����ڣ�*/
#define PWMDEV_IOC_CHANGECYCLE	_IO(PWMDEV_IOC_MAGIC, 2)
/*������ʱ��*/
#define PWMDEV_IOC_STARTTIMER	_IO(PWMDEV_IOC_MAGIC, 3)
/*�رն�ʱ��*/
#define PWMDEV_IOC_CLOSETIMER	_IO(PWMDEV_IOC_MAGIC, 4)

//Watchdog
//-- ioctl para --
#define WD_IOC_MAGIC 	'j'
//��ʼ��Watchdog
#define WD_IOC_INIT		_IO(WD_IOC_MAGIC, 1)
//����
#define WD_IOC_START	_IO(WD_IOC_MAGIC, 2)
//ֹͣ
#define WD_IOC_STOP		_IO(WD_IOC_MAGIC, 3)
//ι��
#define WD_IOC_FEED		_IO(WD_IOC_MAGIC, 4)


/*
	��չ���豸
*/
//��ť
int gfdRotor= -1;
//event0 �¼�
int gfdKeyboard = -1;
//�ں˼���
int gfdKey = -1;
//Led������
int gfdLed = -1;


//�����ͱ����Ƶ��豸�ӿ�
int gfdInterface = 0;
//����
int gfdPump = 0;
//���Ź�
int gfdWdt = 0;


/*
	��ʼ�������豸
*/
static int InitDevice_Key()
{
//keyboard
	gfdKey = open(DEVICE_KEY, O_RDWR);
	if(B_PRINTF)printf("***Device -- Key is %d***.\n", gfdKey);
	if(gfdKey<0) return -1;
	
//fun_btn
//	gfdKeyboard = open(DEVICE_FUNCBTN, O_RDWR);
//	printf("***Device -- Keyboard is %d***.\n", gfdKeyboard);
//	if(gfdKeyboard<0) return -1;
	
//rotor
	gfdRotor= open(DEVICE_ROTOR, O_RDWR);
	if(B_PRINTF)printf("***Device -- Rotor is %d***.\n", gfdRotor);
	gbKeyType = KEY_LR;
	if(gfdRotor<0) return -2;

	
	return 0;
}
static int InitDevice_Led()
{
	
	gfdLed= open(DEVICE_LED, O_RDWR);
	
	if(B_PRINTF)printf("Device -- Led is %d.\n", gfdLed);
	
	if(gfdLed<0) return -1;
	
	return 0;
}


static int InitDevice_Pump()
{
	
	gfdPump = open(DEVICE_PWM, O_RDWR);
	if(B_PRINTF)printf("Device -- PWM is %d.\n", gfdPump);
			
	if(gfdPump<=0) return -1;
		
	return 0;

}

/*
	��ʼ�����Ź�
*/
 int InitDevice_Wdt()
{
	gfdWdt = open(DEVICE_WDT, O_RDWR);
	if(B_PRINTF)printf("Device -- Watchdog  is %d.\n", gfdWdt);
	if(gfdWdt <=0) return -1;

	return 0;
}

//---------------- interface functions -------------------

/*
	��ʼ����չ�豸
*/
int InitExtDevices()
{
	//���豸
	InitDevice_Key();
	InitDevice_Led();

//	InitDevice_Pump();
//	InitDevice_Wdt();
		
	return 0;
}

/*
	led�����Ʋ�������
*/

//�����˸
int RedLed_On(unsigned char freq)
{
	if(gfdLed<0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_RED_LED,freq);
	
	return 0;
}
//�����
int RedLed_Off()
{
	if(gfdLed <0) return -1;

	ioctl(gfdLed,WCE2400_CPLD_DISABLE_RED_LED,NULL);
	
	return 0;
}
//�Ƶ���
int YellowLed_On(unsigned char freq)
{
	if(gfdLed <0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_YELLOW_LED,freq);
	
	return 0;
}
//�Ƶ���
int YellowLed_Off()
{
	if(gfdLed <=0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_DISABLE_YELLOW_LED,NULL);
	
	return 0;
}
//�̵���
int GreenLed_On(unsigned char freq)
{
	if(gfdLed <0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_GREEN_LED,freq);
	
	return 0;
}
//�̵���
int GreenLed_Off()
{
	if(gfdLed ==0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_DISABLE_GREEN_LED,NULL);
	
	return 0;
}




//�췧��
int AdultValveOn()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_QUICK_ON);
	
	return 0;
}
//�췧��
int AdultValveOff()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_QUICK_OFF);
	
	return 0;
}
//�з���
int ChildValveOn()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_MIDDLE_ON);
	
	return 0;
}
//�з���
int ChildValveOff()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_MIDDLE_OFF);
	
	return 0;
}

/*��û�������״̬��1������������0��16��󽫹رյ�Դ*/
int GetWorkStatus()
{
	int res;
	return	POWER_WORKING;

	if(gfdInterface ==0) return -1;
	
	res = ioctl(gfdInterface, IOC_GETPOWERSTATUS);
	
	if(res ==0){
		if(B_PRINTF)printf("%s:%d Power Off: %d.\n", __FILE__, __LINE__, res);
		//�رտ��Ź�
		WDT_Stop();
		if(B_PRINTF)printf("%s:%d Stop Watchdog.\n", __FILE__, __LINE__);
	}
 	
	if(res ==1) return  POWER_WORKING;
	else return POWER_SHUTDOWN;
	
	return res;
}

/*��û�����Դ���ͣ�1������Դ��0�����*/
int GetPowerType()
{
	int res=0;
	return POWER_AC;
	if(gfdInterface ==0) return -1;
	
	res = ioctl(gfdInterface, IOC_GETPOWERTYPE);
	
// 	printf("%s:%d Power Type: %d.\n", __FILE__, __LINE__, res);
	
	if(res ==1) return POWER_AC;
	else return POWER_DC;
	
	return res;
}

/*
	��õ�ص���	
*/
int GetBatteryStatus()
{
	int res =0;
	return 2;
	res = ioctl(gfdInterface, IOC_GETBATTERYSTATUS);
	
// 	printf("%s:%d Battery: %d.\n", __FILE__, __LINE__, res);
	
	return res;
}


/*
	����ռ�ձ�
*/
int PWM_ChangeRate(int rate)
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_CHANGEDUTY, rate);
	
// 	printf("Change PWM rate to %d.\n", rate);
	return res;
}

/*
	����PWM��ʱ��
*/
int PWM_StartTimer()
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_STARTTIMER);
	
	return res;
}

/*
	�رն�ʱ��
*/
int PWM_CloseTimer()
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_CLOSETIMER);
	
	return res;
}

//�������Ź�
int WDT_Start()
{
	int res;
	
	if(gfdWdt == 0) return -1;
	
	//��ʱ���ο��Ź�
	
     //   	write(gfdWdt, "a", 1);
	
//	res = ioctl(gfdWdt, WD_IOC_START);
	
	return res;
}

//ֹͣ���Ź�
int WDT_Stop()
{
	int res;
	
	if(gfdWdt == 0) return -1;
	
//	res = ioctl(gfdWdt, WD_IOC_STOP);
	 write(gfdWdt, "V", 1);
        close(gfdWdt);
        gfdWdt = 0;
	return res;
}


//ι��
int WDT_Feed()
{
	int res;
	
	if(gfdWdt == 0) return -1;

	write(gfdWdt, "a", 1);
//	res = ioctl(gfdWdt, WD_IOC_FEED);

	return res;
}
