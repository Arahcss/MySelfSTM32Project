#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Store.h"
#include "K.h"

int main(void)
{
	OLED_Init();
	K_Init();
	Store_Init();
	OLED_ShowString(1,1,"Flag");
	OLED_ShowString(2,1,"Data");
	
	while(1)
	{
		uint8_t K_Num = K_GetNum();
		if(K_Num==1)
		{
			Store_Data[1]++;
			Store_Data[2] += 2;
			Store_Data[3] += 3;
			Store_Save();
		}
			
		if(K_Num==2)
		{
			Store_Clear();
		}
		for(int i=0;i<4;i++)
			OLED_ShowHexNum(i+1,6,Store_Data[i],4);
	}
}
