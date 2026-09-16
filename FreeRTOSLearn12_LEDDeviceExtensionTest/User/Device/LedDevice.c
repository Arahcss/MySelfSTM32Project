#include "LedDevice.h"
#include "string.h"

stLedDeviceParamTdf	astLedDeviceParam[LED_DEV_NUM];

///	@brief				LED闪烁执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				
void	vLedDeviceBlinkExecute(emLedDevNumTdf	emDevNum)
{	
	//若没有点亮超过阈限次数执行亮灯 
	if(astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount	<	astLedDeviceParam[emDevNum].stRunningParam.ulOnCountThreshold)
	{
		vLedOn(emDevNum);
	}
	else if(astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount	<=	astLedDeviceParam[emDevNum].stRunningParam.ulOnCountThreshold	+	
																																astLedDeviceParam[emDevNum].stRunningParam.ulOffCountThreshold)
	{
		vLedOFF(emDevNum);
	}
	astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount++;//闪烁时计数++

	
	if(astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount	>=	astLedDeviceParam[emDevNum].stRunningParam.ulOnCountThreshold	+	
																																astLedDeviceParam[emDevNum].stRunningParam.ulOffCountThreshold)
	{
		astLedDeviceParam[emDevNum].stRunningParam.ulCurrentCount	=	0;//清零Count
	}
}
	

///	@brief				LED周期执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				根据模式执行不同操作
void vLedDevicePeriodExecute(emLedDevNumTdf	emDevNum)
{
	vLedDeviceBlinkExecute(emDevNum);
	switch(astLedDeviceParam[emDevNum].stRunningParam.emMode)
	{
		case emLedMode_Blink:
		{
			vLedDeviceBlinkExecute(emDevNum);
			break;
		}
		default:
		{
			;
		}
	}
}

const stLedDeviceParamTdf	*c_pstGetLedDeviceParam(emLedDevNumTdf emDevNum)
{
	return &astLedDeviceParam[emDevNum];
}

void vLedDeviceRunningParamInit(stLedRunningParamTdf *pstInit,emLedDevNumTdf emDevNum)
{
		memcpy(&astLedDeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stLedRunningParamTdf)	/	sizeof(uint8_t));
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
	memcpy(&astLedDeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stLedStaticParamTdf)	/	sizeof(uint8_t));
}

///	@brief				LED点亮
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedOn(emLedDevNumTdf emDevNum)
{
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
	
		//设置当前状态
	astLedDeviceParam[emDevNum].stRunningParam.emCurrentStatus = emLedStatus_OFF;
	
	//根据当前状态更新输出引脚电频
	vLedUpdatePinLevel(emDevNum);
}

///	@brief				LED翻转
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vLedToggle(emLedDevNumTdf emDevNum)
{
	//设置当前状态
	astLedDeviceParam [emDevNum].stRunningParam.emCurrentStatus  =  (emLedStatusTdf)!astLedDeviceParam [emDevNum].stRunningParam.emCurrentStatus;
	
	//根据当前状态更新输出引脚电频
	vLedUpdatePinLevel(emDevNum);
}
