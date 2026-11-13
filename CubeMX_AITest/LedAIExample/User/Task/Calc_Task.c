#include "Calc_Task.h"
#include "Calculator_Operation.h"
#include "OLED_Task.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t KeyPadMsgQueueHandle;

///	@brief				计算器任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				阻塞读取键盘消息队列，处理按键后把显示帧投递到显示队列；
///						上电先投递一帧初始画面，显示任务启动先后不影响结果
void vCalcTask(void *argument)
{
	(void)argument;
	uint32_t ulKeyIndex;
	osMessageQueueId_t hDisplayQueue;

	hDisplayQueue = xGetCalcDisplayMsgQueue();

	vCalcInit();
	osMessageQueuePut(hDisplayQueue, c_pstGetCalcDisplayMsg(), 0, 0);

	while(1)
	{
		osMessageQueueGet(KeyPadMsgQueueHandle, &ulKeyIndex, NULL, osWaitForever);

		vCalcInputKey((uint8_t)ulKeyIndex);

		//非阻塞投递，显示任务来不及时丢弃本帧，不阻塞按键处理
		osMessageQueuePut(hDisplayQueue, c_pstGetCalcDisplayMsg(), 0, 0);
	}
}
