#ifndef _LED_OPERATION_H_
#define _LED_OPERATION_H_

#include "LedDevice.h"

///	@brief					LED动作枚举
///
///	@note					由按键处理任务映射后调用 vLedSetAction
typedef enum
{
	emLedAction_On			=	0,
	emLedAction_Off,
	emLedAction_Toggle,
	emLedAction_Breath,
}emLedActionTdf;

void vLedExecuteAll(void);
void vLedInitAll(void);
void vLedSetAction(emLedDevNumTdf emDevNum, emLedActionTdf emAction);

#endif


