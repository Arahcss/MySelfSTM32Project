#include "Button_Operation.h"
#include "button_device.h"

///	@brief			BUTTON初始化
///
///	@note
void vButtonInit(void)
{
	stBtnStaticParamTdf stInit;
	
	stInit.pstGpioBase		=	GPIOA;
	stInit.usGpioPin			=	GPIO_PIN_1;
	stInit.emActiveLevel			=	emBtnActiveLevel_High;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_UP);
	
	stInit.pstGpioBase		=	GPIOA;
	stInit.usGpioPin			=	GPIO_PIN_0;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_DOWN);
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_15;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_ENTER);
	
	stInit.pstGpioBase		=	GPIOC;
	stInit.usGpioPin			=	GPIO_PIN_14;
	stInit.emActiveLevel			=	emBtnActiveLevel_Low;
	stInit.ulDebounceCountThreshold	=	2;
	stInit.ulDoubleClickInterval			=	0;
	stInit.ulLongPressCountThreshold	=	1000000;
	
	emBtnDeviceInit(&stInit,BUTTON_CANCEL);
}
