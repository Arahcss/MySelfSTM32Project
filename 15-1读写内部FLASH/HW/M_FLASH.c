#include "stm32f10x.h"                  // Device header

uint32_t M_FLASH_ReadWord(uint32_t Address)
{
	return *((__IO uint32_t*)(Address));
}


uint16_t M_FLASH_ReadHalfWord(uint32_t Address)
{
	return *((__IO uint16_t*)(Address));
}

uint8_t M_FLASH_ReadByte(uint32_t Address)
{
	return *((__IO uint8_t*)(Address));
}

void M_FLASH_EraseALLPages(void)
{
	FLASH_Unlock();
	FLASH_EraseAllPages();
	FLASH_Lock();
}

void M_FLASH_ErasePage(uint32_t PageAddress)
{
	FLASH_Unlock();
	FLASH_ErasePage(PageAddress);
	FLASH_Lock();
}

void M_FLASH_ProgramWord(uint32_t Address,uint32_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramWord(Address,Data);
	FLASH_Lock();
}

void M_FLASH_ProgramHalfWord(uint32_t Address,uint32_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramHalfWord(Address,Data);
	FLASH_Lock();
}
