#include "stm32f10x.h"                  // Device header
#include <Time.h>
#include "M_RTC.h"

uint16_t M_RTCNum[] = {2026,8,28,23,9,55}; 

void M_RTC_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1) !=0x6666)
	{
		RCC_LSEConfig(RCC_LSE_ON);	
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET);//等待时钟准备
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();//等待时钟同步
		
		RTC_SetPrescaler(32768-1);//预分频值
		RTC_WaitForLastTask();
		
		RTC_SetCounter(1672588795);//配置初始时间
		RTC_WaitForLastTask();
		
		M_RTC_SetTime();
		BKP_WriteBackupRegister(BKP_DR1,0x6666);
	}
	else
	{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();//等待时钟同步
	}
}

void M_RTC_SetTime()
{
	time_t Time_cnt;
	struct tm time_data;
	
	time_data.tm_year = M_RTCNum[0]-1900;//1900是偏移量
	time_data.tm_mon = M_RTCNum[1]-1;
	time_data.tm_mday = M_RTCNum[2];
	time_data.tm_hour = M_RTCNum[3];
	time_data.tm_min = M_RTCNum[4];
	time_data.tm_sec = M_RTCNum[5];
	
	Time_cnt =mktime(&time_data)-8*60*60;
	
	RTC_SetCounter(Time_cnt);
	RTC_WaitForLastTask();
}

void M_RTC_ReadTime()
{
	time_t time_cnt;
	struct tm time_data;
	
	time_cnt = RTC_GetCounter()+8*60*60;
	
	time_data = *localtime(&time_cnt);
	
	M_RTCNum[0]  =time_data.tm_year +1900 ;//1900是偏移量
	M_RTCNum[1]=time_data.tm_mon  +1;
	M_RTCNum[2]=time_data.tm_mday ;
	M_RTCNum[3] =time_data.tm_hour  ;
	M_RTCNum[4] =time_data.tm_min  ;
	M_RTCNum[5]= time_data.tm_sec  ;
}
