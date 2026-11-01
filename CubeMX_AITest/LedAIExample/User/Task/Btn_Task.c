#include "Btn_Task.h"
#include "Btn_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				按键任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期驱动按键扫描消抖
void vBtnTask(void *argument)
{
	(void)argument;

	vBtnInitAll();

	while(1)
	{
		vBtnExecuteAll();
		vTaskDelay(10);
	}
}
