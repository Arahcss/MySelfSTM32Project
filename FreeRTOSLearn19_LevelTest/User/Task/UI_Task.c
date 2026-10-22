#include "freertos.h"
#include "task.h"
#include "UI_Task.h"
#include "OLEDDevice.h"
#include "LedDevice.h"
#include "button_device.h"
#include "Button_Operation.h"
#include "Led_Operation.h"
#include "Ui_Operation.h"
#include "Ui_Wight_Single_Line_Text.h"
#include "Ui_Wight_Button.h"
#include "Page_Led_Ctrl.h"
#include "string.h"




///	@brief			UI任务
///
///	@note
void vUITask()
{
	vOledInit();
	vLedInit();
	vButtonInit();
	
	while(1)
	{
		vTest();
		vTaskDelay(1);
	}
}

