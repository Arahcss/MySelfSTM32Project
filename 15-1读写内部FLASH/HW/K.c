#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void K_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t K_GetNum(void)
{
	uint8_t KNum=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
	{
		
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
		Delay_ms(20);
		KNum=1;
	}
	
	return KNum;
}

uint8_t K_Delay(uint32_t xms)
{
	uint8_t KNum=0; 
	while(xms--)
	{
		if(K_GetNum()==1)
		{
			KNum++;
		}
		Delay_us(1000);
	}
	return KNum;
}
