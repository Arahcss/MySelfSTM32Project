#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
int main(void)
{
	OLED_Init();

	while(1)
	{
		OLED_ShowString(1,1,"6");
		OLED_ShowString(2,1,"6");
		OLED_ShowString(3,1,"6");
		OLED_ShowString(4,1,"6");
	}
}
