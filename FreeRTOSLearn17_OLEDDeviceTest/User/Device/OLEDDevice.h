#ifndef _OLED_DEVICE_H_
#define _OLED_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"

#define	PI	3.141592653

///	@brief					OLED模式枚举
///
///	@note
typedef enum
{
	emOledMode_Static			=	0,
	emOledMode_Blink,
	emOledMode_Breath,
}
emOledModeTdf;

///	@brief					OLED状态枚举
///
///	@note
typedef enum
{
	emOledStatus_OFF			=	0,
	emOledStatus_ON,
}
emOledStatusTdf;

///	@brief					OLED ON时的电平枚举
///
///	@note
typedef enum
{
	emOledOnLevel_Low			=	0,
	emOledOnLevel_High,
}
emOledOnLevelTdf;

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emOledDevNum0				=	0,
	emOledDevNum1,
	emOledDevNum2,
	emOledDevNum3,
	emOledDevNum4,
	emOledDevNum5,
	emOledDevNum6,
	emOledDevNum7,
	emOledDevNum8,
}emOledDevNumTdf;

#define OLED_DEV_NUM		9

#define OLED_BOARD			emOledDevNum0
#define OLED1						emOledDevNum1
#define OLED2						emOledDevNum2
#define OLED3						emOledDevNum3
#define OLED4						emOledDevNum4
#define OLED5						emOledDevNum5
#define OLED6						emOledDevNum6
#define OLED7						emOledDevNum7
#define OLED8						emOledDevNum8

///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef								*pstGpioBase;		//	使用的GPIOx
	uint16_t										usGpioPin;			//	使用的GPIO_PIN_x
	emOledOnLevelTdf						emOnLevel;			//	OLED点亮时的电频
}
stOledStaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	emOledStatusTdf					emCurrentStatus;			//	OLED当前状态
	emOledModeTdf					emMode;							//当前模式
	
	uint32_t								ulCurrentCount;				//当前计数
	uint32_t								ulOnCountThreshold;		//ON	计数阈值
	uint32_t								ulOffCountThreshold;		//OFF	计数阈值

	uint32_t								ulBreathPeriod;				//呼吸周期
}
stOledRunningParamTdf;

typedef struct
{
	stOledRunningParamTdf stRunningParam;		//静态参数
	stOledStaticParamTdf stStaticParam;					//动态参数
}stOledDeviceParamTdf;


#endif
