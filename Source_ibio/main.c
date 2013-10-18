/*************************************************************************** 
 *	Module Name:	main.c	
 *
 *	Abstract:	������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	            2007-05-22 19:41:59
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"

/*
	��ں���
*/
int MiniGUIMain(int argc, const char *argv[])
{
	
	//��ʼ��д�ļ��Ļ�����
	InitWriteFileMutex();
	
	//��ʼ��ϵͳ
	InitSystem();

	//��ʼ����չ�豸
	InitExtDevices();
	
	//���������߳�
	CreateSoundProc();
	
	//--- ��õ�Դ״̬ ---
	GetPowerStatus();
	
	//��ʼ�����ݹ���
	InitDataManage();
	
 	//��ʼ��ϵͳ����
 	InitSystemSerials();

	//�����Զ����ƴ洢
  	StartAutoTrendSave();
	
	//������ӡ��
//	Printer_Open();

	//�������
 	Test_LightAndSound();
	
	//-- ����ͼ�ν��� --
 	GUI_Main(argc, argv);
	
	printf("%s:%d EXIT main\n", __FILE__, __LINE__);
	
	
	return 0;	
}

