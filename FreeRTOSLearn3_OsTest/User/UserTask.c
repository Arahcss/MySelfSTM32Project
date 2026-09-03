#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "freertos.h"
#include "task.h"

extern UART_HandleTypeDef huart1;

void vButtonTask()
{
	HAL_Delay(20);
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==0)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
	}
}

void vUartTask()
{
	uint32_t ulBeginTime;
	ulBeginTime = HAL_GetTick();
	
	printf("uart time:%d \n\r",HAL_GetTick());
	vTaskDelayUntil(&ulBeginTime,1000);//准时延时函数
}

void vAdcTask()
{
	uint32_t ulBeginTime;
	ulBeginTime = HAL_GetTick();
	
	printf("adc begin time:%d \n\r",HAL_GetTick());
	HAL_Delay(200);
	printf("adc end time:%d \n\r",HAL_GetTick());
	vTaskDelayUntil(&ulBeginTime,500);
} 


int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
}
