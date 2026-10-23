#ifndef _DHT11DEVICE_H_
#define _DHT11DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emDht11DevNum0				=	12,
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
}
stDht11StaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	int8_t acHumidity[2];			//湿度原始数据
	int8_t acTemperature[2];	//温度原始数据
}
stDht11RunningParamTdf;

typedef struct
{
	stDht11RunningParamTdf stRunningParam;		//静态参数
	stDht11StaticParamTdf stStaticParam;					//动态参数
}stDht11DeviceParamTdf;

void vDht11DeviceInit(stDht11StaticParamTdf *pstInit,emDht11DevNumTdf emDevNum);
uint8_t ucDht11ReadData(emDht11DevNumTdf emDevNum);


#endif
