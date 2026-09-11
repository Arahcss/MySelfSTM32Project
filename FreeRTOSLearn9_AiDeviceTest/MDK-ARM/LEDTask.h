/* LEDTask.h */
#ifndef __LEDTASK_H
#define __LEDTASK_H

#include "main.h"
#include "cmsis_os.h"

/* 初始化并创建 LED 任务（在 osKernelStart() 之前调用） */
void LEDTaskInit(void);

/* 任务入口 */
void LEDTask(void *argument);

/* LED 控制接口 */
void LED_On(uint8_t idx);       /* 点亮单个 LED (0~3) */
void LED_Off(uint8_t idx);      /* 熄灭单个 LED (0~3) */
void LED_Toggle(uint8_t idx);   /* 翻转单个 LED */
void LED_AllOn(void);           /* 全亮 */
void LED_AllOff(void);          /* 全灭 */

#endif /* __LEDTASK_H */
