/*************************************************************************** 
 *	Module Name:	IncludeFiles.h	
 *
 *	Abstract:	�����������е�һЩ����ͷ�ļ�
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-22 19:44:53
 ***************************************************************************/
 

#ifndef __INCLUDEFILES_H__
#define __INCLUDEFILES_H__

#include <stdio.h>			//printf(), sprintf()
#include <stdlib.h>			//atoi(), atof(), strtol(), strtod()
#include <unistd.h>			//write(), close(), lseek()
#include <linux/ioctl.h>
#include <sys/types.h>		//select() etc.
#include <sys/stat.h>		//S_IRUSR, S_IWUSR �ļ�����
#include <sys/time.h>		//struct timeval
#include <sys/socket.h>		//socket(), setsockopt()
#include <fcntl.h>			//fcntl(), open()
#include <arpa/inet.h>		//inet_addr()
#include <netinet/in.h>		//struct sockaddr_in, htonl(), htons(), ntohl(), ntohs()
#include <netdb.h>			//gethostbyname(), struct hostent
#include <sys/utsname.h>	//uname(), struct utsname
#include <pthread.h>		//pthread_create(), pthread_exit(), pthread_cancel(), pthread_join() etc.
#include <signal.h>			//�ź�
#include <semaphore.h>		//�߳�ͬ���ź�
#include <limits.h>			//һЩ���ݳ�������, ��PIPE_BUF
#include <sys/mman.h>		//mmap
#include <math.h>			
#include <sys/ioctl.h>
#include <linux/kd.h>		//�����豸
#include <errno.h>
#include <termios.h>
#include <getopt.h>
#include <net/if.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/soundcard.h>


/*Minigui ���ͷ�ļ�*/
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

//Printer
#include "gtprint.h"

//Ecg Analysis
// #include <ECGAnalysis.h>
//#include "EcgAnalysisApi.h"

#endif	//__INCLUDEFILES_H__
