#include "stm32f10x.h"

void PWM_Init(void)  // 舵机专用初始化
{
    // 1. 开启 TIM3 和 GPIOA 时钟（TIM3 是 APB1 总线，GPIOA 是 APB2）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置 PA6 为复用推挽输出（对应 TIM3_CH1）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;      // ✅ 接到 PA6
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 配置时基（依旧是 50Hz，周期 20ms）
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;   // ARR = 19999
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;   // PSC = 71
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    // 4. 配置输出比较通道1（对应 PA6）
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500;   // 初始归中（90°）
    TIM_OC1Init(TIM3, &TIM_OCInitStructure); // ✅ 使用 OC1

    // 5. 使能定时器
    TIM_Cmd(TIM3, ENABLE);
}

// 设置舵机角度，Compare 值范围 500~2500（对应 0°~180°）
void PWM_SetCompare1(float Angle)
{
    TIM_SetCompare1(TIM3, Angle /180*2000+500); // ✅ 使用 SetCompare1
}
