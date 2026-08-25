#include "stm32f10x.h"                  // Device header
#include "Game.h"
uint8_t CheckNum=0;

void CheckButton_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//启用中断时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;//配置中断线为14线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//配置中断的状态为开启
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//配置中断的模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//配置中断的触发为下降沿,传感器接收信号后先高，结束信号接收后变低下降触发   
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置中断优先分组模式为2
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//配置中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//配置中断通道开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//配置响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t CheckButton_Get(void)
{
	return CheckNum;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)==SET)
	{
		CheckNum=1;
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
}
