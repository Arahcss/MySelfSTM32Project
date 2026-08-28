#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "K.h"
#include "Delay.h"
#include "AD.h"
#include "CountS.h"
#include "PWM.h"
#include "MPU6050.h"
#include "W25Q64.h"
#include "Serial.h"
#include "stdio.h"
#include "M_RTC.h"

//USARTSend
uint8_t Serial_TxFlag;

//W25Q64
uint8_t MID;
uint16_t DID;
uint8_t ArrayWrite[]={0x01,0x01,0x04,0x05};
uint8_t ArrayRead[4];

//MPU6050
int16_t AX,AY,AZ,GX,GY,GZ;

//Menu
uint8_t Line=1;

void Menu_TimeStart()
{
	M_RTC_ReadTime();
	OLED_ShowNum(1,6,M_RTCNum[0],4);
	OLED_ShowNum(1,11,M_RTCNum[1],2);
	OLED_ShowNum(1,14,M_RTCNum[2],2);
	OLED_ShowNum(2,6,M_RTCNum[3],2);
	OLED_ShowNum(2,9,M_RTCNum[4],2);
	OLED_ShowNum(2,12,M_RTCNum[5],2);
	OLED_ShowNum(3,6,RTC_GetCounter(),10);
}

void Menu_TimeRemind()
{
	OLED_ShowString(1,1,"Date:XXXX-XX-XX");
	OLED_ShowString(2,1,"Time:XX:XX:XX");
	OLED_ShowString(3,1,"CNT:");
}

void Menu_USARTSendRemind()
{
	OLED_ShowString(1,1,"TxPacket");
	OLED_ShowString(3,1,"RxPacket");
}

void Menu_USARTSend()
{
	if(Serial_TxFlag==1)
	{
		OLED_ShowString(2,1,"                  ");
		OLED_ShowString(2,1,"InputDataOver");
		Serial_SendString("Input test\r\n");
		Serial_TxFlag=0;
	}
	 if(Serial_RxFlag==1)
	{
		OLED_ShowString(4,1,"                ");//用于擦除
		OLED_ShowString(4,1,Serial_RxPacket);
		Serial_RxFlag=0;
	}	
}

void Menu_SPIFlashStart()
{
	W25Q64_SectorErase(0x000000);//最好对齐6位
	W25Q64_PageProgram(0x000000,ArrayWrite,4);
	
	W25Q64_ReadData(0x000000,ArrayRead,4);
	
	for(int i=0;i<4;i++)
		OLED_ShowHexNum(2,(i+1)*3,ArrayWrite[i],2);
	
	for(int i=0;i<4;i++)
		OLED_ShowHexNum(3,(i+1)*3,ArrayRead[i],2);
}

void Menu_SPIFlashRemind()
{
	OLED_ShowString(1,1,"MID:   DID:");
	OLED_ShowString(2,1,"W:");
	OLED_ShowString(3,1,"R:");
	W25Q64_ReadID(&MID,&DID);
	OLED_ShowHexNum(1,5,MID,2);
	OLED_ShowHexNum(1,12,DID,4);
}

void Menu_MPU6050Remind()
{
	OLED_ShowString(1,1,"    A       G");
	OLED_ShowString(2,1,"X       X");
	OLED_ShowString(3,1,"Y       Y");
	OLED_ShowString(4,1,"Z       Z");
}

void Menu_MPU6050Start()
{
		MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
		OLED_ShowSignedNum(2,2,AX,5);
		OLED_ShowSignedNum(3,2,AY,5);
		OLED_ShowSignedNum(4,2,AZ,5);
		OLED_ShowSignedNum(2,10,GX,5);
		OLED_ShowSignedNum(3,10,GY,5);
		OLED_ShowSignedNum(4,10,GZ,5);
		Delay_ms(50);
}

void Menu_AllInit()
{
	OLED_Init();
	K_Init();
	AD1_Init();
	CountS_Init();
	PWM_Init();
	MPU6050_Init();
	W25Q64_Init();
	Serial_Init();
	M_RTC_Init();
	
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
		case 3: Menu_MPU6050Start();break;
		case 4: Menu_SPIFlashStart();break;
		case 5: Menu_USARTSend();break;
		case 6: Menu_TimeStart();break;
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
		case 3:Menu_MPU6050Remind();break;
		case 4:Menu_SPIFlashRemind();break;
		case 5:Menu_USARTSendRemind();break;
		case 6:Menu_TimeRemind();break;
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
	uint16_t BreakTime=10000;
	while(BreakTime--)
	{
		uint8_t KNum=Key_Scan();
		Delay_ms(5);
		if(KNum==1)
		{
			Menu_ExitFlag =1;
			KeyAction=0;
			Line=(Line)%6 +1;	
			Menu_ShowLine();
			Menu_ShowCursor();
			Delay_ms(80);
		}
		else if(KNum==2)
		{
			Delay_ms(150);//防抖
			Serial_TxFlag=1;
			Menu_ExitFlag =1;
			KeyAction=0;
			OLED_Clear();//用前清屏
			Menu_ShowTest();//显示提示文字
			while(BreakTime)
			{
				Menu_Enter();
				KNum=Key_Scan();
				Delay_ms(5);
				if(Menu_ExitFlag ==0)
				{
					KeyAction=0;
					Menu_ExitFlag =1;
					break;
				}
			}
			
		}
	}
	
}


