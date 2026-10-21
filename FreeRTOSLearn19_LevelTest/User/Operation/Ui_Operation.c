#include "Ui_Operation.h"
#include "OLEDDevice.h"

///	@brief			OLED初始化
///
///	@note
void vOledInit()
{
	stOledStaticParamTdf stInit;
	
	stInit.pstSclGpioBase	=	GPIOB;
	stInit.usSclGpioPin		=	GPIO_PIN_8;
	stInit.pstSdaGpioBase	=	GPIOB;
	stInit.usSdaGpioPin	 	=	GPIO_PIN_9;
	
	vOledDeviceInit(&stInit,OLED);
}
