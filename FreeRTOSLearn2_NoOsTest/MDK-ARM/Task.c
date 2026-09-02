#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"

extern uint8_t ucButtonFlag;
extern uint8_t ucUartFlag;
extern uint8_t ucAdcFlag;

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
	printf("uart time:%d \n\r",HAL_GetTick());
}

void vAdcTask()
{
	HAL_Delay(200);
	printf("adc time:%d \n\r",HAL_GetTick());
} 

//void vTask()//测试
//{
//	vButtonTask();
//	vUartTask();
//	vAdcTask();
//}

void vTask()
{
	if(ucButtonFlag == 1)
	{
		ucButtonFlag = 0;
		vButtonTask();
	}
	if(ucUartFlag == 1)
	{
		ucUartFlag = 0;
		vUartTask();
	}
	if(ucAdcFlag == 1)
	{
		ucAdcFlag = 0;
		vAdcTask();
	}
	
}

int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
}
