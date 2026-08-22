#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "K.h"
#include "Delay.h"
#include "AD.h"
#include "CountS.h"

uint8_t Line=1;

void Menu_AllInit()
{
	OLED_Init();
	K_Init();
	AD1_Init();
	CountS_Init();
}

void Menu_ADCStart()
{
	OLED_ShowNum(1,1,AD1_GetValue(ADC_Channel_0),4);
	OLED_ShowNum(2,1,AD1_GetValue(ADC_Channel_1),4);
	OLED_ShowNum(3,1,AD1_GetValue(ADC_Channel_2),4);
	OLED_ShowNum(4,1,AD1_GetValue(ADC_Channel_3),4);
	Delay_ms(100);
}

void Menu_Enter()
{
	switch(Line)
	{
		case 1: Menu_ADCStart();break;
		
	}
}

void Menu_ShowCursor()
{
	for(uint8_t i=0;i<4;i++)
	{
		OLED_ShowString(i+1,1," ");
	}
	OLED_ShowString((Line>=4) ? 4: Line,1,">");
}

void Menu_ShowLine()
{
	uint8_t LineNum=1,nextLine= ((Line)/4>=1)?(Line%4):0;
	OLED_Clear();//擦除
	for(uint8_t i =0;i<4;i++)
	{
		switch(i+nextLine)
		{
			case 0:OLED_ShowString(LineNum,2,"ADCacquisition"),LineNum++;break;
			case 1:OLED_ShowString(LineNum,2,"PWMControl"),LineNum++;break;
			case 2:OLED_ShowString(LineNum,2,"MPU6050Date"),LineNum++;break;
			case 3:OLED_ShowString(LineNum,2,"SPIFlash"),LineNum++;break;
			case 4:OLED_ShowString(LineNum,2,"USARTSend"),LineNum++;break;
			case 5:OLED_ShowString(LineNum,2,"TIME"),LineNum++;break;
		}
	}
}

void Menu_Start()
{
	Menu_ShowLine();
	Menu_ShowCursor();
	while(1)
	{
		uint8_t KNum=Key_Scan();
		Delay_ms(5);
		if(KNum==1)
		{
			CountS_Count=1;
			KeyAction=0;
			Line=(Line)%6 +1;	
			Menu_ShowLine();
			Menu_ShowCursor();
			Delay_ms(80);
		}
		else if(KNum==2)
		{
			Delay_ms(100);//防抖
			CountS_Count=1;
			KeyAction=0;
			OLED_Clear();
			while(1)
			{
				Menu_Enter();
				KNum=Key_Scan();
				Delay_ms(5);
				if(CountS_Count==0)
				{
					KeyAction=0;
					CountS_Count=1;
					break;
				}
					
			}
		}
	}
	
}


