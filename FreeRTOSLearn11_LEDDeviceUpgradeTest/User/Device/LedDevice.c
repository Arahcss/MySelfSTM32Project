#include "LedDevice.h"
#include "string.h"

stLedDeviceParamTdf astLedDeviceParam[LED_DEV_NUM];		//0为半载LED,1-8为外部LED

///	@brief				LED设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vLedDeviceInit(stLedDeviceParamTdf *pstInit,emLedDevNumTdf emDevNum)
{
//	astLedDeviceParam[emDevNum].pstGpioBase = pstInit ->pstGpioBase;
//	astLedDeviceParam[emDevNum].emOnLevel = pstInit ->emOnLevel;
//	astLedDeviceParam[emDevNum].usGpioPin = pstInit ->usGpioPin;

	memcpy(&astLedDeviceParam[emDevNum],pstInit,sizeof(stLedDeviceParamTdf)	/	sizeof(uint8_t));
}

///	@brief				LED点亮
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedOn(emLedDevNumTdf emDevNum)
{
	if(astLedDeviceParam[emDevNum].emOnLevel == 0)
	{
		HAL_GPIO_WritePin(astLedDeviceParam[emDevNum].pstGpioBase,astLedDeviceParam[emDevNum].usGpioPin,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(astLedDeviceParam[emDevNum].pstGpioBase,astLedDeviceParam[emDevNum].usGpioPin,GPIO_PIN_SET);
	}
}

///	@brief				LED熄灭
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedOFF(emLedDevNumTdf emDevNum)
{
	if(astLedDeviceParam[emDevNum].emOnLevel == 0)
	{
		HAL_GPIO_WritePin(astLedDeviceParam[emDevNum].pstGpioBase,astLedDeviceParam[emDevNum].usGpioPin,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(astLedDeviceParam[emDevNum].pstGpioBase,astLedDeviceParam[emDevNum].usGpioPin,GPIO_PIN_RESET);
	}
}
