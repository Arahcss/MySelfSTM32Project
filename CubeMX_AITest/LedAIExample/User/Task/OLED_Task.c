#include "OLED_Task.h"
#include "OLED_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				OLED任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				上电初始化OLED后执行一次测试显示，随后空转
void vOledTask(void *argument)
{
	(void)argument;

	vOledInitAll();
	vOledTest();

	while(1)
	{
		vTaskDelay(1000);
	}
}
