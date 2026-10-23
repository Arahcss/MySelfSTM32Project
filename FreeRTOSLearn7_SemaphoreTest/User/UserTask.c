#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdio.h"
#include "freertos.h"
#include "task.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "semphr.h"


extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t ButtonQueueHandle;
extern osMessageQueueId_t TimerQueueHandle;
extern osMessageQueueId_t PrintQueueHandle;
extern osSemaphoreId_t myBinarySem01Handle;
extern osSemaphoreId_t myCountingSem01Handle;

static const uint8_t sc_aucButtonStringUp[] = 				"[Up]Pussing\n\r";
static const uint8_t sc_aucButtonStringDown[] = 		"[Down]Pussing\n\r";
static const uint8_t sc_aucButtonStringEnter[] = 			"[Enter]Pussing\n\r";
static const uint8_t sc_aucButtonStringCancel[] = 		"[Cancel]Pussing\n\r";

void vButtonScan()
{
	uint8_t ucButtonValueCurrent;			//当前按键值
	static uint8_t s_ucButtonValueLast;	//上次按键值
	const uint8_t* c_pucSendString;
	
	ucButtonValueCurrent = 0;
	
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == 0)
	{
		ucButtonValueCurrent |= 0x01;
		if((s_ucButtonValueLast & 0x01) == 0)
		{
			c_pucSendString = sc_aucButtonStringUp;
			xQueueSend(PrintQueueHandle,&c_pucSendString,0);
		}
	}
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0)
	{
		ucButtonValueCurrent |= 0x02;
		if((s_ucButtonValueLast & 0x02) == 0)
		{
			c_pucSendString = sc_aucButtonStringDown;
			xQueueSend(PrintQueueHandle,&c_pucSendString,0);
		}
	}
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15) == 0)
	{
		ucButtonValueCurrent |= 0x04;
		if((s_ucButtonValueLast & 0x04) == 0)
		{
			c_pucSendString = sc_aucButtonStringEnter;
			xQueueSend(PrintQueueHandle,&c_pucSendString,0);
		}
	}
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14) == 0)
	{
		ucButtonValueCurrent |= 0x08;
		if((s_ucButtonValueLast & 0x08) == 0)
		{
			c_pucSendString = sc_aucButtonStringCancel;
			xQueueSend(PrintQueueHandle,&c_pucSendString,0);
		}
	}
	
	if(ucButtonValueCurrent != s_ucButtonValueLast)
	{
		xQueueSend(ButtonQueueHandle,&ucButtonValueCurrent,0);//入队
	}
	
	//轮询按键情况
	uint8_t ucMask = 0x01;
	for(uint8_t i=0 ; i<4;i++)
	{
		if(((ucButtonValueCurrent & ucMask) == 0) && ((s_ucButtonValueLast & ucMask) != 0))
		{
			xSemaphoreGive(myCountingSem01Handle);
		}
		else if(((s_ucButtonValueLast & ucMask) == 0) && ((ucButtonValueCurrent& ucMask) != 0 ))
		{
			xSemaphoreTake(myCountingSem01Handle,0);
		}
		ucMask <<= 1;
	}
	
	//更新上次值
	s_ucButtonValueLast = ucButtonValueCurrent;
	
	vTaskDelay(20);//消抖
	
}

void vPrintButtionString()
{
	uint8_t* p;
	
	xQueueReceive(PrintQueueHandle,&p,portMAX_DELAY);

	printf("%s",p);
	vTaskDelay(1000);
}

void vLedExcute()
{
	uint8_t ucButtonValue;
	uint16_t ucTimerValue;
	
//	//接收到按键的值就执行点亮对于LED
//	if(xQueueReceive(ButtonQueueHandle,&ucButtonValue,0))
//	{
//		if((ucButtonValue & 0x01) == 0)
//		{
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
//		}
//		if((ucButtonValue & 0x02) == 0)
//		{
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
//		}
//		if((ucButtonValue & 0x04) == 0)
//		{
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
//		}
//		if((ucButtonValue & 0x08) == 0)
//		{
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
//		}
//	}

	//获取计数信号量点亮和熄灭对于数量的LED
	switch(uxSemaphoreGetCount(myCountingSem01Handle))
	{
		case 0:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			break;
		}
		case 1:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			break;
		}
		case 2:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			break;
		}
		case 3:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			break;
		}
		case 4:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
			break;
		}
		default:
		{
			;
		}
		
	}

	//接收到定时器的值后翻转LED
	//if(xQueueReceive(TimerQueueHandle,&ucTimerValue,0))
	if(xSemaphoreTake(myBinarySem01Handle,0) )
	{
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
	}
	
	vTaskDelay(10);
}

int fputc(int ch,FILE *f)
{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);//因为HAL库自动的USART是阻塞式发送所以得自己写一个
	while((huart1.Instance -> SR & USART_SR_TXE)==0);
	
	huart1.Instance -> DR = *(uint8_t*)&ch;
	return ch;
}
