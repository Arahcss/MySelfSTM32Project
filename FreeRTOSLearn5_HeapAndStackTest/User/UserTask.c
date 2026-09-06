#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "freertos.h"
#include "task.h"

extern UART_HandleTypeDef huart1;


//尝试撑爆堆栈
void func2()
{
	uint8_t sz[50*4] =  {0};//记得赋个0，不然容易当成无效数组
}

void func1()
{
	uint8_t sz[50*4] =  {0};//记得赋个0，不然容易当成无效数组
	func2();
}

void vPrintTaskInfor(uint8_t *PucName)
{
	TaskHandle_t  TaskHandle;
	TaskStatus_t TaskStatus;
	
	uint8_t aucTaskName[16];
	
	TaskHandle =xTaskGetHandle((const char *)PucName);
	
	vTaskGetInfo(	(TaskHandle_t)TaskHandle,	//任务句柄
							(TaskStatus_t*)&TaskStatus,	//任务信息
							(BaseType_t)pdTRUE,				//允许统计任务堆栈历史最小剩余大小
							(eTaskState)eInvalid);				//函数自己获取任务运行状态

	printf("Task Name: 							%s\r\n", TaskStatus.pcTaskName);
	printf("Task Number: 						%d\r\n", (int)TaskStatus.xTaskNumber);
	printf("Task State: 							%d\r\n", TaskStatus.eCurrentState);
	printf("Current Priority:					%d\r\n", (int)TaskStatus.uxCurrentPriority);
	printf("Base Priority: 						%d\r\n", (int)TaskStatus.uxBasePriority);
	printf("Stack Base Address: 				%#x\r\n", (int)TaskStatus.pxStackBase);
	printf("Stack High Water Mark: 	%d\r\n", TaskStatus.usStackHighWaterMark);
	printf("\r\n");
}

int fputc(int ch,FILE *f)
{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);//因为HAL库自动的USART是阻塞式发送所以得自己写一个
	while((huart1.Instance -> SR & USART_SR_TXE)==0);
	
	huart1.Instance -> DR = *(uint8_t*)&ch;
	return ch;
}
