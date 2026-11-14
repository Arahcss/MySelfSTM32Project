#include "Menu_Task.h"
#include "Menu_Operation.h"
#include "Input_Operation.h"
#include "OLED_Task.h"
#include "Project_Config.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

extern osMessageQueueId_t KeyPadMsgQueueHandle;

///	@brief			把统一输入事件解码为菜单按键
///
///	@param			ulMsg			:	统一输入事件（高字节来源，低字节键码）
///
///	@return					归一化按键
///
///	@note			板载4键：键码0~3直接对应Up/Down/Enter/Cancel；
///						矩阵键盘：键码0~19映射为emInputKey_KeyPadBase + 键号
static emInputKeyTdf s_emMenuDecodeInput(uint32_t ulMsg)
{
	if(INPUT_EVENT_SOURCE(ulMsg) == (uint32_t)emInputSource_Btn)
	{
		return (emInputKeyTdf)INPUT_EVENT_CODE(ulMsg);
	}

	return (emInputKeyTdf)((uint32_t)emInputKey_KeyPadBase + INPUT_EVENT_CODE(ulMsg));
}

///	@brief				菜单任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				上电先投递一帧初始画面，然后以MENU_TICK_PERIOD_MS为周期
///						阻塞等待统一输入队列：收到按键则交给菜单处理，超时则驱动
///						当前应用的周期逻辑；处理完把最新显示帧投递到显示队列
void vMenuTask(void *argument)
{
	uint32_t ulMsg;
	osMessageQueueId_t hDisplayQueue;
	const stAppInterfaceTdf *c_pstApp;

	(void)argument;

	hDisplayQueue = xGetDisplayMsgQueue();

	vMenuInit();

	//非阻塞投递，显示任务来不及时丢弃本帧
	osMessageQueuePut(hDisplayQueue, c_pstMenuGetRender(), 0, 0);

	while(1)
	{
		if(osMessageQueueGet(KeyPadMsgQueueHandle, &ulMsg, NULL, MENU_TICK_PERIOD_MS) == osOK)
		{
			vMenuHandleKey(s_emMenuDecodeInput(ulMsg));

			//非阻塞投递，显示任务来不及时丢弃本帧
			osMessageQueuePut(hDisplayQueue, c_pstMenuGetRender(), 0, 0);
		}
		else
		{
			c_pstApp = c_pstGetActiveApp();
			if((c_pstApp != NULL) && (c_pstApp->vTick != NULL))
			{
				vMenuExecuteTick();

				//非阻塞投递，显示任务来不及时丢弃本帧
				osMessageQueuePut(hDisplayQueue, c_pstMenuGetRender(), 0, 0);
			}
		}
	}
}
