#include "LedDevice.h"
#include "string.h"

stLedDeviceParamTdf	astLedDeviceParam[LED_DEV_NUM];

const stLedDeviceParamTdf	*c_pstGetLedDeviceParam(emLedDevNumTdf emDevNum)
{
	return &astLedDeviceParam[emDevNum];
}

void vLedUpdatePinLevel(emLedDevNumTdf	emDevNum)
{
	uint8_t ucOutput;
	
	//计算输出引脚电频
	ucOutput = !(astLedDeviceParam[emDevNum].stStaticParam.emOnLevel ^ astLedDeviceParam[emDevNum].stRunningParam.emCurrentStatus);
	
	//更新LED输出电频
	HAL_GPIO_WritePin(astLedDeviceParam[emDevNum].stStaticParam.pstGpioBase,astLedDeviceParam[emDevNum].stStaticParam.usGpioPin,(GPIO_PinState)ucOutput);
}

///	@brief				LED设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vLedDeviceInit(stLedStaticParamTdf *pstInit,emLedDevNumTdf emDevNum)
{
//	stLedDeviceParamTdf[emDevNum].pstGpioBase = pstInit ->pstGpioBase;
//	stLedDeviceParamTdf[emDevNum].emOnLevel = pstInit ->emOnLevel;
//	stLedDeviceParamTdf[emDevNum].usGpioPin = pstInit ->usGpioPin;

	memcpy(&astLedDeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stLedStaticParamTdf)	/	sizeof(uint8_t));
}

///	@brief				LED点亮
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedOn(emLedDevNumTdf emDevNum)
{
//	if(stLedDeviceParamTdf[emDevNum].emOnLevel == 0)
//	{
//		HAL_GPIO_WritePin(stLedDeviceParamTdf[emDevNum].pstGpioBase,stLedDeviceParamTdf[emDevNum].usGpioPin,GPIO_PIN_RESET);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(stLedDeviceParamTdf[emDevNum].pstGpioBase,stLedDeviceParamTdf[emDevNum].usGpioPin,GPIO_PIN_SET);
//	}
	//设置当前状态
	astLedDeviceParam [emDevNum].stRunningParam.emCurrentStatus = emLedStatus_ON;
	
	//根据当前状态更新输出引脚电频
	vLedUpdatePinLevel(emDevNum);
}

///	@brief				LED熄灭
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedOFF(emLedDevNumTdf emDevNum)
{
//	if(stLedDeviceParamTdf[emDevNum].emOnLevel == 0)
//	{
//		HAL_GPIO_WritePin(stLedDeviceParamTdf[emDevNum].pstGpioBase,stLedDeviceParamTdf[emDevNum].usGpioPin,GPIO_PIN_SET);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(stLedDeviceParamTdf[emDevNum].pstGpioBase,stLedDeviceParamTdf[emDevNum].usGpioPin,GPIO_PIN_RESET);
//	}
	
		//设置当前状态
	astLedDeviceParam[emDevNum].stRunningParam.emCurrentStatus = emLedStatus_OFF;
	
	//根据当前状态更新输出引脚电频
	vLedUpdatePinLevel(emDevNum);
}
