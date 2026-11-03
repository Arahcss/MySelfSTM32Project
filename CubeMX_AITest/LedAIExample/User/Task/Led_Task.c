#include "Led_Task.h"
#include "Led_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				LED任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期驱动LED模式执行（呼吸灯需高频）
void vLedTask(void *argument)
{
	(void)argument;

	vLedInitAll();

	while(1)
	{
		vLedExecuteAll();
		vTaskDelay(10);
	}
}
