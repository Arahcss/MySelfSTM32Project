#include "Key_Device.h"

uint8_t key_GetLevel(void)
{
    return gpio_get_level(BOOT_GPIO_PIN);
}

void key_init(void)
{
    gpio_config_t gpio_init_struct;
    gpio_init_struct.intr_type      =   GPIO_INTR_DISABLE;
    gpio_init_struct.mode           =   GPIO_MODE_INPUT;
    gpio_init_struct.pull_down_en   =   GPIO_PULLDOWN_DISABLE;
    gpio_init_struct.pull_up_en     =   GPIO_PULLUP_ENABLE;
    gpio_init_struct.pin_bit_mask   =   1ull << BOOT_GPIO_PIN;

    gpio_config(&gpio_init_struct);

    
}

uint8_t KeyScan(uint8_t mode)
{
    uint8_t KeyFlag  =   0;
    static  uint8_t KeyBoot =   1;

    if(mode)
    {
        KeyFlag  =   1;
    }

    if(KeyBoot && (key_GetLevel() == 0))
    {
        vTaskDelay(10);
        KeyBoot = 0;

        if(key_GetLevel() == 0)
        {
            KeyFlag = BOOT_PRES;
        }
    }
    else if(key_GetLevel() == 1)
    {
        KeyBoot = 1;
    }

    return KeyFlag;
}
