#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "freertos.h"
#include "task.h"

extern UART_HandleTypeDef huart1;

void vTask1()
{
	//同优先级情况下为切片式调度，每个分1ms
	//抢占式调度情况下高优先级会打断低优先级任务
	uint32_t ulDelayCount;
	ulDelayCount = HAL_GetTick();
	printf("abcdef");
	vTaskDelayUntil(&ulDelayCount,1);
	portYIELD();//告诉调度器切换任务
}

void vTask2()
{
	printf("ABCDEFGHIJKLMNOPQ");
	vTaskDelay(1);
}

void vTask3()
{
	printf("+");
}

int fputc(int ch,FILE *f)
{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);//因为HAL库自动的USART是阻塞式发送所以得自己写一个
	while((huart1.Instance -> SR & USART_SR_TXE)==0);
	
	huart1.Instance -> DR = *(uint8_t*)&ch;
	return ch;
}
