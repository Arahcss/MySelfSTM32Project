//#include "stm32f1xx_hal.h"
//#include "stdint.h"
//#include "stdio.h"
//#include "freertos.h"
//#include "task.h"
//#include "cmsis_os2.h"
//#include "queue.h"
//#include "semphr.h"

////extern UART_HandleTypeDef huart1;
//extern osSemaphoreId_t myBinarySem01Handle;//二值信号量
//extern osMutexId_t myMutex01Handle;//互斥信号量
//void HighExecute()
//{
//	printf("High0\n\r");
//	
//	//用互斥锁可以有效提高效率并防止在High级等待Low级信号量时先过掉Mid级同时防止过掉正在运行的High级
//	//xSemaphoreTake(myBinarySem01Handle,portMAX_DELAY);
//	xSemaphoreTake(myMutex01Handle,portMAX_DELAY);//用互斥锁替换掉
//	printf("High1\n\r");
//	
//	HAL_Delay(500);
//	
//	printf("High2\n\r");
//	//xSemaphoreGive(myBinarySem01Handle);
//	xSemaphoreGive(myMutex01Handle);
//	
//	vTaskDelay(1000);
//}

//void MidExecute()
//{
//	printf("Mid\n\r");
//	vTaskDelay(1000);
//}

//void LowExecute()
//{
//	printf("Low0\n\r");
//	
//	//xSemaphoreTake(myBinarySem01Handle,portMAX_DELAY);
//	xSemaphoreTake(myMutex01Handle,portMAX_DELAY);
//	printf("Low1\n\r");
//	
//	HAL_Delay(5000);
//	
//	printf("Low2\n\r");
//	//xSemaphoreGive(myBinarySem01Handle);
//	xSemaphoreGive(myMutex01Handle);
//	
//}

//int fputc(int ch,FILE *f)
//{
////	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);//因为HAL库自动的USART是阻塞式发送所以得自己写一个
//	while((huart1.Instance -> SR & USART_SR_TXE)==0);
//	
//	huart1.Instance -> DR = *(uint8_t*)&ch;
//	return ch;
//}
