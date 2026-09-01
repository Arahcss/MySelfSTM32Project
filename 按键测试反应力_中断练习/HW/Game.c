#include "stm32f10x.h"                  // Device header
#include "CheckButton.h"
#include "Delay.h"
#include "OLED.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
uint16_t PastTime = 0;
void GameStrat(void)
{
	uint16_t TotalTime=0;//计数总时间
	for(uint8_t PerGame=0;PerGame<6;PerGame++)
	{
		OLED_ShowString(1,3,"StartTheGame");
		OLED_ShowString(3,1,"PastTime");
		if(CheckButton_Get()==1)
		{
			uint16_t i=0;//计时值重置
			uint8_t RandNum=rand()%2000;
			OLED_ShowString(1,1,"   Saw@Pressing");
			CheckNum=0;
			Delay_ms(RandNum+2000);
			
			//提示按键
			OLED_ShowString(1,1,"@@@@@@@@@@@@@@@");
			OLED_ShowString(2,1,"@@@@@@@@@@@@@@@");
			
			for(i =0;;i++)
			{
				if(CheckButton_Get()==1)
				{
					PastTime=i;
					TotalTime+=PastTime;
					//擦除提示
					OLED_ShowString(1,1,"                ");
					OLED_ShowString(2,1,"                ");
					break;
				}
				else
				{
					Delay_ms(1);
				}
			}
			
		}		
		OLED_ShowNum(4,1,PastTime,5);
		Delay_ms(1000);
	}
	CheckNum=0;
	OLED_ShowString(3,1,"AverageTime");
	OLED_ShowNum(4,1,TotalTime/5,5);
	Delay_ms(2000);
	OLED_ShowString(3,1,"           ");//擦除“AverageTime”
}
