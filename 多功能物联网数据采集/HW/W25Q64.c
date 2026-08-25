#include "stm32f10x.h"                  // Device header
#include "M_SPI.h"
#include "W25Q64_ins.h"

void W25Q64_Init(void)
{
	M_SPI_Init();
}

void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
{
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = M_SPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = M_SPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= M_SPI_SwapByte(W25Q64_DUMMY_BYTE);
	M_SPI_Stop();
}	

void W25Q64_WriteEable(void)
{
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_WRITE_ENABLE);
	M_SPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timout=100000;
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((M_SPI_SwapByte(W25Q64_DUMMY_BYTE)&0x01)==0x01)
		if(Timout-- ==0)
			break;
	
	M_SPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address,uint8_t *DataArray,uint16_t Count)
{
	W25Q64_WriteEable();
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_PAGE_PROGRAM);
	M_SPI_SwapByte(Address >> 16);
	M_SPI_SwapByte(Address >> 8);
	M_SPI_SwapByte(Address);//发送24位地址
	for(uint16_t i=0;i <Count ;i++)//发送写入数组
		M_SPI_SwapByte(DataArray[i]);
	
	M_SPI_Stop();
	W25Q64_WaitBusy();
}

void W25Q64_SectorErase(uint32_t Address)//扇区擦拭
{
	W25Q64_WriteEable();//扇区擦拭前写使能
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	M_SPI_SwapByte(Address >> 16);
	M_SPI_SwapByte(Address >> 8);
	M_SPI_SwapByte(Address);//发送24位地址
	M_SPI_Stop();
	W25Q64_WaitBusy();
}

void W25Q64_ReadData(uint32_t Address,uint8_t *DataArray,uint32_t Count)
{
	M_SPI_Start();
	M_SPI_SwapByte(W25Q64_READ_DATA);
	M_SPI_SwapByte(Address >> 16);
	M_SPI_SwapByte(Address >> 8);
	M_SPI_SwapByte(Address);//发送24位地址
	for(uint32_t i=0;i<Count;i++)
		DataArray[i]=M_SPI_SwapByte(W25Q64_DUMMY_BYTE);
	M_SPI_Stop();
}
