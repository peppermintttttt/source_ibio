
/*************************************************************************** 
 *	Module Name:	NetFunc
 *
 *	Abstract:		���紫�����
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2008-10-21 13:47:46
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "Central.h"


//���߻��׽���
int gfdMonitorSocket = 0;
//�����۲�
int gfdSocket_Bed = 0;
//���߻����ն˿�
int giMonitorPort_Info = 2002;
 //�������ݽ��ն˿�
int giOtherBedPort_Data= 2003;

//////////////// funciton //////////////////////////

//ͨ�������ļ������������
int  LoadNetSetFromCfg(const unsigned char *pCfgFile, PS_NET_CFG pCfg)
{
	int iCfgValue=0;	
	char cCfgBuff[16]={0};
	
	if(pCfgFile == NULL || pCfg == NULL) return -1;
	GetIntValueFromEtcFile(pCfgFile, "NET", "bed", &iCfgValue);
	if(iCfgValue <=0) iCfgValue = 1;
	pCfg->wBedNO = iCfgValue;
	if(B_PRINTF) printf("BedNO: %d.\n", pCfg->wBedNO);
	
	memset(cCfgBuff, 0, sizeof cCfgBuff);
	GetStringFromResFile(pCfgFile, "NET", "ip", cCfgBuff, sizeof cCfgBuff);
	strncpy(pCfg->cIP, cCfgBuff, sizeof cCfgBuff);
	if(B_PRINTF) printf("IP: %s.\n", pCfg->cIP);
	
	memset(cCfgBuff, 0, sizeof cCfgBuff);
	GetStringFromResFile(pCfgFile, "NET", "mask", cCfgBuff, sizeof cCfgBuff);	
	strncpy(pCfg->cMask, cCfgBuff, sizeof cCfgBuff);
	if(B_PRINTF) printf("MASK: %s.\n", pCfg->cMask);

	memset(cCfgBuff, 0, sizeof cCfgBuff);
	GetStringFromResFile(pCfgFile, "NET", "gw", cCfgBuff, sizeof cCfgBuff);	
	strncpy(pCfg->cGW, cCfgBuff, sizeof cCfgBuff);
	if(B_PRINTF) printf("GW: %s.\n", pCfg->cGW);

	return 0;
}		
unsigned char *GetNetIPAddr(int ipval)
{
	//Get Net Info
	static unsigned char tmpstr[16];
	unsigned char *ptmp[4];
	unsigned int ivalue[4];
	
	//IP
	strncpy(tmpstr, gCfgNet.cIP, 16);
	ptmp[0] = (unsigned char *)strtok(tmpstr, ".");
	ivalue[0]=strtol(ptmp[0] ,NULL,10);

	ptmp[1]= (unsigned char *)strtok(NULL, ".");
	ivalue[1]=strtol(ptmp[1] ,NULL,10);
	
	ptmp[2]= (unsigned char *)strtok(NULL, ".");
	ivalue[2]=strtol(ptmp[2] ,NULL,10);
	
	ptmp[3] = (unsigned char *)strtok(NULL, ".");
	ivalue[3]=strtol(ptmp[3] ,NULL,10);

	
	memset(tmpstr, 0, sizeof tmpstr);
	sprintf(tmpstr, "%d.%d.%d.%d", ivalue[0], ivalue[1], ivalue[2],ipval);
	
	
	return tmpstr;
}

//�������������
int NetSet(const PS_NET_CFG pCfg)
{
	char code[50] = {0};

// 	return 0;
	

	if(pCfg == NULL){
		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/ifconfig eth0 %s  up&", "192.168.0.1");
		system(code);

		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/route add default gw %s &", "192.168.0.1");
		system(code);
		
		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/ifconfig lo %s up &", "127.0.0.1");
		system(code);
	}
	else{
		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/ifconfig eth0 %s  up&", pCfg->cIP);
		system(code);

		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/route add default gw %s", pCfg->cGW);
		system(code);
		
		memset(code, 0, sizeof code);
		snprintf(code, sizeof code, "/sbin/ifconfig lo %s up &", "127.0.0.1");
		system(code);
	}
	
	
	return 0;
}

/*
	���緢�ͺ���
	return 
		ʵ�ʷ��͵ĳ���
*/

int NetSend(int iSocketFd, const char *pIP, int iPort, void *pMsg, int iSendLen)
{
	struct sockaddr_in out_addr;
	int iLen;
	char *p;

	if(iSocketFd<=0) return -1;
	
	out_addr.sin_family = AF_INET;
	out_addr.sin_port = htons(iPort);
	
	if(pIP == NULL){
		out_addr.sin_addr.s_addr = inet_addr(GetNetIPAddr(BROADCASTIP));
	}
	else  out_addr.sin_addr.s_addr = inet_addr(pIP); 
	
	bzero(&(out_addr.sin_zero),8);
	iLen  = sendto(iSocketFd, pMsg, iSendLen, 0, (struct sockaddr*)&out_addr,sizeof(struct sockaddr)); 
	
 	//if(B_PRINTF); printf("Net Send %d %sDatas.\n", iLen ,pIP);
 	
	return  iLen;
}


/*
	������պ���
	return :
		ʵ�ʽ��յĳ���
*/
int NetRecvBuff(int iSocketFd, BYTE *pRecvBuff, int *iPort, char *sCentralIP,BYTE* IPNum)
{
	struct sockaddr_in recv_addr;
	int addr_len = sizeof(struct sockaddr_in);
	int iCount;
	if(iSocketFd<=0) return -1;
	
	iCount = recvfrom(iSocketFd, pRecvBuff, NETRECVBUFF_MAX_SIZE, 0, (struct sockaddr *)&recv_addr, (socklen_t*)&addr_len);
	
	strcpy(sCentralIP, inet_ntoa(recv_addr.sin_addr));
	
	*IPNum=((inet_addr(inet_ntoa(recv_addr.sin_addr)))>>24)&0xff;
	
	
	
	*iPort = htons(recv_addr.sin_port);
	
	return iCount;
}



///////////////////////////////// interface /////////////////////////////
/*
	�����ʼ��
*/
int NetInit()
{
	int so_broadcast = TRUE;
	struct sockaddr_in in_addr;	
	
	//ͨ�������ļ������������
//	LoadNetSetFromCfg(gFileMachineConfig, &gCfgNet);
	
	//��������
	NetSet(&gCfgNet);
	
	//����UDP�ķ�ʽ��ʼ������
	gfdMonitorSocket = socket(AF_INET, SOCK_DGRAM, 0);
	// Set Socket Option
	if(setsockopt(gfdMonitorSocket, SOL_SOCKET, SO_BROADCAST, &so_broadcast,sizeof(so_broadcast)) == -1)	{
		if(B_PRINTF) printf("%s:%d setsockopt failure!\n", __FILE__, __LINE__);
	}
		
	//Bind Socket
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(giMonitorPort_Info);
	in_addr.sin_addr.s_addr =INADDR_ANY;
	bzero(&(in_addr.sin_zero),8);
	bind(gfdMonitorSocket, (struct sockaddr *)&in_addr, sizeof (struct sockaddr));
	
#ifdef  OTHERBED_FUNC	
	//��������
	gfdSocket_Bed = socket(AF_INET, SOCK_DGRAM, 0);
	// Set Socket Option
	if(setsockopt(gfdSocket_Bed, SOL_SOCKET, SO_BROADCAST, &so_broadcast,sizeof(so_broadcast)) == -1)	{
		if(B_PRINTF) printf("%s:%d setsockopt failure!\n", __FILE__, __LINE__);
	}
		
	//Bind Socket
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(giOtherBedPort_Data);
	in_addr.sin_addr.s_addr =INADDR_ANY;
	bzero(&(in_addr.sin_zero),8);
	bind(gfdSocket_Bed, (struct sockaddr *)&in_addr, sizeof (struct sockaddr));
	////////////////////////////////////////
#endif
	
	//������緢�͵ı���ͷ
	FillNetMsgHead();
	
	//Send broadcast data
	NetSend_Braodcast();


	//Create thread for receiving data
	CreatNetRecvThread();



#ifdef  OTHERBED_FUNC	
	CreatNetRecvThread_Bed();
#endif

	return 0;
}
