#include "stm32f10x.h"                  // Device header
#include "OLED.h"
uint8_t Line=0;

void Menu_ShowLine(uint8_t FirstLine,uint8_t EndLine)
{
	uint8_t LineNum=1;
	for(uint8_t i =FirstLine;i<EndLine;i++)
	{
		switch(i)
		{
			case 0:OLED_ShowString(LineNum,2,""),LineNum++;break;
			case 1:OLED_ShowString(LineNum,2,""),LineNum++;break;
			case 2:OLED_ShowString(LineNum,2,""),LineNum++;break;
			case 3:OLED_ShowString(LineNum,2,""),LineNum++;break;
			case 4:OLED_ShowString(LineNum,2,""),LineNum++;break;
			case 5:OLED_ShowString(LineNum,2,""),LineNum++;break;
		}
	}
}

void Menu_Start()
{
	//1 2 3 4 5 6
	//
	//
	for(uint8_t i=0;i<6;i++)
	{
		
	}
}


