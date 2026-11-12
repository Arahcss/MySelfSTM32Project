#include "OLED_Task.h"
#include "OLED_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t CalcDisplayMsgQueueHandle;

///	@brief				OLED任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				上电初始化OLED后阻塞等待计算器显示队列，每取到一帧刷新一次
void vOledTask(void *argument)
{
	(void)argument;
	stCalcDisplayMsgTdf stDisplayMsg;

	vOledInitAll();

	while(1)
	{
		osMessageQueueGet(CalcDisplayMsgQueueHandle, &stDisplayMsg, NULL, osWaitForever);

		vOledShowCalcFrame(&stDisplayMsg);
	}
}
