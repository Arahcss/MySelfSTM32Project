#include "stm32f10x.h"
#include "Delay.h"  // 只需要微秒级的精准延时，用于初始化，不再用于等待

// 按键状态机变量
#define KEY_STATE_IDLE     0   // 空闲状态
#define KEY_STATE_PRESSED  1   // 已按下（消抖中或持续按下）
#define KEY_STATE_RELEASED 2   // 已释放（等待判断是单击还是双击）

#define KEY_SHAKE_TIME     5   // 消抖时间，单位：循环周期（约5ms）
#define KEY_LONG_PRESS_TIME 80 // 长按时间，单位：循环周期（约200ms = 1秒）
#define KEY_DOUBLE_TIME    45  // 双击间隔时间，单位：循环周期

// 全局变量，供主循环调用
  // 0=无动作，1=单击，2=双击，3=长按
uint8_t KeyAction = 0; 
// 按键扫描函数，在主循环中周期性调用（建议每5ms调用一次）
uint8_t Key_Scan(void)
{
	uint8_t KeyAction = 0; 
    static uint8_t state = KEY_STATE_IDLE;
    static uint16_t press_counter = 0;
    static uint16_t release_counter = 0;
    static uint8_t is_first_click = 0;  // 标记是否已经有一次单击
    
    uint8_t key_pin = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
    
    switch (state)
    {
        case KEY_STATE_IDLE:  // 空闲状态，检测按键是否按下
            if (key_pin == 0)  // 按下（低电平有效）
            {
                press_counter = 0;
                state = KEY_STATE_PRESSED;
            }
            break;
            
        case KEY_STATE_PRESSED:  // 按下状态，检测是长按还是释放
            if (key_pin == 0)  // 仍然按着
            {
                press_counter++;
                if (press_counter >= KEY_LONG_PRESS_TIME)  // 达到长按时间
                {
                    KeyAction = 3;  // 长按事件
                    is_first_click = 0;  // 清空单击标记
                    // 等待按键释放（但不阻塞，状态转移到释放状态）
                    state = KEY_STATE_RELEASED;
                }
            }
            else  // 按键已释放（可能是单击或双击的第一次）
            {
                // 检查是否满足最短按下时间（消抖）
                if (press_counter >= KEY_SHAKE_TIME)
                {
                    // 如果是第一次单击
                    if (is_first_click == 0)
                    {
                        is_first_click = 1;
                        release_counter = 0;
                        state = KEY_STATE_RELEASED;
                    }
                    else  // 这是第二次单击，说明双击成立
                    {
                        KeyAction = 2;  // 双击事件
                        is_first_click = 0;
                        state = KEY_STATE_IDLE;
                    }
                }
                else  // 按的时间太短，视为抖动，忽略
                {
                    is_first_click = 0;
                    state = KEY_STATE_IDLE;
                }
            }
            break;
            
        case KEY_STATE_RELEASED:  // 释放状态，等待双击超时或新的按键
            if (key_pin == 0)  // 如果又按下了，说明是双击的第二次按下
            {
                // 检查是否在有效双击时间内
                if (release_counter < KEY_DOUBLE_TIME)
                {
                    // 但这需要等第二次释放才能真正确认是双击，先让状态回到PRESSED
                    // 并且标记已经有一次单击
                    // 简单起见，这里可以直接触发双击事件
                    KeyAction = 2;
                    is_first_click = 0;
                    state = KEY_STATE_IDLE;
                }
                else  // 超时后按下，当作新的单击开始
                {
                    // 先触发单击事件
                    if (is_first_click == 1)
                    {
                        KeyAction = 1;
                        is_first_click = 0;
                    }
                    // 然后进入新的按下状态
                    press_counter = 0;
                    state = KEY_STATE_PRESSED;
                }
            }
            else  // 按键持续释放中
            {
                release_counter++;
                if (release_counter >= KEY_DOUBLE_TIME)  // 双击超时
                {
                    if (is_first_click == 1)  // 超时前没有再按，确认为单击
                    {
                        KeyAction = 1;
                        is_first_click = 0;
                    }
                    state = KEY_STATE_IDLE;
                }
            }
            break;
            
        default:
            state = KEY_STATE_IDLE;
            break;
    }
	return KeyAction;
}

void K_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  // 注意：这里应该是 GPIOA 还是 GPIOB？根据你的硬件来
}
