#include "stm32f10x.h"                  // Device header

void M_SPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}

void M_SPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitValue);
}

void M_SPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitValue);
}

uint8_t M_SPI_R_MISO()
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}

void M_SPI_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_CPHA =SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL =SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial =7;
	SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;//八位数据帧
	SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;//高位先行
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);
	
	M_SPI_W_SS(1);
}

void M_SPI_Start(void)
{
	M_SPI_W_SS(0);
}

void M_SPI_Stop(void)
{
	M_SPI_W_SS(1);
}

uint8_t M_SPI_SwapByte(uint8_t ByteSend)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);//TXE SET等待
	
	SPI_I2S_SendData(SPI1,ByteSend);
	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);//监测RXNE

	return SPI_I2S_ReceiveData(SPI1);
}
