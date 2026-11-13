#include "OLED_Task.h"
#include "OLED_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief			显示消息队列深度
///
///	@note			显示任务每处理一帧约需数十毫秒，8帧足够覆盖快速连按
#define OLED_CALC_MSG_QUEUE_COUNT		8

static osMessageQueueId_t s_hCalcDisplayMsgQueue = NULL;

///	@brief			获取计算器显示消息队列（首次调用时创建）
///
///	@param					None
///
///	@return					队列句柄
///
///	@note				队列项大小为sizeof(stCalcDisplayMsgTdf)，必须按结构体整体投递，
///							否则显示帧会被截断（例如只更新前2个字符）
osMessageQueueId_t xGetCalcDisplayMsgQueue(void)
{
	if(s_hCalcDisplayMsgQueue == NULL)
	{
		s_hCalcDisplayMsgQueue = osMessageQueueNew(OLED_CALC_MSG_QUEUE_COUNT,
				sizeof(stCalcDisplayMsgTdf), NULL);
	}

	return s_hCalcDisplayMsgQueue;
}

///	@brief				OLED任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				上电初始化OLED后阻塞等待计算器显示队列，每取到一帧刷新一次
void vOledTask(void *argument)
{
	(void)argument;
	stCalcDisplayMsgTdf stDisplayMsg;
	osMessageQueueId_t hQueue;

	hQueue = xGetCalcDisplayMsgQueue();

	vOledInitAll();

	while(1)
	{
		osMessageQueueGet(hQueue, &stDisplayMsg, NULL, osWaitForever);

		vOledShowCalcFrame(&stDisplayMsg);
	}
}
