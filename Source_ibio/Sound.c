/*************************************************************************** 
 *	Module Name:	Sound
 *
 *	Abstract:	�����̺߳���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-22 10:01:50
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"

//�����ź���
sem_t	semSound;				
sem_t	semBeep;
//����
static pthread_t ptSound;
static pthread_t ptBeep;
	

//-- �����豸 --
static int gfdSound = 0;
//-- �����豸 --
static int gfdMixer = 0;

//-- �����ͱ����Ƶ��豸�ӿ� ---
extern int gfdInterface;

//�����ļ��Ĵ�С Byte
//beep-1368 	//Middle-32756	//Low-13676	//High-111436



//--------- �����ļ�Buffer ----------
//Beep
#define SOUNDBUFF_BEEP_MAX	1368		//1368
unsigned char SoundBuff_Beep[SOUNDBUFF_BEEP_MAX]={0};
//High Alarm
#define SOUNDBUFF_ALM_HIGH_MAX	(117360/2)
unsigned char SoundBuff_Alm_High[SOUNDBUFF_ALM_HIGH_MAX]={0};
//Middle Alarm
#define SOUNDBUFF_ALM_MIDDLE_MAX	60000	// 32756
unsigned char SoundBuff_Alm_Middle[SOUNDBUFF_ALM_MIDDLE_MAX]={0};
//Low Alarm
#define SOUNDBUFF_ALM_LOW_MAX	60000	//13676
unsigned char SoundBuff_Alm_Low[SOUNDBUFF_ALM_LOW_MAX]={0};

//��������
int SetVolume(BYTE bVolume);

static BOOL bAlarming = FALSE;

//�Ƿ���
BOOL gbSilence = FALSE;

/*
	��ʼ����Ƶ�ļ�����
*/
static int InitSoundBuffer()
{
	int fd = 0;
	int iLen = 0;
	
	//Beep
 	if((fd = open ("sound/beep.wav",O_RDWR))<0){
		printf(" Can't open beep.wav!\n");
		return -1;
	}
	memset(SoundBuff_Beep, 0, sizeof SoundBuff_Beep);
	iLen = read(fd, SoundBuff_Beep, sizeof SoundBuff_Beep);
	close(fd);
	
	//Alarm High
	if((fd = open ("sound/Alarm_High.wav",O_RDWR))<0){
		printf("--------------Can't open Alarm_High.wav!\n");
		return -1;
	}
	memset(SoundBuff_Alm_High, 0, sizeof SoundBuff_Alm_High);
	iLen = read(fd, SoundBuff_Alm_High, sizeof SoundBuff_Alm_High);
//	memcpy(SoundBuff_Alm_High+(sizeof SoundBuff_Alm_High)/2,SoundBuff_Alm_High, (sizeof SoundBuff_Alm_High)/2);
	close(fd);
	
	//Alarm Middle
	if((fd = open ("sound/Alarm_Middle.wav",O_RDWR))<0){
		printf(" --------------Can't open Alarm_Middle.wav!\n");
		return -1;
	}
	memset(SoundBuff_Alm_Middle, 0, sizeof SoundBuff_Alm_Middle);
	iLen = read(fd, SoundBuff_Alm_Middle, sizeof SoundBuff_Alm_Middle);
	close(fd);
	
	//Alarm Low
	if((fd = open ("sound/Alarm_Low.wav",O_RDWR))<0){
		printf("----------------Can't open Alarm_Low.wav!\n");
		return -1;
	}
	
	memset(SoundBuff_Alm_Low, 0, sizeof SoundBuff_Alm_Low);
	iLen = read(fd, SoundBuff_Alm_Low, sizeof SoundBuff_Alm_Low);
	close(fd);
	
	return 0;
}


/*
	��ʼ����Ƶ�豸
*/
static int InitSoundDevice()
{
	int i;
	
	//DSP
//  	gfdSound = open("/dev/sound/dsp", O_WRONLY | O_ACCMODE);
 	gfdSound = open("/dev/dsp", O_WRONLY);
	
	if(gfdSound <0){
		printf("%s:%d ----------------------------------Open Sound Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
		
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_RESET,(char *)&i);
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_SYNC,(char *)&i);
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_NONBLOCK,(char *)&i);
  	i=16000;
 	ioctl(gfdSound,SNDCTL_DSP_SPEED,(char *)&i);
//  	i=1;
//  	ioctl(gfdSound,SNDCTL_DSP_CHANNELS,(char *)&i);
//	i=1;
 // 	ioctl(gfdSound,SNDCTL_DSP_STEREO,(char *)&i);
 	i=16;
 	ioctl(gfdSound,SNDCTL_DSP_SETFMT,(char *)&i);
// 	i=3;
// 	ioctl(gfdSound,SNDCTL_DSP_SETTRIGGER,(char *)&i);
// 	i=3;
// 	ioctl(gfdSound,SNDCTL_DSP_SETFRAGMENT,(char *)&i);
// 	i=1;
// 	ioctl(gfdSound,SNDCTL_DSP_PROFILE,(char *)&i);
	
	return 0;
}



/*
*/
static int SyncSoundDevice()
{
	int iValue;
	
	if(gfdSound <=0) return -1;
	
	iValue = 0;
	ioctl(gfdSound,SNDCTL_DSP_SYNC,(char *)&iValue);
	
	return 0;
}

/*
	��������(����mixer)
	bVolume: 0~100
*/
static int SetVolume_Mixer(BYTE bVolume)
{
	int fd;
	long int iVolume = 0;	//�ٷֱ�	
	long int iGain = 0;	//�ٷֱ�
	int iLeft, iRight;
	
	fd = open("/dev/sound/mixer", O_RDWR);

	if(fd<0){
		printf("%s:%d Open Mixer Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
	
	iVolume = bVolume;
	
	//��������
	iGain = (20<<8) + 20;
	ioctl(fd,SOUND_MIXER_WRITE_OGAIN,&iGain);
	
	ioctl(fd,SOUND_MIXER_READ_OGAIN,&iGain);

	iLeft = iGain & 0xFF;
	iRight = (iGain & 0xFF00)>>8;
	printf("-------------%s:%d Gain %ld-- Left:%d, right:%d\n", __FILE__, __LINE__, iGain, iLeft, iRight);
	
	//��������
	ioctl(fd,SOUND_MIXER_WRITE_VOLUME,&iVolume);
	
	ioctl(fd,SOUND_MIXER_READ_VOLUME,&iVolume);
	
	iLeft = iVolume & 0xFF;
	iRight = (iVolume & 0xFF00)>>8;
	printf("-------------%s:%d Volume-- Left:%d, right:%d\n", __FILE__, __LINE__, iLeft, iRight);
	
	close(fd);
	return 0;
}

/*
	��������(����mixer)
	bVolume: 0~100
	ֱ�Ӳ���dsp
*/
static int SetVolume_Dsp(BYTE bVolume)
{
	int fd;
	long int iVolume = 0;	//�ٷֱ�	
	long int iGain = 0;	//�ٷֱ�
	int iLeft, iRight;
	
	fd = gfdSound;

	if(fd<0){
		printf("%s:%d Open Sound Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
	
	iVolume = bVolume;
	
	//��������(���ã�UDA1341������û��ʵ��)
// 	iGain = (20<<8) + 20;
// 	ioctl(fd,SOUND_MIXER_WRITE_OGAIN,&iGain);
// 	
// 	ioctl(fd,SOUND_MIXER_READ_OGAIN,&iGain);
// 	iLeft = iGain & 0xFF;
// 	iRight = (iGain & 0xFF00)>>8;
// // 	printf("-------------%s:%d Gain %d-- Left:%d, right:%d\n", __FILE__, __LINE__, iGain, iLeft, iRight);
	
	//��������
// 	printf("-------------%s:%d Set Volume-- %.ld\n", __FILE__, __LINE__, iVolume);
  	ioctl(fd,SOUND_MIXER_WRITE_VOLUME,&iVolume);
	
	ioctl(fd,SOUND_MIXER_READ_VOLUME,&iVolume);
	
	iLeft = iVolume & 0xFF;
	iRight = (iVolume & 0xFF00)>>8;
//  	printf("-------------%s:%d Read Volume-- Left:%d, right:%d\n", __FILE__, __LINE__, iLeft, iRight);
	
	return 0;
}

/*
	Beep����
*/
static int Beep_Sound()
{
	if(gfdSound <= 0) return -1;
	
	if(gCfgEcg.bBeepVolume == VOLUME_OFF) return -1;
	
	if(!bAlarming){
		//��λ
  		SyncSoundDevice();
		//��������
		SetVolume(gCfgEcg.bBeepVolume*10);
		//����
		write(gfdSound, SoundBuff_Beep, SOUNDBUFF_BEEP_MAX);
	}
	
	return 0;
}
		
/*
	�߼�����
*/
static int Sound_Alarm_High()
{

	if(gfdSound <= 0) return -1;
		
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//��λ
	SyncSoundDevice();
	//��������
	SetVolume(gCfgAlarm.bVolume*10);
	
	//����
	write(gfdSound, SoundBuff_Alm_High, SOUNDBUFF_ALM_HIGH_MAX);
	//��λ
	SyncSoundDevice();
	//����
	write(gfdSound, SoundBuff_Alm_High, SOUNDBUFF_ALM_HIGH_MAX);
	//��ʱ�ȴ��������
	usleep(3000000);
	
	bAlarming = FALSE;
	
	return 0;
}

 /*
	�м�����
 */
static int Sound_Alarm_Middle()
{
	if(gfdSound <= 0) return -1;
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//��λ
	SyncSoundDevice();
	//��������
	SetVolume(gCfgAlarm.bVolume*10);
	//����
	write(gfdSound, SoundBuff_Alm_Middle, SOUNDBUFF_ALM_MIDDLE_MAX);
	
	//��ʱ�ȴ��������
	usleep(1500000);
	
	bAlarming = FALSE;
	return 0;
}

/*
	�ͼ�����
*/
static int Sound_Alarm_Low()
{

	if(gfdSound <= 0) return -1;
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//��λ
	SyncSoundDevice();
	//��������
	SetVolume(gCfgAlarm.bVolume*10);
	//����
	write(gfdSound, SoundBuff_Alm_Low, SOUNDBUFF_ALM_LOW_MAX);
	
	//��ʱ�ȴ��������
	usleep(1200000);
	
	bAlarming = FALSE;
	return 0;
}

static void *ThreadProcBeep(void *arg)
{
	int iSemValue;	//��ͼ�ź���������
	
	for(;;){
		sem_wait(&semBeep);
		
		sem_getvalue(&semBeep, &iSemValue);
		
		if(iSemValue >1){
// 			printf("********iWaveSemValue=%d\n",iSemValue);
			sem_init(&semBeep, 0, 0);
			sem_getvalue(&semBeep, &iSemValue);
// 			printf("********iWaveSemValue=%d\n",iSemValue);
 			continue;
		}

 		Beep_Sound();
	}
}

//------------------- interface -------------------
/*
	���������߳�	
	return: 0-OK, -1:Failure
*/
int CreateSoundProc()
{
	int res;
	
	//��ʼ����Ƶ�豸
	InitSoundDevice();
	
	//��ʼ����Ƶ�ļ�
	InitSoundBuffer();
	
	//���ó�ʼ����,����
	SetVolume(55);
	
	//��ʼ���ź���
	res = sem_init(&semBeep, 0, 0);

	//����beep�߳�
	res = pthread_create(&ptBeep, NULL, ThreadProcBeep, NULL);
			
	return res;
}

/*
	��������
	bVolume : ����(0-100)
*/
int SetVolume(BYTE bVolume)
{

	SetVolume_Dsp(bVolume);
	
	return 0;
}

/*
	Beep����
*/
int Sound_Beep()
{
	if(gfdSound <= 0) return -1;
	
	//��Դ���ڹر�ʱ��������
	if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
		return 0;		
	}
		
	if(!gbSilence){
		sem_post(&semBeep);
	}
	
	return 0;
}

/*
	��������
*/
int AlarmSound(BYTE bClass)
{
	int res=0;
	
	//��Դ���ڹر�ʱ��������
	if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
		return 0;		
	}
	
	if(!gbSilence){
		switch(bClass){
			case ALARM_HIGH:{
				res = Sound_Alarm_High();
			}break;
			case ALARM_MID:{
				res = Sound_Alarm_Middle();
			}break;
			case ALARM_LOW:{
				res = Sound_Alarm_Low();
			}break;
			default:break;
		}
	}
	
	return res;
}

 /*
	�������
 */
int Test_LightAndSound()
{
	if(gfdSound <= 0) return -1;

	//��λ
	SyncSoundDevice();
	//��������
	SetVolume(50);
	//����
	write(gfdSound, SoundBuff_Alm_Middle, SOUNDBUFF_ALM_MIDDLE_MAX);
	//��ʱ�ȴ��������
// 	usleep(1500000);
	
	RedLed_On(LOW_ALARM_LED);
	usleep(300000);
	RedLed_Off();
	YellowLed_On(LOW_ALARM_LED);
	usleep(300000);
	YellowLed_Off();
	GreenLed_On(LOW_ALARM_LED);
	usleep(300000);
	GreenLed_Off();
	
	
	return 0;
}
