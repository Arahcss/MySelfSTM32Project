#ifndef _LED_DEVICE_H_
#define _LED_DEVICE_H_

#include "stm32f1xx_hal.h"

#define LED_BOARD			0
#define LED1						1
#define LED2						2
#define LED3						3
#define LED4						4
#define LED5						5
#define LED6						6
#define LED7						7
#define LED8						8



///	@brief			结构参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef				*pstGpioBase;		//	使用的GPIOx
	uint16_t						usGpioPin;			//	使用的GPIO_PIN_x
	uint8_t						ucOnLevel;			//	LED点亮时的电频
}
stLedDeviceParamTdf;

void vLedOn(uint8_t ucDevNum);
void vLedOFF(uint8_t ucDevNum);
void vLedDeviceInit(stLedDeviceParamTdf *pstInit,uint8_t ucDevNum);

#endif
