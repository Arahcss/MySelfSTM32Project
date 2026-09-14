#ifndef _LED_DEVICE_H_
#define _LED_DEVICE_H_

#include "stm32f1xx_hal.h"

typedef enum
{
	emLedOnLevel_Low			=	0,
	emLedOnLevel_High,
}
emLedOnLevelTdf;

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emLedDevNum0				=	0,
	emLedDevNum1,
	emLedDevNum2,
	emLedDevNum3,
	emLedDevNum4,
	emLedDevNum5,
	emLedDevNum6,
	emLedDevNum7,
	emLedDevNum8,
}emLedDevNumTdf;

#define LED_DEV_NUM		9

#define LED_BOARD			emLedDevNum0
#define LED1						emLedDevNum1
#define LED2						emLedDevNum2
#define LED3						emLedDevNum3
#define LED4						emLedDevNum4
#define LED5						emLedDevNum5
#define LED6						emLedDevNum6
#define LED7						emLedDevNum7
#define LED8						emLedDevNum8



///	@brief			结构参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef								*pstGpioBase;		//	使用的GPIOx
	uint16_t										usGpioPin;			//	使用的GPIO_PIN_x
	emLedOnLevelTdf						emOnLevel;			//	LED点亮时的电频
}
stLedDeviceParamTdf;

void vLedOn(emLedDevNumTdf emDevNum); 
void vLedOFF(emLedDevNumTdf emDevNum);
void vLedDeviceInit(stLedDeviceParamTdf *pstInit,emLedDevNumTdf emDevNum);

#endif
