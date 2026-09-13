#include "LedDevice.h"
#include "string.h"

stLedDeviceParamTdf astLedDeviceParam[9];		//0为半载LED,1-8为外部LED

///	@brief				LED设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			ucDevNum		:	设备编号
///	@note
void vLedDeviceInit(stLedDeviceParamTdf *pstInit,uint8_t ucDevNum)
{
	astLedDeviceParam[ucDevNum].pstGpioBase = pstInit ->pstGpioBase;
	astLedDeviceParam[ucDevNum].ucOnLevel = pstInit ->ucOnLevel;
	astLedDeviceParam[ucDevNum].usGpioPin = pstInit ->usGpioPin;
}

///	@brief				LED点亮
///
///	@param			ucDevNum		:	设备编号
///
///	@note
void vLedOn(uint8_t ucDevNum)
{
	if(astLedDeviceParam[ucDevNum].ucOnLevel == 0)
	{
		HAL_GPIO_WritePin(astLedDeviceParam[ucDevNum].pstGpioBase,astLedDeviceParam[ucDevNum].usGpioPin,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(astLedDeviceParam[ucDevNum].pstGpioBase,astLedDeviceParam[ucDevNum].usGpioPin,GPIO_PIN_SET);
	}
}

///	@brief				LED熄灭
///
///	@param			ucDevNum		:	设备编号
///
///	@note
void vLedOFF(uint8_t ucDevNum)
{
	if(astLedDeviceParam[ucDevNum].ucOnLevel == 0)
	{
		HAL_GPIO_WritePin(astLedDeviceParam[ucDevNum].pstGpioBase,astLedDeviceParam[ucDevNum].usGpioPin,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(astLedDeviceParam[ucDevNum].pstGpioBase,astLedDeviceParam[ucDevNum].usGpioPin,GPIO_PIN_RESET);
	}
}
