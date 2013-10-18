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
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Serial_Funcs.h"

//���岨����
int baud_tbl[] = {B4800, B9600, B19200, B38400, B57600, B115200};
char *baud_str[] = {"4800", "9600", "19200", "38400", "57600", "115200"};


//����д������ͬ���ź���
sem_t 	semSend;
//ttyS1:rs232-j15  ttyS2:ttl-j20 tts0:ttl-j19 tts1:ttl-j16
unsigned char *gSerialsName[] = {"/dev/ttyS1", "/dev/ttyS2", 
	"/dev/tts0", "/dev/tts1", "/dev/tts2"};
/*
	��ʼ������д������ͬ���ź���
*/
int InitSemForPortSend()
{
	int res;
		
	res = sem_init(&semSend, 0, 1);

	return res;
}

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
int port_open(const unsigned char *devname, const char *baud, int size, int check, int stop, int control)
{
	struct termios newterm;
	int speed, i;
	int fd ;

	speed = B115200;
	for(i=0; i<6; i++){
		if(strcmp(baud, baud_str[i])==0){
			speed = baud_tbl[i];
		}
	}
	
	fd = open(devname, O_RDWR | O_NONBLOCK | O_NOCTTY);
	printf("%s:%d open %s !\n", __FILE__, __LINE__, devname);
	if(fd<0){
		DEBUGINFO();
		printf("%s:%d open %s failure!\n", __FILE__, __LINE__, devname);
		return -1;
	}
	
	tcgetattr(fd, &newterm);
	cfmakeraw(&newterm);

	cfsetispeed(&newterm, speed);
	cfsetospeed(&newterm, speed);
	
	switch(control){
		case 1:{//hardware control
			newterm.c_cflag |= CRTSCTS;
		}break;
		case 2:{//software control
			newterm.c_cflag |= IXON | IXOFF |IXANY;
		}break;
		default:{//no flow control	
			newterm.c_cflag &= ~CRTSCTS;
		}break;
	}

	newterm.c_cflag &= ~CSIZE;
	switch(size){
		case 5:{
			newterm.c_cflag |= CS5;
		}break;
		case 6:{
			newterm.c_cflag |= CS6;
		}break;
		case 7:{
			newterm.c_cflag |= CS7;
		}break;
		default:{
			newterm.c_cflag |= CS8;
		}break;
	}

	switch(check){                                
		case 1:{//odd parity
			newterm.c_cflag |= PARENB;	
			newterm.c_cflag &= ~PARODD;
		}break;
		case 2:{//even parity
			newterm.c_cflag |= PARENB;
			newterm.c_cflag |= PARODD;
		}break;
		default:{//no parity
			newterm.c_cflag &= ~PARENB; 
		}break; 	
	}
	
	switch(stop){
		case 2:{//2 stop bits
			newterm.c_cflag |= CSTOPB;	
		}break;
		default:{// 1 stop bit
			newterm.c_cflag &= ~CSTOPB;
		}break;
	}
	tcsetattr(fd, TCSANOW, &newterm);

	return fd;
}

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
int port_close(int fdcom)
{
	if(fdcom>0) {
		close(fdcom);
		return 0;
	}
	else{
		return -1;
	}
}

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
*	����������	���ڷ������� 
*	����:		
*	����:		
********************************************/
int port_send(int fdcom, const unsigned char *datas, int datalen)
{
	
	int len = 0;
	int i;
	
	sem_wait(&semSend);
// 	DEBUGINFO();
// 	printf("%d port_send: ", fdcom);
	for(i=0; i< datalen; i++){
		len = write(fdcom, datas+i, 1);	//ʵ��д��ĳ���
// 		printf("%.2X ", *(data+i));
		if(len != 1){
			tcflush(fdcom, TCOFLUSH);
			sem_post(&semSend);
			return -1;
		}
		usleep(5000);
	}
// 	printf(" End.\n");
	sem_post(&semSend);
			
	return datalen;
}

/*******************************************
*	������:		port_recv0()
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
int port_recv(int fdcom, char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	FD_ZERO(&fs_read);
	FD_SET(fdcom, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = TIMEOUT_USEC;
		
	fs_sel = select(fdcom+1, &fs_read, NULL, NULL, &tv_timeout);
	if(fs_sel>0){
		readlen = read(fdcom, data, datalen);
		return(readlen);
	}
	else{
		return(-1);
	}
	
	return (readlen);
}

/*
	��ʼ��ϵͳ����
*/
int InitSystemSerials()
{
	//��ʼ������д������ͬ���ź���
	InitSemForPortSend();
	
	return 0;
}


