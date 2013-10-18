/*************************************************************************** 
 *	Module Name:	GeneralFuncs.c	
 *
 *	Abstract:	һЩͨ�ú���
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
 sem_t semCalctime;


/**
	���ϵͳ��Դ״̬
*/
int GetPowerStatus()
{
	gPowerStatus.bShutDown = GetWorkStatus();
	gPowerStatus.bPowerType = GetPowerType();
	gPowerStatus.bBatteryStatus = GetBatteryStatus();
	
	//�ж��Ƿ��б���
	IsLow_Power(&gPowerStatus);
	
	return 0;
}
int GetStringFromResFiles(const char* pResFile, const char* pSection, const char* pKey, char* pString, int iLen,const char* pDefultStr)
{
	char* tmpStr;
	char* strDelim="#";
	char* p;
	int iChar;
	int iL;
	int i, j;
	int res;

	
	if(iLen <=1){
		 if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		  return -1;
	} 

	
	res = GetValueFromEtcFile(pResFile, pSection, pKey, pString, iLen-1);	
	
	if(res<0)  {
		//if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		if(pDefultStr!=NULL)
		strncpy(pString,pDefultStr,iLen);
	}
	return iLen;
	
	tmpStr = malloc(iLen);
	
	
	if(tmpStr !=NULL){
		
		res = GetValueFromEtcFile(pResFile, pSection, pKey, tmpStr, iLen);	
		if(res <0){
			if(B_PRINTF) printf("%s:%d Get String Failure.\n", __FILE__, __LINE__);
			free(tmpStr);
			return -1;
		}
 		if(B_PRINTF) printf("\n(%s:%d)Get source string is %s. lenth is %d.\n", __FILE__, __LINE__, tmpStr, strlen(tmpStr));
		p = (char *)strtok(tmpStr, strDelim);
		iL= strlen(p);
		
		snprintf(pString, iL+1, "%s", p);
		
  		if(B_PRINTF) printf("p: %s -- %d    pvalue: %s -- %d\n", p, iL, pString, strlen(pString));
		while((p=(char *)strtok(NULL, strDelim))){ 
			if(isdigit(*p)){
				iChar = atoi(p);
  				if(B_PRINTF) printf("D-p: %s ---%d\n", p, iChar);
				pString[iL]= iChar;
				iL +=1;
			}
			else{
  				if(B_PRINTF) printf("C-p: %s ---%d, iL is %d.\n", p, strlen(p), iL);
				for(i=iL, j=0; i<(iL+strlen(p)+1); i++,j++){
					pString[i] = p[j];
  					if(B_PRINTF) printf("i(%d) %c -> %c\n", i, p[j], pString[i]);
				}
  				if(B_PRINTF) printf("%s --%d\n", pString, strlen(pString));
//  				iL += strlen(p)+2;
				iL += strlen(p);
			}
		}
//   		if(B_PRINTF) printf("\n\n");
		
		free(tmpStr);
	}
	return iLen;
}


/**
	����Դ�ļ�����ȡ�������ַ��������ĳЩ�����ŷ���ַ���
	����:	
	@param pResFile:	������Դ�ļ�	
	@param pSection:�ֶ���
	@param pKey:��ֵ
	@param pString:��ȡ���ַ���
	@param 	iLen: Ҫ��ȡ�ĳ���
	���:
		ʵ�ʻ�ȡ�ĳ��ȣ���1��Error

*/
int GetStringFromResFile(const char* pResFile, const char* pSection, const char* pKey, char* pString, int iLen)
{
	char* tmpStr;
	char* strDelim="#";
	char* p;
	int iChar;
	int iL;
	int i, j;
	int res;

	
	if(iLen <=1){
		 if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		  return -1;
	} 

	
	res = GetValueFromEtcFile(pResFile, pSection, pKey, pString, iLen-1);	
	
	if(res<0)  {
		if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!\n", __FILE__, __LINE__);
		
	}
	return iLen;
	
	tmpStr = malloc(iLen);
	
	
	if(tmpStr !=NULL){
		
		res = GetValueFromEtcFile(pResFile, pSection, pKey, tmpStr, iLen);	
		if(res <0){
			if(B_PRINTF) printf("%s:%d Get String Failure.\n", __FILE__, __LINE__);
			free(tmpStr);
			return -1;
		}
 		if(B_PRINTF) printf("\n(%s:%d)Get source string is %s. lenth is %d.\n", __FILE__, __LINE__, tmpStr, strlen(tmpStr));
		p = (char *)strtok(tmpStr, strDelim);
		iL= strlen(p);
		
		snprintf(pString, iL+1, "%s", p);
		
  		if(B_PRINTF) printf("p: %s -- %d    pvalue: %s -- %d\n", p, iL, pString, strlen(pString));
		while((p=(char *)strtok(NULL, strDelim))){ 
			if(isdigit(*p)){
				iChar = atoi(p);
  				if(B_PRINTF) printf("D-p: %s ---%d\n", p, iChar);
				pString[iL]= iChar;
				iL +=1;
			}
			else{
  				if(B_PRINTF) printf("C-p: %s ---%d, iL is %d.\n", p, strlen(p), iL);
				for(i=iL, j=0; i<(iL+strlen(p)+1); i++,j++){
					pString[i] = p[j];
  					if(B_PRINTF) printf("i(%d) %c -> %c\n", i, p[j], pString[i]);
				}
  				if(B_PRINTF) printf("%s --%d\n", pString, strlen(pString));
//  				iL += strlen(p)+2;
				iL += strlen(p);
			}
		}
//   		if(B_PRINTF) printf("\n\n");
		
		free(tmpStr);
	}
	return iLen;
}


//д��Դ�ļ��Ļ�����
static pthread_mutex_t mtWriteResFile;

/**
	��ʼ��д�ļ��Ļ�����
*/
int InitWriteFileMutex()
{
	
	//��ʼ��д��Դ�ļ��Ļ�����
	pthread_mutex_init(&mtWriteResFile, NULL);
	
	return 0;
}

/**
	������д�������ļ�
	����ͬ��
*/
int SetIntValueToResFile(const char* pEtcFile, const char* pSection, const char* pKey, int iValue, int iLen)
{
	char *strValue;
	
	if((iLen <=0) || (iLen>10)) return 0;
	
	//����
	pthread_mutex_lock(&mtWriteResFile);
	
	strValue = malloc(iLen+1);
	
	if(strValue !=NULL){
		snprintf(strValue, iLen+1, "%d", iValue);	
		if(B_PRINTF) printf("Write %d -- %s, length is %d.\n", iValue, strValue, iLen);
		SetValueToEtcFile(pEtcFile, pSection, pKey, strValue);
		free(strValue);
		//����
		pthread_mutex_unlock(&mtWriteResFile);
		return iLen+1;
	}
	
	//����                 
	pthread_mutex_unlock(&mtWriteResFile);
	return 0;
}


/**
	����ָ����С��TTF����
	����:
	@param size : Ҫ��������Ĵ�С
	���:
 		TTF�߼�����
*/
PLOGFONT CreateFont_TTF( int size)
{
	PLOGFONT  TTF_View;
	
	//�˴�������Ҫ�����޸���������
 	TTF_View = CreateLogFont(NULL, "Times", "ISO8859-1",
 				 FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
 				 FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
 				 size, 0);
	
	return TTF_View;
}


/**
	���ϵͳʱ��
	����:
	@param bsTime: ʱ��
	���: 
		0-OK, -1-Failure
*/
int GetSysTime(S_TIME *bstime)
{
	time_t	  t;
	struct tm   *tm;

	time(&t);
	tm = localtime(&t);
	if(bstime !=NULL){
		bstime->wYear = tm->tm_year +1900;
		bstime->bMonth = tm->tm_mon +1;
		bstime->bDay = tm->tm_mday;
		bstime->bHour = tm->tm_hour;
		bstime->bMin = tm->tm_min;
		bstime->bSec = tm->tm_sec;
	}
	else{
		//if(B_PRINTF) printf("time is null.\n");
		return -1;
	}
	
	return 0;
}


/**
	����ϵͳʱ��
	����:	 
	@param bsTime : ʱ��
	���:
		0-OK, -1-Failure	
*/
int SetSysTime(S_TIME bsTime)
{
	unsigned char code[21] = {0}	;
	
	//-- �������ʱ�� ��ʽ:����ʱ����.�� --
       // date 082509362010.30
	snprintf(code, sizeof code, "date %.2d%.2d%.2d%.2d%.4d.%.2d",
			 bsTime.bMonth, bsTime.bDay, bsTime.bHour, bsTime.bMin, bsTime.wYear, bsTime.bSec);
	system(code);
	
	//-- ����Ӳ��ʱ�� -- 
	system("/sbin/hwclock -w");
	//system("/work/clock -w");
	//settimeofday(&tv, NULL);	
	
	return 0;	
}

/**
	�Զ�����������(��������)
	����:
	@param number: 
	@param denom:	
	���:
		quot(��)	
*/
int B_Div(int number, int denom)
{
	div_t tmp;
	int res;
	
	tmp = div(number, denom);
		
	res = tmp.quot;
	if(tmp.rem >=5) res+=1;
	
	return res;
}

/**
    ==================================================
    Description:    ����Դʱ�侭��iInter����ʱ��desTime
            
    Parameters:                             method of calling:(r/w/rw/other)
    @param srcTime:Դʱ��                                     r 
    @param desTime:Ŀ��ʱ��                                    w       
    @param iInter: ʱ����(��)                                 r
    @param bType: 
	#define TIME_AFTER		0	//����֮���ʱ��
	#define TIME_BEFORE	1	//����֮ǰ��ʱ��
    return:iInter
    ===================================================
*/
long CalcTime(const S_TIME *srcTime, S_TIME *desTime, long int iInter, BYTE bType)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
    
    //ת�� SYSTIMEΪ tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    //ת�� tm Ϊ time_t
	ttmSrc = mktime(&tmSrc);
    //���� (time_t) ttmDes    
	if(bType == TIME_AFTER)
		ttmDes = (time_t)(ttmSrc + iInter);     
	else
		ttmDes = (time_t)(ttmSrc - iInter);     
    //ת�� time_t Ϊ tm
	tmDes = *localtime(&ttmDes);
   //   if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@DesTime: %d-%d-%d %d:%d:%d\n", 
  //         tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
  //         tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
        
	desTime->wYear = tmDes.tm_year+1900;
	desTime->bMonth = tmDes.tm_mon+1;
	desTime->bDay = tmDes.tm_mday;
	desTime->bHour = tmDes.tm_hour;
	desTime->bMin = tmDes.tm_min;
	desTime->bSec = tmDes.tm_sec;

	return iInter;  
}

long NewCalcTime(const S_TIME *srcTime, S_TIME *desTime, long int iInter)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
    
    //ת�� SYSTIMEΪ tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    //ת�� tm Ϊ time_t
	ttmSrc = mktime(&tmSrc);
    //���� (time_t) ttmDes    
	ttmDes = (time_t)(ttmSrc + iInter);     
    //ת�� time_t Ϊ tm
	tmDes = *localtime(&ttmDes);
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@DesTime: %d-%d-%d %d:%d:%d\n", 
//           tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
//           tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
        
	desTime->wYear = tmDes.tm_year+1900;
	desTime->bMonth = tmDes.tm_mon+1;
	desTime->bDay = tmDes.tm_mday;
	desTime->bHour = tmDes.tm_hour;
	desTime->bMin = tmDes.tm_min;
	desTime->bSec = tmDes.tm_sec;

	return iInter;  
}


/*
    ==================================================
    Description:    �Ƚ�ʱ��
            
    Parameters:                             method of calling:(r/w/rw/other)
        srcTime:Դʱ��                                     r 
        desTime:Ŀ��ʱ��                                    r       
        
    return:
        0:  srcTime����desTime
        <0:  srcTime����desTime
        >0: srcTime����desTime
    ===================================================
*/
long CompTime(const S_TIME *srcTime, const S_TIME *desTime)
{
	sem_wait(&semCalctime);
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
	double fInter;      
    
    //ת�� SYSTIMEΪ tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    
	tmDes.tm_year = desTime->wYear -1900;
	tmDes.tm_mon = desTime->bMonth - 1;
	tmDes.tm_mday = desTime->bDay; 
	tmDes.tm_hour = desTime->bHour;
	tmDes.tm_min = desTime->bMin;
	tmDes.tm_sec = desTime->bSec;
//      if(B_PRINTF) printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
//           tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
    
    //ת�� tm Ϊ time_t
	ttmSrc = mktime(&tmSrc);
	ttmDes = mktime(&tmDes);
    
    //�Ƚ� (time_t)
	fInter = difftime(ttmSrc, ttmDes);
     // if(B_PRINTF) printf("%s%d  SrcTime -> DesTime   %f.\n", __FILE__, __LINE__, fInter);   
    	sem_post(&semCalctime);
	return (long)fInter;
}       
static long DateToMins(const S_TIME *sTime)
{
	const long mon[]={0,			
					 31*24*60L,	//Jun
					 (28+31)*24*60L,	//Feb
					 (28+31+31)*24*60L,	//Mar
					 (28+31+31+30)*24*60L,	//Apr
					 (28+31+31+30+31)*24*60L,	//
					 (28+31+31+30+31+30)*24*60L,	//Jue
					 (28+31+31+30+31+30+31)*24*60L,	//Jul
					 (28+31+31+30+31+30+31+31)*24*60L,	//Aug
					 (28+31+31+30+31+30+31+31+30)*24*60L,	//Seb
					 (28+31+31+30+31+30+31+31+30+31)*24*60L,	//Oct
					 (28+31+31+30+31+30+31+31+30+31+30)*24*60,	//Nov
					 (28+31+31+30+31+30+31+31+30+31+30+31)*24*60	//Des
					};
	const  long day[]={ 0, 
					1*24*60L,
					2*24*60L,
					3*24*60L, 
					4*24*60L,
					5*24*60L,
					6*24*60L,
					7*24*60L,
					8*24*60L,
					9*24*60L,
					10*24*60L,
					11*24*60L,
					12*24*60L,
					13*24*60L,
					14*24*60L,
					15*24*60L,
					16*24*60L, 
					17*24*60L,
					18*24*60L,
					19*24*60L,
					20*24*60L,
					21*24*60L,
					22*24*60L,
					23*24*60L,
					24*24*60L,
					25*24*60L,
					26*24*60L,
					27*24*60L,
					28*24*60L,
					29*24*60L,
					30*24*60L,
					31*24*60L };
					
		
	long year, mins;
	year=sTime->wYear-1968;
	mins = year*365*24*60+24*60;
	mins+=(year/4)*( 24*60); //for leap year
	if( !(year&3L) )
	{
		if( sTime->bMonth<=2 ){
			mins -= (24*60);
		}
	}
	mins += mon[sTime->bMonth - 1];
	mins += day[sTime->bDay - 1];
	mins += sTime->bHour*60;
	mins += sTime->bMin;
	return mins;
}
long NewCompTime(const S_TIME *srcTime, const S_TIME *desTime)
{
	double fInter,fInter1;     
	long time1,time2;

	time1=DateToMins(srcTime);
	time2=DateToMins(desTime);
	
	fInter1 = time1-time2;
	return (long)abs(fInter1);
}

/*
	�Է���Ϊ�ֱ��ʱȽ�ʱ��
*/
long CompTimeWithMin(const S_TIME *srcTime, const S_TIME *desTime)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
	double fInter,fInter1;     
	long time1,time2;
#if 1
	time1=DateToMins(srcTime);
	time2=DateToMins(desTime);
	
	fInter1 = time1-time2;
	return (long)fInter1;
#else	
 
    //ת�� SYSTIMEΪ tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = 0;

	tmDes.tm_year = desTime->wYear -1900;
	tmDes.tm_mon = desTime->bMonth - 1;
	tmDes.tm_mday = desTime->bDay; 
	tmDes.tm_hour = desTime->bHour;
	tmDes.tm_min = desTime->bMin;
	tmDes.tm_sec = 0;

    //ת�� tm Ϊ time_t
	ttmSrc = mktime(&tmSrc);
	ttmDes = mktime(&tmDes);
    
    //�Ƚ� (time_t)
	fInter = difftime(ttmSrc, ttmDes);
	//  if(B_PRINTF) printf("%s%d  SrcTime -> DesTime   %f ,,, %f. \n", __FILE__, __LINE__, fInter,fInter1);   

	return (long)fInter/60;
  #endif  
}

/*
	���ԱȽ�ʱ��ĺ���
*/
void TestCompTimeFunc()
{
	S_TIME sTime1, sTime2;
	int iInter;
	static int iSec = 1;
	long time1,time2;
	sTime1.wYear = 1976;
	sTime1.bMonth = 5;
	sTime1.bDay = 6;
	sTime1.bHour = 12;
	sTime1.bMin = 4;
	sTime1.bSec = 0;
	
	sTime2.wYear = 2013;
	sTime2.bMonth = 12;
	sTime2.bDay = 4;
	sTime2.bHour = 2;
	sTime2.bMin = 0;
	sTime2.bSec = 0;
	
	//iInter = CompTimeWithMin(&sTime1, &sTime2);
	time1=DateToMins(&sTime1);
	time2=DateToMins(&sTime2);
	iInter = time2-time1;
	
	if(B_PRINTF) printf("Time1:%.2d:%.2d:%.2d  Time2:%.2d:%.2d:%.2d  L1:%ld, L2:%ld,i=%d.\n", 
		   sTime1.bHour, sTime1.bMin, sTime1.bSec, sTime2.bHour, sTime2.bMin, sTime2.bSec, time1,time2,iInter);

	iInter = CompTimeWithMin(&sTime2, &sTime1);
	printf("iInter:%d\n",iInter);
	
	if(iSec>0) iSec --;
	else iSec = 59;
		
	return ;
}

/*
	��������ȡ��
	ʹ��atofת���ַ�����������ʱ�����,����atof("39.1") �����39.099998
	���Բ������°취ת��:
	FloatToInt((atof("39.1") * 10.0));
*/
int FloatToInt(double fValue)
{
	int res;
	double intergral;
	double fractional;
	
	//�ֽ�������С��
	fractional = modf(fValue, &intergral);
	//�ж�С����С
	if(fractional > 0.5)
		res = (int)fValue +1;
	else
		res = (int)fValue;
	
//  	if(B_PRINTF) printf("%s:%d intergral-->%f,  fractional-->%f.   convert %d.\n", __FILE__, __LINE__, intergral, fractional, res);
	return res;
}
/*
	�˳����жԻ��򣬻ص�������
*/
int ReturnMainView()
{
	gbKeyType = KEY_LR;
	BroadcastMessage(MSG_CLOSE,0,0);
	return 0;
}

/**
 * Bresenham Line Algorithm
 * @param iLineStyle �������� REALWIDELINE or Value>0
 * @param iLineWidth �����߿�
 * @param x1 
 * @param y1
 * @param x2
 * @param y2
 * @param bType �������� WIDELINE_DOT, WIDELINE_RECT, WIDELINE_CYCLE
 */
static void  bresenhamLine(HDC hdc, int iLineStyle, int iLineWidth, int x1, int y1, int x2, int y2, BYTE bType)
{
	int  x, y;
	int  dx, dy;
	int  incx, incy;
	int  balance;
	int i=0;
	int j=0;
	
	if (x2 >= x1){
		dx = x2 - x1;
		incx = 1;
	}
	else{
		dx = x1 - x2;
		incx = -1;
	}

	if (y2 >= y1){
		dy = y2 - y1;
		incy = 1;
	}
	else{
		dy = y1 - y2;
		incy = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy){
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (x != x2){
			if ( (i & iLineStyle)== REALWIDELINE ){
				switch(bType){
					case WIDELINE_RECT:{
						Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
					}break;
					case WIDELINE_CYCLE:{
						Circle(hdc, x, y,iLineWidth);
					}break;
					default:{
 						for(j=0; j<=iLineWidth; j++){
 							SetPixel(hdc, x, y+j,GetPenColor(hdc));
 						}	
					}break;
				}
			}
				
			if (balance >= 0){
				y += incy;
				balance -= dx;
			}
			balance += dy;
			x += incx;
			i ++;
		}
		if ( (i & iLineStyle)== REALWIDELINE ){
			switch(bType){
				case WIDELINE_RECT:{
					Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
				}break;
				case WIDELINE_CYCLE:{
					Circle(hdc, x, y,iLineWidth);
				}break;
				default:{
					for(j=0; j<=iLineWidth; j++){
						SetPixel(hdc, x, y+j,GetPenColor(hdc));
					}
				}break;
			}
		}
	}
	else{
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (y != y2){
			if ( (i & iLineStyle)== REALWIDELINE ){
				switch(bType){
					case WIDELINE_RECT:{
						Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
					}break;
					case WIDELINE_CYCLE:{
						Circle(hdc, x, y,iLineWidth);
					}break;
					default:{
						for(j=0; j<=iLineWidth; j++){
							SetPixel(hdc, x, y+j,GetPenColor(hdc));
						}
					}break;
				}
			}
			if (balance >= 0){
				x += incx;
				balance -= dy;
			}
			balance += dx;
			y += incy;
			i ++;
		}
		if ( (i & iLineStyle)== REALWIDELINE ){
			switch(bType){
				case WIDELINE_RECT:{
					Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
				}break;
				case WIDELINE_CYCLE:{
					Circle(hdc, x, y,iLineWidth);
				}break;
				default:{
					for(j=0; j<=iLineWidth; j++){
						SetPixel(hdc, x, y+j,GetPenColor(hdc));
					}
				}break;
			}
		}
	}
}

//���ƿ��߶�
int DrawWideLine(HDC hdc, int iLineStyle, int iLineWidth, int x1, int y1, int x2, int y2, BYTE bType)
{
 	bresenhamLine(hdc, iLineStyle, iLineWidth, x1, y1, x2, y2,bType);
 	return 0;
}

//���flash���ļ��Ƿ���ȷ
int CheckFlashFiles()
{
	//����Ƿ���������Դ�ļ�

	return 0;
}


//��������
extern CA_TRENDARRAY gTrendDatas;
//�����ھ��
extern HWND ghWndMain;

/*
	���������
*/
int SystemRestart()
{
	
	//�ѵ�ǰ���ƴ洢��Flash
	SaveTrendToFlash(&gTrendDatas);
//	PowerDialog(ghWndMain, NULL, 0, TRUE);
	system("reboot");
	return 0;
}


/*
*       -- ��������ϵ --
*       ViewOrg_X, ViewOrg_Y: ��ͼԭ��; ViewExt_X, ViewExt_Y:  ��ͼ��Χ;
*       WindOrg_X, WindOrg_Y: ��ͼԭ��; WindExt_X, WindExt_Y: ���ڷ�Χ
*/
void  ChangeZBX(HDC hdc, int ViewOrg_X, int ViewOrg_Y, int ViewExt_X, int ViewExt_Y, int WindOrg_X, int WindOrg_Y, int WindExt_X, int WindExt_Y)
{
        POINT Pt_ViewOrg;
        POINT Pt_ViewExt;
        POINT Pt_WindExt;
        POINT Pt_WindOrg;
        
        
        Pt_ViewOrg.x = ViewOrg_X;
        Pt_ViewOrg.y = ViewOrg_Y;
        Pt_ViewExt.x = ViewExt_X;
        Pt_ViewExt.y = ViewExt_Y;
        Pt_WindOrg.x = WindOrg_X;
        Pt_WindOrg.y = WindOrg_Y;
        Pt_WindExt.x = WindExt_X;
        Pt_WindExt.y = WindExt_Y;
        
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetViewportOrg(hdc, &Pt_ViewOrg);
        SetViewportExt(hdc, &Pt_ViewExt);
        SetWindowExt(hdc, &Pt_WindExt);
        SetWindowOrg(hdc, &Pt_WindOrg );
}
