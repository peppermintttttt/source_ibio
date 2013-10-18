/*************************************************************************** 
 *	Module Name:	Serial_Funcs
 *
 *	Abstract:	���ڲ�������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-28 09:14:06
 ***************************************************************************/

#ifndef __SERIAL_FUNCS_H__
#define __SERIAL_FUNCS_H__

//���ݲ����ʼ�����ճ�ʱ
#define TIMEOUT_SEC(buflen,baud) 	(buflen*20/baud+2)
#define TIMEOUT_USEC		0

/*******************************************
*	������:		port_open()
*	����:		
*			devname:�����豸		
*			baud: ������			
*			size:����λ
*			check:	У��			0--No, 1--Odd, 2--even
*			stop: ֹͣλ			1-1, 2-2
*			control:��������		0-- no control, 1--hardware, 2--software
*	���:
*			-1: Error
*
*	����������	�򿪲���ʼ��ָ������ 
*	����:		
*	����:		
********************************************/
int port_open(const unsigned char *devname, const char *baud, int size, int check, int stop, int control);
/*******************************************
*	������:		port_close()
*	����:		
*			devname:�����豸		
*
*	���:
*			-1: Error
*
*	����������	�ر�ָ������ 
*	����:		
*	����:		
********************************************/
int port_close(int fdcom);

/*******************************************
*	������:		port_send()
*	����:		
*			fdcom: ���ھ��						
*			datas: ׼�����͵�����				
*			datalen: ���͵����ݳ���				
*
*	���:
*			-1: Error
*
*	����������	����0�������� 
*	����:		
*	����:		
********************************************/
int port_send(int fdcom, const unsigned char *datas, int datalen);
/*******************************************
*	������:		port_recv()
*	����:		
*		fdcom: ���ھ��						
*		datas: ���ݻ���							
*		datalen: ׼���������ݵĳ���
*		baudrate: ������				
*
*	���:
*			-1: Error
*
*	����������	����0�������� 
*	����:		
*	����:		
********************************************/
int port_recv(int fdcom, char *data, int datalen, int baudrate);

/*
	��ʼ��ϵͳ����
*/
int InitSystemSerials();

#endif	//__SERIAL_FUNCS_H__
