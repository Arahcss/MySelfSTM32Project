#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Led_Device.h"

void app_main(void)
{
    led_init();
    while(1)
    {
        LED_TOGGEL();
        vTaskDelay(500);
    }
}
