#ifndef _DHT11DEVICE_H_
#define _DHT11DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emDht11FsmState_Idle							= 0,		//【空闲状态】
	emDht11FsmState_StartLow,								//【开始低状态】
	emDht11FsmState_StartHigh,								//【开始高状态】
	emDht11FsmState_WaitAckLow,						//【等待答应低状态】
	emDht11FsmState_WaitAckHigh,						//【等待答应高状态】
	emDht11FsmState_WaitReadStart,						//【等待可以开始读状态】
	emDht11FsmState_Reading,								//【读取状态】
}emDht11FsmStateTdf;										

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emDht11DevNum0				=	0,
}emDht11DevNumTdf;

#define DHT11_DEV_NUM		1
#define DHT11							emDht11DevNum0

///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef								*pstGpioBase;		//	使用的GPIOx
	uint16_t										usGpioPin;			//	使用的GPIO_PIN_x
	uint32_t										ulTimerPeriorUs;		//定时器周期
}
stDht11StaticParamTdf;

typedef struct
{
	emDht11FsmStateTdf			emState;					//状态机当前状态
	uint32_t								ulTimerCount;			//定时器计数
	uint32_t								ulLowLevelCount;	//低电平时间计数
	uint32_t								ulHighLevelCount;	//高电平时间计数
	uint8_t								ucBitCount;				//接收bits计数
	uint8_t								aucDataTemp[5];			//临时数据
}stDht11FsmParamTdf;


///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	int8_t acHumidity[2];			//湿度原始数据
	int8_t acTemperature[2];	//温度原始数据
	stDht11FsmParamTdf stFsmParam;			//状态机参数
}
stDht11RunningParamTdf;

typedef struct
{
	stDht11RunningParamTdf stRunningParam;		//静态参数
	stDht11StaticParamTdf stStaticParam;					//动态参数
}stDht11DeviceParamTdf;


void vDht11DeviceInit(stDht11StaticParamTdf *pstInit,emDht11DevNumTdf emDevNum);
uint8_t ucDht11ReadData(emDht11DevNumTdf emDevNum);
void vDht11FsmCallback(emDht11DevNumTdf emDevNum);
const stDht11DeviceParamTdf	*c_pstGetDht11DeviceParam(emDht11DevNumTdf emDevNum);

#endif
