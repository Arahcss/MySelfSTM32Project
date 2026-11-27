#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Led_Device.h"
#include "Key_Device.h"
#include "RGB_Device.h"

void app_main(void)
{
    led_init();
    key_init();
    RGB_Init();

    uint8_t key;

    while(1)
    {
        key =   KeyScan(0);

        switch(key)
        {
            case BOOT_PRES:
            {
                RGB_Toggle();
                break;
            }   
            default:
                break;
        }
        vTaskDelay(10);
    }
}
