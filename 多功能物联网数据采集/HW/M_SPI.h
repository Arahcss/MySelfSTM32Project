#ifndef __M_SPI_H
#define __M_SPI_H

void M_SPI_W_SS(uint8_t BitValue);
void M_SPI_W_SCK(uint8_t BitValue);
void M_SPI_W_MOSI(uint8_t BitValue);
uint8_t M_SPI_R_MISO(void);
void M_SPI_Init(void);
void M_SPI_Start(void);
void M_SPI_Stop(void);
uint8_t M_SPI_SwapByte(uint8_t ByteSend);





#endif
