#ifndef _OLED_TASK_H_
#define _OLED_TASK_H_

#include "cmsis_os.h"
#include "OLED_Operation.h"

///	@brief			获取计算器显示消息队列（首次调用时创建）
///
///	@param					None
///
///	@return					队列句柄
///
///	@note				队列项为stCalcDisplayMsgTdf（34字节），CubeMX的Item Size只能选
///							基本类型，无法表达该结构体，故在用户代码中按sizeof创建；
///							OLED任务优先级高于计算任务，由OLED任务首次调用完成创建
osMessageQueueId_t xGetCalcDisplayMsgQueue(void);

void vOledTask(void *argument);


#endif
