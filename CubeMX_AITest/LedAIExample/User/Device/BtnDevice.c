#include "BtnDevice.h"
#include "string.h"

stBtnDeviceParamTdf	astBtnDeviceParam[BTN_DEV_NUM];

///	@brief				按键扫描执行（读取引脚 + 消抖 + 事件生成）
///
///	@param			emDevNum		:	设备编号
///
///	@note				消抖以本函数被调用的次数为基准
static void vBtnDeviceScanExecute(emBtnDevNumTdf	emDevNum)
{
	uint8_t ucPinLevel;
	emBtnStatusTdf emRawStatus;

	//读取引脚电平
	ucPinLevel = (uint8_t)HAL_GPIO_ReadPin(astBtnDeviceParam[emDevNum].stStaticParam.pstGpioBase, astBtnDeviceParam[emDevNum].stStaticParam.usGpioPin);

	//换算为逻辑状态
	emRawStatus = (emBtnStatusTdf)!(ucPinLevel ^ astBtnDeviceParam[emDevNum].stStaticParam.emActiveLevel);

	//原始状态与稳定状态一致则清零消抖计数
	if(emRawStatus == astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus)
	{
		astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;
	}
	else
	{
		//消抖计数累加
		astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount++;

		//达到阈值后更新稳定状态
		if(astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount >= astBtnDeviceParam[emDevNum].stStaticParam.usDebounceThreshold)
		{
			astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus = emRawStatus;

			astBtnDeviceParam[emDevNum].stRunningParam.usDebounceCount = 0;

			//生成边沿事件
			if(emRawStatus == emBtnStatus_Pressed)
			{
				astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_Press;
			}
			else
			{
				astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_Release;
			}
		}
	}
}

///	@brief				按键周期执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				需以固定周期调用，调用周期即消抖的时间基准
void vBtnDevicePeriodExecute(emBtnDevNumTdf	emDevNum)
{
	vBtnDeviceScanExecute(emDevNum);
}

///	@brief				获取按键设备参数
///
///	@param			emDevNum		:	设备编号
///
///	@note				返回const指针，仅供只读访问
const stBtnDeviceParamTdf	*c_pstGetBtnDeviceParam(emBtnDevNumTdf emDevNum)
{
	return &astBtnDeviceParam[emDevNum];
}

///	@brief				获取按键稳定状态
///
///	@param			emDevNum		:	设备编号
///
///	@note				消抖后的状态，不受 emBtnGetEvent 影响
emBtnStatusTdf emBtnGetStatus(emBtnDevNumTdf emDevNum)
{
	return astBtnDeviceParam[emDevNum].stRunningParam.emCurrentStatus;
}

///	@brief				读取并清除按键边沿事件
///
///	@param			emDevNum		:	设备编号
///
///	@note				读取后事件自动清零，避免重复处理
emBtnEventTdf emBtnGetEvent(emBtnDevNumTdf emDevNum)
{
	emBtnEventTdf emEvent;

	emEvent = astBtnDeviceParam[emDevNum].stRunningParam.emEvent;

	astBtnDeviceParam[emDevNum].stRunningParam.emEvent = emBtnEvent_None;

	return emEvent;
}

///	@brief				按键运行参数初始化
///
///	@param			pstInit			:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///
///	@note
void vBtnDeviceRunningParamInit(stBtnRunningParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
	memcpy(&astBtnDeviceParam[emDevNum].stRunningParam, pstInit, sizeof(stBtnRunningParamTdf) / sizeof(uint8_t));
}

///	@brief				按键设备初始化
///
///	@param			pstInit			:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///
///	@note				引脚模式与上下拉由CubeMX配置，本函数只记录静态参数
void vBtnDeviceInit(stBtnStaticParamTdf *pstInit, emBtnDevNumTdf emDevNum)
{
	memcpy(&astBtnDeviceParam[emDevNum].stStaticParam, pstInit, sizeof(stBtnStaticParamTdf) / sizeof(uint8_t));
}
