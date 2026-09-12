#ifndef _LED_DEVICE_H_
#define _LED_DEVICE_H_

#include "stm32f1xx_hal.h"

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


#endif
