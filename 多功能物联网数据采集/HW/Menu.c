#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "K.h"
#include "Delay.h"
#include "AD.h"
#include "CountS.h"
#include "PWM.h"

uint8_t Line=1;

void Menu_AllInit()
{
	OLED_Init();
	K_Init();
	AD1_Init();
	CountS_Init();
	PWM_Init();
}

void Menu_ADCStart()
{
	OLED_ShowNum(1,12,AD1_GetValue(ADC_Channel_0),4);
	OLED_ShowNum(2,12,AD1_GetValue(ADC_Channel_1),4);
	OLED_ShowNum(3,12,AD1_GetValue(ADC_Channel_2),4);
	OLED_ShowNum(4,12,AD1_GetValue(ADC_Channel_3),4);
	Delay_ms(100);
}

void Menu_ADCRemind()
{
	OLED_ShowString(1,1,"ReflectMod:");
	OLED_ShowString(2,1,"ThermalMod:");
	OLED_ShowString(3,1,"MonitorMod:");
	OLED_ShowString(4,1,"AdjustMod :");
}

void Menu_PWMRemind()
{
	OLED_ShowString(1,1,"ServoStarted");
}

void Menu_PWMContolStart()
{
	for(uint8_t i=0;i<=180;i+=30)
		{
			PWM_SetCompare1(i);
			Delay_ms(80);
		}
}

void Menu_Enter()
{
	switch(Line)
	{
		case 1: Menu_ADCStart();break;
		case 2: Menu_PWMContolStart();break;
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

void Menu_ShowTest()
{
	switch(Line)
	{
		case 1:Menu_ADCRemind();break;
		case 2:Menu_PWMRemind();break;
	}
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
			Delay_ms(150);//防抖
			CountS_Count=1;
			KeyAction=0;
			OLED_Clear();//用前清屏
			Menu_ShowTest();//显示提示文字
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


