#ifndef _KEYPAD_DEVICE_H_
#define _KEYPAD_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "Project_Config.h"

///	@brief					设备号枚举
///
///	@note					行优先编号，设备号 = 行索引 * KEYPAD_COL_NUM + 列索引；
///							调整 KEYPAD_ROW_NUM / KEYPAD_COL_NUM 时需同步增删成员
typedef enum
{
	emKeyPadDevNum0				=	0,
	emKeyPadDevNum1,
	emKeyPadDevNum2,
	emKeyPadDevNum3,
	emKeyPadDevNum4,
	emKeyPadDevNum5,
	emKeyPadDevNum6,
	emKeyPadDevNum7,
	emKeyPadDevNum8,
	emKeyPadDevNum9,
	emKeyPadDevNum10,
	emKeyPadDevNum11,
	emKeyPadDevNum12,
	emKeyPadDevNum13,
	emKeyPadDevNum14,
	emKeyPadDevNum15,
}emKeyPadDevNumTdf;

///	@brief					按键稳定状态枚举
///
///	@note
typedef enum
{
	emKeyPadStatus_Released		=	0,
	emKeyPadStatus_Pressed,
}emKeyPadStatusTdf;

///	@brief					按键边沿事件枚举
///
///	@note					由 emKeyPadGetEvent 读取后自动清零
typedef enum
{
	emKeyPadEvent_None			=	0,
	emKeyPadEvent_Press,
	emKeyPadEvent_Release,
}emKeyPadEventTdf;

///	@brief			行列引脚描述
///
///	@note			行引脚为上拉输入，列引脚为开漏输出，模式由CubeMX配置
typedef struct
{
	GPIO_TypeDef				*pstGpioBase;			//	使用的GPIOx
	uint16_t					usGpioPin;				//	使用的GPIO_PIN_x
}stKeyPadPinTdf;

///	@brief			静态参数定义
///
///	@note			每个按键只记录所属行列索引，引脚信息由行列引脚表统一管理
typedef struct
{
	uint8_t						ucRowIndex;				//	行索引 0~KEYPAD_ROW_NUM-1
	uint8_t						ucColIndex;				//	列索引 0~KEYPAD_COL_NUM-1
	uint16_t					usDebounceThreshold;	//	消抖阈值（周期次数）
}stKeyPadStaticParamTdf;

///	@brief			运行参数定义
///
///	@note
typedef struct
{
	emKeyPadStatusTdf			emCurrentStatus;		//	消抖后的稳定状态
	emKeyPadEventTdf			emEvent;				//	待处理边沿事件
	uint16_t					usDebounceCount;		//	当前消抖计数
}stKeyPadRunningParamTdf;

///	@brief			按键设备参数总结构
///
///	@note
typedef struct
{
	stKeyPadRunningParamTdf		stRunningParam;		//运行参数
	stKeyPadStaticParamTdf		stStaticParam;		//静态参数
}stKeyPadDeviceParamTdf;

void vKeyPadDeviceInit(const stKeyPadStaticParamTdf *pstInit, emKeyPadDevNumTdf emDevNum);
void vKeyPadDeviceRowPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucRowIndex);
void vKeyPadDeviceColPinInit(const stKeyPadPinTdf *pstInit, uint8_t ucColIndex);
void vKeyPadDevicePeriodExecute(void);
const stKeyPadDeviceParamTdf	*c_pstGetKeyPadDeviceParam(emKeyPadDevNumTdf emDevNum);
emKeyPadStatusTdf emKeyPadGetStatus(emKeyPadDevNumTdf emDevNum);
emKeyPadEventTdf emKeyPadGetEvent(emKeyPadDevNumTdf emDevNum);

#endif
