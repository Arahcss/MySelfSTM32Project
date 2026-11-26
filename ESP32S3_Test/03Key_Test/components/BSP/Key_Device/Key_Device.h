#ifndef __KEY_DEVICE__
#define __KEY_DEVICE__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BOOT_GPIO_PIN   GPIO_NUM_0
#define BOOT_PRES       1

uint8_t key_GetLevel(void);
void key_init(void);
uint8_t KeyScan(uint8_t mode);

#endif  
