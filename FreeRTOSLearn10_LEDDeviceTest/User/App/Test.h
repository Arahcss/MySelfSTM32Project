#ifndef _TEST_H_
#define _TEST_H_

#include "stm32f1xx_hal.h"

#define LED_ON										GPIO_PIN_RESET
#define LED_OFF										GPIO_PIN_SET
#define LED_UPDATE(LED_STATUS)		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,LED_STATUS)


void vTaskExecute(void);




#endif
