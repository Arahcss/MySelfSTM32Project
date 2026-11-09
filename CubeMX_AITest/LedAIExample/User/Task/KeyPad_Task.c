#include "KeyPad_Task.h"
#include "KeyPad_Operation.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

///	@brief				矩阵键盘扫描任务
///
///	@param			argument		:	CubeMX传入参数（未使用）
///
///	@note				10ms周期扫描消抖；事件由业务层通过
///						emKeyPadGetPressEvent / ulKeyPadGetPressedMask 轮询获取
void vKeyPadTask(void *argument)
{
	(void)argument;

	vKeyPadInitAll();

	while(1)
	{
		vKeyPadExecuteAll();

		vTaskDelay(10);
	}
}
