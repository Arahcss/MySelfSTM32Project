#include "LedDevice.h"
#include "string.h"
#include "Test.h"
#include "DHT11Device.h"

void vLedInit(void)
{
	stLedStaticParamTdf 		stStaticInit;
	stLedRunningParamTdf	stRunningInit;
	
	stRunningInit.emMode						= emLedMode_Breath;
	stRunningInit.ulCurrentCount			= 0;
	stRunningInit.ulOnCountThreshold	= 100;
	stRunningInit.ulOffCountThreshold	=	100;
	stRunningInit.ulBreathPeriod				= 10000;
	vLedDeviceRunningParamInit(&stRunningInit,LED_BOARD);
	
	stStaticInit.pstGpioBase	= GPIOC;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_13;
	vLedDeviceInit(&stStaticInit,LED_BOARD);
	
	stStaticInit.pstGpioBase	= GPIOB;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_1;
	vLedDeviceInit(&stStaticInit,LED1);
	
	stStaticInit.pstGpioBase	= GPIOB;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_0;
	vLedDeviceInit(&stStaticInit,LED2);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_2;
	vLedDeviceInit(&stStaticInit,LED3);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_3;
	vLedDeviceInit(&stStaticInit,LED4);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_4;
	vLedDeviceInit(&stStaticInit,LED5);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_5;
	vLedDeviceInit(&stStaticInit,LED6);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_6;
	vLedDeviceInit(&stStaticInit,LED7);
	
	stStaticInit.pstGpioBase	= GPIOA;
	stStaticInit.emOnLevel		= emLedOnLevel_Low;
	stStaticInit.usGpioPin 		= GPIO_PIN_7;
	vLedDeviceInit(&stStaticInit,LED8);
	
}

void vDht11Init(void)
{
	stDht11StaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOB;
	stInit.usGpioPin			=	GPIO_PIN_12;
	stInit.ulTimerPeriorUs = 5;
	vDht11DeviceInit(&stInit,DHT11);
}

///	@brief				测试执行
///
///	@note				实现测试功能
void vTaskExecute(void)
{
	vLedOFF(0);//板载LED好闪眼睛。。。
//	uint8_t i;
//	for(i=1;i<9;i++)
//	{
//		vLedOn(i);
//		HAL_Delay(100);
//		
//		vLedOFF(i);
//		HAL_Delay(100);
//	}
	
//	for(i = 1 ;i< 9 ;i++)
//	{
//		vLedToggle((emLedDevNumTdf)i);
//		HAL_Delay(100);
//	}
	
//	vLedDevicePeriodExecute(LED_BOARD);
//	HAL_Delay(0);
	
//	ucDht11ReadData(DHT11);
//	HAL_Delay(2000);
}

