#include "Led_Operation.h"

///	@brief					初始化所有LED设备
///
///	@param					None
///
///	@note					静态参数与运行参数（含呼吸周期）均在此初始化
void vLedInitAll(void)
{
	stLedStaticParamTdf stInit = {0};
	stLedRunningParamTdf stRunningInit = {0};

	stInit.pstGpioBase = GPIOC;
	stInit.usGpioPin = GPIO_PIN_13;
	stInit.emOnLevel = emLedOnLevel_Low;

	vLedDeviceInit(&stInit, emLedDevNum0);

	//运行参数初始化：默认静态模式，预置呼吸参数（计数基准 = LedTask 1ms周期）
	stRunningInit.emMode = emLedMode_Static;
	//PWM步长周期 = 10次计数 × 1ms = 10ms（100Hz，无频闪）
	stRunningInit.ulOnCountThreshold = 5;
	stRunningInit.ulOffCountThreshold = 5;
	//呼吸总周期 = 2000次计数 × 1ms = 2s，分200个PWM步长
	stRunningInit.ulBreathPeriod = 2000;

	vLedDeviceRunningParamInit(&stRunningInit, emLedDevNum0);
}

///	@brief					设置LED动作
///
///	@param					emDevNum		:	设备编号
///	@param					emAction		:	目标动作
///
///	@note					ON/OFF/TOGGLE 切到静态模式后立即执行；BREATHE 切到呼吸模式
void vLedSetAction(emLedDevNumTdf emDevNum, emLedActionTdf emAction)
{
	switch(emAction)
	{
		case emLedAction_On:
		{
			vLedSetMode(emDevNum, emLedMode_Static);
			vLedOn(emDevNum);
			break;
		}
		case emLedAction_Off:
		{
			vLedSetMode(emDevNum, emLedMode_Static);
			vLedOff(emDevNum);
			break;
		}
		case emLedAction_Toggle:
		{
			vLedSetMode(emDevNum, emLedMode_Static);
			vLedToggle(emDevNum);
			break;
		}
		case emLedAction_Breath:
		{
			vLedSetMode(emDevNum, emLedMode_Breath);
			break;
		}
		default:
		{
			break;
		}
	}
}

///	@brief					执行所有LED设备
///
///	@param					None
///
///	@note					按当前模式执行（Static保持 / Breath调制）
void vLedExecuteAll(void)
{
	vLedDevicePeriodExecute(emLedDevNum0);
}
