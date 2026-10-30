#ifndef _BTN_DEVICE_H_
#define _BTN_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "Project_Config.h"

///	@brief					设备号枚举
///
///	@note					与 Project_Config.h 中 BUTTON_* 宏对应
typedef enum
{
	emBtnDevNum0				=	0,
	emBtnDevNum1,
	emBtnDevNum2,
	emBtnDevNum3,
}emBtnDevNumTdf;

///	@brief					按键按下时的电平枚举
///
///	@note
typedef enum
{
	emBtnActiveLevel_Low		=	0,
	emBtnActiveLevel_High,
}emBtnActiveLevelTdf;

///	@brief					按键稳定状态枚举
///
///	@note
typedef enum
{
	emBtnStatus_Released		=	0,
	emBtnStatus_Pressed,
}emBtnStatusTdf;

///	@brief					按键边沿事件枚举
///
///	@note					由 emBtnGetEvent 读取后自动清零
typedef enum
{
	emBtnEvent_None			=	0,
	emBtnEvent_Press,
	emBtnEvent_Release,
}emBtnEventTdf;

///	@brief			静态参数定义
///
///	@note
typedef struct
{
	GPIO_TypeDef				*pstGpioBase;			//	使用的GPIOx
	uint16_t					usGpioPin;				//	使用的GPIO_PIN_x
	emBtnActiveLevelTdf			emActiveLevel;			//	按下时的电平
	uint16_t					usDebounceThreshold;	//	消抖阈值（周期次数）
}
stBtnStaticParamTdf;

///	@brief			运行参数定义
///
///	@note
typedef struct
{
	emBtnStatusTdf				emCurrentStatus;		//	消抖后的稳定状态
	emBtnEventTdf				emEvent;				//	待处理边沿事件
	uint16_t					usDebounceCount;		//	当前消抖计数
}
stBtnRunningParamTdf;

///	@brief			按键设备参数总结构
///
///	@note
typedef struct
{
	stBtnRunningParamTdf	stRunningParam;		//运行参数
	stBtnStaticParamTdf		stStaticParam;		//静态参数
}
stBtnDeviceParamTdf;

void vBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum);
void vBtnDeviceRunningParamInit(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum);
void vBtnDevicePeriodExecute(emBtnDevNumTdf emDevNum);
const stBtnDeviceParamTdf	*c_pstGetBtnDeviceParam(emBtnDevNumTdf emDevNum);
emBtnStatusTdf emBtnGetStatus(emBtnDevNumTdf emDevNum);
emBtnEventTdf emBtnGetEvent(emBtnDevNumTdf emDevNum);

#endif
