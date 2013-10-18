/*************************************************************************** 
 *	Module Name:	AlarmManger
 *
 *	Abstract:		������Ϣ����
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-27 10:12:29
 ***************************************************************************/
#ifndef __ALARMMANAGE_H__
#define __ALARMMANAGE_H__


//��������ϢID(δ��˵����,���������û��ɵ�)
#define AP_NONE				0
//ECG
#define AP_HR_HIGH				AP_NONE+1		
#define AP_HR_LOW				AP_NONE+2		
#define AP_ECG_WEAKSIGNAL		AP_NONE+3		//�ź�̫��,�߼�����
//RESP
#define AP_RR_HIGH				AP_NONE+4
#define AP_RR_LOW				AP_NONE+5
#define AP_APNEA				AP_NONE+6		//��Ϣ,�߼�����,�û����Թر�
//SPO2
#define AP_SPO2_HIGH			AP_NONE+7
#define AP_SPO2_LOW			AP_NONE+8
#define AP_SPO2_LOWEST		AP_NONE+9
//Pulse
#define AP_PR_HIGH				AP_NONE+10
#define AP_PR_LOW				AP_NONE+11
//TEMP
#define AP_T1_HIGH				AP_NONE+12
#define AP_T1_LOW				AP_NONE+13
#define AP_T2_HIGH				AP_NONE+14
#define AP_T2_LOW				AP_NONE+15
#define AP_TD_HIGH				AP_NONE+16
//NIBP
#define AP_NSYS_HIGH			AP_NONE+17
#define AP_NSYS_LOW			AP_NONE+18
#define AP_NDIA_HIGH			AP_NONE+19
#define AP_NDIA_LOW			AP_NONE+20
#define AP_NMEAN_HIGH			AP_NONE+21
#define AP_NMEAN_LOW			AP_NONE+22
//CO2
#define AP_ETCO2_HIGH			AP_NONE+23
#define AP_ETCO2_LOW			AP_NONE+24
#define AP_FICO2_HIGH			AP_NONE+25
//IBP1
#define AP_ISYS1_HIGH			AP_NONE+26
#define AP_ISYS1_LOW			AP_NONE+27
#define AP_IDIA1_HIGH			AP_NONE+28
#define AP_IDIA1_LOW			AP_NONE+29
#define AP_IMEAN1_HIGH			AP_NONE+30
#define AP_IMEAN1_LOW			AP_NONE+31
//IBP2
#define AP_ISYS2_HIGH			AP_NONE+32
#define AP_ISYS2_LOW			AP_NONE+33
#define AP_IDIA2_HIGH			AP_NONE+34
#define AP_IDIA2_LOW			AP_NONE+35
#define AP_IMEAN2_HIGH			AP_NONE+36
#define AP_IMEAN2_LOW			AP_NONE+37

#define AP_ST1_HIGH				AP_NONE+38
#define AP_ST1_LOW				AP_NONE+39
#define AP_ST2_HIGH				AP_NONE+40
#define AP_ST2_LOW				AP_NONE+41

#define AP_ARR					AP_NONE+42




//���������������
//#define ALM_PHY_INFOMAX		38
#define ALM_PHY_INFOMAX		43
//�����������������Ϣ����
#define ALM_PHY_INFOLEN		50

//����������ϢID
#define AT_NONE			0
//ECG Info
#define AT_ECG_COMERR				AT_NONE+1		//ECG ͨѶ���ϣ�����
#define AT_ECG_COMSTOP			AT_NONE+2		//ECG ͨѶֹͣ������
#define AT_ECG_MODULEERR			AT_NONE+3		//ECG ģ����ϣ�����
#define AT_ECGOFF_V					AT_NONE+4		//V�����䣭����
#define AT_ECGOFF_RA				AT_NONE+5		//RA���䣭����
#define AT_ECGOFF_LL				AT_NONE+6		//LL���䣭����
#define AT_ECGOFF_LA				AT_NONE+7		//LA���䣭����
#define AT_ECG_LIMITERR			AT_NONE+8 		//�����޴�����
#define AT_ECG_INTERFERE			AT_NONE+9 		//����̫�󣭣���
#define AT_ECG_OVERRANGE			AT_NONE+10 		//����ֵ����������Χ������

#define AT_ECG_OVERLOAD1			AT_NONE+11 		//����ֵ����������Χ������new
#define AT_ECG_OVERLOAD2			AT_NONE+12 		//����ֵ����������Χ������new
#define AT_ECG_OVERLOAD3			AT_NONE+13 		//����ֵ����������Χ������new

//RESP Info
#define AT_RESP_LIMITERR			AT_NONE+14 		//�����޴�����
#define AT_RESP_OVERRANGE			AT_NONE+15 		//����ֵ����������Χ������
#define AT_CVA						AP_NONE+16		//�Ķ�����--------��	new

//SpO2 Info
#define AT_SPO2_COMERR			AT_NONE+17		//ͨѶ���ϣ�����
#define AT_SPO2_COMSTOP			AT_NONE+18		//ͨѶֹͣ������
#define AT_SPO2_MODULEINITERR		AT_NONE+19		//ģ���ʼ�����󣭣���
#define AT_SPO2_MODULEERR			AT_NONE+20		//����ģ����ϣ�����
#define AT_SPO2_LIMITERR			AT_NONE+21 		//�����޴�����
#define AT_SPO2_OVERRANGE			AT_NONE+22 		//����ֵ����������Χ������
#define AT_SPO2_NOSEMSOR			AT_NONE+23 		//δ��̽ͷ
#define AT_SPO2_SENSORERR			AT_NONE+24 		//̽ͷ���ϻ���
#define AT_SPO2_UNKNOWN			AT_NONE+25 		//̽ͷ�޷�ʶ��
#define AT_SPO2_INCOMPATIBLE		AT_NONE+26 		//̽ͷ������
#define AT_SPO2_SENSOROFF			AT_NONE+27 		//̽ͷ����
//�ϲ�SpO2��������
#define AT_SPO2_ERROR				AT_NONE+28		//�������󣭣������ɴ�����������
//Pulse Info
#define AT_PULSE_LIMITERR			AT_NONE+29 		//�����޴�����
#define AT_PULSE_OVERRANGE		AT_NONE+30 		//����ֵ����������Χ������
//NIBP Info
#define AT_NIBP_COMERR				AT_NONE+31		//ͨѶ���ϣ�����
#define AT_NIBP_COMSTOP			AT_NONE+32		//ͨѶֹͣ������
#define AT_NIBP_MODULEERR			AT_NONE+33		//ģ����ϣ�����
#define AT_NIBP_SYS_LIMITERR		AT_NONE+34	 	//�����޴�����
#define AT_NIBP_DIA_LIMITERR		AT_NONE+35	 	//�����޴�����
#define AT_NIBP_MEAN_LIMITERR		AT_NONE+36 		//�����޴�����
#define AT_NIBP_OVERRANGE			AT_NONE+37 		//����ֵ����������Χ������
//�ϲ�NIBP��������
#define AT_NIBP_ERROR				AT_NONE+38		//�������󣭣������ɴ�����������
//TEMP Info
#define AT_TEMP_COMERR			AT_NONE+39		//ͨѶ���ϣ�����
#define AT_TEMP_COMSTOP			AT_NONE+40		//ͨѶֹͣ������
#define AT_TEMP_MODULEERR			AT_NONE+41		//ģ����ϣ�����
#define AT_TEMP_T1OVERRANGE		AT_NONE+42		//TEMP1�������ޣ�����
#define AT_TEMP_T2OVERRANGE		AT_NONE+43		//TEMP2�������ޣ�����
#define AT_TEMP_T1SENSOROFF		AT_NONE+44		//TEMP1̽ͷ���䣭����
#define AT_TEMP_T2SENSOROFF		AT_NONE+45		//TEMP2̽ͷ���䣭����
#define AT_TEMP_T1LIMITERRR		AT_NONE+46		//�����޴�����
#define AT_TEMP_T2LIMITERRR		AT_NONE+47		//�����޴�����
#define AT_TEMP_TDLIMITERRR		AT_NONE+48		//�����޴�����
//CO2
#define AT_CO2_COMERR				AT_NONE+49		//ͨѶ���ϣ�����
#define AT_CO2_COMSTOP			AT_NONE+50		//ͨѶֹͣ������
#define AT_CO2_MODULEERR			AT_NONE+51		//ģ����ϣ�����
#define AT_CO2_OVERRANGE			AT_NONE+52		//ģ����ϣ�����
#define AT_CO2_ETCO2LIMITERR		AT_NONE+53		//�����޴�����
#define AT_CO2_FICO2LIMITERR		AT_NONE+54		//�����޴�����
#define AT_CO2_AWRRLIMITERR		AT_NONE+55		//�����޴�����
//IBP1
#define AT_IBP1_COMERR				AT_NONE+56		//ͨѶ���ϣ�����
#define AT_IBP1_COMSTOP			AT_NONE+57		//ͨѶֹͣ������
#define AT_IBP1_MODULEERR			AT_NONE+58		//ģ����ϣ�����
#define AT_IBP1_SYS_LIMITERR		AT_NONE+59 		//�����޴�����
#define AT_IBP1_DIA_LIMITERR			AT_NONE+60 		//�����޴�����
#define AT_IBP1_MEAN_LIMITERR		AT_NONE+61 		//�����޴�����
#define AT_IBP1_OVERRANGE			AT_NONE+62 		//����ֵ����������Χ������
#define AT_IBP1_SENSOROFF			AT_NONE+63		//̽ͷ���䣭����
//IBP2
#define AT_IBP2_COMERR				AT_NONE+64		//ͨѶ���ϣ�����
#define AT_IBP2_COMSTOP			AT_NONE+65		//ͨѶֹͣ������
#define AT_IBP2_MODULEERR			AT_NONE+66		//ģ����ϣ�����
#define AT_IBP2_SYS_LIMITERR		AT_NONE+67 		//�����޴�����
#define AT_IBP2_DIA_LIMITERR		AT_NONE+68 		//�����޴�����
#define AT_IBP2_MEAN_LIMITERR		AT_NONE+69 		//�����޴�����
#define AT_IBP2_OVERRANGE			AT_NONE+70 		//����ֵ����������Χ������
#define AT_IBP2_SENSOROFF			AT_NONE+71		//̽ͷ���䣭����
//SYSTEM Info
#define AT_BATTERY_LOW			AT_NONE+72		//��ص����ͣ�����
													
#define AT_BOARD33V_LOW			AT_NONE+73		//ϵͳ״̬3.3V��ѹ̫�ͣ�����new
#define AT_BOARD33V_HIGH			AT_NONE+74		//ϵͳ״̬3.3V��ѹ̫�ߣ�����new
#define AT_BOARD5V_LOW				AT_NONE+75		//ϵͳ״̬5V��ѹ̫�ͣ�����new
#define AT_BOARD5V_HIGH			AT_NONE+76		//ϵͳ״̬5V��ѹ̫�ߣ�����new
#define AT_SELFTEST_WATCHDOG		AT_NONE+77		//ϵͳ�Լ�watchdog���󣭣���new
#define AT_SELFTEST_AD				AT_NONE+78		//ϵͳ�Լ�A/D���󣭣���new
#define AT_SELFTEST_RAM			AT_NONE+79		//ϵͳ�Լ�RAM���󣭣���new
#define AT_SELFTEST_ROM			AT_NONE+80		//ϵͳ�Լ�ROM���󣭣���new
#define AT_SELFTEST_CPU			AT_NONE+81		//ϵͳ�Լ�CPU���󣭣���new
#define AT_SELFTEST_IBP				AT_NONE+82		//ϵͳ�Լ�IBP���󣭣���new
#define AT_SELFTEST_NBP			AT_NONE+83		//ϵͳ�Լ�NBP���󣭣���new
#define AT_SELFTEST_SPO2			AT_NONE+84		//ϵͳ�Լ�SPO2���󣭣���new
#define AT_SELFTEST_TEMP			AT_NONE+85		//ϵͳ�Լ�TEMP���󣭣���new
#define AT_SELFTEST_RESP			AT_NONE+86		//ϵͳ�Լ�RESP���󣭣���new
#define AT_SELFTEST_ECG			AT_NONE+87		//ϵͳ�Լ�ECG���󣭣���new

#define AT_POWER_ABNORMAL 		AT_NONE+88		//�ϴ��쳣�ػ�������new
#define AT_POWER_FORCE 	 		AT_NONE+89		//�ϴ�ǿ�ƹػ�������new
#define AT_POWER_COMM		  		AT_NONE+90		//��ԴͨѶ���󣭣���new
													
//����������������
#define	ALM_TEC_INFOMAX			91		
//������������������Ϣ����
#define	ALM_TEC_INFOLEN			50		

//����״̬
#define ALMSTATUS_ALARM		1
#define ALMSTATUS_NORMAL		0

/*
	������Ϣ�ṹ
*/
typedef struct _B_ALM_INFO{
	BYTE bAlmID;	//������ϢID��0��û�б�����Ϣ
	BYTE bStatus;	//ALMSTATUS_NORMAL��������ALMSTATUS_ALARM������
	BYTE bClass;	//ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH, ALARM_INFO
	BYTE bPrint;	//�Ƿ���б�����ӡ: ALARM_PRINT_OFF, ALARM_PRINT_ON
}B_ALM_INFO, *PB_ALM_INFO;


//������Ϣ��ʾ����ɫ
#define HIGHALM_BKCOLOR	COLOR_red
#define MIDALM_BKCOLOR		COLOR_yellow
#define LOWALM_BKCOLOR		COLOR_cyan
#define NORMAL_BKCOLOR		COLOR_darkgray
#define NORMAL_TXTCOLOR		COLOR_lightwhite
#define NONE_BKCOLOR		COLOR_black
//#define NONE_BKCOLOR		 RGB2Pixel (hdc, 40, 60, 110)
#define INFO_BKCOLOR		COLOR_cyan
#define INFO_TXTCOLOR		COLOR_black




#endif  //__ALARMMANAGE_H__	
