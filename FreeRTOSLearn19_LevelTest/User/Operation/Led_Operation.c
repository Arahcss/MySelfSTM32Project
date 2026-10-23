#include "Led_Operation.h"
#include "LedDevice.h"

///	@brief			LED初始化
///
///	@note
void vLedInit()
{
	stLedStaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_13;
	stInit.emOnLevel			=	emLedOnLevel_Low;
	
	vLedDeviceInit(&stInit,LED_BOARD);
}
