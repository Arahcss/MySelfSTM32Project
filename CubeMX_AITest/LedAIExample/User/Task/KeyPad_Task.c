#include "KeyPad_Task.h"
#include "KeyPad_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t KeyPadMsgQueueHandle;

///	@brief				矩阵键盘扫描任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期扫描消抖，检测到Press事件后逐条投递键号到KeyPadMsgQueue；
///						键号 = 行索引*KEYPAD_COL_NUM + 列索引
void vKeyPadTask(void *argument)
{
	(void)argument;
	emKeyPadDevNumTdf emDevNum;

	vKeyPadInitAll();

	while(1)
	{
		vKeyPadExecuteAll();

		//采集所有按下事件，逐条入队（非阻塞，队列满则丢弃）
		while((emDevNum = emKeyPadGetPressEvent()) < KEYPAD_DEV_NUM)
		{
			uint32_t ulMsg = (uint32_t)emDevNum;
			osMessageQueuePut(KeyPadMsgQueueHandle, &ulMsg, 0, 0);
		}

		vTaskDelay(10);
	}
}
