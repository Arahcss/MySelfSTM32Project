#include "BtnProcess_Task.h"
#include "Led_Operation.h"
#include "BtnDevice.h"
#include "Project_Config.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t BtnMsgQueueHandle;

///	@brief				按键处理任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				阻塞读按键消息队列，映射按键到LED动作
void vBtnProcessTask(void *argument)
{
	(void)argument;
	uint32_t ulMsg;

	while(1)
	{
		osMessageQueueGet(BtnMsgQueueHandle, &ulMsg, NULL, osWaitForever);

		switch((emBtnDevNumTdf)ulMsg)
		{
			case BUTTON_UP:
			{
				vLedSetAction(LED_BOARD, emLedAction_On);
				break;
			}
			case BUTTON_DOWN:
			{
				vLedSetAction(LED_BOARD, emLedAction_Off);
				break;
			}
			case BUTTON_ENTER:
			{
				vLedSetAction(LED_BOARD, emLedAction_Toggle);
				break;
			}
			case BUTTON_CANCEL:
			{
				vLedSetAction(LED_BOARD, emLedAction_Breath);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}
