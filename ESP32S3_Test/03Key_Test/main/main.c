#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Led_Device.h"
#include "Key_Device.h"

void app_main(void)
{
    led_init();
    key_init();

    uint8_t key;

    while(1)
    {
        key =   KeyScan(0);

        switch(key)
        {
            case BOOT_PRES:
            {
                LED_TOGGLE();
                break;
            }   
            default:
                break;
        }
        vTaskDelay(10);
    }
}
