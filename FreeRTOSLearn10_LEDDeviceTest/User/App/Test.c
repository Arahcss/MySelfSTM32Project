#include "LedDevice.h"
#include "string.h"
#include "Test.h"

void vLedInit()
{
	stLedDeviceParamTdf stInit;
	
	stInit.pstGpioBase	= GPIOC;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_13;
	vLedDeviceInit(&stInit,LED_BOARD);
	
	stInit.pstGpioBase	= GPIOB;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_1;
	vLedDeviceInit(&stInit,LED1);
	
	stInit.pstGpioBase	= GPIOB;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_0;
	vLedDeviceInit(&stInit,LED2);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_2;
	vLedDeviceInit(&stInit,LED3);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_3;
	vLedDeviceInit(&stInit,LED4);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_4;
	vLedDeviceInit(&stInit,LED5);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_5;
	vLedDeviceInit(&stInit,LED6);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_6;
	vLedDeviceInit(&stInit,LED7);
	
	stInit.pstGpioBase	= GPIOA;
	stInit.ucOnLevel		= 0;
	stInit.usGpioPin 		= GPIO_PIN_7;
	vLedDeviceInit(&stInit,LED8);
	
	
}

///	@brief				测试执行
///
///	@note				实现测试功能
void vTaskExecute()
{
	uint8_t i;
	for(i=0;i<9;i++)
	{
		vLedOn(i);
		HAL_Delay(500);
		
		vLedOFF(i);
		HAL_Delay(500);
	}
	
	
}

