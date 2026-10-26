#include "Led_Task.h"
#include "Led_Operation.h"
#include "FreeRTOS.h"
#include "task.h"

/// @brief LED任务
/// @param  
void vLedTask(void)
{
    vLedInitAll();
    while(1)
    {
        vLedExcuteAll();
        vTaskDelay(1000);
    }
    
}
