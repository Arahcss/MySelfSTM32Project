#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "freertos.h"
#include "task.h"
#include "cmsis_os2.h"
#include "queue.h"

extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t ButtonQueueHandle;

void vButtonScan()
{
	uint8_t ucButtonValueCurrent;			//当前按键值
	static uint8_t s_ucButtonValueLast;	//上次按键值
	
	ucButtonValueCurrent = 0;
	
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == 0)
	{
		ucButtonValueCurrent |= 0x01;
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0)
	{
		ucButtonValueCurrent |= 0x02;
	}
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15) == 0)
	{
		ucButtonValueCurrent |= 0x04;
	}
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14) == 0)
	{
		ucButtonValueCurrent |= 0x08;
	}
	
	if(ucButtonValueCurrent != s_ucButtonValueLast)
	{
		xQueueSend(ButtonQueueHandle,&ucButtonValueCurrent,0);//入队
	}
	
	s_ucButtonValueLast = ucButtonValueCurrent;
	
	vTaskDelay(20);//消抖
	
}

void vLedExcute()
{
	uint8_t ucButtonValue;
	
	if((ucButtonValue & 0x01) == 0)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	if((ucButtonValue & 0x02) == 0)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
	}
	if((ucButtonValue & 0x04) == 0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
	}
	if((ucButtonValue & 0x08) == 0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
	}
	
	xQueueReceive(ButtonQueueHandle,&ucButtonValue,portMAX_DELAY);
}

int fputc(int ch,FILE *f)
{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);//因为HAL库自动的USART是阻塞式发送所以得自己写一个
	while((huart1.Instance -> SR & USART_SR_TXE)==0);
	
	huart1.Instance -> DR = *(uint8_t*)&ch;
	return ch;
}
