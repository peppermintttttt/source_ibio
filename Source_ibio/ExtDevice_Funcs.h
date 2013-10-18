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

#ifndef __INCLUDEFILES_H__
#define __INCLUDEFILES_H__
      	
/*
	��ʼ����չ�豸
*/
int InitExtDevices();
/*
	ע��,�˴��ƹ��On and Off ��ioctl���ø㷴��,�Ǻ�
*/
//�����
int RedLightOn();
//�����
int RedLightOff();
//�Ƶ���
int YellowLightOn();
//�Ƶ���
int YellowLightOff();
//�췧��
int AdultValveOn();
//�췧��
int AdultValveOff();
//�з���
int ChildValveOn();
//�з���
int ChildValveOff();
/*��û�������״̬��1������������0��16��󽫹رյ�Դ*/
int GetWorkStatus();
/*��û�����Դ���ͣ�1������Դ��0�����*/
int GetPowerType();
/*
	��õ�ص���	
*/
int GetBatteryStatus();
/*
	����ռ�ձ�
*/
int PWM_ChangeRate(int rate);
/*
	����PWM��ʱ��
*/
int PWM_StartTimer();
/*
	�رն�ʱ��
*/
int PWM_CloseTimer();
/*
	���̲���	
*/
int Key_Up();
int Key_Down();
int Key_Left();
int Key_Right();
int Key_Tab();
int Key_ShiftTab();
int Key_Space();
int Key_Enter();
int Key_Esc();
      	
/*
	���Ź�
*/
int WDT_Start();
int WDT_Feed();
      	
#endif	//__INCLUDEFILES_H__
