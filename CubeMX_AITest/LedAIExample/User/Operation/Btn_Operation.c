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
