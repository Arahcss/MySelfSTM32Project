#ifndef _OLED_TASK_H_
#define _OLED_TASK_H_

#include "cmsis_os.h"
#include "OLED_Operation.h"

///	@brief			获取显示消息队列（首次调用时创建）
///
///	@param					None
///
///	@return					队列句柄
///
///	@note				队列项为stDisplayMsgTdf（统一显示帧，约76字节），CubeMX的Item Size
///							只能选基本类型，无法表达该结构体，故在用户代码中按sizeof创建；
///							由OLED任务首次调用完成创建，菜单任务随后取到同一句柄
osMessageQueueId_t xGetDisplayMsgQueue(void);

void vOledTask(void *argument);


#endif
