/*************************************************************************** 
 *	Module Name:	DlgNew_DRUG
 *
 *	Abstract:ҩ��Ũ�ȼ���͵ζ���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-17 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_DRUG 
#define DID_DRUG_HELP			DID_DRUG
#define DID_DRUG_OK				DID_DRUG+1
#define DID_DRUG_CAPTION		DID_DRUG+2
#define DID_DRUG_TITRATION		DID_DRUG+3

#define DID_DRUG_DRUGNAME		DID_DRUG+4
#define DID_DRUG_WEIGHT			DID_DRUG+10
#define DID_DRUG_AMOUNT			DID_DRUG+15
#define DID_DRUG_VOLUME			DID_DRUG+20
#define DID_DRUG_CONCENTRATION		DID_DRUG+25
#define DID_DRUG_DOSEMIN		DID_DRUG+30
#define DID_DRUG_DOSEHR			DID_DRUG+35
#define DID_DRUG_DOSEKGMIN		DID_DRUG+40
#define DID_DRUG_DOSEKGHR		DID_DRUG+45
#define DID_DRUG_INFORATE		DID_DRUG+50
#define DID_DRUG_DRIPRATE		DID_DRUG+55
#define DID_DRUG_DRIPSIZE		DID_DRUG+60
#define DID_DRUG_DURATION		DID_DRUG+65

//-----------ҩ�ﵥλ--------------------
#define	DRUG_UNIT_MCG		0x00
#define	DRUG_UNIT_MG		0x01
#define	DRUG_UNIT_G		0x02
#define	DRUG_UNIT_MEQ		0x03
#define	DRUG_UNIT_U		0x04
#define	DRUG_UNIT_KU		0x05
#define	DRUG_UNIT_MU		0x06
#define	DRUG_UNIT_MLHR	0x07
#define	DRUG_UNIT_GTTMIN	0x08
#define	DRUG_UNIT_GTTML	0x0a
#define	DRUG_UNIT_HR		0x0b
#define	DRUG_UNIT_ML		0x0c
#define	DRUG_UNIT_KG		0x0d
#define	DRUG_UNIT_MSK		0xc0
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#define DRUG_LINK_ML		0x40	//��λ����	bit4
#define	DRUG_LINK_MSK		0x3f

//---------ҩ�����Ŵ���---------------
#define DRUG_SCALE_VAL		100

//------------ҩ������-------------------
#define V_DRUG_NAMEA		0
#define V_DRUG_NAMEB		1
#define V_DRUG_NAMEC		2
#define V_DRUG_NAMED		3
#define V_DRUG_NAMEE		4
#define V_DRUG_AMINO		5	//AMINOPHYLLINE
#define	V_DRUG_DOBUT		6	//DOBUTAMINE
#define	V_DRUG_DOPAM		7	//DOPAMINE
#define	V_DRUG_EPINE		8	//EPINEPHRINE
#define	V_DRUG_HEPAR		9
#define	V_DRUG_ISUPR		10
#define	V_DRUG_LIDOC		11
#define	V_DRUG_NIPRI		12
#define	V_DRUG_NITRO		13
#define	V_DRUG_PITOC		14

#define	DRUG_DATA_AMOUNT		0
#define	DRUG_DATA_VOLUME		1
#define	DRUG_DATA_CONCEN		2
#define	DRUG_DATA_DOSEMIN		3
#define	DRUG_DATA_DOSEHR		4
#define	DRUG_DATA_DOSEKGMIN	5
#define	DRUG_DATA_DOSEKGHR	6
#define	DRUG_DATA_INFRATE		7
#define	DRUG_DATA_DRIPRATE	8
#define	DRUG_DATA_DRIPSIZE		9
#define	DRUG_DATA_DURATION	10

#define	DRUG_LIMIT_WEIGHT		0
#define	DRUG_LIMIT_AMOUNT		1
#define	DRUG_LIMIT_VOLUME		2
#define	DRUG_LIMIT_CONCEN		3
#define	DRUG_LIMIT_DOSEMIN	4
#define	DRUG_LIMIT_DOSEHR		5
#define	DRUG_LIMIT_DOSEKGMIN	6
#define	DRUG_LIMIT_DOSEKGHR	7
#define	DRUG_LIMIT_INFRATE		8
#define	DRUG_LIMIT_DRIPRATE	9
#define	DRUG_LIMIT_DRIPSIZE	10
#define	DRUG_LIMIT_DURATION	11

	

//ҩ������ ��λ
float	 gDrugData[11];	//AMOUNT			VOLUME					CONCEN				DOSE/MIN			DOSE/HR 	 DOSE/KG/MIN	 DOSE/KG/HR 		 INFRATE				DROPRATE			DROPSIZE		DURATION
char	 gDrugUnit[11]={DRUG_UNIT_MG, DRUG_UNIT_ML, DRUG_UNIT_MCG|DRUG_LINK_ML, DRUG_UNIT_MCG, DRUG_UNIT_MG, DRUG_UNIT_MCG, DRUG_UNIT_MCG, DRUG_UNIT_MLHR, DRUG_UNIT_GTTMIN, DRUG_UNIT_GTTML, DRUG_UNIT_HR};
		
float gStepValue[12]={0};
float gLimitHigh[12]={25000.0,999999.9,999999.9,999999.9,999999.9,999999.9,999999.9,999999.9,99999.9,99999.9,99999.9,99999.9};
float gLimitLow[12]={100.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0};

float  gWeight; //��������


unsigned char *gDrugName;

//��ʱ�洢�����ޣ����ڵ�ʱ��ʹ��
static float iTmpWeight =0, iTmpAmount =0,iTmpVolume =0,iTmpConcent =0,iTmpDosemin =0,iTmpDosehr =0;
static float iTmpDosekgmin=0,iTmpDosekghr=0,iTmpInfRate=0,iTmpDripRate=0,iTmpDripSize=0, iTmpDuration=0;

//����ƶ���ť�Ƿ���
static BOOL bCobDrugName = FALSE;
static BOOL bEdit = FALSE;
//�����Ի���
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//�ؼ�����
static HWND btnExit,btnTitration;
static HWND stcDrugName,stcWeight,stcAmount,stcVolume,stcConcentration,stcDosemin,stcDosehr;
static HWND stcDosekgmin,stcDosekghr,stcInfRate,stcDripRate,stcDripSize,stcDuration;
static HWND cobDrugName,editWeight,editAmount,editVolume,editConcentration,editDosemin,editDosehr;
static HWND editDosekgmin,editDosekghr,editInfRate,editDripRate,editDripSize,editDuration;
static HWND stcWeightU,stcAmountU,stcVolumeU,stcConcentrationU,stcDoseminU,stcDosehrU;
static HWND stcDosekgminU,stcDosekghrU,stcInfRateU,stcDripRateU,stcDripSizeU,stcDurationU;

//�ؼ��ص�����
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

//�Ի�������
static DLGTEMPLATE DlgSet= {
	//WS_VISIBLE | WS_CAPTION | WS_BORDER,
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	0, DLG_Y+80, DLG_LENGHT+150, DLG_HIGH-80,	
	"",
	0, 0,
	1,		
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"static",
		WS_CHILD | WS_VISIBLE | SS_LEFT ,
		//WS_CHILD | WS_VISIBLE | SS_LEFT|SS_WHITERECT ,
		5,DLG_HIGH-80-50, DLG_LENGHT+150-15, 40,
		DID_DRUG_HELP,
		"",
		0
	}
};
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/min = ҩ��Ũ�� �� ���� �� ÿ�����
//
//////////////////////////////////////////////////
void Drug_DripRate_To_DoseMin(  )
{
	gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_CONCEN]*gDrugData[DRUG_DATA_DRIPRATE]/gDrugData[DRUG_DATA_DRIPSIZE];
	gDrugUnit[DRUG_DATA_DOSEMIN] = gDrugUnit[DRUG_DATA_CONCEN];
	
	if( gDrugData[DRUG_DATA_DOSEMIN]>9999.9949 )
	{
		do{
			int blv;
			blv = TRUE;
			switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
			{
			case	DRUG_UNIT_MCG:		//mcg(΢��)
				gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MG;	//mg(����)
				break;
			case	DRUG_UNIT_MG:		//mg(����)
				gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_G;	//g(��)
				break;
			case	DRUG_UNIT_U:		//Unit(��λ)
				gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
				break;
			case	DRUG_UNIT_KU:		//KU(ǧ��λ)
				gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MU;	//MI(����λ)
				break;
			default:blv = FALSE;
				break;
			}

			if( blv )
				gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEMIN]/1000.0;
			else
				break;
		}
		while( gDrugData[DRUG_DATA_DOSEMIN]>9999.9949 );
	}
	else if( gDrugData[DRUG_DATA_DOSEMIN]<10.0 ){
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:		//g(��)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:		//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEMIN]*1000.0;
	}
}
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/hr = ����/min �� 60
//
//////////////////////////////////////////////////
void Drug_DoseMin_To_DoseHr(  )
{
	gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEMIN]*60;
	gDrugUnit[DRUG_DATA_DOSEHR] = gDrugUnit[DRUG_DATA_DOSEMIN];
	while( gDrugData[DRUG_DATA_DOSEHR]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEMIN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEHR]/1000.0;
		else
			break;
	}
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/hr = ҩ������ �� ����ʱ�� 
//	ע��:ͳһ���㵥λ
//
//////////////////////////////////////////////////
void Drug_Dura_TO_DoseHr(  )
{
	gDrugData[DRUG_DATA_DOSEHR]=gDrugData[DRUG_DATA_AMOUNT]/gDrugData[DRUG_DATA_DURATION];
	gDrugUnit[DRUG_DATA_DOSEHR]=gDrugUnit[DRUG_DATA_AMOUNT];

	if( gDrugData[DRUG_DATA_DOSEHR]<10.0 ){
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_AMOUNT]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:		//g(��)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:		//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEHR]*1000.0;
	}
}
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/min = ����/hr �� 60
//
//////////////////////////////////////////////////
void Drug_DoseHr_To_DoseMin( )
{
	gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEHR]/60;
	gDrugUnit[DRUG_DATA_DOSEMIN] = gDrugUnit[DRUG_DATA_DOSEHR];
	if( gDrugData[DRUG_DATA_DOSEMIN] < 10.0 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:		//g(��)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:		//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEMIN]*1000.0;
	}
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/hr = ��Һ�ٶ� �� ҩ��Ũ��
//
//////////////////////////////////////////////////
void Drug_INFrate_To_DoseHr( )
{
	gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_INFRATE]*gDrugData[DRUG_DATA_CONCEN];
	gDrugUnit[DRUG_DATA_DOSEHR] = gDrugUnit[DRUG_DATA_CONCEN];

	while( gDrugData[DRUG_DATA_DOSEHR]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MU;	//MU(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEHR]/1000.0;
		else
			break;
	}
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//
//	ҩ��Ũ�� = ҩ������ �� Һ���ݻ�
//
//////////////////////////////////////////////////
void	Drug_Concen_Calc( )
{
	//---------------calculate Concentra-----------------
	gDrugData[DRUG_DATA_CONCEN] = gDrugData[DRUG_DATA_AMOUNT]/gDrugData[DRUG_DATA_VOLUME];
	gDrugUnit[DRUG_DATA_CONCEN] = gDrugUnit[DRUG_DATA_AMOUNT];
	if( gDrugData[DRUG_DATA_CONCEN] < 10.0 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_AMOUNT]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:			//mg(����)
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:			//g(��)
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:			//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:			//MU(����λ)
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_CONCEN] = gDrugData[DRUG_DATA_AMOUNT]*1000/gDrugData[DRUG_DATA_VOLUME];
	}
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/hr  = ҩ��Ũ�� �� ��Һ�ٶ�
//	����/min = ����/hr  �� 60
//
//////////////////////////////////////////////////
void Drug_DOSE_MIN_HR_Calc( )
{
	////////////////////////////////
	//	ע��: ��Һ�ٶ�(ml/hr) ����/СʱΪ��λ
	//	�ڼ��� ����/min  ��ʱ��Ҫ�� 60
	gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_CONCEN]*gDrugData[DRUG_DATA_INFRATE];
	gDrugUnit[DRUG_DATA_DOSEHR] = gDrugUnit[DRUG_DATA_CONCEN];
	
	gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEHR]/60;
	gDrugUnit[DRUG_DATA_DOSEMIN] = gDrugUnit[DRUG_DATA_DOSEHR];
	
	while( gDrugData[DRUG_DATA_DOSEHR]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:			//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:			//mg(����)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:			//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:			//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MU;	//MU(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEHR]/1000.0;
		else
			break;
	}

	while( gDrugData[DRUG_DATA_DOSEMIN]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:			//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:			//mg(����)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:			//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:			//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEMIN]/1000.0;
		else
			break;
	}
}

//-------------����  ҩ������-----------
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	ҩ������ = Һ���ݻ� �� ҩ��Ũ��
//
//////////////////////////////////////////////////
void Drug_Amount_Calc( )
{
	gDrugData[DRUG_DATA_AMOUNT] = gDrugData[DRUG_DATA_VOLUME]*gDrugData[DRUG_DATA_CONCEN];
	gDrugUnit[DRUG_DATA_AMOUNT] = gDrugUnit[DRUG_DATA_CONCEN];
	while( gDrugData[DRUG_DATA_AMOUNT]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_AMOUNT] = gDrugData[DRUG_DATA_AMOUNT]/1000.0;
		else
			break;
	}
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	
//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
//
//////////////////////////////////////////////////
void Drug_INFrate_Calc(  )
{
	
	float	concen;
	float	dosehr;

	concen = gDrugData[DRUG_DATA_CONCEN];
	dosehr = gDrugData[DRUG_DATA_DOSEHR];
	if( gDrugUnit[DRUG_DATA_CONCEN] != gDrugUnit[DRUG_DATA_DOSEHR] )
	{
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			concen *= 1000.0;
			break;
		case	DRUG_UNIT_G:		//g(��)
		case	DRUG_UNIT_MU:		//MU(����λ)
			concen *= 1000000.0;
			break;
		default:break;
		}

		switch( gDrugUnit[DRUG_DATA_DOSEHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			dosehr *= 1000.0;
			break;
		case	DRUG_UNIT_G:		//g(��)
		case	DRUG_UNIT_MU:		//MU(����λ)
			dosehr *= 1000000.0;
			break;
		default:break;
		}
	}
	gDrugData[DRUG_DATA_INFRATE] = dosehr/concen;
}

//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/kg/hr = ����/kg/min �� 60
//
//////////////////////////////////////////////////
void Drug_DoseKgHr_Calc(  )
{
	gDrugData[DRUG_DATA_DOSEKGHR] = gDrugData[DRUG_DATA_DOSEKGMIN]*60;
	gDrugUnit[DRUG_DATA_DOSEKGHR] = gDrugUnit[DRUG_DATA_DOSEKGMIN];
	while( gDrugData[DRUG_DATA_DOSEKGHR]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGMIN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGHR] = gDrugData[DRUG_DATA_DOSEKGHR]/1000.0;
		else
			break;
	}
}
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/hr  = ����/kg/hr  �� ��������
//	����/min = ����/kg/min �� ��������
//
//////////////////////////////////////////////////
void	Drug_DOSEKG_TO_DOSE_Calc(  )
{
	char buff[30];
	GetWindowText(editWeight,buff,sizeof(buff));
	gWeight =atof(buff);
	gWeight /= DRUG_SCALE_VAL;

	gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEKGHR]*gWeight;
	gDrugUnit[DRUG_DATA_DOSEHR] = gDrugUnit[DRUG_DATA_DOSEKGHR];
	
	gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEKGMIN]*gWeight;
	gDrugUnit[DRUG_DATA_DOSEMIN] = gDrugUnit[DRUG_DATA_DOSEKGMIN];

	while( gDrugData[DRUG_DATA_DOSEHR]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEHR] = gDrugData[DRUG_DATA_DOSEHR]/1000.0;
		else
			break;
	}

	while( gDrugData[DRUG_DATA_DOSEMIN]>9999.9949 )
	{
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGMIN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEMIN] = gDrugData[DRUG_DATA_DOSEMIN]/1000.0;
		else
			break;
	}
}
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/kg/min = ����/kg/hr �� 60
//
//////////////////////////////////////////////////
void Drug_DoseKgMin_Calc(  )
{
	gDrugData[DRUG_DATA_DOSEKGMIN] = gDrugData[DRUG_DATA_DOSEKGHR]/60;
	gDrugUnit[DRUG_DATA_DOSEKGMIN] = gDrugUnit[DRUG_DATA_DOSEKGHR];
	if( gDrugData[DRUG_DATA_DOSEKGMIN] < 10.0 ){
		int blv;
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MCG; //mcg(΢��)
			break;
		case	DRUG_UNIT_G:		//g(��)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:		//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGMIN] = gDrugData[DRUG_DATA_DOSEKGMIN]*1000.0;
	}
}
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	
//	���� = ����/min �� ÿ����� �� ҩ��Ũ��
//
//////////////////////////////////////////////////
void Drug_DripRate_Calc(  )
{
	float	concen;
	float	dosemin;
	concen = gDrugData[DRUG_DATA_CONCEN];
	dosemin = gDrugData[DRUG_DATA_DOSEMIN];
	if( gDrugUnit[DRUG_DATA_CONCEN] != gDrugUnit[DRUG_DATA_DOSEMIN] )
	{
		switch( gDrugUnit[DRUG_DATA_CONCEN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:	//mg(����)
		case	DRUG_UNIT_KU:	//KU(ǧ��λ)
			concen *= 1000.0;
			break;
		case	DRUG_UNIT_G:	//g(��)
		case	DRUG_UNIT_MU:	//MU(����λ)
			concen *= 1000000.0;
			break;
		default:break;
		}

		switch( gDrugUnit[DRUG_DATA_DOSEMIN]&(~DRUG_UNIT_MSK))
		{
		case	DRUG_UNIT_MG:	//mg(����)
		case	DRUG_UNIT_KU:	//KU(ǧ��λ)
			dosemin *= 1000.0;
			break;
		case	DRUG_UNIT_G:	//g(��)
		case	DRUG_UNIT_MU:	//MU(����λ)
			dosemin *= 1000000.0;
			break;
		default:break;
		}
	}
	gDrugData[DRUG_DATA_DRIPRATE] = dosemin*gDrugData[DRUG_DATA_DRIPSIZE]/concen;
}

//------------------����  ����ʱ��----------------
//////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����ʱ�� = ҩ������ �� ����/hr
//	ע��:ͳһ���㵥λ
//
//////////////////////////////////////////////////
void Drug_Duration_Calc(  )
{
	float	amount;
	float	dosehr;

	amount = gDrugData[DRUG_DATA_AMOUNT];
	dosehr = gDrugData[DRUG_DATA_DOSEHR];
	if( gDrugUnit[DRUG_DATA_AMOUNT] != gDrugUnit[DRUG_DATA_DOSEHR] ){
		switch( gDrugUnit[DRUG_DATA_AMOUNT]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:	//mg(����)
		case	DRUG_UNIT_KU:	//KU(ǧ��λ)
			amount *= 1000.0;
			break;
		case	DRUG_UNIT_G:	//g(��)
		case	DRUG_UNIT_MU:	//MU(����λ)
			amount *= 1000000.0;
			break;
		default:break;
		}

		switch( gDrugUnit[DRUG_DATA_DOSEHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:	//mg(����)
		case	DRUG_UNIT_KU:	//KU(ǧ��λ)
			dosehr *= 1000.0;
			break;
		case	DRUG_UNIT_G:	//g(��)
		case	DRUG_UNIT_MU:	//MU(����λ)
			dosehr *= 1000000.0;
			break;
		default:break;
		}
	}
	gDrugData[DRUG_DATA_DURATION] = amount/dosehr;
	
}




//**************************************************************************************************************************
///////////////////////////////////////// ҩ����㹫ʽ/////////////////////////////////
//	ҩ��Ũ�� = ҩ������ / Һ���ݻ�
//	��Һ�ٶ� = ҩ����� / ҩ��Ũ��
//	����ʱ�� = ҩ������ / ҩ�����
//	���� ����/kg/min(hr) ʱ��Ҫ�ȳ�2��ϵ���ٳ�����ֵ ��Ϊ(1~500��ʾ0.5~250����)
///////////////////////////////////////////////////////////////////////////////////////

//-------------����  ����/kg/min & ����/kg/hr-----------
////////////////////////////////////////////////////////
//
//	�������ļ��㹫ʽ
//	����/kg/min = ����/min �� ��������
//	����/kg/hr  = ����/hr �� ��������
//
////////////////////////////////////////////////////////
void	Drug_DOSE_KG_MIN_HR_Calc( )
{
	int 	blv;
	int i;
	unsigned char buff[50]={0};
	GetWindowText(editWeight,buff,sizeof(buff));
	gWeight=atof(buff)*DRUG_SCALE_VAL;
	
	//-------------------------------------����(xxx/kg/min)-------------------------------
	gDrugData[DRUG_DATA_DOSEKGMIN] = gDrugData[DRUG_DATA_DOSEMIN]*DRUG_SCALE_VAL/gWeight;
	gDrugUnit[DRUG_DATA_DOSEKGMIN] = gDrugUnit[DRUG_DATA_DOSEMIN];
	if( gDrugData[DRUG_DATA_DOSEKGMIN] < 10.0 )
	{
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGMIN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:	//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:	//g(��)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:	//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:	//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGMIN]  *= 1000.0;
		
	}
	
	else if( gDrugData[DRUG_DATA_DOSEKGMIN] > 9999.9949 )
	{
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGMIN]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//Unit(��λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_KU;	//KU(ǧ��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEKGMIN] = DRUG_UNIT_MU;	//MI(����λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGMIN] /= 1000.0;
	}
	
	//-------------------------------------����(xxx/kg/hr)-------------------------------
	gDrugData[DRUG_DATA_DOSEKGHR] = gDrugData[DRUG_DATA_DOSEHR]*DRUG_SCALE_VAL/gWeight;//����(xxx/kg/hr)
	gDrugUnit[DRUG_DATA_DOSEKGHR] = gDrugUnit[DRUG_DATA_DOSEHR];
	if( gDrugData[DRUG_DATA_DOSEKGHR] < 10.0 )
	{
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MCG;	//mcg(΢��)
			break;
		case	DRUG_UNIT_G:		//g(��)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_KU:		//KU(ǧ��λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_U;	//Unit(��λ)
			break;
		case	DRUG_UNIT_MU:		//MU(����λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_KU; 	//KU(ǧ��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGHR] *= 1000.0;
	}

	else if( gDrugData[DRUG_DATA_DOSEKGHR] > 9999.9949 ){
		blv = TRUE;
		switch( gDrugUnit[DRUG_DATA_DOSEKGHR]&(~DRUG_UNIT_MSK) )
		{
		case	DRUG_UNIT_MCG:		//mcg(΢��)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MG;	//mg(����)
			break;
		case	DRUG_UNIT_MG:		//mg(����)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_G;	//g(��)
			break;
		case	DRUG_UNIT_U:		//U(��λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_KU;	//KU(��λ)
			break;
		case	DRUG_UNIT_KU:		//KU(��λ)
			gDrugUnit[DRUG_DATA_DOSEKGHR] = DRUG_UNIT_MU; 	//MU(��λ)
			break;
		default:blv = FALSE;
			break;
		}
		if( blv )
			gDrugData[DRUG_DATA_DOSEKGHR] /= 1000.0;
	}

	
}

//���ñ༭������
static int SetEditText(HWND hWnd, float iLimit)
{	
	unsigned char strLimit[32];
	float fv;
	fv = iLimit;
	fv /= DRUG_SCALE_VAL;
	//if(B_PRINTF) printf("iLimit=%f,fv=%f\n",iLimit,fv);
	snprintf(strLimit, sizeof(strLimit),"%.2f", fv);
	//if(B_PRINTF) printf("striLimit=%s\n",strLimit);
	SetWindowText(hWnd, strLimit);
	
	return 0;
}
//ʵ��ֵת�����˵�ֵ
 float Drug_Real_To_Valume( float real )
{
	char		buff[10];
	
	sprintf( buff, "%5.2f", real );
	real  = atof( buff );
	
	return	(real*100.0);
}
//�˵�ֵת����ʵ��ֵ
float Drug_Valume_To_Real(  signed   long menuval )
{
	return	(float)( menuval/100.0);
}
 
//���ò���
void Drug_Step_Set( HWND hWnd , int valsync ,int step,int sub)
{
	int val;
	float fValue;
	char buff[50];
	GetWindowText(hWnd,buff,sizeof(buff));
	if(sub!=0){
		fValue =atof(buff)-1.0;//�ж��Ƿ�Ϊ�����ٽ�ֵ����
	}
	else{
		fValue =atof(buff);
	}
	if( fValue<10 ){		//val<10.0	step = 0.1
		if(sub!=0)fValue+=1.0;
		val = fValue/0.1+0.5;//��������
		gStepValue[step]= 0.1*DRUG_SCALE_VAL;
	}else if( fValue<100){	//val<100.0	step = 1.0
		if(sub!=0)fValue+=1.0;
		val =fValue/1;
		gStepValue[step]= 1*DRUG_SCALE_VAL;
	}else if( fValue<1000){	//val<1000.0	step = 10.0
		if(sub!=0)fValue+=1.0;
		val =fValue/10;
		gStepValue[step]= 10*DRUG_SCALE_VAL;
	}else{						//val<10000.0	step = 100.0
		if(sub!=0)fValue+=1.0;
		val = fValue/100;
		gStepValue[step]=100*DRUG_SCALE_VAL;
	}
	if(hWnd==editWeight)
		gStepValue[step]=0.5*DRUG_SCALE_VAL;
	
	if( valsync ){
		SetEditText(hWnd,val*gStepValue[step]);
	}
}

//------------ҩ�ﵥλ����ת�����ַ���-------------
void	Drug_Unit_To_String( char *pStr, int unittype )
{
	switch( unittype&(~DRUG_UNIT_MSK) )
	{
		case	DRUG_UNIT_MCG:		strcpy( pStr, "mcg" );		break;
		case	DRUG_UNIT_MG:		strcpy( pStr, "mg" );		break;
		case	DRUG_UNIT_G:			strcpy( pStr, "g" );			break;
		case	DRUG_UNIT_MEQ:		strcpy( pStr, "mEq" );		break;
		case	DRUG_UNIT_U:			strcpy( pStr, "U" );			break;
		case	DRUG_UNIT_KU:		strcpy( pStr, "KU" );			break;
		case	DRUG_UNIT_MU:		strcpy( pStr, "MU" );		break;
		case	DRUG_UNIT_KG:		strcpy( pStr, "kg" );			break;
		case	DRUG_UNIT_ML:		strcpy( pStr, "ml" );			break;
		case	DRUG_UNIT_MLHR:		strcpy( pStr, "ml/hr" );		break;
		case	DRUG_UNIT_GTTMIN:	strcpy( pStr, "GTT/min" );	break;
		case	DRUG_UNIT_GTTML:		strcpy( pStr, "GTT/ml" );		break;
		case	DRUG_UNIT_HR:		strcpy( pStr, "hr" );			break;
		default:					strcpy( pStr, "?" );			break;
	}

	switch( unittype&(~DRUG_LINK_MSK) )
	{
	case	DRUG_LINK_ML:	strcat( pStr, "/ml" );	break;
	default:					break;
	}
}	


 //��ʾҩ�����ĵ�λ
void Show_Drug_Unit( HWND hWnd, int unittype )
{
	char	buff[10];
	Drug_Unit_To_String( buff, unittype );
	SetWindowText(hWnd,buff);
}

 //�����ֵ
void DrugSetValue()
{
	SetEditText(editAmount,Drug_Real_To_Valume(gDrugData[DRUG_DATA_AMOUNT]));
	SetEditText(editVolume,Drug_Real_To_Valume(gDrugData[DRUG_DATA_VOLUME]));
	SetEditText(editConcentration,Drug_Real_To_Valume(gDrugData[DRUG_DATA_CONCEN]));
	SetEditText(editDosemin,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEMIN]));
	SetEditText(editDosehr,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEHR]));
	SetEditText(editDosekgmin,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEKGMIN]));
	SetEditText(editDosekghr,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEKGHR]));
	SetEditText(editInfRate,Drug_Real_To_Valume(gDrugData[DRUG_DATA_INFRATE]));
	SetEditText(editDripRate,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DRIPRATE]));
	SetEditText(editDripSize,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DRIPSIZE]));
	SetEditText(editDuration,Drug_Real_To_Valume(gDrugData[DRUG_DATA_DURATION]));
	
}
 /*��ֵ��ʼ��*/
 void  Drug_Calc_Init ( )
 {
  	
	int	drugname;
	drugname =  SendMessage(cobDrugName, CB_GETCURSEL, 0, 0);
		
	switch( drugname )
	{
	case	V_DRUG_HEPAR:
		gDrugData[DRUG_DATA_AMOUNT] =  12.50;	//KU
		gDrugUnit[DRUG_DATA_AMOUNT] =  DRUG_UNIT_KU;
		
		gDrugData[DRUG_DATA_VOLUME] =  250.00;	//ml
		
		gDrugData[DRUG_DATA_CONCEN] =  50.00;	//U/ml
		gDrugUnit[DRUG_DATA_CONCEN] =  DRUG_UNIT_U|DRUG_LINK_ML;
		
		gDrugData[DRUG_DATA_DOSEMIN] = 50.00;	//U
		gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_U;
		
		gDrugData[DRUG_DATA_DOSEMIN] = 3.00;	//KU
		gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_KU;
		
		gDrugData[DRUG_DATA_INFRATE] = 60.00;	//ml/hr		
		gDrugData[DRUG_DATA_DRIPRATE]= 20.0;	//GTT/min
		gDrugData[DRUG_DATA_DRIPSIZE]= 20.0;	//GTT/ml
		gDrugData[DRUG_DATA_DURATION]= 4.17;	//hr
		break;
	case	V_DRUG_NAMEE:
	case	V_DRUG_NAMED:
	case	V_DRUG_PITOC:
		gDrugData[DRUG_DATA_AMOUNT] = 400.0;	//400.00	ҩ������(Unit)
		gDrugData[DRUG_DATA_VOLUME] = 250.0;	//250.00	Һ���ݻ�(ml)
		gDrugData[DRUG_DATA_CONCEN] = 1.6;	//1.60		ҩ��Ũ��(Unit/ml)
		gDrugData[DRUG_DATA_DOSEMIN]= 2.5;	//2.5		����(Unit/min)
		gDrugData[DRUG_DATA_DOSEHR] = 150.0;	//150.00	����(Unit/hr)
		if( drugname == V_DRUG_NAMEE ){
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_MEQ;	//mEq
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_MEQ|DRUG_LINK_ML;//mEq/ml
			gDrugUnit[DRUG_DATA_DOSEMIN]= DRUG_UNIT_MEQ;	//mEq
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MEQ;	//mEq
		}else{
			gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_U;	//Unit
			gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_U|DRUG_LINK_ML;//Unit/ml
			gDrugUnit[DRUG_DATA_DOSEMIN]= DRUG_UNIT_U;	//Unit
			gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_U;	//Unit
		}
		gDrugData[DRUG_DATA_INFRATE]  = 93.75;	//93.75		��Һ�ٶ�(ml/hr)
		gDrugData[DRUG_DATA_DRIPRATE] = 31.25;	//31.25		����(GTT/min)
		gDrugData[DRUG_DATA_DRIPSIZE] = 20.0;	//20.00		ÿ�����(GTT/ml)
		gDrugData[DRUG_DATA_DURATION] = 2.67;	//2.67		����ʱ��(hr)
		break;
	default:
		gDrugData[DRUG_DATA_AMOUNT] = 400.0;	//400.00	ҩ������(mg)
		gDrugUnit[DRUG_DATA_AMOUNT] = DRUG_UNIT_MG;		//mg

		gDrugData[DRUG_DATA_VOLUME] = 250.0;	//250.00	Һ���ݻ�(ml)
		
		gDrugData[DRUG_DATA_CONCEN] = 1.60;	//1.60		ҩ��Ũ��(mg/ml)
		gDrugUnit[DRUG_DATA_CONCEN] = DRUG_UNIT_MG|DRUG_LINK_ML;	//mg/ml
		
		gDrugData[DRUG_DATA_DOSEMIN] = 2640.0;	//2640.00 	����(mcg/min)
		gDrugUnit[DRUG_DATA_DOSEMIN] = DRUG_UNIT_MCG;		//mcg

		gDrugData[DRUG_DATA_DOSEHR] = 158.4;	//158.40	����(mg/hr)
		gDrugUnit[DRUG_DATA_DOSEHR] = DRUG_UNIT_MG;			//mg
		
		gDrugData[DRUG_DATA_INFRATE]  = 99.0;	//99.00		��Һ�ٶ�(ml/hr)
		gDrugData[DRUG_DATA_DRIPRATE] = 33.0;	//33.00		����(GTT/min)
		gDrugData[DRUG_DATA_DRIPSIZE] = 20.0;	//20.00		ÿ�����(GTT/ml)
		gDrugData[DRUG_DATA_DURATION] = 2.53;	//2.53		����ʱ��(hr)
		break;
	}
	
	SetEditText(editWeight,(70.0*DRUG_SCALE_VAL));
	Drug_DOSE_KG_MIN_HR_Calc(); //DOSE/KG/MIN	 DOSE/KG/HR

	DrugSetValue();

 }
//ҩ����㵥λ��ʼ��
void Drug_Calc_Unit_Init( )
{
	
	Show_Drug_Unit( stcWeightU,		DRUG_UNIT_KG );
	Show_Drug_Unit( stcAmountU,	gDrugUnit[DRUG_DATA_AMOUNT] );

	Show_Drug_Unit( stcVolumeU,	DRUG_UNIT_ML );

	Show_Drug_Unit( stcConcentrationU,	gDrugUnit[DRUG_DATA_CONCEN] );
	Show_Drug_Unit( stcDoseminU,	gDrugUnit[DRUG_DATA_DOSEMIN]);
	Show_Drug_Unit( stcDosehrU,	gDrugUnit[DRUG_DATA_DOSEHR] );
	Show_Drug_Unit( stcDosekgminU,	gDrugUnit[DRUG_DATA_DOSEKGMIN] );
	Show_Drug_Unit( stcDosekghrU,	gDrugUnit[DRUG_DATA_DOSEKGHR] );

	Show_Drug_Unit( stcInfRateU,	DRUG_UNIT_MLHR );
	Show_Drug_Unit( stcDripRateU,	DRUG_UNIT_GTTMIN );
	Show_Drug_Unit( stcDripSizeU,	DRUG_UNIT_GTTML );

	Show_Drug_Unit( stcDurationU,	DRUG_UNIT_HR );
}


static int DrawTABFocusColor(HWND hWnd)
{
	RECT rc;
	HDC hdc= (HDC)NULL;
	GetWindowRect(hWnd, &rc);	
	hdc = GetClientDC(GetParent(hWnd));
	if(hdc != (HDC)NULL){
		SetPenColor(hdc, SETFOCUS_COLOR);
		Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
		Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
		ReleaseDC(hdc);
	}
	
	return 0;
}
static int DrawUDFocusColor(HWND hWnd)
{	
	RECT rc;
	HDC hdc= (HDC)NULL;
	GetWindowRect(hWnd, &rc);	
	hdc = GetClientDC(GetParent(hWnd));
	if(hdc != (HDC)NULL){
		SetPenColor(hdc, COLOR_green);
		Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
		Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
		ReleaseDC(hdc);
	}
	
	return 0;
}
static float GetEditValue(HWND hWnd)
{
	unsigned char buff[50]={0};
	float value;
	GetWindowText(hWnd,buff,sizeof(buff));
	//if(B_PRINTF) printf("buff=%s,atof=%f\n",buff,atof(buff)*100.0);
	value=atof(buff)*DRUG_SCALE_VAL;	
	//if(B_PRINTF) printf("value=%f\n",value);
	return value;
}

void Ctl_Func_DrugCalc_Weight()
{
	//����DOSE/KG/MIN	DOSE/KG/HR
	Drug_DOSE_KG_MIN_HR_Calc();	//DOSE/KG/MIN	DOSE/KG/HR
	SetEditText(editDosekgmin, Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEKGMIN]));
	SetEditText(editDosekghr, Drug_Real_To_Valume(gDrugData[DRUG_DATA_DOSEKGHR]));
							
}

void Ctl_Func_DrugCalc_Amount()
{
	//GetWindowText(editAmount,buff,sizeof(buff));
	gDrugData[DRUG_DATA_AMOUNT] = GetEditValue(editAmount)/DRUG_SCALE_VAL;
	
	////////////////////////////////////////////////	
	//	ҩ�������ı�ʱ	
	//	step1. ���� (ҩ��Ũ��)
	//	step2. ���� (����/hr & ����/min)
	//	step3. ���� (����/kg/hr & ����/kg/min)
	//	
	//	����/hr = ��Һ�ٶ� �� ҩ��Ũ��
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//
	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////

	///////////////////////////////////////////////
	//	����:	����ҩ��Ũ��
	//	�㷨:	ҩ��Ũ�� = ҩ������/Һ���ݻ�

	Drug_Concen_Calc();
	Drug_DOSE_MIN_HR_Calc();
	Drug_DOSE_KG_MIN_HR_Calc();

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

void	Ctl_Func_DrugCalc_Volume()
{
	//GetWindowText(editVolume,buff,sizeof(buff));
	gDrugData[DRUG_DATA_VOLUME] = GetEditValue(editVolume)/DRUG_SCALE_VAL;

	////////////////////////////////////////////////	
	//	Һ���ݻ��ı�ʱ	
	//	step1. ���� (ҩ��Ũ��)
	//	step2. ���� (����/hr & ����/min)
	//	step3. ���� (����/kg/hr & ����/kg/min)
	//	step4. ���� (����ʱ��)
	//
	//	ҩ��Ũ�� = ҩ������/Һ���ݻ�
	//	����/hr = ��Һ�ٶ� �� ҩ��Ũ��
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	����ʱ�� = ҩ������ �� ����/hr
	//
	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////

	Drug_Concen_Calc(); 
	Drug_DOSE_MIN_HR_Calc();
	Drug_DOSE_KG_MIN_HR_Calc();
	Drug_Duration_Calc();

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

void	Ctl_Func_DrugCalc_Concen( )
{
	//memset(buff, 0, sizeof(buff));
	//GetWindowText(editConcentration,buff,sizeof(buff));
	gDrugData[DRUG_DATA_CONCEN] = GetEditValue(editConcentration)/DRUG_SCALE_VAL;

	////////////////////////////////////////////////	
	//	ҩ��Ũ�ȸı�ʱ	
	//	step1. ���� (ҩ������)
	//	step2. ���� (����/hr & ����/min)
	//	step3. ���� (����/kg/hr & ����/kg/min)
	//
	//	ҩ������ = ҩ��Ũ��* Һ���ݻ�
	//	����/hr = ��Һ�ٶ� �� ҩ��Ũ��
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	����ʱ�� = ҩ������ �� ����/hr
	//
	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_Amount_Calc();
	Drug_DOSE_MIN_HR_Calc();
	Drug_DOSE_KG_MIN_HR_Calc();

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}


void	Ctl_Func_DrugCalc_DoseMin( )
{
	//GetWindowText(editDosemin,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DOSEMIN]  = GetEditValue(editDosemin)/DRUG_SCALE_VAL;

	////////////////////////////////////////////////
	//	����/min �ı�
	//	step1. ���� (����/hr)
	//	step2. ���� (����/kg/hr)
	//	step3. ���� (����/kg/min)
	//	step4. ���� ��Һ�ٶ�
	//	step5. ���� ����
	//	step6. ���� ÿ�����
	//	step7. ���� ����ʱ��
	//
	//	����/hr = ����/min �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DoseMin_To_DoseHr();	//���� ����/hr
	Drug_DOSE_KG_MIN_HR_Calc();	//���� ����/kg/hr & ����/kg/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

void	Ctl_Func_DrugCalc_DoseHr()
{
	//GetWindowText(editDosehr,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DOSEHR]  = GetEditValue(editDosehr)/DRUG_SCALE_VAL;

	////////////////////////////////////////////////
	//	����/min �ı�
	//	step1. ���� (����/hr)
	//	step2. ���� (����/kg/hr)
	//	step3. ���� (����/kg/min)
	//	step4. ���� ��Һ�ٶ�
	//	step5. ���� ����
	//	step6. ���� ÿ�����
	//	step7. ���� ����ʱ��
	//
	//	����/hr = ����/min �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DoseHr_To_DoseMin();	//���� ����/min
	Drug_DOSE_KG_MIN_HR_Calc(); //���� ����/kg/hr & ����/kg/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

void	Ctl_Func_DrugCalc_DoseKgMin(  )
{
	
	//GetWindowText(editDosekgmin,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DOSEKGMIN]  = GetEditValue(editDosekgmin)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	����/kg/min �ı�ʱ
	//	step1. ���� (����/min)
	//	step2. ���� (����/kg/hr)
	//	step3. ���� (����/kg/min)
	//	step4. ���� ��Һ�ٶ�
	//	step5. ���� ����
	//	step6. ���� ÿ�����
	//	step7. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��
	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DoseKgHr_Calc();		//���� ����kg/hr
	Drug_DOSEKG_TO_DOSE_Calc();	//���� ����/hr & ����/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}
void	Ctl_Func_DrugCalc_DoseKgHr()
{
	//GetWindowText(editDosekghr,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DOSEKGHR] = GetEditValue(editDosekghr)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	����/kg/hr �ı�ʱ
	//	step1. ���� (����/min)
	//	step2. ���� (����/kg/hr)
	//	step3. ���� (����/kg/min)
	//	step4. ���� ��Һ�ٶ�
	//	step5. ���� ����
	//	step6. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DoseKgMin_Calc();		//���� ����kg/min
	Drug_DOSEKG_TO_DOSE_Calc(); //���� ����/hr & ����/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

void	Ctl_Func_DrugCalc_InfRate()
{
	//GetWindowText(editInfRate,buff,sizeof(buff));
	gDrugData[DRUG_DATA_INFRATE] = GetEditValue(editInfRate)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	��Һ�ٶ� �ı�ʱ
	//	step1. ���� (����/hr, ����/min)
	//	step2. ���� (����/kg/hr, ����/kg/min)
	//	step3. ���� ����
	//	step7. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_INFrate_To_DoseHr();	//ͨ�� ��Һ�ٶ� ���� ����/hr
	Drug_DoseHr_To_DoseMin();	//ͨ�� ����/hr ���� ����/min
	Drug_DOSE_KG_MIN_HR_Calc(); //���� ����/kg/hr & ����/kg/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();

}
void	Ctl_Func_DrugCalc_DripRate()
{
	//GetWindowText(editDripRate,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DRIPRATE]  = GetEditValue(editDripRate)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	���� �ı�ʱ
	//	step1. ���� (����/hr, ����/min)
	//	step2. ���� (����/kg/hr, ����/kg/min)
	//	step3. ���� ����
	//	step7. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��
	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DripRate_To_DoseMin();	//ͨ�� ���� ���� ����/min
	Drug_DoseMin_To_DoseHr();	//ͨ�� ����/min ���� ����/hr
	Drug_DOSE_KG_MIN_HR_Calc();	//���� ����/kg/hr & ����/kg/min
	Drug_Duration_Calc();		//���� ����ʱ��
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}
void	Ctl_Func_DrugCalc_DripSize()
{
	//GetWindowText(editDripSize,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DRIPSIZE]  = GetEditValue(editDripSize)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	ÿ����� �ı�ʱ
	//	step1. ���� (����/hr, ����/min)
	//	step2. ���� (����/kg/hr, ����/kg/min)
	//	step3. ���� ����
	//	step7. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_DripRate_To_DoseMin(); //ͨ�� ÿ����� ���� ����/min
	Drug_DoseMin_To_DoseHr();	//ͨ�� ����/min ���� ����/hr
	Drug_DOSE_KG_MIN_HR_Calc(); //���� ����/kg/hr & ����/kg/min
	Drug_Duration_Calc();		//���� ����ʱ��

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}
void	Ctl_Func_DrugCalc_Duration()
{
	//GetWindowText(editDuration,buff,sizeof(buff));
	gDrugData[DRUG_DATA_DURATION]  = GetEditValue(editDuration)/DRUG_SCALE_VAL;
	////////////////////////////////////////////////
	//	����ʱ�� �ı�ʱ
	//	step1. ���� (����/hr, ����/min)
	//	step2. ���� (����/kg/hr, ����/kg/min)
	//	step3. ���� ����
	//	step7. ���� ����ʱ��
	//
	//	����/min = ����/hr �� 60
	//	����/kg/hr = ����/hr �� ��������
	//	����/kg/min = ����/min �� ��������
	//	
	//	����ʱ�� = ҩ������ �� ����/hr
	//	��Һ�ٶ� = ����/hr �� ҩ��Ũ��
	//	����	 = ����/min �� ÿ����� �� ҩ��Ũ��

	//	ע��:��Ҫ�����������㲽��
	///////////////////////////////////////////////
	Drug_Dura_TO_DoseHr();		//ͨ�� ����ʱ�� ���� ����/hr
	Drug_DoseHr_To_DoseMin();	//ͨ�� ����/hr ���� ����/min
	Drug_DOSE_KG_MIN_HR_Calc();	//���� ����/kg/hr & ����/kg/min
	Drug_INFrate_Calc();		//���� ��Һ�ٶ�
	Drug_DripRate_Calc();		//���� ����

	//������ֵ
	DrugSetValue();
	Drug_Calc_Unit_Init();
}

/*
	�����ؼ��ص�����
*/
static int EditProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	char strHelp[200] = {0};
	//���ð�������������ɫ��������ɫ
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_DRUG_DRUGNAME+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_drugname", strHelp, sizeof strHelp,"Select the drug name.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_WEIGHT+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_weight", strHelp, sizeof strHelp,"Enter the patient's weight.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_AMOUNT+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_amount", strHelp, sizeof strHelp,"Enter the amount of drug.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_VOLUME+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_volume", strHelp, sizeof strHelp,"Enter the volume of solution.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_CONCENTRATION+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_concent", strHelp, sizeof strHelp,"Enter the drug concentration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DOSEMIN+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_dosemin", strHelp, sizeof strHelp,"Enter the drug dosage to be infused per minute.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DOSEHR+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_dosehr", strHelp, sizeof strHelp,"Enter the drug doseage to be infused per hour.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DOSEKGMIN+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_dosekgmin", strHelp, sizeof strHelp,"Enter the drug doseage to be infused per kg per minute.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DOSEKGHR+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_dosekghr", strHelp, sizeof strHelp,"Enter the drug dosage to be infused per kg per hour.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_INFORATE+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_infrate", strHelp, sizeof strHelp,"Enter the rate(ml\%h) at which the solution is infused.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DRIPRATE+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_driprate", strHelp, sizeof strHelp,"Enter the rate(gtt\%min) at which the solution is infused.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DRIPSIZE+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_dripsize", strHelp, sizeof strHelp,"This value represents how many drops of drug solution per milliliter.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_DURATION+1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_duration", strHelp, sizeof strHelp,"Enter the time that is needed to infuse the solution.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_TITRATION:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "help_titration", strHelp, sizeof strHelp,"Access the titration table.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUG_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
					SetWindowText(hWnd, strHelp);
				}break;
				default:{
					SetWindowText(hWnd, "");
				}break;
			}break;
		}break;	
	}

	temp = (*OldEditProcHelp)(hWnd, message, wParam, lParam);
	return(temp);
}
/*
	�б����Ϣ�ص�����
*/
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	RECT rc;
	HDC hdc= (HDC)NULL;
	int interface;
	GetWindowRect(hWnd, &rc);	
	switch(message){
		case MSG_SETFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				//if(B_PRINTF) printf("rc.=%d %d %d %d\n",rc.left,rc.top,rc.right,rc.bottom);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_KILLFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
	case MSG_COMMAND:{
		case MSG_KEYUP:{
			/*	
			switch(wParam){
				case SCANCODE_ENTER:{
					id = GetDlgCtrlID(hWnd);
					switch(id){
						case DID_DRUG_DRUGNAME+1:{
							bCobDrugName = !bCobDrugName;
							if(bCobDrugName){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
							if(B_PRINTF) printf("index = %d\n",index);
							SendMessage(cobDrugName, CB_SETCURSEL,index,0);
							Drug_Calc_Init();
							Drug_Calc_Unit_Init();
						}break;		
					
				}break;
				
			}break;
		
		}
		*/		
		
		if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
					
					switch(id){
						case DID_DRUG_DRUGNAME+1:{
							bCobDrugName = !bCobDrugName;
							if(bCobDrugName){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							
							index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
							if(B_PRINTF) printf("index = %d\n",index);
						//	SendMessage(cobDrugName, CB_SETCURSEL,index,0);
							Drug_Calc_Init();
							Drug_Calc_Unit_Init();
						}break;
					}	
				//	gbKeyType = KEY_TAB;
				}break;
			
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);}


/*
	��ť��Ϣ�ص�����
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	
	switch(message){
		case MSG_SETFOCUS:{
			SetWindowBkColor(hWnd, SETFOCUS_COLOR);
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;	
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}
/*
	�༭��ص�����
*/

static int EditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int iVaue;
	int id;
	RECT rc;
	HDC hdc= (HDC)NULL;
	unsigned char buff[50];
	GetWindowRect(hWnd, &rc);
	switch(message){
		case MSG_SETFOCUS:{
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
				
			}
		}break;
		case MSG_KILLFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_KEYUP:{
			id = GetDlgCtrlID(hWnd);
			switch(wParam){
				case SCANCODE_CURSORBLOCKDOWN:{
					switch(id){
						case DID_DRUG_WEIGHT+1:{
							Drug_Step_Set(editWeight,0,DRUG_LIMIT_WEIGHT,FALSE);
							iTmpWeight=GetEditValue(editWeight);
							if(iTmpWeight<gLimitHigh[DRUG_LIMIT_WEIGHT]){
								iTmpWeight += gStepValue[DRUG_LIMIT_WEIGHT];
								SetEditText(hWnd, iTmpWeight);
							}
						}break;
						case DID_DRUG_AMOUNT+1:{
							Drug_Step_Set(editAmount,1,DRUG_LIMIT_AMOUNT,FALSE);
							iTmpAmount=GetEditValue(editAmount);
							if(iTmpAmount<gLimitHigh[DRUG_LIMIT_AMOUNT]){
								iTmpAmount+= gStepValue[DRUG_LIMIT_AMOUNT];
								SetEditText(editAmount, iTmpAmount);
							}
						}break;
						case DID_DRUG_VOLUME+1:{
							Drug_Step_Set(editVolume,1,DRUG_LIMIT_VOLUME,FALSE);
							iTmpVolume=GetEditValue(editVolume);
							if(iTmpVolume<gLimitHigh[DRUG_LIMIT_VOLUME]){
								iTmpVolume+= gStepValue[DRUG_LIMIT_VOLUME];
								SetEditText(editVolume, iTmpVolume);
							}
						}break;
						case DID_DRUG_CONCENTRATION+1:{
							Drug_Step_Set(editConcentration,1,DRUG_LIMIT_CONCEN,FALSE);
							iTmpConcent=GetEditValue(editConcentration);
							if(iTmpConcent<gLimitHigh[DRUG_LIMIT_CONCEN]){
								iTmpConcent+= gStepValue[DRUG_LIMIT_CONCEN];
								SetEditText(editConcentration, iTmpConcent);
							}
						}break;
						case DID_DRUG_DOSEMIN+1:{
							Drug_Step_Set(editDosemin,1,DRUG_LIMIT_DOSEMIN,FALSE);
							iTmpDosemin=GetEditValue(editDosemin);
							if(iTmpDosemin<gLimitHigh[DRUG_LIMIT_DOSEMIN]){
								iTmpDosemin+= gStepValue[DRUG_LIMIT_DOSEMIN];
								SetEditText(editDosemin, iTmpDosemin);
							}
						}break;
						case DID_DRUG_DOSEHR+1:{
							Drug_Step_Set(editDosehr,1,DRUG_LIMIT_DOSEHR,FALSE);
							iTmpDosehr=GetEditValue(editDosehr);
							if(iTmpDosehr<gLimitHigh[DRUG_LIMIT_DOSEHR]){
								iTmpDosehr+= gStepValue[DRUG_LIMIT_DOSEHR];
								SetEditText(editDosehr, iTmpDosehr);
							}
						}break;
						case DID_DRUG_DOSEKGMIN+1:{
							Drug_Step_Set(editDosekgmin,1,DRUG_LIMIT_DOSEKGMIN,FALSE);
							iTmpDosekgmin=GetEditValue(editDosekgmin);
							if(iTmpDosekgmin<gLimitHigh[DRUG_LIMIT_DOSEKGMIN]){
								iTmpDosekgmin+= gStepValue[DRUG_LIMIT_DOSEKGMIN];
								SetEditText(editDosekgmin, iTmpDosekgmin);
							}
						}break;
						case DID_DRUG_DOSEKGHR+1:{
							Drug_Step_Set(editDosekghr,1,DRUG_LIMIT_DOSEKGHR,FALSE);
							iTmpDosekghr=GetEditValue(editDosekghr);
							if(iTmpDosekghr<gLimitHigh[DRUG_LIMIT_DOSEKGHR]){
								iTmpDosekghr+= gStepValue[DRUG_LIMIT_DOSEKGHR];
								SetEditText(editDosekghr, iTmpDosekghr);
							}
						}break;
						case DID_DRUG_INFORATE+1:{
							Drug_Step_Set(editInfRate,1,DRUG_LIMIT_INFRATE,FALSE);
							iTmpInfRate=GetEditValue(editInfRate);
							if(iTmpInfRate<gLimitHigh[DRUG_LIMIT_INFRATE]){
								iTmpInfRate+= gStepValue[DRUG_LIMIT_INFRATE];
								SetEditText(editInfRate, iTmpInfRate);
							}
						}break;
						case DID_DRUG_DRIPRATE+1:{
							Drug_Step_Set(editDripRate,1,DRUG_LIMIT_DRIPRATE,FALSE);
							iTmpDripRate=GetEditValue(editDripRate);
							if(iTmpDripRate<gLimitHigh[DRUG_LIMIT_DRIPRATE]){
								iTmpDripRate+= gStepValue[DRUG_LIMIT_DRIPRATE];
								SetEditText(editDripRate, iTmpDripRate);
							}
						}break;
						case DID_DRUG_DRIPSIZE+1:{
							Drug_Step_Set(editDripSize,1,DRUG_LIMIT_DRIPSIZE,FALSE);
							iTmpDripSize=GetEditValue(editDripSize);
							if(iTmpDripSize<gLimitHigh[DRUG_LIMIT_DRIPSIZE]){
								iTmpDripSize+= gStepValue[DRUG_LIMIT_DRIPSIZE];
								SetEditText(editDripSize, iTmpDripSize);
							}
						}break;
						case DID_DRUG_DURATION+1:{
							Drug_Step_Set(editDuration,1,DRUG_LIMIT_DURATION,FALSE);
							iTmpDuration=GetEditValue(editDuration);
							if(iTmpDuration<gLimitHigh[DRUG_LIMIT_DURATION]){
								iTmpDuration+= gStepValue[DRUG_LIMIT_DURATION];
								SetEditText(editDuration, iTmpDuration);
							}
						}break;
						
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_DRUG_WEIGHT+1:{
							Drug_Step_Set(editWeight,0,DRUG_LIMIT_WEIGHT,TRUE);
							iTmpWeight=GetEditValue(editWeight);
							if(iTmpWeight>gLimitLow[DRUG_LIMIT_WEIGHT]){
								iTmpWeight -= gStepValue[DRUG_LIMIT_WEIGHT];
								SetEditText(hWnd, iTmpWeight);
							}
						}break;
						case DID_DRUG_AMOUNT+1:{
							Drug_Step_Set(editAmount,1,DRUG_LIMIT_AMOUNT,TRUE);
							iTmpAmount=GetEditValue(editAmount);
							if(iTmpAmount>gLimitLow[DRUG_LIMIT_AMOUNT]){
								iTmpAmount-= gStepValue[DRUG_LIMIT_AMOUNT];
								SetEditText(editAmount, iTmpAmount);
							}
						}break;
						case DID_DRUG_VOLUME+1:{
							Drug_Step_Set(editVolume,1,DRUG_LIMIT_VOLUME,TRUE);
							iTmpVolume=GetEditValue(editVolume);
							if(iTmpVolume>gLimitLow[DRUG_LIMIT_VOLUME]){
								iTmpVolume-= gStepValue[DRUG_LIMIT_VOLUME];
								SetEditText(editVolume, iTmpVolume);
							}
						}break;
						case DID_DRUG_CONCENTRATION+1:{
							Drug_Step_Set(editConcentration,1,DRUG_LIMIT_CONCEN,TRUE);
							iTmpConcent=GetEditValue(editConcentration);
							if(iTmpConcent>gLimitLow[DRUG_LIMIT_CONCEN]){
								iTmpConcent-= gStepValue[DRUG_LIMIT_CONCEN];
								SetEditText(editConcentration, iTmpConcent);
							}
						}break;
						case DID_DRUG_DOSEMIN+1:{
							Drug_Step_Set(editDosemin,1,DRUG_LIMIT_DOSEMIN,TRUE);
							iTmpDosemin=GetEditValue(editDosemin);
							if(iTmpDosemin>gLimitLow[DRUG_LIMIT_DOSEMIN]){
								iTmpDosemin-= gStepValue[DRUG_LIMIT_DOSEMIN];
								SetEditText(editDosemin, iTmpDosemin);
							}
						}break;
						case DID_DRUG_DOSEHR+1:{
							Drug_Step_Set(editDosehr,1,DRUG_LIMIT_DOSEHR,TRUE);
							iTmpDosehr=GetEditValue(editDosehr);
							if(iTmpDosehr>gLimitLow[DRUG_LIMIT_DOSEHR]){
								iTmpDosehr-= gStepValue[DRUG_LIMIT_DOSEHR];
								SetEditText(editDosehr, iTmpDosehr);
							}
						}break;
						case DID_DRUG_DOSEKGMIN+1:{
							Drug_Step_Set(editDosekgmin,1,DRUG_LIMIT_DOSEKGMIN,TRUE);
							iTmpDosekgmin=GetEditValue(editDosekgmin);
							if(iTmpDosekgmin>gLimitLow[DRUG_LIMIT_DOSEKGMIN]){
								iTmpDosekgmin-= gStepValue[DRUG_LIMIT_DOSEKGMIN];
								SetEditText(editDosekgmin, iTmpDosekgmin);
							}
						}break;
						case DID_DRUG_DOSEKGHR+1:{
							Drug_Step_Set(editDosekghr,1,DRUG_LIMIT_DOSEKGHR,TRUE);
							iTmpDosekghr=GetEditValue(editDosekghr);
							if(iTmpDosekghr>gLimitLow[DRUG_LIMIT_DOSEKGHR]){
								iTmpDosekghr-= gStepValue[DRUG_LIMIT_DOSEKGHR];
								SetEditText(editDosekghr, iTmpDosekghr);
							}
						}break;
						case DID_DRUG_INFORATE+1:{
							Drug_Step_Set(editInfRate,1,DRUG_LIMIT_INFRATE,TRUE);
							iTmpInfRate=GetEditValue(editInfRate);
							if(iTmpInfRate>gLimitLow[DRUG_LIMIT_INFRATE]){
								iTmpInfRate-= gStepValue[DRUG_LIMIT_INFRATE];
								SetEditText(editInfRate, iTmpInfRate);
							}
						}break;
						case DID_DRUG_DRIPRATE+1:{
							Drug_Step_Set(editDripRate,1,DRUG_LIMIT_DRIPRATE,TRUE);
							iTmpDripRate=GetEditValue(editDripRate);
							if(iTmpDripRate>gLimitLow[DRUG_LIMIT_DRIPRATE]){
								iTmpDripRate-= gStepValue[DRUG_LIMIT_DRIPRATE];
								SetEditText(editDripRate, iTmpDripRate);
							}
						}break;
						case DID_DRUG_DRIPSIZE+1:{
							Drug_Step_Set(editDripSize,1,DRUG_LIMIT_DRIPSIZE,TRUE);
							iTmpDripSize=GetEditValue(editDripSize);
							if(iTmpDripSize>gLimitLow[DRUG_LIMIT_DRIPSIZE]){
								iTmpDripSize-= gStepValue[DRUG_LIMIT_DRIPSIZE];
								SetEditText(editDripSize, iTmpDripSize);
							}
						}break;
						case DID_DRUG_DURATION+1:{
							Drug_Step_Set(editDuration,1,DRUG_LIMIT_DURATION,TRUE);
							iTmpDuration=GetEditValue(editDuration);
							if(iTmpDuration>gLimitLow[DRUG_LIMIT_DURATION]){
								iTmpDuration-= gStepValue[DRUG_LIMIT_DURATION];
								SetEditText(editDuration, iTmpDuration);
							}
						}break;
						
						}
				}break;
					
				case SCANCODE_ENTER:{
					switch(id){
						case DID_DRUG_WEIGHT+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_Weight();
						
						}break;
						case DID_DRUG_AMOUNT+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
								
							
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							
							}
							Ctl_Func_DrugCalc_Amount();
							
							
						}break;
						case DID_DRUG_VOLUME+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_Volume();
						
						}break;
						case DID_DRUG_CONCENTRATION+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_Concen();
						
						}break;
						case DID_DRUG_DOSEMIN+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DoseMin();
						
						}break;
						case DID_DRUG_DOSEHR+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DoseHr();
						
						}break;
						case DID_DRUG_DOSEKGMIN+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DoseKgMin();
						
						}break;
						case DID_DRUG_DOSEKGHR+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DoseKgHr();
						
						}break;
						case DID_DRUG_INFORATE+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_InfRate();
							
						}break;
						case DID_DRUG_DRIPRATE+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DripRate();
						
						}break;
						case DID_DRUG_DRIPSIZE+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_DripSize();
						
						}break;
						case DID_DRUG_DURATION+1:{
							bEdit= !bEdit;
							if(bEdit){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							Ctl_Func_DrugCalc_Duration();
						
						}break;
						
					}	
				}break;
			}
			return 0;
		}break;	
	}
		
	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
	return(temp);
}

/*
	�����ؼ�����
*/
static int CreateCtlProc(HWND hDlg)
{

	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_DRUG_CAPTION, 0, 5, 420, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_DRUG_HELP);
	
	btnTitration= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUG_TITRATION, 200, 210, 90, 26, hDlg, 0);
	btnExit= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUG_OK, 300, 210, 90, 26, hDlg, 0);
	

	stcDrugName = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DRUGNAME, 5, 30+30*0+4, 75, 26, hDlg, 0);


	stcWeight = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_WEIGHT, 5, 30+30*1+4, 75, 26, hDlg, 0);
	stcAmount = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_AMOUNT, 5, 30+30*2+4, 75, 26, hDlg, 0);
	stcVolume = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_VOLUME, 5, 30+30*3+4, 75, 26, hDlg, 0);
	stcConcentration = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_CONCENTRATION, 5, 30+30*4+4, 80, 26, hDlg, 0);
	stcDosemin = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DOSEMIN, 5, 30+30*5+4, 75, 26, hDlg, 0);
	stcDosehr = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DOSEHR, 5, 30+30*6+4, 75, 26, hDlg, 0);
	
	stcDosekgmin = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DOSEKGMIN,200 , 30+30*0+4, 75, 26, hDlg, 0);
	stcDosekghr = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DOSEKGHR, 200, 30+30*1+4, 75, 26, hDlg, 0);
	stcInfRate = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_INFORATE, 200, 30+30*2+4, 75, 26, hDlg, 0);
	stcDripRate = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DRIPRATE, 200, 30+30*3+4, 75, 26, hDlg, 0);
	stcDripSize = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DRIPSIZE, 200, 30+30*4+4, 75, 26, hDlg, 0);
	stcDuration = CreateWindow("static", "", WS_CHILD| SS_NOTIFY | WS_VISIBLE,  DID_DRUG_DURATION, 200, 30+30*5+4, 75, 26, hDlg, 0);
	

	cobDrugName = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST | WS_TABSTOP|CBS_READONLY,  
							DID_DRUG_DRUGNAME+1, 85, 30+30*0, 100, 26, hDlg, 0);
	editWeight = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_WEIGHT+1, 85, 30+30*1, 60, 23, hDlg, 0);
	editAmount = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_AMOUNT+1, 85, 30+30*2, 60, 23, hDlg, 0);
	editVolume = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_VOLUME+1, 85, 30+30*3, 60, 23, hDlg, 0);
	editConcentration = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_CONCENTRATION+1, 85, 30+30*4, 60, 23, hDlg, 0);
	editDosemin = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_DOSEMIN+1, 85, 30+30*5, 60, 23, hDlg, 0);
	editDosehr = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_DOSEHR+1, 85, 30+30*6, 60, 23, hDlg, 0);
	editDosekgmin = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_DOSEKGMIN+1, 270, 30+30*0, 60, 23, hDlg, 0);
	editDosekghr = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_DOSEKGHR+1, 270, 30+30*1, 60, 23, hDlg, 0);
	editInfRate = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,DID_DRUG_INFORATE+1, 270, 30+30*2, 60, 23, hDlg, 0);
	editDripRate = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_DRIPRATE+1, 270, 30+30*3, 60, 23, hDlg, 0);
	editDripSize = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_DRIPSIZE+1, 270, 30+30*4, 60, 23, hDlg, 0);
	editDuration = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,DID_DRUG_DURATION+1, 270, 30+30*5, 60, 23, hDlg, 0);
	
	
	stcWeightU = CreateWindow("static", "kg", WS_CHILD| WS_VISIBLE,  DID_DRUG_WEIGHT+2, 150, 30+30*1+4, 50, 26, hDlg, 0);
	stcAmountU= CreateWindow("static", "mg", WS_CHILD| WS_VISIBLE,  DID_DRUG_AMOUNT+2, 150, 30+30*2+4, 50, 26, hDlg, 0);
	stcVolumeU = CreateWindow("static", "ml", WS_CHILD| WS_VISIBLE,  DID_DRUG_VOLUME+2, 150, 30+30*3+4, 50, 26, hDlg, 0);
	stcConcentrationU = CreateWindow("static", "mg/ml", WS_CHILD| WS_VISIBLE,  DID_DRUG_CONCENTRATION+2, 150, 30+30*4+4, 50, 26, hDlg, 0);
	stcDoseminU = CreateWindow("static", "mcg", WS_CHILD| WS_VISIBLE,  DID_DRUG_DOSEMIN+2, 150, 30+30*5+4, 50, 26, hDlg, 0);
	stcDosehrU = CreateWindow("static", "mg", WS_CHILD| WS_VISIBLE,  DID_DRUG_DOSEHR+2, 150, 30+30*6+4, 50, 26, hDlg, 0);
	stcDosekgminU = CreateWindow("static", "mcg", WS_CHILD| WS_VISIBLE,  DID_DRUG_DOSEKGMIN+2, 335, 30+30*0+4, 50, 26, hDlg, 0);
	stcDosekghrU = CreateWindow("static", "mcg", WS_CHILD| WS_VISIBLE,  DID_DRUG_DOSEKGHR+2, 335, 30+30*1+4, 50, 26, hDlg, 0);
	stcInfRateU = CreateWindow("static", "ml/hr", WS_CHILD| WS_VISIBLE,  DID_DRUG_INFORATE+2, 335, 30+30*2+4, 50, 26, hDlg, 0);
	stcDripRateU = CreateWindow("static", "GTT/min", WS_CHILD| WS_VISIBLE,  DID_DRUG_DRIPRATE+2, 335,30+30*3+4, 50, 26, hDlg, 0);
	stcDripSizeU = CreateWindow("static", "GTT/hr", WS_CHILD| WS_VISIBLE,  DID_DRUG_DRIPSIZE+2, 335, 30+30*4+4, 50, 26, hDlg, 0);
	stcDurationU = CreateWindow("static", "hr", WS_CHILD| WS_VISIBLE,  DID_DRUG_DURATION+2, 335, 30+30*5+4, 50, 26, hDlg, 0);

		
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "caption", strMenu, sizeof strMenu,"Recorder Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "titration", strMenu, sizeof strMenu,"Titration>>");	
	SetWindowText(btnTitration, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnExit, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "drug_name", strMenu, sizeof strMenu,"Drug Name");	
	SetWindowText(stcDrugName, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "weight", strMenu, sizeof strMenu,"Weight");	
	SetWindowText(stcWeight, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "amount", strMenu, sizeof strMenu,"Amount");	
	SetWindowText(stcAmount, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "volume", strMenu, sizeof strMenu,"Volume");	
	SetWindowText(stcVolume, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "concentration", strMenu, sizeof strMenu,"Concentration");	
	SetWindowText(stcConcentration, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "dose_min", strMenu, sizeof strMenu,"Dose/min");	
	SetWindowText(stcDosemin, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "dose_hr", strMenu, sizeof strMenu,"Dose/hr");	
	SetWindowText(stcDosehr, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "dose_kg_min", strMenu, sizeof strMenu,"Dose/kg/min");	
	SetWindowText(stcDosekgmin, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "dose_kg_hr", strMenu, sizeof strMenu,"Dose/kg/hr");	
	SetWindowText(stcDosekghr, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "inf_rate", strMenu, sizeof strMenu,"Inf Rate");	
	SetWindowText(stcInfRate, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "drip_rate", strMenu, sizeof strMenu,"Drip Rate");	
	SetWindowText(stcDripRate, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "drip_size", strMenu, sizeof strMenu,"Drip Size");	
	SetWindowText(stcDripSize, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUG, "duration", strMenu, sizeof strMenu,"Duration");	
	SetWindowText(stcDuration, strMenu);

		
	
	return 0;
}

static int InitCobList(HWND hDlg)
{
	int i;
	unsigned char*strDrugName[15]={"Drug A", "Drug B","Drug C","Drug D","Drug E",
								"AMINOPHYLLINE","DOBUTAMINE","DOPAMINE","EPINEPHRINE",
								"HEPARIN","ISUPREL","LIDOCAINE","NIPRIDE","NITROGLYCERIN","PITOCIN"};
	for(i=0;i<15;i++){
		SendMessage(cobDrugName, CB_ADDSTRING, 0, (LPARAM)strDrugName[i]);
	}
		SendMessage(cobDrugName, CB_SETCURSEL,0,0);
	return 0;
}


static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnExit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnTitration, BtnProc);
	OldCobProc = SetWindowCallbackProc(cobDrugName, CobProc);

	OldEditProc = SetWindowCallbackProc(editWeight, EditProc);
	OldEditProc = SetWindowCallbackProc(editAmount, EditProc);
	OldEditProc = SetWindowCallbackProc(editVolume, EditProc);
	OldEditProc = SetWindowCallbackProc(editConcentration, EditProc);
	OldEditProc = SetWindowCallbackProc(editDosemin, EditProc);
	OldEditProc = SetWindowCallbackProc(editDosehr, EditProc);
	OldEditProc = SetWindowCallbackProc(editDosekgmin, EditProc);
	OldEditProc = SetWindowCallbackProc(editDosekghr, EditProc);
	OldEditProc = SetWindowCallbackProc(editInfRate, EditProc);
	OldEditProc = SetWindowCallbackProc(editDripRate, EditProc);
	OldEditProc = SetWindowCallbackProc(editDripSize, EditProc);
	OldEditProc = SetWindowCallbackProc(editDuration, EditProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

/*
	�Ի���ص�����
*/
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
	RECT rcCaption;
	switch(message){
		case MSG_INITDIALOG:{
			//�����ؼ�
			CreateCtlProc(hDlg);
			//���ÿؼ��ַ�
			InitCtlName(hDlg);
			//��ʼ���б��
			InitCobList(hDlg);
			//���ÿؼ��ص�����
			InitCtlCallBackProc(hDlg);
				
			//��ֵ��ʼ��
			Drug_Calc_Init();
			//��ʼ����λ
			Drug_Calc_Unit_Init();
			gbKeyType = KEY_TAB;
		SetFocus(btnExit);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_DRUG_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
					}break;
				case DID_DRUG_TITRATION:{
					CreateDRUGTITR(hDlg);
					RestorWave();
					}break;
				}
		}break;	
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_LR;	
			//�ָ�����λ��
		//	ChangeWavePanelsLeft((PRECT)NULL);	
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	�����Ի���
*/
void CreateDRUG(HWND hWnd)
{
		int High_Dlg;
		
if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=160;
#if SCREEN_1024
	DlgSet.x = SCREEN_LEFT+100,
	DlgSet.y = SCREEN_TOP+180, 
#elif  SCREEN_640
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-290, 		
#else 
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-290-High_Dlg, 		
#endif
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

