#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "K.h"
#include "Delay.h"

uint8_t Line=1;

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
	K_Init();
	Menu_ShowLine();
	Menu_ShowCursor();
	while(1)
	{
		uint8_t KNum=Key_Scan();
		Delay_ms(10);
		if(KNum==1)
		{
			KeyAction=0;
			Line=(Line)%6 +1;	
			Menu_ShowCursor();
			Menu_ShowLine();
		}
	}
	
}


