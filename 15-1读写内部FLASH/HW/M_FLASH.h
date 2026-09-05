#ifndef __M_FLASH_H
#define __M_FLASH_H

uint32_t M_FLASH_ReadWord(uint32_t Address);
uint16_t M_FLASH_ReadHalfWord(uint32_t Address);
uint8_t M_FLASH_ReadByte(uint32_t Address);
void M_FLASH_EraseALLPages(void);
void M_FLASH_ErasePage(uint32_t PageAddress);
void M_FLASH_ProgramWord(uint32_t Address,uint32_t Data);
void M_FLASH_ProgramHalfWord(uint32_t Address,uint32_t Data);

#endif
