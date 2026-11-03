#include "Btn_Task.h"
#include "Btn_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t BtnMsgQueueHandle;

///	@brief				按键获取任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期扫描消抖，检测到Press事件后逐条入队
void vBtnTask(void *argument)
{
	(void)argument;
	emBtnDevNumTdf emBtn;

	vBtnInitAll();

	while(1)
	{
		vBtnExecuteAll();

		//采集所有按下事件，逐条入队（非阻塞，队列满则丢弃）
		while((emBtn = emBtnGetPressEvent()) < BTN_DEV_NUM)
		{
			uint32_t ulMsg = (uint32_t)emBtn;
			osMessageQueuePut(BtnMsgQueueHandle, &ulMsg, 0, 0);
		}

		vTaskDelay(10);
	}
}
