#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "K.h"

int main(void)
{
	OLED_Init();
	K_Init();
	
	OLED_ShowString(1,1,"WWDG TEST");
	
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST)==SET)
	{
		OLED_ShowString(2,1,"WWDGRST");
		Delay_ms(500);
		OLED_ShowString(2,1,"                ");
		Delay_ms(100);
		
		RCC_ClearFlag();//记得清除标志物
	}
	else
	{
		OLED_ShowString (3,1,"RST");
		Delay_ms(500);
		OLED_ShowString (3,1,"      ");
		Delay_ms(100);
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
	
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(0x40|21);//窗口值   30ms
	WWDG_Enable(0x40|54);//50ms
	
	while(1)
	{
		K_GetNum();//测试持续按住按键堵塞
		
		//WWDG_SetCounter(0x40|54);//窗口看门狗喂狗过快超过窗口值会导致循环复位
		
		OLED_ShowString(4,1,"FEED");
		Delay_ms(20);
		OLED_ShowString(4,1,"       ");
		Delay_ms(20);
		//Delay_ms(800);//符合喂狗时间内的延时
		WWDG_SetCounter(0x40|54);
	}
}
