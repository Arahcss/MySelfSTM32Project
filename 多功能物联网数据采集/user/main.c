#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Menu.h"
int main(void)
{
	OLED_Init();

	while(1)
	{
		Menu_Start();
	}
}
