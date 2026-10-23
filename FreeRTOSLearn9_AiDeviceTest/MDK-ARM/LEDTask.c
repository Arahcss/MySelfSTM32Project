/* LEDTask.c */
#include "LEDTask.h"

#define LED_COUNT   4
#define LED_DELAY   300      /* 流水间隔，单位 ms，可自行调整 */

/* LED 引脚映射表: PA6, PA7, PB0, PB1 */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} LED_Map_t;

static const LED_Map_t s_ledMap[LED_COUNT] = {
    { GPIOA, GPIO_PIN_6 },
    { GPIOA, GPIO_PIN_7 },
    { GPIOB, GPIO_PIN_0 },
    { GPIOB, GPIO_PIN_1 },
};

/* 任务相关 */
osThreadId_t LEDTaskHandle;
static const osThreadAttr_t LEDTask_attributes = {
    .name       = "LEDTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityLow,
};

/* 内部：写单个 LED —— 共阳极：低电平亮 */
static inline void LED_Write(uint8_t idx, GPIO_PinState state)
{
    if (idx >= LED_COUNT) return;
    HAL_GPIO_WritePin(s_ledMap[idx].port, s_ledMap[idx].pin, state);
}

/* ---------- 对外接口 ---------- */
void LED_On(uint8_t idx)      { LED_Write(idx, GPIO_PIN_RESET); }
void LED_Off(uint8_t idx)     { LED_Write(idx, GPIO_PIN_SET);   }

void LED_Toggle(uint8_t idx)
{
    if (idx >= LED_COUNT) return;
    HAL_GPIO_TogglePin(s_ledMap[idx].port, s_ledMap[idx].pin);
}

void LED_AllOn(void)
{
    for (uint8_t i = 0; i < LED_COUNT; i++) LED_Write(i, GPIO_PIN_RESET);
}

void LED_AllOff(void)
{
    for (uint8_t i = 0; i < LED_COUNT; i++) LED_Write(i, GPIO_PIN_SET);
}

/* ---------- 任务实现 ---------- */
void LEDTask(void *argument)
{
    (void)argument;
    uint8_t idx = 0;

    LED_AllOff();          /* 上电默认全灭 */

    for (;;)
    {
        /* 只点亮当前位置的 LED，其余熄灭 —— 流水效果 */
        for (uint8_t i = 0; i < LED_COUNT; i++)
        {
            LED_Write(i, (i == idx) ? GPIO_PIN_RESET : GPIO_PIN_SET);
        }

        idx = (uint8_t)((idx + 1) % LED_COUNT);

        osDelay(LED_DELAY);
    }
}

/* ---------- 初始化接口 ---------- */
void LEDTaskInit(void)
{
    LEDTaskHandle = osThreadNew(LEDTask, NULL, &LEDTask_attributes);
}
