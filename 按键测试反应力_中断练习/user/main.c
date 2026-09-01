#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "CheckButton.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include  <Game.h>

int main(void)
{
	OLED_Init();
	CheckButton_Init();
	
	while(1)
	{
		GameStrat();
	}
}
