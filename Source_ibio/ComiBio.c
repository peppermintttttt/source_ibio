/*************************************************************************** 
 *	Module Name:	ComiBio
 *
 *	Abstract:	������iBio ģ��ͨ�ų���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			hejp	2012-12-25 20:25:35
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Serial_Funcs.h"
#include "General_Funcs.h"
#include "Central.h"
#include "ComiBio.h"
#include "NewDrawWave.h"

#define CHANNEL1  0x00
#define CHANNEL2  0x10
#define ECGLEAD3 	1
#define ECGLEAD5 	0

#define DATAID 	0
#define DATAHEAD 	1
//����Ӧ��
#define CMDOK  		0
#define ERR_SUM  		1
#define ERR_LEN  		2
#define ERR_INVALCMD  	3
#define ERR_CMDPARA 		4
#define ERR_ACCEPT 		5

//test
#define ID_ECGWAVE_TRUE	 0
#define ID_RESPWAVE_TRUE	 0
#define ID_SPO2_WAVE_TRUE	 0

//��������
extern unsigned char *gSerialsName[];
//MemDC
extern HDC hdcWaveMem ;
//��ͼ����
extern HDC hdcWave;
extern NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];
//ģ���Ӧ�Ĵ��ںţ��˴�ʹ�ñ��������Ǻ꣬��Ϊ���Ժ���Զ�̬���䴮��
//int gSerialIndex_iBio = DEV_SERIAL0;		//RS232
int gSerialIndex_iBio = DEV_SERIAL2;		//RSttf
//ģ���Ӧ�Ĵ����豸
int gfdSerial_iBio = -1;

//����д������ͬ���ź���
sem_t semSend_iBio;

//���ڽ��ջ���
#define RECVBUFF_MAX_IBIO		128

//���屨�ĵ���С����
#define PACK_MIN_LENGTH_IBIO		15

//���ڽ����߳�ID
pthread_t ptID_iBioRecv;
extern BOOL gbSaveRealEcgData;

//������
int gbHaveBeep_QRS ;

//������
BOOL gbHaveBeep_Pulse ;
//ECG�Ƿ��ڶ���״̬
BOOL gbiBioECGCaling = FALSE;
BOOL gSTStatus=FALSE;
BYTE gST1Wave[250],gST2Wave[250];

//NIBP��ѹ����
#define NIBP_PROTECT_ADULT			330
#define NIBP_PROTECT_ADULT_S		300
#define NIBP_PROTECT_CHILD			264
#define NIBP_PROTECT_CHILD_S		240
#define NIBP_PROTECT_BABY			165
#define NIBP_PROTECT_BABY_S		150

//��ѹ���������� ���ݲ������(20ms)����
extern int giNibpProtectCounter ;
//15s�ı���ʱ��
#define NIBP_PROTECT_TIME		750		//50pack x 15s
//�����ѹ������־
extern BOOL gbNibpProtect ;
//NIBPУ׼��־λ
extern BOOL gbNibpCal ;
//NIBP©������־λ
extern BOOL gbNibpLeak ;
//NIBP ����ֹͣ��ť
extern BOOL gbNibpKey_Start ;

//�������������״̬
extern BOOL gbCentralStatus;
//�������������״̬
extern BOOL gbOtherCentralStatus;

extern BOOL gbSaveWaveValuetofile;
//�����緢�͵Ĳ��λ�����(1��)��ʵ�ʴ���ĳ���
extern U8 gbNetWaveBuff_Ecg[ECG_WAVE_PER_SEC*4];			//200 x 4Leads
extern int giNetWaveLength_Ecg;
extern U8 gbNetWaveBuff_SpO2[SPO2_WAVE_PER_SEC];
extern int giNetWaveLength_SpO2;
extern U8 gbNetWaveBuff_Resp[RESP_WAVE_PER_SEC];
extern int giNetWaveLength_Resp;
extern U8 gbNetWaveBuff_CO2[CO2_WAVE_PER_SEC];
extern int giNetWaveLength_CO2;
extern U16 gbNetWaveBuff_IBP1[IBP_WAVE_PER_SEC];	
extern int giNetWaveLength_Ibp1;
extern U16 gbNetWaveBuff_IBP2[IBP_WAVE_PER_SEC];	
extern int giNetWaveLength_Ibp2;

BOOL gbHavePack=FALSE;
BOOL gSave=FALSE;
#define SAVETIME 125*10 //�������ݵĳ���
unsigned int gSaveWave[SAVETIME];
static int ccc=0;
char gPackLength[128] =
{
	0 , // 00: ����
	2 , // 01: ϵͳ��λ��Ϣ
	5 , // 02: ϵͳ״̬
	9 , // 03: ϵͳ�Լ�
	5 , // 04: ����Ӧ��
	8 , // 05: ECG��������
	5 , // 06: �������Ӻ��ź���Ϣ
	5 , // 07: ����
	0 , // 08: ����
	5 , // 09: ����(********δ����***********)
	0 ,//6 , // 0a: ����(********δ����***********)
	7 , // 0b: STƫ��
	9 , // 0c: ST�β���ֵ
	0 , // 0d: ����
	0 , // 0e: ����
	0, // 0f: ����
	4, // 10:������
	5, // 11: ������
	6, // 12:��Ϣ����
	4, // 13: CVA������Ϣ
	0, // 14:����
	8, // 15:��������
	5, // 16: SPO2��������
	7, // 17: SPO2����
	7, // 18: IBP��������
	4, // 19: IBP״̬
	10, // 1a: IBPѹ��
	6, // 1b: IBPУ���У׼��Ϣ
	7, // 1c: IBPУ���У׼ʱ��
	0, // 1d:����
	0, // 1e: ����
	0, // 1f: ����
	7, // 20: NBPʵʱ��������
	4, // 21: NBP����������
	9, // 22: NBP�������1
	5, // 23: NBP�������2
	8, // 24: NBP
	0, // 25:����
	0, // 26:����
	0, // 27:����
	0, // 28:����
	0, // 29:����
	0, // 2a: ����
	0, // 2b:����
	0, // 2c: ����
	0, // 2d:����
	0, // 2e: ����
	0, // 2f: ����
	0, // 30:����
	0, // 31:����
	0, // 32:����
	0, // 33:����
	0, // 34:����
	0, // 35:����
	0, // 36:����
	0, // 37:����
	0, // 38:����
	0, // 39:����
	0, // 3a: ����
	0, // 3b:����
	0, // 3c: ����
	0, // 3d:����
	0, // 3e: ����
	0, // 3f: ����
	2, // 40:��ȡ�Լ���
	4, // 41:��������ѡ��
	4, // 42:������Ϣ����
	0, // 43:����
	0, // 44:����
	4, // 45:����ģʽѡ��:3,5��
	4, // 46:������ʽѡ��
	4, // 47:�˲���ʽѡ��
	4, // 48:�ĵ�����
	4, // 49: 1mVУ׼�ź�����
	4, // 4a: ��Ƶ��������
	4, // 4b:�𲫷�����������
	7, // 4c: ST������ISO ��ST��
	4, // 4d:�����ĵ����ͨ��
	2, // 4e: �ĵ���ѧϰ����
	0, // 4f: ����
	4, // 50:��������
	0,// 51: ����
	4, // 52:��Ϣ����ʱ��ѡ��
	4, // 53:����̽ͷ��������
	4, // 54: SPO2����
	2, // 55: NBP��ʼһ���ֶ�/�Զ�����
	2, // 56: NBP��������
	4, // 57: NBP������������
	2, // 58: NBP��ʼУ׼
	2, // 59: NBP��λ
	2, // 5a: NBP©�����
	2, // 5b: NBP��ѯ״̬
	5, // 5c: NBP���ó��γ���ѹ��
	2, // 5d: NBP��ʼSTAT����
	2, // 5e: NBP��ѯ���
	0, // 5f: ����
	4, // 60: IBP����ѹ������
	4, // 61: IBPУ��
	7, // 62: IBP1У׼
	7, // 63: IBP2У׼
	4, // 64:���������˲�ģʽ
	5, // 65: IBP��ѯ
	0, // 66:����
	0,///4, // 67:����(********δ����***********)
	0, // 68:����
	0, // 69:����
	0, // 6a: ����
	0, // 6b:����
	0, // 6c: ����
	0, // 6d:����
	0, // 6e: ����
	0, // 6f: ����
	0, // 70:����
	0, // 71:����
	0, // 72:����
	0, // 73:����
	0, // 74:����
	0, // 75:����
	0, // 76:����
	0, // 77:����
	0, // 78:����
	0, // 79:����
	0, // 7a: ����
	0, // 7b:����
	0, // 7c: ����
	0, // 7d:����
	0, // 7e: ����
	0, // 7f: ����
};
#define ECGCOUNT	376
#define RESPCOUNT	754
#define SPO2COUNT	152
#define IBP1COUNT	96
#define IBP2COUNT	96
unsigned int DEMO_ECG_II[ECGCOUNT]={
	0x0abb,0x0aa2,0x0a67,0x0a31,0x09f0,0x09a4,0x0950,0x08f6,0x0895,0x083c,0x07f2,0x07a6,0x0770,0x0763,
	0x0767,0x0764,0x0769,0x0775,0x0786,0x079f,0x07b4,0x07bd,0x07bd,0x07ba,0x07b7,0x07b4,0x07b9,0x07ba,0x07ba,0x07bd,
	0x07bb,0x07b4,0x07b1,0x07b4,0x07b9,0x07be,0x07c6,0x07c7,0x07c0,0x07bc,0x07b7,0x07b1,0x07b4,0x07bf,0x07c6,0x07cb,
	0x07ce,0x07c9,0x07c1,0x07ba,0x07ba,0x07bd,0x07c5,0x07cf,0x07d1,0x07d1,0x07d1,0x07ce,0x07c9,0x07ca,0x07d0,0x07d5,
	0x07db,0x07e2,0x07df,0x07df,0x07df,0x07dd,0x07de,0x07e3,0x07e5,0x07e5,0x07e6,0x07e8,0x07e9,0x07e9,0x07ec,0x07ec,
	0x07ee,0x07f2,0x07f3,0x07f1,0x07f1,0x07f4,0x07f7,0x07fc,0x0800,0x0800,0x07fe,0x07ff,0x07fe,0x07fe,0x0801,0x0804,
	0x0809,0x080b,0x080d,0x080f,0x0810,0x0812,0x0813,0x0817,0x081b,0x0823,0x0829,0x082a,0x0830,0x0832,0x0834,0x083a,
	0x083f,0x0840,0x0845,0x084a,0x084c,0x084c,0x084f,0x0852,0x0853,0x0851,0x0850,0x0852,0x0852,0x0850,0x084f,0x084c,
	0x084a,0x0846,0x0840,0x083d,0x083b,0x0834,0x082e,0x082b,0x0825,0x081b,0x0816,0x080f,0x0801,0x07fa,0x07f0,0x07e3,
	0x07d9,0x07d5,0x07d1,0x07ce,0x07ca,0x07c6,0x07c1,0x07bb,0x07ba,0x07b9,0x07b7,0x07bb,0x07ba,0x07b7,0x07b8,0x07b7,
	0x07b5,0x07b6,0x07b9,0x07b9,0x07b9,0x07ba,0x07b9,0x07b7,0x07b8,0x07b9,0x07b7,0x07b9,0x07bb,0x07b9,0x07b7,0x07b9,
	0x07b8,0x07b7,0x07ba,0x07bb,0x07b8,0x07b7,0x07b9,0x07ba,0x07b7,0x07bb,0x07bc,0x07ba,0x07b9,0x07bb,0x07ba,0x07ba,
	0x07bc,0x07b9,0x07b7,0x07bb,0x07b9,0x07b7,0x07ba,0x07ba,0x07b8,0x07b9,0x07b9,0x07b9,0x07bb,0x07ba,0x07b8,0x07b8,
	0x07b9,0x07ba,0x07b9,0x07ba,0x07ba,0x07ba,0x07b9,0x07bb,0x07bb,0x07ba,0x07bc,0x07bd,0x07bc,0x07bc,0x07ba,0x07b8,
	0x07b8,0x07bc,0x07ba,0x07b9,0x07be,0x07bd,0x07b9,0x07b9,0x07bc,0x07ba,0x07b7,0x07b9,0x07ba,0x07b9,0x07bb,0x07be,
	0x07ba,0x07b9,0x07ba,0x07ba,0x07b9,0x07ba,0x07ba,0x07bb,0x07bb,0x07ba,0x07ba,0x07bc,0x07ba,0x07b8,0x07ba,0x07bd,
	0x07bc,0x07bc,0x07bd,0x07bb,0x07b9,0x07b9,0x07bb,0x07bb,0x07b8,0x07ba,0x07bd,0x07bc,0x07bc,0x07bd,0x07bb,0x07ba,
	0x07bc,0x07bb,0x07b9,0x07bc,0x07bd,0x07bd,0x07c3,0x07ca,0x07cf,0x07d5,0x07db,0x07e2,0x07e9,0x07f3,0x07fa,0x07fe,
	0x0801,0x0806,0x080a,0x080d,0x0814,0x0815,0x0814,0x0818,0x0818,0x0818,0x081a,0x081b,0x0819,0x0818,0x0814,0x0810,
	0x0809,0x0807,0x0804,0x0800,0x07fe,0x07fa,0x07f0,0x07e9,0x07e5,0x07dc,0x07d3,0x07cc,0x07c7,0x07c2,0x07bf,0x07bd,
	0x07ba,0x07bb,0x07bd,0x07ba,0x07b8,0x07bd,0x07bf,0x07bb,0x07bd,0x07be,0x07bb,0x07ba,0x07ba,0x07b8,0x07b7,0x07bc,
	0x07bd,0x07bb,0x07bc,0x07be,0x07bc,0x07bc,0x07bc,0x07bb,0x07b9,0x07ba,0x07bd,0x07bc,0x07be,0x07bf,0x07bb,0x07bb,
	0x07bd,0x07ba,0x07bb,0x07bc,0x07bd,0x07ba,0x07b2,0x07a9,0x079d,0x0790,0x0785,0x0777,0x0769,0x076f,0x0799,0x07d8,
	0x0817,0x085d,0x08a7,0x08f9,0x0952,0x09a4,0x09f4,0x0a43,0x0a9b,0x0abb,

};
unsigned int DEMO_ECG_I[ECGCOUNT]={
	0x0a0f,0x09ea,0x09bb,0x09a2,0x0980,
	0x094d,0x0910,0x08d0,0x0890,0x0857,0x0825,0x07f8,0x07dd,0x07e2,0x07e2,0x07d5,0x07d2,0x07d4,0x07d8,0x07e5,0x07f6,
	0x07f6,0x07f0,0x07f1,0x07f0,0x07eb,0x07e7,0x07ed,0x07f3,0x07f5,0x07f4,0x07ec,0x07e5,0x07e8,0x07ec,0x07ed,0x07f2,
	0x07f8,0x07ef,0x07ea,0x07ee,0x07e6,0x07e2,0x07ee,0x07fd,0x07fe,0x07fb,0x07f9,0x07f0,0x07eb,0x07ee,0x07f0,0x07f3,
	0x0801,0x0804,0x07fe,0x07f8,0x07f5,0x07ee,0x07f1,0x0800,0x0803,0x0802,0x0803,0x0805,0x0800,0x07fb,0x07ff,0x0805,
	0x0808,0x0810,0x080e,0x0806,0x080a,0x080a,0x0808,0x0809,0x0811,0x0812,0x0811,0x0814,0x080d,0x0806,0x080c,0x0818,
	0x0819,0x081a,0x081c,0x0819,0x0814,0x0816,0x0819,0x081c,0x0826,0x082e,0x082b,0x0825,0x0824,0x0821,0x0821,0x082c,
	0x082f,0x082e,0x0833,0x0839,0x0835,0x0830,0x0835,0x083c,0x0841,0x0849,0x0849,0x0844,0x0849,0x084e,0x084c,0x084d,
	0x0856,0x0855,0x084f,0x0854,0x084f,0x0845,0x0849,0x0852,0x084f,0x0849,0x0849,0x0843,0x083b,0x0839,0x0837,0x0833,
	0x0835,0x0835,0x082c,0x0822,0x081a,0x0812,0x080b,0x080b,0x0808,0x0800,0x07fe,0x07ff,0x07f8,0x07f0,0x07f0,0x07f3,
	0x07f2,0x07f5,0x07f5,0x07ec,0x07eb,0x07ee,0x07ee,0x07ed,0x07f1,0x07f2,0x07ef,0x07f0,0x07ef,0x07e4,0x07e8,0x07f3,
	0x07f3,0x07ef,0x07ef,0x07ee,0x07ea,0x07ea,0x07ec,0x07eb,0x07f0,0x07f6,0x07f2,0x07ea,0x07eb,0x07eb,0x07e7,0x07ef,
	0x07f6,0x07ef,0x07ef,0x07f4,0x07ef,0x07e6,0x07e9,0x07ef,0x07f0,0x07f4,0x07f4,0x07ed,0x07eb,0x07ed,0x07ed,0x07ee,
	0x07f2,0x07f1,0x07ee,0x07f0,0x07f0,0x07e7,0x07ea,0x07f4,0x07f3,0x07ef,0x07f1,0x07f0,0x07e9,0x07ea,0x07ee,0x07ee,
	0x07f2,0x07f9,0x07f5,0x07ee,0x07ec,0x07ea,0x07e8,0x07f1,0x07f8,0x07f2,0x07f1,0x07f5,0x07ef,0x07e8,0x07ec,0x07f2,
	0x07f2,0x07f4,0x07f5,0x07ed,0x07eb,0x07ef,0x07ed,0x07ed,0x07f4,0x07f7,0x07f1,0x07f3,0x07f2,0x07ea,0x07ec,0x07f7,
	0x07f6,0x07f0,0x07f3,0x07f4,0x07ed,0x07ec,0x07ef,0x07ef,0x07f1,0x07f7,0x07f6,0x07ef,0x07ee,0x07ed,0x07ea,0x07f1,
	0x07f7,0x07f2,0x07f1,0x07f6,0x07f0,0x07e6,0x07ee,0x07f4,0x07f1,0x07f7,0x07f9,0x07ee,0x07eb,0x07f0,0x07f0,0x07f4,
	0x0800,0x0801,0x0800,0x0805,0x080b,0x0807,0x080b,0x081a,0x081e,0x081a,0x0820,0x0823,0x081f,0x0822,0x0827,0x0826,
	0x082d,0x0833,0x082c,0x0827,0x0828,0x0825,0x0821,0x082a,0x082e,0x0825,0x0820,0x0821,0x081a,0x080f,0x080d,0x0810,
	0x080b,0x080b,0x0809,0x07fe,0x07f8,0x07f6,0x07ee,0x07ed,0x07f4,0x07f6,0x07f0,0x07ee,0x07f1,0x07e9,0x07ea,0x07f5,
	0x07f4,0x07f1,0x07f5,0x07f3,0x07eb,0x07eb,0x07f1,0x07ef,0x07f3,0x07fa,0x07f6,0x07ef,0x07f0,0x07ee,0x07e7,0x07f0,
	0x07f9,0x07f2,0x07f3,0x07f5,0x07ee,0x07e7,0x07ed,0x07f2,0x07f0,0x07f6,0x07f7,0x07ef,0x07ef,0x07f2,0x07ea,0x07e4,
	0x07e5,0x07dc,0x07ca,0x07c4,0x07b9,0x07a7,0x07b1,0x07e4,0x0810,0x0839,0x0871,0x08a5,0x08d7,0x0916,0x0957,0x098d,
	0x09c6,0x0a00,0x0a0f,

};
unsigned int DEMO_ECG_V[ECGCOUNT]={
	0x062c,0x0640,0x0668,0x0695,0x06ca,0x0702,0x0749,
	0x0798,0x07cb,0x07e1,0x07ea,0x07ea,0x07e6,0x07e9,0x07e7,0x07e3,0x07e5,
	0x07e5,0x07de,0x07de,0x07e6,0x07e7,0x07e8,0x07f0,0x07f0,0x07e3,0x07e0,
	0x07de,0x07db,0x07e0,0x07eb,0x07ee,0x07ef,0x07f4,0x07f2,0x07e8,0x07e6,
	0x07e6,0x07e4,0x07ea,0x07f8,0x07f4,0x07ee,0x07f2,0x07f0,0x07e8,0x07ed,
	0x07f4,0x07ee,0x07f2,0x07fb,0x07f6,0x07f3,0x07f8,0x07f5,0x07f2,0x07f3,
	0x07f5,0x07ef,0x07f3,0x07fc,0x07f8,0x07f6,0x07fe,0x07fe,0x07f5,0x07f9,
	0x07fc,0x07f5,0x07f9,0x0800,0x07ff,0x07fd,0x0800,0x07fd,0x07f8,0x07fd,
	0x0801,0x07fd,0x0800,0x0808,0x0801,0x07ff,0x0807,0x0805,0x0801,0x0805,
	0x080b,0x0806,0x0809,0x080d,0x0808,0x0808,0x0814,0x0814,0x0813,0x0819,
	0x0818,0x0810,0x0815,0x081e,0x0819,0x081c,0x0827,0x0822,0x081c,0x081f,
	0x081f,0x0818,0x081c,0x0823,0x081b,0x0818,0x0820,0x0818,0x080f,0x0816,
	0x0819,0x0812,0x0814,0x081b,0x0811,0x080a,0x0811,0x080b,0x0803,0x0806,
	0x0805,0x07fb,0x07fa,0x07f8,0x07ea,0x07eb,0x07f4,0x07ef,0x07e9,0x07ee,
	0x07e8,0x07db,0x07e0,0x07e9,0x07e0,0x07e1,0x07ec,0x07ea,0x07e0,0x07e3,
	0x07e2,0x07dc,0x07e1,0x07e8,0x07e1,0x07e0,0x07e9,0x07e3,0x07dc,0x07e3,
	0x07e4,0x07de,0x07e6,0x07ec,0x07e0,0x07de,0x07e9,0x07e2,0x07dc,0x07e7,
	0x07ec,0x07e1,0x07e1,0x07e7,0x07dd,0x07de,0x07e9,0x07e5,0x07e0,0x07e9,
	0x07e8,0x07dd,0x07e1,0x07e5,0x07dd,0x07e0,0x07ed,0x07e7,0x07de,0x07e4,
	0x07e5,0x07dc,0x07e2,0x07eb,0x07e3,0x07e3,0x07ea,0x07e0,0x07db,0x07e6,
	0x07e7,0x07de,0x07e6,0x07ec,0x07df,0x07dd,0x07e9,0x07e3,0x07dc,0x07e7,
	0x07ea,0x07e3,0x07e5,0x07e6,0x07db,0x07dd,0x07ea,0x07e3,0x07e0,0x07e9,
	0x07e5,0x07db,0x07e2,0x07e5,0x07dd,0x07e1,0x07ec,0x07e6,0x07de,0x07e5,
	0x07e4,0x07dc,0x07e5,0x07e9,0x07e0,0x07e5,0x07ea,0x07e1,0x07db,0x07e6,
	0x07e4,0x07dd,0x07e8,0x07ec,0x07de,0x07df,0x07e9,0x07e1,0x07df,0x07e9,
	0x07e8,0x07e0,0x07e5,0x07e6,0x07dc,0x07df,0x07eb,0x07e4,0x07e3,0x07ef,
	0x07e6,0x07da,0x07e3,0x07e7,0x07db,0x07e1,0x07ef,0x07ea,0x07e6,0x07f2,
	0x07f6,0x07f2,0x07fe,0x0808,0x0804,0x080b,0x0815,0x080e,0x080b,0x0818,
	0x0819,0x0814,0x0821,0x0828,0x081c,0x081c,0x0826,0x081f,0x0819,0x0827,
	0x0826,0x0819,0x081c,0x081b,0x080c,0x080c,0x0816,0x080a,0x0805,0x080d,
	0x0803,0x07f6,0x07f5,0x07f1,0x07e6,0x07e9,0x07ee,0x07e6,0x07df,0x07e5,
	0x07e0,0x07da,0x07e4,0x07e7,0x07e1,0x07e5,0x07ea,0x07df,0x07db,0x07e6,
	0x07e3,0x07dc,0x07e4,0x07e8,0x07de,0x07df,0x07e7,0x07de,0x07db,0x07e7,
	0x07e6,0x07de,0x07e1,0x07e0,0x07d9,0x07e2,0x07e8,0x07de,0x07e0,0x07ec,
	0x07e3,0x07db,0x07e1,0x07e2,0x07dc,0x07e2,0x07ea,0x07e0,0x07dd,0x07e6,
	0x07e1,0x07db,0x07e1,0x07e4,0x07e4,0x07f4,0x0802,0x0803,0x080f,0x082b,
	0x083a,0x0848,0x0861,0x0871,0x087a,0x088c,0x0890,0x0866,0x0836,0x0815,
	0x07e3,0x07a4,0x076a,0x0728,0x06e3,0x06ab,0x0678,0x068c,0x06ac,
};
unsigned int DEMO_RESP[RESPCOUNT]={
	0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c0,
	0x00c0,0x00c0,0x00bf,0x00bf,0x00bf,0x00be,0x00be,0x00bd,0x00bd,0x00bc,
	0x00bc,0x00bb,0x00bb,0x00bb,0x00bb,0x00ba,0x00ba,0x00b9,0x00b9,0x00b8,
	0x00b8,0x00b7,0x00b7,0x00b6,0x00b6,0x00b6,0x00b6,0x00b6,0x00b5,0x00b5,
	0x00b5,0x00b4,0x00b3,0x00b3,0x00b2,0x00b1,0x00b1,0x00b0,0x00af,0x00af,
	0x00ae,0x00ae,0x00ad,0x00ad,0x00ac,0x00ac,0x00ab,0x00aa,0x00a9,0x00a8,
	0x00a8,0x00a7,0x00a7,0x00a6,0x00a6,0x00a5,0x00a5,0x00a4,0x00a4,0x00a3,
	0x00a2,0x00a1,0x00a0,0x00a0,0x009f,0x009e,0x009d,0x009c,0x009b,0x009b,
	0x009a,0x0099,0x0098,0x0097,0x0096,0x0095,0x0094,0x0094,0x0093,0x0092,
	0x0091,0x0091,0x0090,0x008f,0x008e,0x008d,0x008d,0x008c,0x008b,0x008a,
	0x0089,0x0088,0x0087,0x0086,0x0085,0x0084,0x0083,0x0082,0x0081,0x0080,
	0x007f,0x007e,0x007e,0x007d,0x007c,0x007b,0x007a,0x0079,0x0078,0x0076,
	0x0075,0x0075,0x0074,0x0073,0x0072,0x0071,0x0070,0x006f,0x006e,0x006d,
	0x006c,0x006c,0x006b,0x006a,0x0069,0x0069,0x0068,0x0067,0x0066,0x0065,
	0x0064,0x0063,0x0062,0x0061,0x0061,0x0060,0x005f,0x005e,0x005d,0x005c,
	0x005b,0x005b,0x005b,0x005a,0x005a,0x0059,0x0058,0x0058,0x0057,0x0057,
	0x0056,0x0056,0x0055,0x0054,0x0054,0x0054,0x0054,0x0053,0x0053,0x0053,
	0x0052,0x0052,0x0052,0x0052,0x0052,0x0052,0x0051,0x0051,0x0050,0x004f,
	0x004f,0x004e,0x004e,0x004d,0x004d,0x004c,0x004c,0x004c,0x004c,0x004b,
	0x004b,0x004b,0x004a,0x004a,0x0049,0x0049,0x0049,0x0049,0x0049,0x0049,
	0x0049,0x0049,0x0049,0x0049,0x0049,0x0049,0x004a,0x004a,0x004a,0x004a,
	0x004a,0x004a,0x004a,0x004a,0x004a,0x004a,0x004a,0x004b,0x004b,0x004b,
	0x004b,0x004c,0x004c,0x004c,0x004d,0x004d,0x004d,0x004d,0x004e,0x004e,
	0x004f,0x004f,0x0050,0x0050,0x0050,0x0050,0x0051,0x0052,0x0052,0x0053,
	0x0054,0x0055,0x0056,0x0056,0x0057,0x0058,0x0059,0x005a,0x005a,0x005b,
	0x005c,0x005d,0x005e,0x005e,0x005f,0x0060,0x0060,0x0061,0x0061,0x0062,
	0x0063,0x0063,0x0064,0x0064,0x0065,0x0065,0x0066,0x0067,0x0068,0x0069,
	0x006a,0x006b,0x006b,0x006c,0x006d,0x006e,0x006e,0x006f,0x0070,0x0071,
	0x0072,0x0073,0x0074,0x0076,0x0077,0x0077,0x0078,0x0079,0x007a,0x007b,
	0x007c,0x007d,0x007e,0x007f,0x0080,0x0081,0x0082,0x0083,0x0085,0x0086,
	0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008e,0x008f,
	0x0090,0x0092,0x0093,0x0094,0x0096,0x0097,0x0098,0x0099,0x009a,0x009a,
	0x009b,0x009c,0x009d,0x009e,0x009e,0x009f,0x00a0,0x00a1,0x00a2,0x00a3,
	0x00a3,0x00a4,0x00a4,0x00a5,0x00a5,0x00a6,0x00a6,0x00a7,0x00a7,0x00a7,
	0x00a8,0x00a9,0x00a9,0x00aa,0x00ab,0x00ab,0x00ab,0x00ab,0x00ac,0x00ac,
	0x00ad,0x00ae,0x00ae,0x00ae,0x00af,0x00af,0x00b0,0x00b1,0x00b1,0x00b2,
	0x00b3,0x00b3,0x00b4,0x00b4,0x00b5,0x00b5,0x00b6,0x00b6,0x00b6,0x00b6,
	0x00b6,0x00b6,0x00b7,0x00b7,0x00b7,0x00b7,0x00b7,0x00b6,0x00b6,0x00b6,
	0x00b6,0x00b5,0x00b5,0x00b4,0x00b4,0x00b3,0x00b3,0x00b3,0x00b2,0x00b2,
	0x00b2,0x00b1,0x00b1,0x00b1,0x00b1,0x00b1,0x00b0,0x00b0,0x00b0,0x00af,
	0x00af,0x00ae,0x00ad,0x00ad,0x00ac,0x00ac,0x00ac,0x00ab,0x00ab,0x00ab,
	0x00aa,0x00aa,0x00a9,0x00a9,0x00a8,0x00a8,0x00a7,0x00a7,0x00a7,0x00a6,
	0x00a6,0x00a6,0x00a5,0x00a4,0x00a4,0x00a3,0x00a2,0x00a2,0x00a1,0x00a0,
	0x00a0,0x009f,0x009f,0x009e,0x009d,0x009d,0x009c,0x009b,0x009b,0x009a,
	0x0099,0x0098,0x0097,0x0096,0x0095,0x0094,0x0094,0x0093,0x0092,0x0091,
	0x0090,0x008f,0x008e,0x008d,0x008c,0x008b,0x008a,0x0088,0x0087,0x0087,
	0x0086,0x0085,0x0084,0x0083,0x0082,0x0082,0x0081,0x0081,0x0080,0x0080,
	0x007f,0x007e,0x007e,0x007d,0x007d,0x007c,0x007b,0x007a,0x0079,0x0078,
	0x0078,0x0078,0x0077,0x0077,0x0076,0x0075,0x0074,0x0073,0x0072,0x0071,
	0x0070,0x006f,0x006e,0x006d,0x006c,0x006b,0x006a,0x0069,0x0069,0x0068,
	0x0067,0x0066,0x0065,0x0064,0x0063,0x0062,0x0061,0x0060,0x005f,0x005e,
	0x005e,0x005d,0x005c,0x005b,0x005a,0x0059,0x0058,0x0058,0x0057,0x0057,
	0x0056,0x0056,0x0055,0x0055,0x0054,0x0053,0x0053,0x0052,0x0052,0x0051,
	0x0050,0x0050,0x004f,0x004e,0x004e,0x004d,0x004c,0x004c,0x004b,0x004b,
	0x004b,0x004b,0x004a,0x004a,0x004a,0x0049,0x0049,0x0048,0x0048,0x0048,
	0x0047,0x0047,0x0046,0x0045,0x0045,0x0044,0x0044,0x0044,0x0044,0x0043,
	0x0043,0x0042,0x0042,0x0042,0x0042,0x0042,0x0042,0x0041,0x0041,0x0041,
	0x0041,0x0041,0x0041,0x0042,0x0042,0x0042,0x0043,0x0043,0x0043,0x0044,
	0x0044,0x0044,0x0044,0x0044,0x0044,0x0045,0x0045,0x0045,0x0046,0x0046,
	0x0047,0x0047,0x0048,0x0049,0x004a,0x004a,0x004b,0x004b,0x004c,0x004c,
	0x004c,0x004d,0x004d,0x004e,0x004f,0x0050,0x0050,0x0051,0x0052,0x0053,
	0x0053,0x0054,0x0055,0x0055,0x0056,0x0056,0x0057,0x0058,0x0059,0x0059,
	0x005a,0x005b,0x005b,0x005c,0x005d,0x005e,0x005f,0x0060,0x0061,0x0061,
	0x0062,0x0063,0x0063,0x0064,0x0065,0x0066,0x0066,0x0067,0x0068,0x0068,
	0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,0x006f,0x0070,0x0072,0x0073,
	0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x007b,0x007c,0x007d,
	0x007d,0x007e,0x007f,0x007f,0x0080,0x0081,0x0082,0x0083,0x0084,0x0086,
	0x0087,0x0088,0x0089,0x008b,0x008c,0x008d,0x008f,0x0090,0x0091,0x0092,
	0x0093,0x0094,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,
	0x009c,0x009d,0x009e,0x009e,0x009f,0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,
	0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00aa,0x00ab,0x00ac,
	0x00ac,0x00ad,0x00ae,0x00af,0x00af,0x00b0,0x00b1,0x00b2,0x00b2,0x00b3,
	0x00b4,0x00b4,0x00b5,0x00b5,0x00b6,0x00b6,0x00b7,0x00b7,0x00b7,0x00b8,
	0x00b8,0x00b9,0x00ba,0x00ba,0x00bb,0x00bb,0x00bc,0x00bc,0x00bc,0x00bd,
	0x00bd,0x00be,0x00be,0x00be,0x00be,0x00bf,0x00bf,0x00c0,0x00c0,0x00c1,
	0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,
};

unsigned int DEMO_SPO2[SPO2COUNT]={
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //1
   0,  1,  2,  3,  5,  7,  9, 12, 16, 19, //2
  23, 27, 31, 34, 37, 40, 43, 45, 48, 50, //3
  53, 55, 57, 58, 59, 60, 61, 62, 62, 62, //4
  63, 63, 63, 63, 62, 61, 61, 59, 58, 58, //5

  57, 56, 55, 53, 53, 52, 52, 51, 50, 49, //6
  46, 45, 44, 43, 42, 41, 40, 39, 39, 39, //7
  40, 40, 41, 42, 43, 43, 43, 43, 43, 43, //8
  43, 43, 43, 42, 42, 41, 40, 40, 39, 38, //9
  36, 35, 33, 32, 31, 30, 29, 28, 27, 26, //10
  
  25, 24, 23, 23, 23, 23, 21, 21, 20, 19, //11
  17, 17, 16, 16, 15, 15, 14, 14, 13, 13, //12
  13, 11, 11, 11, 11, 10, 10, 10,  9,  9, //13
   8,  7,  6,  5,  4,  3,  2,  1,  0,  0, //14
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //15
   0,  0
};
unsigned int DEMO_IBP1[IBP1COUNT]={
0x0079,0x0079,0x0078,0x0078,0x0078,0x0077,0x0076,0x0076,0x0075,0x0074,
0x0072,0x0071,0x006f,0x006e,0x006c,0x006a,0x0069,0x0068,0x0067,0x0067,
0x0067,0x0067,0x0067,0x0067,0x0067,0x0067,0x0066,0x0065,0x0065,0x0064,
0x0062,0x0061,0x0060,0x005f,0x005e,0x005d,0x005d,0x005c,0x005c,0x005b,
0x005b,0x005a,0x0059,0x0059,0x0058,0x0058,0x0057,0x0056,0x0056,0x0055,
0x0055,0x0054,0x0054,0x0053,0x0053,0x0052,0x0052,0x0052,0x0051,0x0051,
0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,
0x0051,0x0051,0x0051,0x0051,0x0052,0x0053,0x0054,0x0056,0x0058,0x005b,
0x005e,0x0061,0x0063,0x0066,0x0068,0x006b,0x006e,0x0071,0x0073,0x0076,
0x0077,0x0078,0x0079,0x0079,0x0079,0x0079,
   };
unsigned int DEMO_IBP2[IBP2COUNT]={
0x001a,0x001a,
0x0019,0x0019,0x0018,0x0016,0x0015,0x0013,0x0013,0x0012,0x0012,0x0012,
0x0012,0x0012,0x0012,0x0011,0x0011,0x0011,0x0010,0x000f,0x000f,0x000e,
0x000d,0x000d,0x000c,0x000b,0x000a,0x0009,0x0008,0x0007,0x0006,0x0005,
0x0004,0x0003,0x0002,0x0001,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0001,0x0001,0x0001,0x0001,0x0002,0x0002,0x0003,0x0003,0x0003,
0x0003,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,
0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0006,0x0006,
0x0006,0x0006,0x0006,0x0006,0x0007,0x0007,0x0007,0x0008,0x0009,0x000a,
0x000b,0x000c,0x000f,0x0012,0x0014,0x0016,0x0018,0x0019,0x0019,0x001a,
0x001a,0x001a,0x001a,0x001a,
   };
//ECG ȡ�յ�ĺ���
static int Filter_I(int iValue)
{
	static int a = 0;
	static int b = 0;
	static int iCount = 0;
	int c;
	int iRes;
	
//  	return iValue;
	
	// init
	if(iCount ==0){
		a = iValue;
		iCount ++;
		return iValue;
	}
	if(iCount ==1){
		b = iValue;
		iCount ++;
		return iValue;
	}

	//�Ƚ�,ȡ�յ�
	c = iValue;
	iRes = c;
	if(b>a && b>c) iRes = b;
	if(b<a && b<c) iRes = b;
	if(b>a && b<c) iRes = c;
	if(b<a && b>c) iRes = c;
	
	//��ֵ
	a = b; 
//  	b = iValue;
  	b = iRes;

	return iRes;
}

static int Filter_II(int iValue)
{
	static int a = 0;
	static int b = 0;
	static int iCount = 0;
	int c;
	int iRes;
	
//       return iValue;
	
	// init
	if(iCount ==0){
		a = iValue;
		iCount ++;
		return iValue;
	}
	if(iCount ==1){
		b = iValue;
		iCount ++;
		return iValue;
	}

	//�Ƚ�,ȡ�յ�
	iRes = iValue;
	c = iValue;
	if(b>a && b>c){
//  		if(B_PRINTF) printf("---1 ");
		iRes = b;
	} 
	if(b<a && b<c) {
// 		if(B_PRINTF) printf("---2 ");
		iRes = b;
	}
	if(b>a && b<c){
// 		if(B_PRINTF) printf("---3 ");
		iRes = c;
	}
	if(b<a && b>c){
// 		if(B_PRINTF) printf("---4 ");
		iRes = c;
	}
// 	if(B_PRINTF) printf("\n");
	
	//��ֵ
	a = b; 
// 	b = iValue;
  	b = iRes;

	return iRes;
}

/*
	��ECG�Ĳ���Ϊ��׼���յ�200����ʱ�������ݱ�����������͹㲥����  1s
	�����������ݵ���������������۲�
*/
static int SendNetData()
{
	int res;
	S_ECG_VALUE ecgValue;
	int i;
	static int netsend=0;
	
	if(giNetWaveLength_Ecg >=ECG_WAVE_PER_SEC*3){
	//printf("giNetWaveLength_Ibp1=%d,giNetWaveLength_Ibp2=%d\n",giNetWaveLength_Ibp1,giNetWaveLength_Ibp2);
		//���spo2 resp �������㣬�������
		if(giNetWaveLength_SpO2<SPO2_WAVE_PER_SEC){
			for(i=0;i<SPO2_WAVE_PER_SEC-giNetWaveLength_SpO2;i++){
				gbNetWaveBuff_SpO2[giNetWaveLength_SpO2]=gbNetWaveBuff_SpO2[giNetWaveLength_SpO2-1];
				giNetWaveLength_SpO2++;
			}
		}else{
			giNetWaveLength_SpO2=SPO2_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Resp<RESP_WAVE_PER_SEC){
			for(i=0;i<RESP_WAVE_PER_SEC-giNetWaveLength_Resp;i++){
				gbNetWaveBuff_Resp[giNetWaveLength_Resp]=gbNetWaveBuff_Resp[giNetWaveLength_Resp-1];
				giNetWaveLength_Resp++;
			}
		}else{
			giNetWaveLength_Resp=RESP_WAVE_PER_SEC;
		}
	
		if(giNetWaveLength_Ibp1<IBP_WAVE_PER_SEC){
			
			for(i=0;i<IBP_WAVE_PER_SEC-giNetWaveLength_Ibp1;i++){
				gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1]=gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1-1];
				giNetWaveLength_Ibp1++;
			}
		}else{
			giNetWaveLength_Ibp1=IBP_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Ibp2<IBP_WAVE_PER_SEC){
			for(i=0;i<IBP_WAVE_PER_SEC-giNetWaveLength_Ibp2;i++){
				gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2]=gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2-1];
				giNetWaveLength_Ibp2++;
			}
		}else{
			giNetWaveLength_Ibp2=IBP_WAVE_PER_SEC;
		}
		

			NetSend_Data();
	}
	
	
	return 0;
}
/*
	ʵʱ��ӡ
*/
int Real_Wave_Print()
{
	
	//����ʵʱ��ӡ����  200=1s 
		if(giPrintRealWaveIndex>= 200){
			if((gPrinterStatus != PRINT_STATUS_NOPRN )
						 && (gPrinterStatus != PRINT_STATUS_ERR)
					//	 && (gPrinterStatus != PRINT_STATUS_BUSY)
						 && (gPrinterWorkType == PRNWORK_REAL)) {
						 
				//����ʵʱ��ӡ�ķ�ʽ�����������ǹ̶�ʱ�䣬�ж��Ƿ�Ҫ��������
 				if(gCfgPrinter.iDuration == 0) {
					//������ӡ��ֱ������ֹͣ��
					printf("duration printing......\n");
				//	Printer_SetRealWave(&gPrintRealWaves);
					gPrintRealWaves.iCount=3;
					gPrintRealWaves.iLength=giPrintRealWaveIndex;
					Printer_PrintRealWave(&gPrintRealWaves);
				}
				else{
				//��ӡ�̶�ʱ��
					if(giPrintRealWaveCount < gCfgPrinter.iDuration){
					//	Printer_SetRealWave(&gPrintRealWaves);
						gPrintRealWaves.iCount=3;
						gPrintRealWaves.iLength=giPrintRealWaveIndex;
						if(B_PRINTF) printf("11%s:%d Print %d Countg CfgPrinter.iDuration=%d.\n", __FILE__, __LINE__, giPrintRealWaveCount,gCfgPrinter.iDuration);

						Printer_PrintRealWave(&gPrintRealWaves);
						giPrintRealWaveCount ++;
					}
					else{
						//ֹͣʵʱ��ӡ
						Printer_StopPrintRealWave();

 						if(B_PRINTF) printf("22%s:%d Stop Print. %d Count.\n", __FILE__, __LINE__, giPrintRealWaveCount);
					}
				}
			}
			giPrintRealWaveIndex = 0;
		}
	
	
	return 0;
}

/*
	���ø��������Ĳ���ֵ����ͬʱ��1�� 2�����������������ݿɼ������
	��1�� 2����ʱ��ֻ��ֵ����Ӧ����
*/
static int SetECGWaveValue(unsigned int Ch1Value,unsigned int Ch2Value)
{
	int i;
	//if(B_PRINTF) printf("*****%d******%d*****\n",Ch1Value,Ch2Value);
	//����Ϊ7��ͬ��ʱͨ��1 ,2
	if(gCfgSystem.bInterface==SCREEN_7LEADECG||gCfgSystem.bInterface==SCREEN_MULTIECG){
		gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
		gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;

		//�˴�����LA������жϣ�������䣬�������㵼��Ϊ���� 2007-12-01 16:52:22
		if(!gValueEcg.bLAOff && !gValueEcg.bLLOff){
			//III=LL-LA=II-Is
			gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
			//aVR = -(I+II)/2
			gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
			//aVL=(I-III)/2
			gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
			//aVF=(II+III)/2
			gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
		}
		else{
			gValueEcg.iWaves[ECGWAVE_III] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVR] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVL] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVF] = BASELINE_ECG;
		}
		gValueEcg.iWaves[ECGWAVE_V] = BASELINE_ECG;
	}else{
		//��ͨ����v��ʱ����Ҫ���������޸ĵ�����������1,2�����������
		if( gCfgEcg.bChannel1==ECGWAVE_V ){
			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
				if(i != gCfgEcg.bChannel1)
					gValueEcg.iWaves[i] = BASELINE_ECG;
		 	}
			gValueEcg.iWaves[ECGWAVE_V] = Ch1Value;
			switch(gCfgEcg.bChannel2){
				case 0:{//I
					gValueEcg.iWaves[ECGWAVE_I] = Ch2Value;
				}break;
				case 1:{//II
					gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;
				}break;
				case 2:{//III
					gValueEcg.iWaves[ECGWAVE_III] = Ch2Value;
				}break;
				case 3:{//AVR
					gValueEcg.iWaves[ECGWAVE_AVR] = Ch2Value;
				}break;
				case 4:{//AVL
					gValueEcg.iWaves[ECGWAVE_AVL] = Ch2Value;
				}break;
				case 5:{//AVF
					gValueEcg.iWaves[ECGWAVE_AVF] = Ch2Value;
				}break;
				case 6:{//V
					gValueEcg.iWaves[ECGWAVE_V] = Ch2Value;
				}break;
			}
		}else if(gCfgEcg.bChannel2==ECGWAVE_V){
			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
				if(i != gCfgEcg.bChannel1)
					gValueEcg.iWaves[i] = BASELINE_ECG;
		 	}
			gValueEcg.iWaves[ECGWAVE_V] = Ch2Value;
			switch(gCfgEcg.bChannel1){
				case 0:{//I
					gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
				}break;
				case 1:{//II
					gValueEcg.iWaves[ECGWAVE_II] = Ch1Value;
				}break;
				case 2:{//III
					gValueEcg.iWaves[ECGWAVE_III] = Ch1Value;
				}break;
				case 3:{//AVR
					gValueEcg.iWaves[ECGWAVE_AVR] = Ch1Value;
				}break;
				case 4:{//AVL
					gValueEcg.iWaves[ECGWAVE_AVL] = Ch1Value;
				}break;
				case 5:{//AVF
					gValueEcg.iWaves[ECGWAVE_AVF] = Ch1Value;
				}break;
				case 6:{//V
					gValueEcg.iWaves[ECGWAVE_V] = Ch1Value;
				}break;
			}
		}else{
			gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
			gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;

			//�˴�����LA������жϣ�������䣬�������㵼��Ϊ���� 2007-12-01 16:52:22
			if(!gValueEcg.bLAOff && !gValueEcg.bLLOff){
				//III=LL-LA=II-Is
				gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
				//aVR = -(I+II)/2
				gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
				//aVL=(I-III)/2
				gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
				//aVF=(II+III)/2
				gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
			}
			else{
				gValueEcg.iWaves[ECGWAVE_III] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVR] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVL] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVF] = BASELINE_ECG;
			}
			gValueEcg.iWaves[ECGWAVE_V] = BASELINE_ECG;
		}
	
	}
	return 0;
}
/*
	���沨�����ݵ��ļ��У�������
*/
static int SaveWaveValuetoFile()
{
	int i;
	static int savecount=0;
	if(savecount<=SAVETIME){
	
		gSaveWave[savecount]=gValueIbp1.iWave;
	//	gSaveWave[savecount]=Ch1Value;
		
//		if(B_PRINTF) printf("%04x %d ",gSaveWave[savecount],savecount);
			if(savecount%125==0)
				if(!B_PRINTF) printf("**save %d s**\n",savecount/125);
		savecount++;
	}else{
			savecount=0;
			if(!B_PRINTF) printf("wave save---%d--------------------\n",savecount);
			FILE *fp;
			fp  = fopen("/work/data/Moni/ibp_wave.bin", "w");
			
			for(i=0;i<SAVETIME;i++){

				if(i%10==0){
					fprintf(fp,"\n0x%04x,",gSaveWave[i]);
				}else{
					fprintf(fp,"0x%04x,",gSaveWave[i]);
				}
			}
			fclose(fp);
		}
	/*
	if(gbSaveWaveValuetofile){
		if(B_PRINTF) printf("wave save---%d--------------------\n",savecount);
			FILE *fp;
			fp  = fopen("ECGChannel1.bin", "w");
			
			for(i=0;i<500*10;i++){

				if(i%16==0){
					fprintf(fp,"\n0x%04x,",gSaveWave[i]);
				}else{
					fprintf(fp,"0x%04x,",gSaveWave[i]);
				}
			}
			fclose(fp);
		gbSaveWaveValuetofile=FALSE;
	}
	*/
	return 0;
}
#define REALTOPAPER 	3
#define REALTOPAPER_ECG 	1.6
int ECG_Value_To_Printer(int EcgValue)
{
	int value=0;
	if((EcgValue/REALTOPAPER_ECG)>80){
			value=80+(EcgValue/REALTOPAPER_ECG-80)*2.5;
			if(value>160) value=160;
	}else{
			value=80-(80-EcgValue/REALTOPAPER_ECG)*2.5;
			if(value<0) value=0;
	}
	return value;
}
int Save_net_ecg_wave()
{
	static BOOL bSkip;
	
	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){
		if(gCfgEcg.bChannel1==ECGWAVE_V||gCfgEcg.bChannel2==ECGWAVE_V){
			// �����������	
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = BASELINE_ECG;
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = BASELINE_ECG;
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_V];
			
		}else{
			// �����������	
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_V];
		
		}
		
	}else{
		// �����������
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_III];
	}

	//��250�������㽵��200��������
	if((giNetWaveLength_Ecg==0)||((giNetWaveLength_Ecg/3)%5)!=0||bSkip){
		giNetWaveLength_Ecg +=3;	
		bSkip=FALSE;
	}else {
		bSkip=TRUE;
	}

//	giNetWaveLength_Ecg +=3;	
	return 0;
}

int Save_print_wave_value()
{
	static BOOL bSkip;
	int test;
	int ecg1value,ecg2value;
		//���ĵ������Ϊ��׼���ʵʱ����
//		printf("bWave1=%d,bWave2=%d,bWave3=%d\n",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3);
		//Wave1
			
		ecg1value=ECG_Value_To_Printer(gValueEcg.iWaves[gCfgEcg.bChannel1]);
		ecg2value=ECG_Value_To_Printer(gValueEcg.iWaves[gCfgEcg.bChannel2]);
		
			
		switch(gCfgPrinter.bWave1){
			case 0: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] =ecg1value; break;
			case 1: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = ecg2value; break;   
			//case WID_SPO2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
		//	case WID_RESP: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
		//	case WID_IBP1: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
		//	case WID_IBP2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
		//	case WID_CO2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = 0; break;
		}
	
		//Wave2
		switch(gCfgPrinter.bWave2){
		//	case WID_ECG1: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = ecg1value; break;
		//	case WID_ECG2: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = ecg2value; break;   
			case 1: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
			case 2: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
			case 3: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
			case 4: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
			case 5: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] =0; break;
		}
		//Wave3
		switch(gCfgPrinter.bWave3){
		//	case WID_ECG1: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = ecg1value; break;
		//	case WID_ECG2: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = ecg2value; break;   
			case 1: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
			case 2: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
			case 3: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
			case 4: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
			case 5: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = 0; break;
		}
			
		//��250�������㽵��200��������
		if((giPrintRealWaveIndex==0)||(giPrintRealWaveIndex%5)!=0||bSkip){
			giPrintRealWaveIndex ++;
			bSkip=FALSE;
		}else {
			bSkip=TRUE;
		}
		
	return 0;
}
/*
	���ͺ���
*/
static int iBio_Send(const unsigned char *datas, int datalen)
{
	int len = 0;
	int i;
	
	if(gfdSerial_iBio <=0 || datalen <=0 || datas ==NULL) return -1;
		
	sem_wait(&semSend_iBio);
	if(B_PRINTF) printf("port_send: ");
	for(i=0; i< datalen; i++){
		len = write(gfdSerial_iBio, datas+i, 1);	//ʵ��д��ĳ���
		if(B_PRINTF) printf("%.2X ", *(datas+i));
		if(len != 1){
			tcflush(gfdSerial_iBio, TCOFLUSH);
			sem_post(&semSend_iBio);
			return -1;
		}
// 		usleep(5000);
	}
	
	if(B_PRINTF) printf(" End.\n");
	sem_post(&semSend_iBio);

	return datalen;
}


/*
	���պ���
*/
static int iBio_Recv(char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	if(gfdSerial_iBio<=0 || datalen <=0 || data ==NULL) return -1;

	FD_ZERO(&fs_read);
	FD_SET(gfdSerial_iBio, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = 100;//TIMEOUT_USEC;
	
	fs_sel = select(gfdSerial_iBio+1, &fs_read, NULL, NULL, &tv_timeout);
	
	if(fs_sel>0){
		readlen = read(gfdSerial_iBio, data, datalen);
		//if(B_PRINTF) printf("%s:%d RECV Data %d.\n", __FILE__, __LINE__, readlen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}
/*
	��������
*/
int iBio_Analyze(unsigned char*data,int len)
{
	int i;
	
	if(data==NULL||len<2)
		return -1;
	
	switch(data[DATAID]){
/*************************************************************************
*  01: ϵͳ��λ��Ϣ
**************************************************************************/		
		case ID_RESET:{//01: ϵͳ��λ��Ϣ
			if(B_PRINTF) printf("ID_RESET\n");
			//���¶�ȡϵͳ���÷��͵�ģ���
			ResetSysCfgToModule();
			iBioCMD_RESET();
		}break;
		
/*************************************************************************
*  02: ϵͳ״̬
*	data[2]:��ѹ���bit7~4:����,bit3~2:3.3v��ѹ���bit1~0:5v��ѹ���
**************************************************************************/		
		case ID_COM_STS:{// 02: ϵͳ״̬
		
			switch(data[2]&0x0c){
				case 0x00: ///3.3v��ѹ����
					SystemStatus.bState33V=0;
				break;
				case 0x01: ///3.3v��ѹ̫��
					SystemStatus.bState33V=1;
				break;
				case 0x10: ///3.3v��ѹ̫��
					SystemStatus.bState33V=2;
				break;
				default:
				break;
			}
			switch(data[2]&0x03){
				case 0x00: ///5v��ѹ����
					SystemStatus.bState5V=0;
				break;
				case 0x01: ///5v��ѹ̫��
					SystemStatus.bState5V=1;
				break;
				case 0x10: ///5v��ѹ̫��
					SystemStatus.bState5V=2;
				break;
				default:
				break;
			}
			
			IsAlm_Voltage(&SystemStatus,FALSE);
//			if(B_PRINTF) printf("SystemStatus.bState33V=%d,SystemStatus.bState5V=%d\n",SystemStatus.bState33V,SystemStatus.bState5V);
		}break;
		
/*************************************************************************
*  03: ϵͳ�Լ�
*	data[2]:�Լ���1,bit7~5:������4~0:Watchdog,A/D,RAM,ROM,CPU
*	data[3]:�Լ���2,bit7~6:������5~0:IBP,NBP,SPO2,TEMP,RESP,ECG
*	data[4]:�汾��,�汾�š�10������10����1.0��20����2.0��
*	data[5~7]:ģ���ʶ1~3
**************************************************************************/
		case ID_SELFTEST:{// 03: ϵͳ�Լ�
		//	if(B_PRINTF) printf("system state!");
			//�Լ���1
			SystemStatus.bSelWatchdog=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x10);//watchdog 0-�Լ���ȷ1-�Լ����
			SystemStatus.bSelAD=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x08);
			SystemStatus.bSelRAM=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x04);
			SystemStatus.bSelROM=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x02);
			SystemStatus.bSelCPU=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x01);
			//�Լ���2
			SystemStatus.bSelIBP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x20);
			SystemStatus.bSelNBP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x10);
			SystemStatus.bSelSPO2=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x08);
			SystemStatus.bSelTEMP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x04);
			SystemStatus.bSelRESP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x02);
			SystemStatus.bSelECG=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x01);
			//�汾��
			SystemStatus.wVersion=((data[4]&0x7f)+((data[1]&0x04)<<5));
			//ģ���ʾ1
			SystemStatus.wModuleSta1=((data[5]&0x7f)+((data[1]&0x08)<<4));
			//ģ���ʾ2
			SystemStatus.wModuleSta2=((data[6]&0x7f)+((data[1]&0x10)<<3));
			//ģ���ʾ3
			SystemStatus.wModuleSta3=((data[7]&0x7f)+((data[1]&0x20)<<2));
			
			IsAlm_SelfTest(&SystemStatus,FALSE);
			if(B_PRINTF) printf("ID_SELFTEST bSelWatchdog:%d,bSelAD:%d,bSelRAM:%d,bSelROM:%d,bSelCPU:%d\n",
					SystemStatus.bSelWatchdog,SystemStatus.bSelAD,SystemStatus.bSelRAM,SystemStatus.bSelROM,SystemStatus.bSelCPU);
			if(B_PRINTF) printf(" ID_SELFTEST bSelIBP:%d,bSelNBP:%d,bSelSPO2:%d,bSelTEMP:%d,bSelRESP:%d,bSelECG:%d\n",
					SystemStatus.bSelIBP,SystemStatus.bSelNBP,SystemStatus.bSelSPO2,SystemStatus.bSelTEMP,SystemStatus.bSelRESP,SystemStatus.bSelECG);
			if(B_PRINTF) printf("ID_SELFTEST wVersion:%d ,wModuleSta1:%x,wModuleSta2:%x,wModuleSta3:%x \n",
					SystemStatus.wVersion,SystemStatus.wModuleSta1,SystemStatus.wModuleSta2,SystemStatus.wModuleSta3);
		}break;

/*************************************************************************
*   04: ����Ӧ��
*	data[2]:Ӧ������������ID��
*	data[3]:	Ӧ����Ϣ
**************************************************************************/	
		case ID_CMD_ACK:{// 04: ����Ӧ��
			int CmdId;
			int CmdSta;
			CmdId=((data[2]&0x7f)+((data[1]&0x01)<<7));
			CmdSta=((data[3]&0x7f)+((data[1]&0x02)<<6));
			
			switch(CmdSta){
				case	CMDOK://����ɹ�
					if(B_PRINTF) printf("CMDOK 0x%x\n",CmdId);
				break;
				case	ERR_SUM://У��ʹ���
					if(B_PRINTF) printf("ERR_SUM 0x%x\n",CmdId);
				break;
				case	ERR_LEN://��������ȴ���
					if(B_PRINTF) printf("ERR_LEN 0x%x\n",CmdId);
				break;
				case	ERR_INVALCMD://��Ч����
					if(B_PRINTF) printf("ERR_INVALCMD 0x%x\n",CmdId);
				break;
				case	ERR_CMDPARA://����������ݴ���
					if(B_PRINTF) printf("ERR_CMDPARA 0x%x\n",CmdId);
				break;
				case	ERR_ACCEPT://�������
					if(B_PRINTF) printf("ERR_ACCEPT 0x%x\n",CmdId);
				break;
			}
			
		}break;
		
/*************************************************************************
*   05: ECG��������
*	data[2~3]:ͨ��1��������
*	data[4~5]:ͨ��2��������
*	data[6~7]:ͨ��3��������(���ݲ����岻ͬ����Щ��)
*	data[8]:ECG״̬STATUS bit7~2:����bit1:�Ƿ����������bit0:�Ƿ��������
**************************************************************************/	
		case ID_ECGWAVE:{// 05: ECG��������
			
			unsigned int Ch1Value,Ch2Value,Ch3Value;//channel1~3ͨ��1 2 3����
			BOOL HavePacemaker,HaveHeartBeat;
			static int DemoCountEcg=0;
			static int beepcount=0,packcount=0;
			double fGain = 0.0;
			int j;
			Ch1Value=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			Ch1Value+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			Ch2Value=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			Ch2Value+=(((data[5]&0x7f)+((data[1]&0x08)<<4)));
			
		//	Ch3Value=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
		//	Ch3Value+=(((data[7]&0x7f)+((data[1]&0x20)<<2)));
			HavePacemaker=(((data[6]&0x7f)+((data[1]&0x10)<<3)))&(0x01<<1);
			HaveHeartBeat=(((data[6]&0x7f)+((data[1]&0x10)<<3)))&0x01;

			if(HaveHeartBeat&&!gbViewDemoData) {
				beepcount=(beepcount+1)%5;
				if(beepcount==0)
					gbHaveBeep_QRS=TRUE;
			}else{
				beepcount=0;
			}
			//�Ѳ��δ����ı�3s 
			//SaveWaveValuetoFile();
		
	
		 	Ch1Value >>=4;
			Ch2Value >>=4;
			Ch3Value=128;
				//����𲫼���ź�
				//TODO:������ź���Ҫ�˲�����
			if(HavePacemaker&&!gbViewDemoData){
				if(packcount>100){
					gbHavePack=TRUE;
					packcount=0;
				}
				gValueEcg.bPacingpulse = 1;
			}
			else{
				gValueEcg.bPacingpulse = 0;
			}
			packcount=(packcount+1)%500;
		//��ȡ�ĵ粨������,
	 	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){

				/////////////////////////////////////////////////////
			if(gSave){
				//I & II�˲�
				gValueEcg.iWaves[ECGWAVE_I] = Filter_I(gValueEcg.iWaves[ECGWAVE_I]);
				gValueEcg.iWaves[ECGWAVE_II] = Filter_II(gValueEcg.iWaves[ECGWAVE_II]);
				
			if(gbViewDemoData){
					gValueEcg.iWaves[ECGWAVE_I] = DEMO_ECG_I[DemoCountEcg]>>4;
					gValueEcg.iWaves[ECGWAVE_II] = DEMO_ECG_II[DemoCountEcg]>>4;
					//III=LL-LA=II-Is
					gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
					//aVR = -(I+II)/2
					gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
					//aVL=(I-III)/2
					gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
					//aVF=(II+III)/2
					gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
					
					gValueEcg.iWaves[ECGWAVE_V] = DEMO_ECG_V[DemoCountEcg]>>4;
				if(DemoCountEcg==0){
					gbHaveBeep_QRS=TRUE;
					gValueEcg.bPacingpulse = 1;
					gbHavePack=TRUE;
				
				}else if(DemoCountEcg>10)
					gValueEcg.bPacingpulse = 0;

				DemoCountEcg+=2;
				DemoCountEcg=DemoCountEcg%ECGCOUNT;
			}else{
				//���ø���������ֵ
				SetECGWaveValue(Ch1Value,Ch2Value);
			}
				//�洢��������
				Save_net_ecg_wave();
				//�洢��ӡ��������
			//	Save_print_wave_value();
				//������ݵ�ʵʱ�洢������(ԭʼ1x����)
				FillOneRealSaveData();	
				
				//��ӵ�ʵʱECG�洢����
			//	FillOneRealEcgData();
	
			//	if(gbSaveRealEcgData){
			//		FillOneRealEcg_Data();
			//	}

			#if 0
				//���Ų��εĹ�ʽ
				//y = xy - (x-1)BASE	�Ŵ� 
				//y = (y - BASE)*x + BASE  //΢������
				//�������,Ӳ���̶�Ϊ1x����
				switch(gCfgEcg.bGain){
					case ECGGAIN_025X:{
						//fGain = 0.25;
						fGain = 0.5;
					}break;
					case ECGGAIN_05X:{
						fGain = 0.5;
					}break;                                                                 
					case ECGGAIN_2X:{
						fGain = 2.0;
					}break;
					default:{//ECGGAIN_1X
						fGain = 1.0;
					}break;
				}
				//����У׼ϵ��
				fGain = fGain * ((float)gCfgEcg.iAdjust / 100.0);
				//У׼����
				for(j=ECGWAVE_I; j<=ECGWAVE_V; j++){
					gValueEcg.iWaves[j] = (float)(gValueEcg.iWaves[j])*fGain - (fGain-1.0)*(float)BASELINE_ECG;
				}
		#endif
				//----------------------------------------------------
			
				ReceivingData_Ecg1(&gValueEcg);		//��ӵ��������1��
				ReceivingData_Ecg2(&gValueEcg);
				
			   }
			   gSave=!gSave;
			}
			else{///3����
	 			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
					if(i != gCfgEcg.bChannel1)
						gValueEcg.iWaves[i] = BASELINE_ECG;
	 			}
				if(gSave){
				//I & II�˲�
				gValueEcg.iWaves[ECGWAVE_I] = Filter_I(gValueEcg.iWaves[ECGWAVE_I]);
				gValueEcg.iWaves[ECGWAVE_II] = Filter_II(gValueEcg.iWaves[ECGWAVE_II]);
				
				if(gbViewDemoData){
		 				gValueEcg.iWaves[ECGWAVE_I] = DEMO_ECG_I[DemoCountEcg]>>4;
		 				gValueEcg.iWaves[ECGWAVE_II] = DEMO_ECG_II[DemoCountEcg]>>4;
						//III=LL-LA=II-Is
						gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
						if(DemoCountEcg==10)gbHaveBeep_QRS=TRUE;

						DemoCountEcg+=2;
						DemoCountEcg=DemoCountEcg%ECGCOUNT;
				}else{
					switch(gCfgEcg.bChannel1){
						case 0:
							gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
						break;
						case 1:
							gValueEcg.iWaves[ECGWAVE_II] = Ch1Value;
						break;
						case 2:
							gValueEcg.iWaves[ECGWAVE_III] = Ch1Value;
						break;
						default:
							gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
						break;
					}
						/////////////////////////////////////////////////////
			
					
				}
					
				//������ݵ�ʵʱ�洢������(ԭʼ1x����)
				
				FillOneRealSaveData();	
				
		
				//�洢��������
				Save_net_ecg_wave();
				//�洢��ӡ��������
				Save_print_wave_value();

				
		#if 0		
				/////////////////////////////////////////////////////
				// �����������
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_III];
			
				giNetWaveLength_Ecg +=3;
		#endif	
		#if 0
				//�������,Ӳ���̶�Ϊ1x����
				
				switch(gCfgEcg.bGain){
					case ECGGAIN_025X:{
						fGain = 0.25;
					}break;
					case ECGGAIN_05X:{
						fGain = 0.5;
					}break;                                                                 
					case ECGGAIN_2X:{
						fGain = 2.0;
					}break;
					default:{//ECGGAIN_1X
						fGain = 1.0;
					}break;
				}
				for(j=ECGWAVE_I; j<=ECGWAVE_V; j++){
					gValueEcg.iWaves[j] = (float)(gValueEcg.iWaves[j])*fGain - (fGain-1.0)*(float)BASELINE_ECG;
//  					gValueEcg.iWaves[j] = (gbsEcgValue.wave[i]-BASELINE_ECG)*fGain+ BASELINE_ECG;
				}
				
				#endif
				//----------------------------------------------------

				ReceivingData_Ecg1(&gValueEcg);		//��ӵ��������1��
				ReceivingData_Ecg2(&gValueEcg);		//��ӵ��������2��
				}
				gSave=!gSave;
			}
	
		}break;

/*************************************************************************
*    06: �������Ӻ��ź���Ϣ
*	data[2]:������Ϣbit7~4:����bit3~0:V,RA,LA,LL����������Ϣ��1���������䣬0������������
*	data[3]:���ر���bit7~3:����bit2~0:ͨ��3~1������Ϣ����0��������1�����أ�
**************************************************************************/	
		case ID_LEAD_STS:{// 06: �������Ӻ��ź���Ϣ
			BYTE MsgLead;//������Ϣ
			BYTE MsgOverload;//���ر���
			MsgLead=((data[2]&0x7f)+((data[1]&0x01)<<7));
			MsgOverload=((data[3]&0x7f)+((data[1]&0x02)<<6));
			if(gbViewDemoData){
				gValueEcg.bLLOff=FALSE;
				gValueEcg.bLAOff=FALSE;
				gValueEcg.bRAOff=FALSE;
				gValueEcg.bVOff=FALSE;
				gValueResp.bOff=FALSE;
			}else{
				//������Ϣ
				//LL����������Ϣ��1���������䣬0������������
				if(MsgLead&0x01){
					gValueEcg.bLLOff=TRUE;
				}else{
					gValueEcg.bLLOff=FALSE;
					gValueResp.bOff=FALSE;
				}
				 
				//LA����������Ϣ��1���������䣬0����������
				if(MsgLead&0x02)
					gValueEcg.bLAOff=TRUE;
				else
					gValueEcg.bLAOff=FALSE;
				
				//RA����������Ϣ��1���������䣬0������������
				if(MsgLead&0x04){
					gValueEcg.bRAOff=TRUE;
					
				}else{
					gValueEcg.bRAOff=FALSE;
					gValueEcg.bConnected = TRUE;
				}
				
				if(MsgLead&0x01||MsgLead&0x04)
					gValueResp.bOff=TRUE;
				else
					gValueResp.bOff=FALSE;
			//V����������Ϣ��1���������䣬0����������)
				if(gCfgEcg.bLeadType==ECGTYPE_5LEAD){
					if(MsgLead&0x08)
						gValueEcg.bVOff=TRUE;
					else
						gValueEcg.bVOff=FALSE;
				
				
				}
			}
			IsOff_LL(&gValueEcg);
			IsOff_LA(&gValueEcg);
			IsOff_RA(&gValueEcg);
			IsOff_V(&gValueEcg);		
		
			
			//���ر���
			if(MsgOverload&0x01)//ECGͨ��1������Ϣ����0��������1�����أ�
				gValueEcg.bOverLoad1=TRUE;
			else
				gValueEcg.bOverLoad1=FALSE;
			if(MsgOverload&0x02)//ECGͨ��2������Ϣ����0��������1�����أ�
				gValueEcg.bOverLoad2=TRUE;
			else
				gValueEcg.bOverLoad2=FALSE;
			if(MsgOverload&0x04)//ECGͨ��3������Ϣ����0��������1�����أ�
				gValueEcg.bOverLoad3=TRUE;
			else
				gValueEcg.bOverLoad3=FALSE;

			IsAlm_ECGOverload(&gValueEcg,&gCfgEcg,FALSE);
//			if(B_PRINTF) printf(" %x MsgLead LL:%d,LA:%d,RA:%d,V:%d\n",data[2],gValueEcg.bLLOff,gValueEcg.bLAOff,gValueEcg.bRAOff,gValueEcg.bVOff);
//			if(B_PRINTF) printf(" %x bOverLoad1:%d,bOverLoad1:%d,bOverLoad1:%d\n",data[3],gValueEcg.bOverLoad1,gValueEcg.bOverLoad2,gValueEcg.bOverLoad3);

	}break;
	
/*************************************************************************
*  07: ����
*	data[2~3]:���ʣ�16λ�з���������Ч���ݷ�Χ������/��ͯ0��300BPM��������0��350BPM
**************************************************************************/	
		case ID_HR:{// 07: ����
			short EcgHR;
			EcgHR=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			EcgHR+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			
			if(gbViewDemoData){
				gValueEcg.bHRInval=FALSE;
				gValueEcg.wHR = 80;
			}else{
				if(EcgHR>0){
					gValueEcg.wHR = EcgHR;
					gValueEcg.bHRInval=FALSE;
				}else{
					gValueEcg.wHR=0;//��Чֵ
					gValueEcg.bHRInval=TRUE;
				}	
			}
			IsAlm_HR(&gValueEcg, &gCfgEcg, FALSE);	
//		if(B_PRINTF) printf("HR:%d  %d\n",EcgHR,gValueEcg.wHR);

		}break;
/*************************************************************************
*  0b: STƫ��
*	data[2~3]:ST1ƫ��,STƫ��ֵΪ16λ���з�����,���е�ֵ������100��.
*	data[4~5]:ST2ƫ��,	��:125����1.25mv,-125����-1.25mv.-10000 ������Чֵ.
*	data[6~7]:ST3ƫ��,������������
**************************************************************************/	
		case ID_ST:{// 0b: STƫ��
			short ST1,ST2,ST3;
			
			ST1=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			ST1+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			ST2=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			ST2+=(((data[5]&0x7f)+((data[1]&0x08)<<4)));
			
		//	ST3=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
		//	ST3+=(((data[7]&0x7f)+((data[1]&0x20)<<2)));
	
		//	if(B_PRINTF) printf("``````````````ST1:%d,ST2:%d\n",ST1,ST2);
		if(gbViewDemoData){
			gValueEcg.iST1=25;
			gValueEcg.iST2=-25;
		}else{
			if(ST1!=-10000) gValueEcg.iST1=ST1;
			else	gValueEcg.iST1 = 0;
			
			if(ST2!=-10000) gValueEcg.iST2 = ST2;
			else	gValueEcg.iST2 = 0;
			
		}
		
		IsAlm_ST1(&gValueEcg, &gCfgEcg, FALSE);	
		IsAlm_ST2(&gValueEcg, &gCfgEcg, FALSE);	
		
		}break;
/*************************************************************************
*  0c: ST��ģ�岨��ֵ
*	data[2]:˳���,bit7:0-ͨ��1,1-ͨ��2,bit6~0˳���:0��49,��50�δ��ͣ�ÿ��5���ֽ�
*	data[3~7]:STģ������1~5	,8λ�޷�����,250���ֽڵ�STģ�岨��������ɳ���Ϊ1���ӵ���	
*				�粨�Σ����λ���Ϊ128����125������ΪR��λ��
**************************************************************************/	
		case ID_ST_PATTERN:{// 0c: ST��ģ�岨��ֵ
			BYTE bCount;BYTE bChannel;
			BYTE ST[5];
			int i;
			bChannel=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x80);
			bCount=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x7F);
			ST[0]=((data[3]&0x7f)+((data[1]&0x02)<<6));
			ST[1]=((data[4]&0x7f)+((data[1]&0x04)<<5));
			ST[2]=((data[5]&0x7f)+((data[1]&0x08)<<4));
			ST[3]=((data[6]&0x7f)+((data[1]&0x10)<<3));
			ST[4]=((data[7]&0x7f)+((data[1]&0x20)<<2));
			switch(bChannel){
			case 0://ͨ��1
				if(bCount==0){
					for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}else{
					
					for(i=0;i<5;i++){
						gST1Wave[bCount*5+i]=ST[i];
					}
					if(bCount==49)
						gSTStatus=TRUE;
				
				}
			break;
			case 0x80://ͨ��2
				if(bCount==0){
						for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}else{
					for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}
			break;
			}
			
			
		}break;
		
/*************************************************************************
*  10:������
*	data[2]:������������,�����������ݣ�8λ�޷���������Ч���ݷ�Χ0��256��
*			��RA/LL��������ʱ��������Ϊ128�������ݰ�ÿ�뷢��125�Ρ�	
**************************************************************************/	
		case ID_RESPWAVE:{// 10:������
			unsigned char RRWaveValue;
			static int DemoCountRresp;
			static BOOL bSkip;
			RRWaveValue=((data[2]&0x7f)+((data[1]&0x01)<<7));
			

			if(gbViewDemoData){
				gValueResp.iWave=DEMO_RESP[DemoCountRresp];
				//TODO:otherbed demo
			//	NetReceivingData_Resp(&gValueResp);
				DemoCountRresp++;
				DemoCountRresp=DemoCountRresp%RESPCOUNT;
			}else{
				if(!gValueResp.bOff)
					gValueResp.iWave=RRWaveValue;	//  0~255
				else
					gValueResp.iWave = BASELINE_RESP;
			}
		//	ReceivingData_Resp(&gValueResp);
			ReceivingData_Resp(&gValueResp);
			NewFillOneCrgData(TRUE);
			static BOOL skip=TRUE;
			gbNetWaveBuff_Resp[giNetWaveLength_Resp] = gValueResp.iWave;
			
		
			//�����������
			gbNetWaveBuff_Resp[giNetWaveLength_Resp] = (gValueResp.iWave*100)/256;
			
			//��125�������㽵��50 ��������
			if((giNetWaveLength_Resp==0)||(giNetWaveLength_Resp%5)!=0||bSkip){
				if(skip)
					giNetWaveLength_Resp +=1;
				skip=!skip;
				bSkip=FALSE;
			}else {
				bSkip=TRUE;
			}
			
			
		//	giNetWaveLength_Resp +=1;
		
			
			//if(B_PRINTF) printf("RESPwave:%d\n",gValueResp.iWave);

		}break;
		
/*************************************************************************
*   11: ������
*	data[2~3]:16λ�з���������Ч���ݷ�Χ������6��120BrPM����ͯ/������6��150BrPM
*			������Чֵ����Ϣʱ������Ϊ0��
**************************************************************************/	
		case ID_RR:{// 11: ������
			short RRValue;
			RRValue=(((short)(data[1]&0x01)<<7)+(data[2]&0x7f))<<8;
			RRValue+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));

		if(gbViewDemoData){
				gValueResp.bOff=FALSE;
				gValueResp.bInval=FALSE;	
				gValueResp.wRR = 20;
		}else{
			if(RRValue>0){
				//�˴�Ҫ�ж�RR��Դ
				switch(gCfgResp.bSource){
					case RR_SRC_ECG:{
						gValueResp.wRR=(short)RRValue;
					}break; 	
					case RR_SRC_CO2:{
						gValueResp.wRR = gValueCO2.wAwRR;
					}break;
				}
				gValueResp.bInval=FALSE;				
			}else{
				gValueResp.bInval=TRUE;
				gValueResp.wRR=0;
			}
			
		}
			IsAlm_RR(&gValueResp, &gCfgResp, FALSE);


		//		if(B_PRINTF) printf("RESPwRR:%d,%d\n",RRValue,gValueResp.wRR);
	
		}break;
		
/*************************************************************************
*  12:��Ϣ����
*	data[2]0��û��apnea������Ϣ��1����apnea������Ϣ��
**************************************************************************/		
		case ID_APNEA:{ // 12:��Ϣ����
			unsigned char RR_APNEA;
			RR_APNEA=((data[2]&0x7f)+((data[1]&0x01)<<7));
			gValueResp.bApneaAlm=RR_APNEA;
			//����NewDrawWave.c 1�붨ʱ����ʵ�֣�
//			IsAlm_Apnea(&gValueResp, &gCfgResp, FALSE);
			//	if(B_PRINTF) printf("gValueResp.bApnea=%d\n",gValueResp.bApnea);
		}break;
		
/*************************************************************************
* 13: CVA������Ϣ
*	data[2]0��û��CVA������Ϣ��1����CVA������Ϣ��
**************************************************************************/
		case ID_CVA:{ // 13: CVA������Ϣ
			unsigned char RR_CVA;
			RR_CVA=((data[2]&0x7f)+((data[1]&0x01)<<7));
			gValueResp.bCVAAlm=RR_CVA;
			IsAlm_CVA(&gValueResp,&gCfgResp, FALSE);
	//		printf("RR_CVA=%d\n",RR_CVA);
		}break;
		
/*************************************************************************
* 15:��������
*	data[2]̽ͷ״̬status BIT7~2:����,Bit1:����ͨ��2��Bit0:����ͨ��1(0:����1:����)
*	data[3~6]�������ݣ�16λ�з���������Ч���ݷ�Χ��0��500����������10������λ�����϶ȡ�
*	���磬204����20.4�档-100������Ч���ݡ������ݰ�ÿ�뷢��1�Ρ�
**************************************************************************/
		case ID_TEMP:{ // 15:��������
			BYTE TempSta;//̽ͷ״̬
			short TempValue1,TempValue2;//��������
			TempSta=((data[2]&0x7f)+((data[1]&0x01)<<7));
			TempValue1=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			TempValue1+=((data[4]&0x7f)+((data[1]&0x04)<<5));	
			TempValue2=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			TempValue2+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			
		
			if(gbViewDemoData){
				gValueTemp.bOff_T1=FALSE;
				gValueTemp.bOff_T2=FALSE;
				
				gValueTemp.wT1=385;
				gValueTemp.wT2 =365;
				IsAlm_T1(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_T2(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_TD(&gValueTemp, &gCfgTemp, FALSE);
			}else{
			
				//TEMP1����̽ͷ����״̬
				if(TempSta&0x01!=0){
					gValueTemp.bOff_T1=TRUE;
				}else{
					gValueTemp.bOff_T1=FALSE;
					gValueTemp.bConnected_T1=TRUE;
				}
				//TEMP2����̽ͷ����״̬
				if((TempSta&(0x01<<1))!=0){
					gValueTemp.bOff_T2=TRUE;
				}else{
					gValueTemp.bOff_T2=FALSE;
					gValueTemp.bConnected_T2=TRUE;
				}
			
				//Temp1��������
				if(TempValue1>0){//-100��Чֵ
					gValueTemp.wT1 = TempValue1 + gCfgTemp.iAdjust_T1;
				}else{
					gValueTemp.wT1=0;
				}
				IsAlm_T1(&gValueTemp, &gCfgTemp, FALSE);
				//Temp2��������
				if(TempValue2>0){//-100��Чֵs
					gValueTemp.wT2 = TempValue2  + gCfgTemp.iAdjust_T2;
					
				}else{
					gValueTemp.wT2=0;
				
				}
				IsAlm_T2(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_TD(&gValueTemp, &gCfgTemp, FALSE);//�����T2���ݣ�����TD����
			}
			
	 		IsOff_T1(&gValueTemp);
			IsOff_T2(&gValueTemp);

			
			
		//	if(B_PRINTF) printf("TempValue1:%d,TempValue2:%d\n",gValueTemp.wT1,	gValueTemp.wT2);
			
		}break;
		
/*************************************************************************
*16: SPO2��������	
*	data[2],SPO2���Σ�8λ�޷����������ݷ�Χ��0��255��
*	data[3],SPO2����״̬��
*			bit7:SPO2̽ͷ��ָ�����־��1��ʾ̽ͷ��ָ����
*			bit6:������־����λ���ڸñ�־Ϊ1ʱ���Խ�������������ʾ
*			bit5:����������־��1��ʾ������������
*			bit4:SPO2̽ͷ�����־��1��ʾ̽ͷ����
*			bit3~0:SPO2 ��ͼ(��ʾ�����������),0��15
**************************************************************************/
		case ID_SPO2_WAVE:{ // 16: SPO2��������
			unsigned int SPO2WaveValue;
			BYTE FingerOFF,HavePluse,SearchPluse,SensorOFF;
			BYTE Strength;
			static int DemoCountSpo2=0;
			static BOOL bSkip;
			SPO2WaveValue=((data[2]&0x7f)+((data[1]&0x01)<<7));
			SensorOFF=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x10;
			SearchPluse=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x20;
			HavePluse=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x40;
			FingerOFF=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x80;
			Strength=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x0f;
	
			//SPO2̽ͷ��ָ�����־�������demo���־λ��Ϊ��
				if(gbViewDemoData){//demo
					gValueSpO2.bOff=FALSE;
					gValuePulse.bOff =FALSE;
				}else{//normal
					if(FingerOFF!=0){
						gValueSpO2.bOff=TRUE;
						gValuePulse.bOff =TRUE;
						gValueSpO2.bErrCode = 0;
					}else{
						gValueSpO2.bOff=FALSE;
						gValuePulse.bOff =FALSE;
						gValueSpO2.bConnected = TRUE;
						//����������־��1��ʾ������������
						if(SearchPluse!=0){
							gValueSpO2.bSearchPulse=TRUE;
							gValueSpO2.bErrCode |= SPO2_SEARCHPULSE;
						}
						else{
							gValueSpO2.bSearchPulse=FALSE;
							gValueSpO2.bErrCode &= (~SPO2_SEARCHPULSE);
						}
					}
				}
			//�ж����䱨�� 
			IsOff_SpO2(&gValueSpO2);
			//SPO2̽ͷ�����־��
			if(SensorOFF!=0){
				gValueSpO2.bNoSensor=TRUE;
			}else{
				gValueSpO2.bNoSensor=FALSE;
				
			}
			
			//������־����λ���ڸñ�־Ϊ1ʱ���Խ�������������ʾ
			if(HavePluse!=0&&!gbViewDemoData) gbHaveBeep_Pulse=TRUE;
				
			
			//����ǿ��״̬0~3��
			
			gValuePulse.bStrength  = Strength;
			if(gValuePulse.bStrength >15) gValuePulse.bStrength = 15;
			if(gValueSpO2.bOff) gValuePulse.bStrength = 0;	
			
//			if(B_PRINTF) printf("%d,%d,%d,%d,%d\n",gValueSpO2.bOff,gValueSpO2.bNoSensor,gValueSpO2.bSearchPulse,gbHaveBeep_Pulse,gValueSpO2.bStrength);
			//�ж��Ƿ��в�������
			isErr_SpO2(&gValueSpO2);


			//���SpO2����, 0~255,���ź�ʱ��ֵΪ128
		//	ReceivingData_SpO2(&gValueSpO2);
	//		ReceivingData_SpO2(&gValueSpO2);
			if(gbViewDemoData){//demo
				gValueSpO2.bWave=(DEMO_SPO2[DemoCountSpo2])*2;
				if(DemoCountSpo2==0)gbHaveBeep_Pulse=TRUE;
				//TODO:otherbed demo
			//	NetReceivingData_SpO2(&gValueSpO2);
				DemoCountSpo2++;
				DemoCountSpo2=DemoCountSpo2%SPO2COUNT;
			}else{//normal
		 		if(!gValueSpO2.bOff){
					gValueSpO2.bWave=SPO2WaveValue;//TODO:����һ�£���Ҫ����Խ����ж�
				}
		 		else{
					gValueSpO2.bWave =128;
				}
			}
			
			ReceivingData_SpO2(&gValueSpO2);
			ReceivingData_SpO2(&gValueSpO2);
			
			static BOOL skip=TRUE;
			gbNetWaveBuff_SpO2[giNetWaveLength_SpO2] = gValueSpO2.bWave;
			
			//�����������
			gbNetWaveBuff_SpO2[giNetWaveLength_SpO2] = (gValueSpO2.bWave*100)/256;
			//��125�������㽵��50��������
			if((giNetWaveLength_SpO2==0)||(giNetWaveLength_SpO2%5)!=0||bSkip){
					if(skip)
						giNetWaveLength_SpO2 +=1;
					skip=!skip;
				bSkip=FALSE;
			}else {
				bSkip=TRUE;
			}
			
		//	giNetWaveLength_SpO2 +=1;

		//	if(B_PRINTF) printf("gValueSpO2.bWave:%d\n",SPO2WaveValue);
	
		}break;
		
/*************************************************************************
*   17: SPO2����
*	data[2]:�����Ͷ���Ϣ,bit7~6:����,bit5:�����Ͷ��½���־,bit4:����ʱ��̫����־
*			bit3~0:�ź�ǿ��(0��8��15 ������Чֵ)����ʾ����������ǿ��
*	data[3~4]:16λ�з�����,��Ч���ݷ�Χ��0��255BPM��-100 ������Чֵ��
*	data[5]:�����Ͷ�(8λ�з�����),��Ч���ݷ�Χ��0��100%��-100 ������Чֵ��
**************************************************************************/	
		case ID_SPO2_PR:{ // 17: SPO2����
			BYTE Signal,Searchtime,Spo2drop;
			short PR;
			char Spo2Value;
			Signal=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x0f;
			Searchtime=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x10;
			Spo2drop=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x20;
			PR=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			PR+=((data[4]&0x7f)+((data[1]&0x04)<<5));
			Spo2Value=((data[5]&0x7f)+((data[1]&0x08)<<4));

			if(Searchtime!=0)
				gValueSpO2.bSearchTimeout=TRUE;
			else
				gValueSpO2.bSearchTimeout=FALSE;
			
			if(Spo2drop!=0)
				gValueSpO2.bSpo2drop=TRUE;
			else
				gValueSpO2.bSpo2drop=FALSE;
			
			if(gbViewDemoData) {
				gValuePulse.wPR = gValueEcg.wHR;
				gValuePulse.bInvalPR=FALSE;
			}else{
				if(PR>0){
					gValuePulse.wPR=PR;
					gValuePulse.bInvalPR=FALSE;
				}else{
					gValuePulse.wPR=0;
					gValuePulse.bInvalPR=TRUE;
				}
			}
			IsAlm_PR(&gValuePulse, &gCfgPulse, FALSE);
			//�ź�ǿ��
			if(Signal!=15){
					gValueSpO2.bStrength=Signal;
				if(gValueSpO2.bStrength >8) gValueSpO2.bStrength = 8;
				if(gValueSpO2.bStrength <3 && !gValueSpO2.bOff){
					gValueSpO2.bErrCode |= SPO2_WEAKSIGNAL;
				}
				else{
					gValueSpO2.bErrCode &= (~SPO2_WEAKSIGNAL);
				}
			}else{
				gValueSpO2.bStrength=0;
			}
			
			//�������λΪ1 ���Ǵ���128����	
			if(gbViewDemoData){
					gValueSpO2.bSpO2 =98;
					gValueSpO2.bInvalSpo2=FALSE;
			}else{ 
				if(!(Spo2Value&0x01<<7)){
					
					gValueSpO2.bSpO2 = Spo2Value ;
					
					if(gValueSpO2.bSpO2>100) gValueSpO2.bSpO2 = 100;
					
					gValueSpO2.bInvalSpo2=FALSE;
				}else{
					gValueSpO2.bSpO2=0;
					gValueSpO2.bInvalSpo2=TRUE;
				}
			}
		IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
		IsAlmLowest_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);	
		/*	//alarm debug---------
			gValueSpO2.bInvalSpo2=FALSE;
			gValueSpO2.bOff=FALSE;
			gValueSpO2.bSpO2 = 85 ;
			if(gValueSpO2.bSpO2>100) gValueSpO2.bSpO2 = 100;
			IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
			IsAlmLowest_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
		
			//alarm debug---------*/
//			if(B_PRINTF) printf("bSearchTimeout:%d,bSpo2drop:%d,Spo2Value:0x%04x,bSpO2:%d,wPR:%d\n",
//				gValueSpO2.bSearchTimeout,gValueSpO2.bSpo2drop,Spo2Value,gValueSpO2.bSpO2,gValuePulse.wPR);
		
		}break;
		
/*************************************************************************
*   18: IBP��������
*	data[2~3]:IBP1����ֵ
*	data[4~5]:IBP2����ֵ,����õ���IBP ������ԭʼ�Ĳ���ֵ��
*				ʵ�ʵ�ѹ��ֵ��Ҫ����У���У׼���������á�
*	ʵ��IBP ��ѹ�����㹫ʽ�����������ݨC У��ֵ��*У׼ֵ/ 2 (mmHg)
**************************************************************************/	
		case ID_IBP_WAVE:{ // 18: IBP��������
			short WaveIbp1,WaveIbp2;
			WaveIbp1=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			WaveIbp1+=((data[3]&0x7f)+((data[1]&0x02)<<6));
			WaveIbp2=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			WaveIbp2+=((data[5]&0x7f)+((data[1]&0x08)<<4));
			static BOOL bSkip_ibp;
			static int DemoCountibp;
			
			//TODO:
			if(gbViewDemoData){//demo
				gValueIbp1.iWave=(DEMO_IBP1[DemoCountibp]);
				gValueIbp2.iWave=(DEMO_IBP2[DemoCountibp]);

				DemoCountibp++;
				DemoCountibp=DemoCountibp%IBP2COUNT;
			}else{//normal
				gValueIbp1.iWave=WaveIbp1/2;
				gValueIbp2.iWave=WaveIbp2/2;
			}
			
			ReceivingData_Ibp1(&gValueIbp1);
			ReceivingData_Ibp1(&gValueIbp1);
			
		//	gValueIbp2.iWave=(WaveIbp2-gValueIbp2.bZeroResult)*gValueIbp2.bCalResult/2;
			
			ReceivingData_Ibp2(&gValueIbp2);
			ReceivingData_Ibp2(&gValueIbp2);
			
			static BOOL skip_ibp=TRUE;
				//�����������
			gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1] = gValueIbp1.iWave;
			gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2] = gValueIbp2.iWave;				
			//��125�������㽵��100��������
			if((giNetWaveLength_Ibp1==0)||(giNetWaveLength_Ibp1%5)!=0||bSkip_ibp){
					
						giNetWaveLength_Ibp1+=1;
						giNetWaveLength_Ibp2+=1;
						
				bSkip_ibp=FALSE;
			}else {
				bSkip_ibp=TRUE;
			}
		//	if(B_PRINTF) printf("WaveIbp1:%04x,WaveIbp2:%04x \n",WaveIbp1,WaveIbp2);
		}break;
		
/*************************************************************************
*    19: IBP״̬
*	data[2]:IBP����״̬bit7~2:����,1:IBP2����״̬,0:IBP1����״̬(0������������1����������)
**************************************************************************/	
		case ID_IBP_STS:{ // 19: IBP״̬
			BYTE Ibp1Lead,Ibp2Lead;
			Ibp1Lead=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x01;
			Ibp2Lead=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x02;
			
			//TODO:
			
			
			if(gbViewDemoData){
				gValueIbp1.bOff=FALSE;
				gValueIbp2.bOff=FALSE;
			}else{
				if(Ibp1Lead!=0){
					gValueIbp1.bOff=TRUE;
					
				}else{
					gValueIbp1.bOff=FALSE;
					gValueIbp1.bConnected = TRUE;
				}
				if(Ibp2Lead!=0){
					gValueIbp2.bOff=TRUE;
					
				}else{
					gValueIbp2.bOff=FALSE;
					gValueIbp2.bConnected = TRUE;
				}	
			}
			IsOff_IBP1(&gValueIbp1);
			IsOff_IBP2(&gValueIbp2);
			
	//		if(!B_PRINTF) printf("--ibp off--%d %d----\n",(data[2]&0x7f)+((data[1]&0x01)<<7),gValueIbp2.bOff);
		}break;
		
/*************************************************************************
*  1a: IBPѹ��
*	data[2]:ͨ���ţ�0��ͨ��1��1��ͨ��2
*	data[3~4]:IBP����ѹ
*	data[5~6]:IBPƽ��ѹ
*	data[7~8]:IBP����ѹIBPѹ����Χ��-50��300mmHg��-100������Чֵ��
**************************************************************************/	
		case ID_IBP_DATA:{ // 1a: IBPѹ��
			BYTE IbpLead;//����0-IBP1,1-IBP2
			short IbpSys,IbpDia,IbpMean;
			
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			IbpDia=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			IbpDia+=((data[4]&0x7f)+((data[1]&0x04)<<5));
			IbpMean=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			IbpMean+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			IbpSys=((data[7]&0x7f)+((data[1]&0x20)<<2))<<8;
			IbpSys+=((data[8]&0x7f)+((data[1]&0x40)<<1));

			
			if(IbpLead==0){//ibp1
				if(gbViewDemoData){
					gValueIbp1.iSys=120;
					gValueIbp1.iDia=80;
					gValueIbp1.iMean=100;
				}else{
					gValueIbp1.iSys=IbpSys;
					gValueIbp1.iDia=IbpDia;
					gValueIbp1.iMean=IbpMean;
				}
				if(B_PRINTF) printf("+11+++IbpSys,IbpDia,IbpMean,%d %d %d++++\n",IbpSys,IbpDia,IbpMean);
			}else{//ibp2
				if(gbViewDemoData){
					gValueIbp2.iSys=25;
					gValueIbp2.iDia=0;
					gValueIbp2.iMean=13;
				}else{
					gValueIbp2.iSys=IbpSys;
					gValueIbp2.iDia=IbpDia;
					gValueIbp2.iMean=IbpMean;
				}
				if(B_PRINTF) printf("22++IbpSys,IbpDia,IbpMean%d %d %d++++\n",IbpSys,IbpDia,IbpMean);
			}
		
		}break;
		
/*************************************************************************
*  1b: IBPУ���У׼��Ϣ
*	data[2]:IBPͨ���� �ֽ�ֵ��0�� ͨ��1��1�� ͨ��2
*	data[3]:У׼���,ȡֵ:0-У��ɹ�,1 ����ѹ����������,
*			2 У��ѹ������������Χ,3 �������䣨У��ʧ�ܣ� 
*			4��9 ����10 û��У��(��λ��ģ�鷢��û��У�㣬����������λ��)
*	data[4]:IBPУ׼��Ϣ,ȡֵ:0 У׼�ɹ� ,1 ����ѹ���������ģ�
*			2 ѹ������������Χ,3 δУ�㣨У׼ʧ�ܣ�
*			4 �������䣨У׼ʧ�ܣ�5��9 ����
*			10 û��У׼(��λ��ģ�鷢��û��У׼������������λ��)
**************************************************************************/	
		case ID_IBP_CAL:{ // 1b: IBPУ���У׼��Ϣ
			BYTE IbpLead;
			BYTE ZeroResult,CalResult;
			
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			CalResult=(data[3]&0x7f)+((data[1]&0x02)<<6);
			ZeroResult=(data[4]&0x7f)+((data[1]&0x04)<<5);
			if(IbpLead==0){
				//ibp1
				if(gValueIbp1.Calviewmode==1)
					gValueIbp1.bCalResult=CalResult;
			//	if(gValueIbp1.Calviewmode==2)
					gValueIbp1.bZeroResult=ZeroResult;
				if(B_PRINTF) printf("11gValueIbp1.bZeroResult=%d wave=%d\n",gValueIbp1.bZeroResult,gValueIbp1.iWave);
				if(B_PRINTF) printf("111gValueIbp1.bCalResult=%d\n",gValueIbp1.bCalResult);
				IBP1Zeroinfo();
				IBP1Calinfo();
			}else{
				if(gValueIbp2.Calviewmode==1)
					gValueIbp2.bCalResult=CalResult;
			//	if(gValueIbp2.Calviewmode==2)
					gValueIbp2.bZeroResult=ZeroResult;
				if(B_PRINTF) printf("22gValueIbp2.bZeroResult=%d\n",gValueIbp2.bZeroResult);
				if(B_PRINTF) printf("222gValueIbp2.bCalResult=%d\n",gValueIbp2.bCalResult);
				IBP2Zeroinfo();
				IBP2Calinfo();
			}
			
			//TODO:
		//	if(B_PRINTF) printf("%d,%d,%d\n",IbpLead,ZeroResult,CalResult);
		}break;
		
/*************************************************************************
*   1c: IBPУ���У׼ʱ��
*	data[2]:IBPͨ���� �ֽ�ֵ��0�� ͨ��1��1�� ͨ��2
*	data[3]:У׼ʱ�䣭��1��31
*	data[4]:У׼ʱ�䣭��1��12��
*	data[5]:У׼ʱ�䣭��2000��2255��Ĭ�ϣ���2000�꿪ʼ����һ���ֽڱ�ʾ����λ��
**************************************************************************/	
		case ID_IBP_CAL_TIME:{ // 1c: IBPУ���У׼ʱ��
			BYTE IbpLead;
			BYTE CalDay,CalMon,CalYear;//У׼ʱ��:������
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			CalDay=(data[3]&0x7f)+((data[1]&0x02)<<6);
			CalMon=(data[4]&0x7f)+((data[1]&0x04)<<5);
			CalYear=(data[5]&0x7f)+((data[1]&0x08)<<4);
			
			gValueIbp1.sCalTime.bDay=CalDay;
			gValueIbp1.sCalTime.bMonth=CalMon;
			gValueIbp1.sCalTime.wYear=CalYear+2000;
			gValueIbp2.sCalTime.bDay=CalDay;
			gValueIbp2.sCalTime.bMonth=CalMon;
			gValueIbp2.sCalTime.wYear=CalYear+2000;
			
			if(IbpLead==0){
				IBP1time();
			}else{	
				IBP2time();
			}
				//TODO:
			if(B_PRINTF) printf("%d-%d-%d\n",CalDay,CalMon,CalYear);
			
		}break;
		
/*************************************************************************
*  20: NBPʵʱ��������
*	data[2~3]:���ѹ��,���ݷ�Χ��0��300mmHg��-100������Чֵ��
*	data[4]:������ʹ��־,0����ʾ���ʹ������ ��1����ʾ�ڳ���/��ͯģʽ�£���⵽�����������
*	data[5]:��������,��ֵ1~5:�ֶ�/�Զ�/STAT/У׼/©�����
**************************************************************************/	
		case ID_NBP_CUFPRE:{ // 20: NBPʵʱ��������
			short Pressure;//���ѹ��
			BYTE CuffResult,MeasureResult;
			
			Pressure=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			Pressure+=((data[3]&0x7f)+((data[1]&0x02)<<6));

			CuffResult=((data[4]&0x7f)+((data[1]&0x04)<<5));
			MeasureResult=((data[5]&0x7f)+((data[1]&0x08)<<4));

			if(Pressure>0){
				gValueNibp.wPressure=Pressure;
				//�������ѹ����ģʽ���й�ѹ����
				switch(gCfgNibp.bObject){
					case NIBP_OBJECT_CHILD:{
						if(gValueNibp.wPressure > NIBP_PROTECT_CHILD){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
					case NIBP_OBJECT_BABY:{
						if(gValueNibp.wPressure > NIBP_PROTECT_BABY){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
					default:{//NIBP_OBJECT_ADULT
						if(gValueNibp.wPressure > NIBP_PROTECT_ADULT){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
				}
				gValueNibp.bInvalPre=FALSE;
				gValueNibp.bEndType=0;
				gValueNibp.bMeasureOK = FALSE;
				gbNibpKey_Start = FALSE;
				
			}else{
				gValueNibp.bInvalPre=TRUE;
			}
			
	//0����ʾ���ʹ������ ��1����ʾ�ڳ���/��ͯģʽ�£���⵽�����������
	//		if(CuffResult!=0){
			if(CuffResult!=0){
				if(B_PRINTF) printf("#######%x#################\n",gValueNibp.wErrCode &NIBP_ERR_CUFF_MISTAKE);
				StopiBioNibpMeasure();
				if((gValueNibp.wErrCode &NIBP_ERR_CUFF_MISTAKE)==0)
					gValueNibp.wErrCode |= NIBP_ERR_CUFF_MISTAKE;
				//	isErr_iBioNibp(&gValueNibp);
					
			}else{
				gValueNibp.wErrCode&=~NIBP_ERR_CUFF_MISTAKE;
			}
			switch(MeasureResult){
			case 1:///1�����ֶ�������ʽ�� 
				gValueNibp.bMeasureType=NIBP_STATUS_MANUAL;
				break;
			case 2:///2�����Զ�������ʽ��
				gValueNibp.bMeasureType=NIBP_STATUS_AUTO;
				break;
			case 3:///3����STAT������ʽ�� 
				gValueNibp.bMeasureType=NIBP_STATUS_SERIES;
				break;
			case 4:///4����У׼��ʽ��
				gValueNibp.bMeasureType=NIBP_STATUS_CALIBRATION;
				break;
			case 5:///5����©�������
				gValueNibp.bMeasureType=NIBP_STATUS_DETECTION;
				break;
			}
			GetNibpPollState();
			if(B_PRINTF) printf("ID_NBP_CUFPRE 0x20 CuffResult:%d Type=%d .wPressure=%d\n",CuffResult,gValueNibp.bMeasureType,gValueNibp.wPressure);
		}break;
		
/*************************************************************************
*    21: NBP����������
*	data[2]:��������,��ֵ1~5:�ֶ�/�Զ�/STAT/У׼/©�����10-ϵͳ����
**************************************************************************/	
		case ID_NBP_END:{  // 21: NBP����������
			BYTE EndStatus;
			EndStatus=((data[2]&0x7f)+((data[1]&0x01)<<7));
			switch(EndStatus){
			case 1:///1���ֶ�������ʽ�²�������
				gValueNibp.bEndType=NIBP_STATUS_MANUAL;
				break;
			case 2:///2���Զ�������ʽ�²�������
				gValueNibp.bEndType=NIBP_STATUS_AUTO;
				break;
			case 3:///3��STAT��������
				gValueNibp.bEndType=NIBP_STATUS_SERIES;
				break;
			case 4:///4����У׼��ʽ�²�������
				gbNibpCal=FALSE;
				gValueNibp.bEndType=NIBP_STATUS_CALIBRATION;
				break;
			case 5:///5����©������в�������
				gbNibpLeak=FALSE;
				gValueNibp.bEndType=NIBP_STATUS_DETECTION;
				break;
			case 10:///10��ϵͳ���󣬾��������Ϣ��NBP״̬��
				gValueNibp.bEndType=NIBP_STATUS_SYSERROR;
				break;
				
			}
			gValueNibp.bMeasureType=NIBP_STATUS_STOP;
			if(gValueNibp.bEndType!=0||gValueNibp.bEndType!=NIBP_STATUS_SYSERROR){
				//��ǲ������
				gValueNibp.bMeasureOK = TRUE;
				gbNibpKey_Start = TRUE;
				gValueNibp.wPressure=0;
			
				GetNibpResult();
			}
			
			if(B_PRINTF) printf("ID_NBP_END 0x21,gValueNibp.bEndType=%d\n",gValueNibp.bEndType);
		}break;
		
/*************************************************************************
*  22: NBP�������1
*	data[2~3]:����ѹ:
*	data[4~5]:����ѹ:
*	data[6~7]:ƽ��ѹ:16λ�з����������ݷ�Χ0��300mmHg��-100������Чֵ��
**************************************************************************/	
		case ID_NBP_RESULT1:{ // 22: NBP�������1
			short NbpSys,NbpDia,NbpMean;
			NbpSys=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			NbpSys+=((data[3]&0x7f)+((data[1]&0x02)<<6));
			NbpDia=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			NbpDia+=((data[5]&0x7f)+((data[1]&0x08)<<4));
			NbpMean=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
			NbpMean+=((data[7]&0x7f)+((data[1]&0x20)<<2));
			if(NbpSys>0&&NbpDia>0&&NbpMean>0){
				gValueNibp.wSys=NbpSys;
				gValueNibp.wDia=NbpDia;
				gValueNibp.wMean=NbpMean;
			}
			if(gValueNibp.bCanceled){
				gValueNibp.wSys=0;
				gValueNibp.wDia=0;
				gValueNibp.wMean=0;
				IsAlm_NIBP(FALSE, FALSE);
			}
			
			
			if(B_PRINTF) printf("ID_NBP_RESULT1 0x22.wSys=%d,.wDia=%d,.wMean=%d\n",gValueNibp.wSys,gValueNibp.wDia,gValueNibp.wMean);
		}break;
		
/*************************************************************************
*  23: NBP�������2 
*	data[2~3]:���ʣ�16λ�з�������-100������Чֵ
**************************************************************************/	
		case ID_NBP_RESULT2:{ // 23: NBP�������2 wpr
			short NbpPR;
			NbpPR=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			NbpPR+=((data[3]&0x7f)+((data[1]&0x02)<<6));

			if(NbpPR>0){
				gValueNibp.wPR=NbpPR;
			}
			if(gValueNibp.bCanceled){
				gValueNibp.wPR=0;
			}
			if(gValueNibp.bMeasureOK && !gValueNibp.bCanceled){
				if(B_PRINTF) printf("NIBP OK. %d / %d   %d. Error Code is %d\n", 
						gValueNibp.wSys, gValueNibp.wDia, gValueNibp.wMean, gValueNibp.wErrCode);

				IsAlm_NIBP(FALSE, FALSE);
				
				GetSysTime(&(gValueNibp.sTime));
				
				//�洢һ��NIBP����, ����
				RecOneTrendRecord(TRENDNIBP);
				gValueNibp.bMeasureOK=FALSE;
			}
			if(B_PRINTF) printf("ID_NBP_RESULT2 0x23,gValueNibp.wPR=%d\n",gValueNibp.wPR);
		}break;
		
/*************************************************************************
*   24: NBP״̬
*	data[2]:NBP״̬bit7~6:����,bit5~4:������Ϣ00:��ʾ����ģʽ01:��ʾ��ͯģʽ02:��ʾ������ģʽ
*			bit3~0:NBP״̬,
*	data[3]:��������,0-�ֶ�1~14:/1/2/3/4/5/10/15/30/60/90/120/480,15-STAT
*	data[4]:��������,0���޴���,1���������2��©�� 3����ѹ���� 
*			4�����ź�,5������Χ 6�������˶�7����ѹ8���źű���
*			9��©�����ʧ�� 10��ϵͳ���� 11����ʱ 
*	data[5]:NBPʣ��ʱ��,16λ�޷���������λ���롣
**************************************************************************/	
		case ID_NBP_STS:{ // 24: NBP
			BYTE NbpStatus,NbpObject,NbpPeriod,NbpError;
			unsigned int NbpRemTime;
			NbpObject=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x30)>>4;
			NbpStatus=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x0f;
			NbpPeriod=((data[3]&0x7f)+((data[1]&0x02)<<6));
			NbpError=((data[4]&0x7f)+((data[1]&0x04)<<5));
			NbpRemTime=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			NbpRemTime+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			
			//������Ϣ
			switch(NbpObject){
				case 0://����ģʽ
					gValueNibp.bObject=NIBP_OBJECT_ADULT;
					break;
				case 1://��ͯģʽ
					gValueNibp.bObject=NIBP_OBJECT_CHILD;
					break;
				case 2://������ģʽ
					gValueNibp.bObject=NIBP_OBJECT_BABY;
					break;
			}
			//NBP״̬
			switch(NbpStatus){
				case 0://NBP��λ���
					gValueNibp.bStatus=NIBP_STATUS_STOP;
					break;
				case 1://�ֶ�������
					gValueNibp.bStatus=NIBP_STATUS_MANUAL;
					break;
				case 2:// �Զ�������
					gValueNibp.bStatus=NIBP_STATUS_AUTO;
					break;
				case 3://STAT������ʽ��
					gValueNibp.bStatus=NIBP_STATUS_SERIES;
					break;
				case 4:// У׼��
					gValueNibp.bStatus=NIBP_STATUS_CALIBRATION;
					break;
				case 5:// ©�������
					gValueNibp.bStatus=NIBP_STATUS_DETECTION;
					break;
				case 6://NBP��λ
					gValueNibp.bStatus=NIBP_STATUS_RESET;
					break;
				case 10://ϵͳ����
					gValueNibp.bStatus=NIBP_STATUS_SYSERROR;
					break;
				default:
					gValueNibp.bStatus=NIBP_STATUS_STOP;
					break;
			}
			//NBP��������
			switch(NbpPeriod){
				case 0://���ֶ�������ʽ��
					gValueNibp.bPeriod=0;
					break;
				case 1://���Զ�������ʽ�£���Ӧ����Ϊ1����
					gValueNibp.bPeriod=1;
					break;
				case 2:// ����Ϊ2����
					gValueNibp.bPeriod=2;
					break;
				case 3://����Ϊ3����
					gValueNibp.bPeriod=3;
					break;
				case 4:// ����Ϊ4����
					gValueNibp.bPeriod=4;
					break;
				case 5:// ����Ϊ5����
					gValueNibp.bPeriod=5;
					break;
				case 6://����Ϊ10����
					gValueNibp.bPeriod=10;
					break;
				case 7://����Ϊ15����
					gValueNibp.bPeriod=15;
					break;
				case 8://����Ϊ30����
					gValueNibp.bPeriod=30;
					break;
				case 9://����Ϊ1Сʱ60min
					gValueNibp.bPeriod=60;
					break;
				case 10:// ����Ϊ1.5Сʱ90smin
					gValueNibp.bPeriod=90;
					break;
				case 11://����Ϊ2Сʱ120min
					gValueNibp.bPeriod=120;
					break;
				case 12:// ����Ϊ3Сʱ180min
					gValueNibp.bPeriod=180;
					break;
				case 13://����Ϊ4Сʱ240min
					gValueNibp.bPeriod=240;
					break;
				case 14://����Ϊ8Сʱ480min
					gValueNibp.bPeriod=480;
					break;
				case 15://��STAT������ʽ��
					gValueNibp.bPeriod=0;
					break;
			}
		//NBP��������
			switch(NbpError){
				case 0://�޴���
					if(!(gValueNibp.wErrCode&NIBP_ERR_CUFF_MISTAKE)){
						gValueNibp.wErrCode&=~0xffff;
					}
		//			if(B_PRINTF) printf("*******%x*********************************\n",gValueNibp.wErrCode);
					break;
				case 1:////������� 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_CUFF_TOOLAX;
					break;
				case 2://©�� 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_CUFF_LEAK;
					break;
				case 3://��ѹ���� 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_PRESSURE;
					break;
				case 4://΢���ź�
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_WEAK_SIGNAL;
					break;
				case 5://����Χ
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_OVER_RANGE;
					break;
				case 6://�����˶�
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_EXCESSIVE_MOTION;
					break;
				case 7://��ѹ 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_OVERPRESSURE;
					break;
				case 8://�źű��� 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_SIGNAL_SATURATION;
					break;
				case 9://©�����ʧ��
					gValueNibp.wErrCode=0;
					gbNibpLeak=FALSE;
					gValueNibp.wErrCode|=NIBP_ERR_CHECKLEAK;
					break;
				case 10://ϵͳ���� 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_MODULE_FAULT;
					break;
				case 11://��ʱ 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_TIMEOUT;
					break;
				default:
					gValueNibp.wErrCode&=~0xffff;
					break;
			}
			if(gValueNibp.wErrCode!=0){
				gValueNibp.bMeasureOK = FALSE;
				//�ж��Ƿ��Ѿ��������ڲ���
			//	StopNibpAutoMeasure_Error();
			}
			isErr_iBioNibp(&gValueNibp);
			if(B_PRINTF) printf("ID_NBP_STS0x24,NbpObject=%d,NbpStatus=%d,NbpPeriod=%d,NbpError=%04x\n",NbpObject,NbpStatus,NbpPeriod,gValueNibp.wErrCode);
		}
	}

	//��ECG�Ĳ���Ϊ��׼���յ�200����ʱ�������ݱ�����������͹㲥����  200Hz
		
			SendNetData();

		//ʵʱ��ӡ
			Real_Wave_Print();
	
	return 0;
}

/********************************************************************
��������: ������͵���λ����
����: pack----Ҫ���͵��������ָ�룬����Ϊ��δ��֯�õ����ID+DATA��
����ֵ: �ޡ�
********************************************************************/
void SendCommand (unsigned char* pack)
{
	unsigned char head;
	int i;
	int len;
	unsigned char checksum;
	len =gPackLength [*pack];

	/* ���������С��2��ֱ�ӷ���*/
	if (len < 2 )
		return;
	
	/* ��ʼ ����У���*/
	checksum = *(pack);
	if (len > 2 ){
		head = 0;
		for (i = len-2; i > 1; i --) {
			head <<= 1;
			/* ���λ��1 */
			*(pack+i) = *(pack+i-1) | 0x80;
			/* ����У���*/
			checksum += *(pack+i);
			/* ��������ͷ*/
			head |= (*(pack+i-1)&0x80) >> 7;
		}
		/* ����ͷ���λ��1 */
		*(pack+1) = head | 0x80;
		/* У��ͼ�������ͷ*/
		checksum += *(pack+1);
	}
	/* ����У���*/
	*(pack+len-1) = checksum | 0x80;
	/* TODO: �û���������λ�����ڷ�������*/
	/* uart.write(pack, len); */
	
	iBio_Send(pack,len);
}


/******************************************************************
* �����������ӻ������лָ����ݣ�������������Ȼ����ԭ�������У�
*���������ͷ��ɾ����
*
*������
* 	pack ������ָ�룬��ָ�����ָ������Ϳ�ʼ�����ݻ�������
* 	len ��Ҫ��������ݰ�����(���������ͺ�У���)��
*
*����ֵ��
*	 0: ���OK
* 	-1: �����ȴ���
* 	-2: У��ʹ���
*******************************************************************/
int iBio_UnPackData(unsigned char * pack, int len)
{
	unsigned char head;
	int i;
	unsigned char checksum;
	
	if ( len < 2 )
		return -1;
	
	/* ��ʼ����У���*/
	checksum = *pack;
	for (i = 1; i < len - 1; i ++) 
		 checksum+=(*(pack+i)&0x7f);
	
	if ( (checksum&0x7f ) != (*(pack+len-1) &0x7f ))
	{
		//if(B_PRINTF) printf("check sum ERR pack[0]=%d#$#$#$#$#$$#$#$#$$#$#$\n",pack[0]);
		return -2;
	}
	
#if 0
	if(*pack==0x0a) {
		for(i=0;i<len;i++)
			if(B_PRINTF) printf("%02x  ",pack [i]);
		//if(B_PRINTF) printf("\n");
		if(B_PRINTF) printf("\n check sum=%x %x\n",checksum ,(*(pack+len-1)));
	}
#endif
	//��������
	iBio_Analyze(pack,len);
	
	return 0;
}

/********************************************************************
* �����������Ӵ��ڽ��ջ������н����
*������
*	data ���ڻ�����ָ�룻
* 	len Ҫ����Ĵ��ڻ��������ݳ��ȣ�
*����ֵ��
* 	0: ���OK
* 	-1: �����ȴ���
* 	-2: У��ʹ���
*******************************************************************/
int iBio_UnBind (unsigned char *data, int length)
{
	static unsigned char sCurrPacket[10];
	static int sPacketIdFound =0;/*���ܵ�������ID �ı�־��1��ʾ���յ�*/
	static int sCurrPacketLen;/*��ǰ���ܵ��İ�����*/
	unsigned char current_data;
	int result =0;
	int i,res=0;
	/* ��ȡ������������*/
	while (length > 0)
	{
		current_data = * data;
		/* ���յ��������ֽ�*/
		if ( sPacketIdFound ){
		/* ��ǰ�����������ֽڣ���Ч�������ֽڱ�����ڵ���0x80 */
			if (current_data >= 0x80 ){
				// ��������
				sCurrPacket [sCurrPacketLen++] = current_data;
					//if(B_PRINTF) printf("sCurrPacketLen=%d,gPackLength [sCurrPacket [0]]=%d\n",sCurrPacketLen,gPackLength [sCurrPacket [0]]);
				/* ���յ������������ݣ���ʼ���*/
				if (sCurrPacketLen == gPackLength [sCurrPacket [0]]) {
					//���У����Լ���������
					res=iBio_UnPackData(sCurrPacket, sCurrPacketLen);
				//	if(res<0) if(B_PRINTF)printf("%s%d check sum error res=%d\n",__FILE__,__LINE__,res);
					
					sPacketIdFound = 0;
				}
				
			}else{
				/*��ǰ�������С��0x80, ��ض��Ǵ���*/
				sPacketIdFound = 0;
				result = -1;
			}
		}else{
			/* ��鵱ǰ�ֽ��Ƿ���ȷ�İ�����ID */
			if (current_data<0x80&&gPackLength [current_data] > 0 )
			{
				sCurrPacketLen = 1;
				sCurrPacket[0] = current_data;
				sPacketIdFound = 1;
				
			}else {
				/* ��ǰ���ݲ�����ȷ�İ�����*/
				result = -1;
			//	if(B_PRINTF) printf("[3]#########[%x]\n",current_data);
			}
		
		}
		
				
		/* ָ����һ����������*/
		data ++;
		/* ���ݳ��ȼ�1 */
		length --;
	}
	return (result);
}

/*
	ģ�鸴λ��ϢӦ��
*/
int iBioCMD_RESET()
{
	unsigned char packet[10];
	packet[0]=ID_RESET;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD RESET\n");
	return 0;
}
/*
	��ȡ�Լ���
*/
int iBioCMD_GET_POST_RESULT()
{
	unsigned char packet[10];
	packet[0]=CMD_GET_POST_RESULT;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_GET_POST_RESULT\n");
	return 0;
}
/*
	������������
*/
int iBioCMD_PAT_TYPE(BYTE PatType)
{
	unsigned char packet[10];
	packet[0]=CMD_PAT_TYPE;
	switch(PatType){
		case 0:{//����
			packet[1]=0;
		}break;
		case 1:{//��ͯ
			packet[1]=1;
		}break;
		case 2:{//������
			packet[1]=2;
		}break;
		default:{//����
			packet[1]=0;
		}break;
	}
	if(B_PRINTF) printf("Send CMD_PAT_TYPEpacket[1]=%d\n",packet[1]);
	SendCommand(packet);
	//if(B_PRINTF) printf("Send CMD_PAT_TYPE\n");
	return 0;
}
/*
	��������3��5
*/
int iBioCMD_LEADSYSTEM(BYTE LeadSet)
{
	unsigned char packet[10];
	packet[0]=CMD_LEADSYSTEM;
	switch(LeadSet){
		case 0:{///5����
			packet[1]=1;
		}break;
		case 1:{///3����
			packet[1]=0;
		}break;
		default:{///5����
			packet[1]=1;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("CMD_LEADSYSTEM\n");
	return 0;
}
/*
	������ʽ����
*/
int iBioCMD_LEADTYPE(BYTE Channel,BYTE Lead)
{
	unsigned char packet[10];
	packet[0]=CMD_LEADTYPE;
	switch(Channel){
		case 0x00:{//ͨ��1
			packet[1]=Channel;
		}break;
		case 0x10:{//ͨ��2
			packet[1]=Channel;
		}break;
		default:{//ͨ��1
			packet[1]=0x00;
		}break;
	}
	switch(Lead){
		case 0x00:{//I����
			packet[1]|=0x01;
		}break;
		case 0x01:{//II����
			packet[1]|=0x02;
		}break;
		case 0x02:{//III����
			packet[1]|=0x03;
		}break;
		case 0x03:{//AVL����
			packet[1]|=0x04;
		}break;
		case 0x04:{//AVR����
			packet[1]|=0x05;
		}break;
		case 0x05:{//AVF����
			packet[1]|=0x06;
		}break;
		case 0x06:{//V����
			packet[1]|=0x07;
		}break;
		default:{//II����
			packet[1]|=0x02;
		}break;
	}
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_LEADTYPE\n");
	return 0;
}
/*
	�ĵ��˲���ʽ����
*/
int iBioCMD_FILTER_MODE(BYTE FilterMode)
{
	unsigned char packet[10];
	packet[0]=CMD_FILTER_MODE;
	switch(FilterMode){
		case 0x00:{//���
			packet[1]=FilterMode;
		}break;
		case 0x01:{//�໤
			packet[1]=FilterMode;
		}break;
		case 0x02:{//����
			packet[1]=FilterMode;
		}break;
		default:{//���
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_FILTER_MODE\n");
	return 0;
}

/*
	�ĵ���������
*/
int iBioCMD_ECG_GAIN(BYTE Channel,BYTE EcgGain)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_GAIN;
	switch(Channel){
		case 0x00:{//ͨ��1
			packet[1]=Channel;
		}break;
		case 0x10:{//ͨ��2
			packet[1]=Channel;
		}break;
		case 0x20:{//ͨ��3
			packet[1]=Channel;
		}break;
		default:{//ͨ��1
			packet[1]=0x00;
		}break;
	}
	switch(EcgGain){
		case 0x00:{//x0.25
			packet[1]|=0x00;
		}break;
		case 0x01:{//x0.5
			packet[1]|=0x01;
		}break;
		case 0x02:{///x1
			packet[1]|=0x02;
		}break;
		case 0x03:{//x2
			packet[1]|=0x03;
		}break;
		default:{//x1
			packet[1]|=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_GAIN\n");
	return 0;
}

/*
	�ĵ�У׼
*/
int iBioCMD_ECG_CAL(BYTE CalMode)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_CAL;
	switch(CalMode){
		case 0x00:{//��
			packet[1]=CalMode;
		}break;
		case 0x01:{//��
			packet[1]=CalMode;
		}break;
		default:{//��
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_CAL\n");
	return 0;
}
/*
	��Ƶ�������ƿ���
*/
int iBioCMD_ECG_TRAP(BYTE TrapMode)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_TRAP;
	switch(TrapMode){
		case 0x00:{//��
			packet[1]=TrapMode;
		}break;
		case 0x01:{//��
			packet[1]=TrapMode;
		}break;
		default:{//��
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_TRAP\n");
	return 0;
}
/*
	�𲫷�������
*/
int iBioCMD_ID_PACE(BYTE Pace)
{
	unsigned char packet[10];
	packet[0]=CMD_ID_PACE;
	switch(Pace){
		case 0x00:{//��
			packet[1]=0;
		}break;
		case 0x01:{//��mode1
			packet[1]=1;
		}break;
		case 0x02:{//��mode2
			packet[1]=1;
		}break;
		default:{//��
			packet[1]=0;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ID_PACE\n");
	return 0;
}
/*
	ST������ISO��ST��
*/
int iBioCMD_ST_ISO(int ISO,int ST)
{
	unsigned char packet[10];
	packet[0]=CMD_ST_ISO;
	packet[1]=(ISO>>8)&0xff; //ISO1
	packet[2]=ISO&0xff;	//ISO2
	packet[3]=(ST>>8)&0xff;//ST1
	packet[4]=ST&0xff;	//ST2
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ST_ISO\n");
	return 0;
}

/*
	���ʼ���ͨ��
*/
int iBioCMD_ECG_CHANNEL(BYTE EcgChannel)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_CHANNEL;
	switch(EcgChannel){
		case 0x00:{//ͨ��1
			packet[1]=EcgChannel;
		}break;
		case 0x01:{//ͨ��2
			packet[1]=EcgChannel;
		}break;
		case 0x02:{//ͨ��3
			packet[1]=EcgChannel;
		}break;
		case 0x03:{//�Զ�ѡ��
			packet[1]=EcgChannel;
		}break;
		default:{//ͨ��1
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_CHANNEL\n");
	return 0;
}

/*
	�������¼���
*/
int iBioCMD_ECG_LEARN()
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_LEARN;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_LEARN\n");
	return 0;
}

/*
	������������
*/
int iBioCMD_RESP_GAIN(BYTE RespGain)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_GAIN;
	switch(RespGain){
		case 0x00:{//x0.25
			packet[1]=RespGain;
		}break;
		case 0x01:{//x0.5
			packet[1]=RespGain;
		}break;
		case 0x02:{//x1
			packet[1]=RespGain;
		}break;
		case 0x03:{//x2
			packet[1]=RespGain;
		}break;
		case 0x04:{//x4
			packet[1]=RespGain;
		}break;
		default:{//x1
			packet[1]=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_RESP_GAIN\n");
	return 0;
}

/*
	����I/II ��������
*/
int iBioCMD_RESP_TYPE(BYTE LeadSet)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_TYPE;
	switch(LeadSet){
		case 0x00:{//I����
			packet[1]=0x00;
		}break;
		case 0x01:{//II����
			packet[1]=0x01;
		}break;
		default:{//II����
			packet[1]=0x01;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_RESP_TYPE\n");
	return 0;
}
/*
	��Ϣ����ʱ������
*/
int iBioCMD_APNEA_ALM(BYTE Apnea)
{
	unsigned char packet[10];
	packet[0]=CMD_APNEA_ALM;
	switch(Apnea){
		case 0x00:{//������
			packet[1]=Apnea;
		}break;
		case 0x01:{//10s
			packet[1]=Apnea;
		}break;
		case 0x02:{//15s
			packet[1]=Apnea;
		}break;
		case 0x03:{//20s
			packet[1]=Apnea;
		}break;
		case 0x04:{//25s
			packet[1]=Apnea;
		}break;
		case 0x05:{//30s
			packet[1]=Apnea;
		}break;
		case 0x06:{//35s
			packet[1]=Apnea;
		}break;
		case 0x07:{//40s
			packet[1]=Apnea;
		}break;
		default:{//20s
			packet[1]=0x03;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_APNEA_ALM\n");
	return 0;
}
/*
	����̽ͷ��������
*/
int iBioCMD_TEMP_TYPE(BYTE TempType)
{
	unsigned char packet[10];
	packet[0]=CMD_TEMP_TYPE;
	switch(TempType){
		case 0x00:{//YSI̽ͷ
			packet[1]=TempType;
		}break;
		case 0x01:{//CY-F̽ͷ
			packet[1]=TempType;
		}break;
		default:{//YSI̽ͷ
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_TEMP_TYPE\n");
	return 0;
}
/*
	SPO2���ñ��Ͷȼ���������
*/
int iBioCMD_SPO2(BYTE Spo2Sense)
{
	unsigned char packet[10];
	packet[0]=CMD_SPO2;
	switch(Spo2Sense){
		case 0x01:{//�����ȸ�
			packet[1]=Spo2Sense;
		}break;
		case 0x02:{//��������
			packet[1]=Spo2Sense;
		}break;
		case 0x03:{//�����ȵ�
			packet[1]=Spo2Sense;
		}break;
		default:{//��������
			packet[1]=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_SPO2\n");
	return 0;
}
/*
	NBP�����ֶ�/�Զ�����
*/
int iBioCMD_NBP_START()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_START;

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_START\n");
	return 0;
}
/*
	NBP��ֹ����
*/
int iBioCMD_NBP_END()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_END;

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_END\n");
	return 0;
}
/*
	NBP������������
*/
int iBioCMD_NBP_PERIOD(BYTE NBPMode)
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PERIOD;
	switch(NBPMode){
		case 0://�ֶ�����
		case 1:///1 ����
		case 2:///2 ����
		case 3:///3 ����
		case 4:///4 ����
		case 5:///5 ����
		case 6:///10 ����
		case 7:///15 ����
		case 8:///30 ����
		case 9:///60 ����
		case 10:///90 ����
		case 11:///120 ����
		case 12:///180 ����
		case 13:///240����
		case 14:///480 ����
			packet[1]=NBPMode;
			break;
		default:{//�ֶ�����
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PERIOD\n");
	return 0;
}
/*
	NBPУ׼
*/
int iBioCMD_NBP_CAL()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_CAL;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_CAL\n");
	return 0;
}
/*
	NBP��λ
*/
int iBioCMD_NBP_RESET()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_RESET;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_RESET\n");
	return 0;
}
/*
	NBP©�����
*/
int iBioCMD_NBP_PNEUMATIC()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PNEUMATIC;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PNEUMATIC\n");
	return 0;
}
/*
	NBP��ѯ״̬
*/
int iBioCMD_NBP_POLL_STATE()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_POLL_STATE;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_POLL_STATE\n");
	return 0;
}
/*
	NBP���ó��γ���ѹ��
*/
int iBioCMD_NBP_PRE_PRESSURE(BYTE PatientType,unsigned int PresetPressure)
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PRE_PRESSURE;
	if(B_PRINTF) printf("PatientType=%d PresetPressure=%d \n",PatientType,PresetPressure);
	switch(PatientType){
		case 0://����
		case 1://��ͯ
		case 2://������
			packet[1]=PatientType;
			break;
		default:{//����
			packet[1]=0;
		}break;
		if(B_PRINTF) printf("`````packet[1]=%d \n",packet[1]);

	}
	switch(PresetPressure){
		case 60://60 mmHg
		case 70://70 mmHg
		case 80://80 mmHg
		case 100://100 mmHg
		case 120://120 mmHg
		case 140://140 mmHg
		case 150://150 mmHg
		case 160://160 mmHg
		case 180://180 mmHg
		case 200://200 mmHg
		case 220://220 mmHg
		case 240://240 mmHg
			packet[2]=PresetPressure;
			break;
		default:{//160 mmHg
			packet[2]=160;
		}break;
		if(B_PRINTF) printf("`````packet[2]=%d \n",packet[2]);
	}
	
	if(B_PRINTF) printf("`````packet[1]=%d \n",packet[1]);
	if(B_PRINTF) printf("`````packet[2]=%d \n",packet[2]);
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PRE_PRESSURE%d %d\n",packet[1],packet[2]);
	return 0;
}

/*
	NBP����STAT����
*/
int iBioCMD_NBP_CONTINUE()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_CONTINUE;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_CONTINUE\n");
	return 0;
}

/*
	NBP��ѯ�������
*/
int iBioCMD_NBP_GET_RESULT()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_GET_RESULT;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_GET_RESULT\n");
	return 0;
}
/*
	IBPͨ��ѹ������
*/
int iBioCMD_IBP_CHANNEL(BYTE Channel1,BYTE Channel2,BYTE Name1,BYTE Name2)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CHANNEL;
	switch(Channel1){
		case 0://��������1(map)
		case 1://��������3(sys,map,dia)
			packet[1]|=3<<Channel1;
		break;
		default://��������3(sys,map,dia)
			packet[1]|=3<<1;
		break;
	}
	switch(Name1){
		case 0://ART
		case 1://PA
		case 2://CVP
		case 3://LAP
		case 4://RAP
		case 5://ICP
		case 6://P1
		case 7://P2
			packet[1]|=Name1;
		break;
		default:
			packet[1]|=0x00;
		break;
	}
	switch(Channel2){
		case 0://��������1(map)
		case 1://��������3(sys,map,dia)
			packet[1]|=7<<Channel2;
		break;
		default://��������3(sys,map,dia)
			packet[1]|=7<<1;
		break;
	}
	switch(Name2){
		case 0://ART
		case 1://PA
		case 2://CVP
		case 3://LAP
		case 4://RAP
		case 5://ICP
		case 6://P1
		case 7://P2
			packet[1]|=4<<Name2;
		break;
		default:
			packet[1]|=4<<0x02;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CHANNEL\n");
	return 0;
}
/*
	IBPУ��
*/
int iBioCMD_IBP_ZERO(BYTE IbpChannel)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_ZERO;
	switch(IbpChannel){
		case 0://ͨ��1
		case 1://ͨ��2
			packet[1]=IbpChannel;
		break;
		default:
			packet[1]=0;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_ZERO\n");
	return 0;
}
/*
	IBPͨ��1У׼
*/
int iBioCMD_IBP_CAL1(BYTE Pressure,S_TIME *bstime)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CAL1;
	packet[1]=Pressure;
	packet[2]=bstime->bDay;
	packet[3]=bstime->bMonth;
	packet[4]=bstime->wYear-2000;
	//printf("%d,%d,%d,%d\n",packet[1],packet[2],packet[3],packet[4]);

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CAL1\n");
	return 0;
}
/*
	IBPͨ��2У׼
*/
int iBioCMD_IBP_CAL2(BYTE Pressure,S_TIME *bstime)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CAL2;
	packet[1]=Pressure;
	packet[2]=bstime->bDay;
	packet[3]=bstime->bMonth;
	packet[4]=bstime->wYear;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CAL2\n");
	return 0;
}
/*
	IBP�˲�ģʽ����
*/
int iBioCMD_IBP_FILTER(BYTE IbpFilterMode)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_FILTER;
	switch(IbpFilterMode){
		case 0://ƽ��
		case 1://����
			packet[1]=IbpFilterMode;
		break;
		default:
			packet[1]=1;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_FILTER\n");
	return 0;
}
/*
	IBP��ѯ״̬
*/
int iBioCMD_IBP_POLL_STATE(BYTE IbpChannel,BYTE IbpId)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_POLL_STATE;
	switch(IbpChannel){
		case 0://ͨ��1
		case 1://ͨ��2
			packet[1]=IbpChannel;
		break;
		default://ͨ��1
			packet[1]=0;
		break;
	}
	switch(IbpId){
		case 0://IBPУ���У׼��Ϣ
			packet[2]=0x1b;
		break;
		case 1://IBPУ׼ʱ��
			packet[2]=0x1c;
		break;
		default://ͨ��1
			packet[2]=0x1b;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_POLL_STATE\n");
	return 0;
}
/*
	���յ�ģ�鸴λ��Ϣ�����¶�ȡϵͳ���÷�����ģ��
*/
int ResetSysCfgToModule()
{
	printf("*******reset**************\n");
	
	//������Ϣ����object
	iBioCMD_PAT_TYPE(gCfgPatient.bObject);

	//��������3/5����
	iBioCMD_LEADSYSTEM(gCfgEcg.bLeadType);
	
	//����I/II��������
	iBioCMD_RESP_TYPE(gCfgResp.bLeadType);//new
	
	//������ʽѡ��
	ChangeECGLeadChannel();
	//�˲���ʽѡ��
	iBioCMD_FILTER_MODE(gCfgEcg.bMode);

	//�ĵ�����ѡ��
	iBioCMD_ECG_GAIN(gCfgEcg.bChannel1,gCfgEcg.bGain);
	//iBioCMD_ECG_GAIN(gCfgEcg.bChannel2,gCfgEcg.bGain);
	///1mvУ׼�ź�����
	iBioCMD_ECG_CAL(gCfgEcg.b1mvCal);//new
		
	//��Ƶ��������
	//iBioCMD_ECG_TRAP(gCfgEcg.bFilterAC);
	iBioCMD_ECG_TRAP(1);
	//�𲫷�������
	iBioCMD_ID_PACE(gCfgEcg.bPacemaker);
	//ST������ISO ��ST��
	iBioCMD_ST_ISO(gCfgEcg.bISO,gCfgEcg.bST);//new
	//��������ѡ��
	iBioCMD_RESP_GAIN(gCfgResp.bGain);
	//��Ϣ����ʱ������
	iBioCMD_APNEA_ALM(gCfgResp.bApnea);//new
	//����̽ͷ��������
	iBioCMD_TEMP_TYPE(gCfgTemp.bTempType);//new
	//SPO2����������
	iBioCMD_SPO2(gCfgSpO2.bResponse);
	//NBP�ֶ�/�Զ�����
	//iBioCMD_NBP_PERIOD(0);//����Ϊ�ֶ�
	//NBP���ó��γ���ѹ��
	if(B_PRINTF) printf("gCfgNibp.bObject:%d ,gCfgNibp.wInitPres:%d\n",gCfgNibp.bObject,gCfgNibp.wInitPres);
	iBioCMD_NBP_PRE_PRESSURE(gCfgNibp.bObject,gCfgNibp.wInitPres);
	//IBPѹ��ͨ������
	iBioCMD_IBP_CHANNEL(gCfgIbp1.bViewMode,gCfgIbp2.bViewMode,gCfgIbp1.bLabel,gCfgIbp2.bLabel);//new
	//����IBP�����˲�����
	iBioCMD_IBP_FILTER(gCfgIbp1.bFilterMode);//new
	//��ȡϵͳ�Լ���
	iBioCMD_GET_POST_RESULT();

	return 0 ;
}
//���ò�������
int SetiBioPatientType()
{
	iBioCMD_PAT_TYPE(gCfgPatient.bObject);
	return 0;
}
//����NIBP��������
int SetiBioNIBPPatType()
{
	iBioCMD_PAT_TYPE(gCfgNibp.bObject);
	return 0;
}
//����ECG����
int SetiBioECGGain()
{
	iBioCMD_ECG_GAIN(0x00,gCfgEcg.bGain);
	iBioCMD_ECG_GAIN(0x10,gCfgEcg.bGain);
	return 0;	
}

//����ECG�������ͺͲ�������
int SetiBioEcgLeadType()
{
	//��������3/5����
	iBioCMD_LEADSYSTEM(gCfgEcg.bLeadType);
		
	return 0;
}

///3������ʽѡ��
int SetiBioECG3LeadChannel()
{
	iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);///3����ֻ��ͨ��1
	
	return 0;
}

///5������ʽѡ��
int SetiBioECG5LeadChannel()
{
	//5����������ʹ��Ĭ��ͨ��
	iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
	iBioCMD_LEADTYPE(CHANNEL2,gCfgEcg.bChannel2);
		
	return 0;
}
int ChangeECGLeadChannel()
{
	if(gbiBioECGCaling){
	
		return 0;	
	}
		
	//��7��ͬ��
	if(gCfgSystem.bInterface!=SCREEN_7LEADECG&&
		gCfgSystem.bInterface!=SCREEN_MULTIECG){
		//ͨ��1,2 Ϊv��
		if(gCfgEcg.bChannel1==ECGWAVE_V||gCfgEcg.bChannel2==ECGWAVE_V){
			iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
			iBioCMD_LEADTYPE(CHANNEL2,gCfgEcg.bChannel2);
		}else{
			if(gCfgEcg.bLeadType==ECGLEAD5){///5����
				iBioCMD_LEADTYPE(CHANNEL2,2);
				
				iBioCMD_LEADTYPE(CHANNEL1,0);
				iBioCMD_LEADTYPE(CHANNEL2,1);
				
			}else
				iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
		}
	}else{
		if(gCfgEcg.bLeadType==ECGLEAD5){///5����
				iBioCMD_LEADTYPE(CHANNEL2,2);
				
				iBioCMD_LEADTYPE(CHANNEL1,0);
				iBioCMD_LEADTYPE(CHANNEL2,1);

		}else
			iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
	}
	
	return 0;
}
//����ECG������ʽ
int SetiBioECGMode()
{
	
	//�˲���ʽѡ��
	iBioCMD_FILTER_MODE(gCfgEcg.bMode);
	return 0;	
}
//�����𲫷�������
int SetiBioEcgPace()
{
	//�𲫷�������
	iBioCMD_ID_PACE(gCfgEcg.bPacemaker);
	return 0;
}

//����ECG�����˲����ƹ�Ƶ�˲�
int SetiBioEcgACFilter()
{
	//��Ƶ��������
	iBioCMD_ECG_TRAP(gCfgEcg.bFilterAC);
	return 0;
}

//��ʼECGУ׼
int StartiBioCalibrateECG()
{
	ChangeECGLeadChannel();
	iBioCMD_ECG_CAL(1);
	
	//�Ƿ��ڶ���״̬
 	gbiBioECGCaling = TRUE;
	return 0;
}

//ֹͣECGУ׼
int StopiBioCalibrateECG()
{
	iBioCMD_ECG_CAL(0);
	//�Ƿ��ڶ���״̬
 	gbiBioECGCaling = FALSE;
	return 0;
}
//����ISO ST ��
int SetiBioEcgISOandST()
{
	//ST������ISO ��ST��
	iBioCMD_ST_ISO(gCfgEcg.bISO,gCfgEcg.bST);//new
	
	return 0;
}
//����RESP����
int SetiBioRespGain()
{
	//��������ѡ��
	iBioCMD_RESP_GAIN(gCfgResp.bGain);
	return 0;
}
//������Ϣ����ʱ��
int SetiBioRespApnea()
{
	iBioCMD_APNEA_ALM(gCfgResp.bApnea);//new
	return 0;
}
//����respI/II��������
int SetiBioRespType(BYTE leadtype)
{
	iBioCMD_RESP_TYPE(leadtype);
	return 0;
}
//SPO2����������
int SetiBioSpO2Sense()
{
	
	iBioCMD_SPO2(gCfgSpO2.bResponse);
	return 0;
}

//��ʼNIBP����
int StartiBioNibpMeasure()
{
	iBioCMD_NBP_START();
	//��λ������
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;	
	gValueNibp.wErrCode=0;
	return 0;
}

//ȡ��NIBP����
int StopiBioNibpMeasure()
{
	iBioCMD_NBP_END();
	
	//��λ������
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;

	return 0;
}
//ѪѹУ׼
int CalibrateiBioNibp()
{
	iBioCMD_NBP_CAL();
	 gbNibpProtect = FALSE;
	 gbNibpCal=TRUE;
	return 0;
}
//©�����
//iPressure:����ѹ��
int iBioNibpLeakTest()
{
	iBioCMD_NBP_PNEUMATIC();
	 gbNibpLeak=TRUE;
	gbNibpProtect = FALSE;
	gValueNibp.wErrCode&=~0xffff;
	return 0;
}
//���ó�ʼ������ѹ��
int SetiBioNibpInitPres()
{
	//NBP���ó��γ���ѹ��
	iBioCMD_NBP_PRE_PRESSURE(gCfgNibp.bObject,gCfgNibp.wInitPres);
	
	gbNibpProtect = FALSE;
	return 0;
}
//��ȡNIBP�������
int GetNibpResult()
{
	iBioCMD_NBP_GET_RESULT();
	return 0;
}
//��ȡNIBP��ǰ״̬
int GetNibpPollState()
{
	iBioCMD_NBP_POLL_STATE();
	return 0;
}
//����NIBP��������
int SetIBioNibpPeriod(BYTE Period)
{
	iBioCMD_NBP_PERIOD(Period);
	return 0;
}
//����̽ͷ��������
int SetIBioTempType()
{
	iBioCMD_TEMP_TYPE(gCfgTemp.bTempType);//new
	return 0;
}
	
/*
	�����߳�
*/
void  *ptProc_iBioRecv(void *arg)
{
	int iLen; 
	int i;
	unsigned char bBuff[RECVBUFF_MAX_IBIO];	
	
	if(B_PRINTF) printf("(%s:%d)create read thread for %s .\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_iBio]);
	
	for(;;){

		iLen = iBio_Recv(bBuff, RECVBUFF_MAX_IBIO, 115200);

		if(iLen>0){
			
 			iBio_UnBind(bBuff,iLen);
		}
		else{
		//	if(B_PRINTF) printf("(%s:%d) %s ID:%d receive error.\n\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_iBio], gSerialIndex_iBio);
			
		}
	}
	
	if(B_PRINTF) printf("\nBye from %d thread.\n", gfdSerial_iBio);
	pthread_exit("Bye");
}



/*
	��ʼ������ģ��
*/
int InitiBio()
{
	int res;
		
	if(B_PRINTF) printf("%s:%d Init iBio board ...\n", __FILE__, __LINE__);
	//��ʼ���ź���
	res = sem_init(&semSend_iBio, 0, 1);	
	
	//�򿪴����豸
	gfdSerial_iBio = port_open(gSerialsName[gSerialIndex_iBio], "115200", 8, 0, 1, 0);
	if(gfdSerial_iBio<0){
		if(B_PRINTF) printf("%s:%d Serial_iBio ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d Serial_iBio=%d OK!\n", __FILE__, __LINE__,gfdSerial_iBio);
		
	//ע������߳� 
	res = pthread_create(&ptID_iBioRecv, NULL, ptProc_iBioRecv, NULL);
	if(res<0){
		if(B_PRINTF) printf("%s:%d BioRecv pthread ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d OK!\n", __FILE__, __LINE__);
//	iBioCMD_ECG_TRAP(1);
	return 0;
}

////////////////////////////////////////
