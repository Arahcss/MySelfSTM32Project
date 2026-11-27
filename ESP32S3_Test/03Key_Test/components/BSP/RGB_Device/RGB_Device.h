#ifndef __RGB_DEVICE__
#define __RGB_DEVICE__

#include "driver/gpio.h"
#include "led_strip.h"
#include "esp_log.h"

#define RGB_GPIO_PIN    GPIO_NUM_48
#define LED_STRIP_LED_COUNT 1

void RGB_Init(void);
void RGB_Toggle(void);

#endif