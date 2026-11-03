#include "Btn_Operation.h"

///	@brief					初始化所有按键设备
///
///	@param					None
///
///	@note					引脚与上下拉由CubeMX配置，此处只登记静态参数
void vBtnInitAll(void)
{
	stBtnStaticParamTdf stInit = {0};

	//UP按键
	stInit.pstGpioBase = GPIOA;
	stInit.usGpioPin = GPIO_PIN_1;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_UP);

	//DOWN按键
	stInit.pstGpioBase = GPIOA;
	stInit.usGpioPin = GPIO_PIN_0;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_DOWN);

	//ENTER按键
	stInit.pstGpioBase = GPIOC;
	stInit.usGpioPin = GPIO_PIN_15;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_ENTER);

	//CANCEL按键
	stInit.pstGpioBase = GPIOC;
	stInit.usGpioPin = GPIO_PIN_14;
	stInit.emActiveLevel = emBtnActiveLevel_Low;
	stInit.usDebounceThreshold = 2;
	vBtnDeviceInit(&stInit, BUTTON_CANCEL);
}

///	@brief					采集一个按下事件的按键号
///
///	@param					None
///
///	@return					按下事件的按键设备号；无按下事件返回 BTN_DEV_NUM
///
///	@note					每次调用只读清第一个Press事件，需循环调用以处理多键
emBtnDevNumTdf emBtnGetPressEvent(void)
{
	emBtnDevNumTdf emDevNum;

	for(emDevNum = emBtnDevNum0; emDevNum < BTN_DEV_NUM; emDevNum++)
	{
		if(emBtnGetEvent(emDevNum) == emBtnEvent_Press)
		{
			return emDevNum;
		}
	}

	return BTN_DEV_NUM;
}

///	@brief					执行所有按键设备
///
///	@param					None
///
///	@note					需以固定周期调用，建议10ms
void vBtnExecuteAll(void)
{
	emBtnDevNumTdf emDevNum;

	for(emDevNum = emBtnDevNum0; emDevNum < BTN_DEV_NUM; emDevNum++)
	{
		vBtnDevicePeriodExecute(emDevNum);
	}
}
