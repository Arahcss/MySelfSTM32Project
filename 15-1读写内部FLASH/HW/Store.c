#include "stm32f10x.h"                  // Device header
#include "M_FLASH.h"

#define STORE_STATE_ADDRESS				0x0800FC00
#define STORE_COUNT								512

uint16_t Store_Data[STORE_COUNT];

void Store_Init(void)
{
	if(M_FLASH_ReadHalfWord(STORE_STATE_ADDRESS) != 0xA5A5)
	{
		M_FLASH_ErasePage(STORE_STATE_ADDRESS);
		M_FLASH_ProgramHalfWord(STORE_STATE_ADDRESS,0xA5A5);
		for(uint16_t i=1;i<512;i++)
		{
			M_FLASH_ProgramHalfWord(STORE_STATE_ADDRESS + i * 2,0x0000);
		}
	}
	
	for(uint16_t i=0;i<512;i++)
	{
		Store_Data[i] = M_FLASH_ReadHalfWord(STORE_STATE_ADDRESS +i * 2);
	}
}

void Store_Save(void)
{
	M_FLASH_ErasePage(STORE_STATE_ADDRESS);
	for(uint16_t i=0;i<512;i++)
	{
		M_FLASH_ProgramHalfWord(STORE_STATE_ADDRESS + i * 2 ,Store_Data[i]);
	}
}

void Store_Clear(void)
{
	for(uint16_t i=1 ;i<512;i++)
	{
		Store_Data[i]=M_FLASH_ReadHalfWord(STORE_STATE_ADDRESS +i *2);
	}
	Store_Save();
}

