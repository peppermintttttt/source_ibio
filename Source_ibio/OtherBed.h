/*************************************************************************** 
 *	Module Name:	OtherBed.h
 *
 *	Abstract:	������������
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2009-10-20 14:18:30
 ***************************************************************************/
#ifndef __OTHERBED_H__
#define __OTHERBED_H__

typedef struct {
	//Information
	BYTE bBedNO;		//����
	char sIP[16];				//IP
	int iPort;						//�˿�

}S_OTHERBED_DATA, *PS_OTHERBED_DATA;

#endif //__OTHERBED_H__
