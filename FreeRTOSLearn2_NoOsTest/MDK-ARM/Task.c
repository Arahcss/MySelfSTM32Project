#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

void vButtonTask()
{
	HAL_Delay(20);
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
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
	printf("uart time:%d \n\r",HAL_GetTick());
}

void vAdcTask()
{
	HAL_Delay(200);
	printf("adc time:%d \n\r",HAL_GetTick());
}

void vTask()
{
	vButtonTask();
	vUartTask();
	vAdcTask();
}

int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
}
