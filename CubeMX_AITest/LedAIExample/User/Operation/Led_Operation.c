#include "Led_Operation.h"

///	@brief					初始化所有LED设备
///
///	@param					None
///
///	@note
void vLedInitAll(void)
{
    stLedStaticParamTdf stInit = {0};
    
    stInit.pstGpioBase = GPIOC;
    stInit.usGpioPin = GPIO_PIN_13;
    stInit.emOnLevel = emLedOnLevel_Low;
    
    vLedDeviceInit(&stInit,emLedDevNum0);
}

/// @brief 执行所有LED设备
/// @param 
/// @note 
void vLedExcuteAll()
{
    
    // vLedDevicePeriodExecute(emLedDevNum0);
    vLedToggle(emLedDevNum0);
}

