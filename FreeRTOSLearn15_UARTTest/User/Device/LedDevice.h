#ifndef _LED_DEVICE_H_
#define _LED_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"

#define	PI	3.141592653

///	@brief					LED模式枚举
///
///	@note
typedef enum
{
	emLedMode_Static			=	0,
	emLedMode_Blink,
	emLedMode_Breath,
}
emLedModeTdf;

///	@brief					LED状态枚举
///
///	@note
typedef enum
{
	emLedStatus_OFF			=	0,
	emLedStatus_ON,
}
emLedStatusTdf;

///	@brief					LED ON时的电平枚举
///
///	@note
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

///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef								*pstGpioBase;		//	使用的GPIOx
	uint16_t										usGpioPin;			//	使用的GPIO_PIN_x
	emLedOnLevelTdf						emOnLevel;			//	LED点亮时的电频
}
stLedStaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	emLedStatusTdf					emCurrentStatus;			//	LED当前状态
	emLedModeTdf					emMode;							//当前模式
	
	uint32_t								ulCurrentCount;				//当前计数
	uint32_t								ulOnCountThreshold;		//ON	计数阈值
	uint32_t								ulOffCountThreshold;		//OFF	计数阈值

	uint32_t								ulBreathPeriod;				//呼吸周期
}
stLedRunningParamTdf;

typedef struct
{
	stLedRunningParamTdf stRunningParam;		//静态参数
	stLedStaticParamTdf stStaticParam;					//动态参数
}stLedDeviceParamTdf;

void vLedOn(emLedDevNumTdf emDevNum); 
void vLedOFF(emLedDevNumTdf emDevNum);
void vLedDeviceInit(stLedStaticParamTdf *pstInit,emLedDevNumTdf emDevNum);
const stLedDeviceParamTdf	*c_pstGetLedDeviceParam(emLedDevNumTdf emDevNum);
void vLedToggle(emLedDevNumTdf emDevNum);
void vLedDeviceRunningParamInit(stLedRunningParamTdf *pstInit,emLedDevNumTdf emDevNum);
void vLedDevicePeriodExecute(emLedDevNumTdf	emDevNum);
void	vLedDeviceBlinkExecute(emLedDevNumTdf	emDevNum);


#endif
