#include "OLEDDevice.h"
#include "OLEDFont.h"
#include "string.h"

stOledDeviceParamTdf	astOledDeviceParam[OLED_DEV_NUM];

///	@brief				SDA引脚置高
///
///	@param			emDevNum		:	设备编号
///
///	@note				直接操作BSRR寄存器，速度快
static void s_vOledSdaSet(emOledDevNumTdf emDevNum)
{
	astOledDeviceParam[emDevNum].stStaticParam.pstSdaGpioBase->BSRR = astOledDeviceParam[emDevNum].stStaticParam.usSdaGpioPin;
}

///	@brief				SDA引脚置低
///
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledSdaReset(emOledDevNumTdf emDevNum)
{
	astOledDeviceParam[emDevNum].stStaticParam.pstSdaGpioBase->BSRR = (uint32_t)astOledDeviceParam[emDevNum].stStaticParam.usSdaGpioPin << 16u;
}

///	@brief				SCL引脚置高
///
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledSclSet(emOledDevNumTdf emDevNum)
{
	astOledDeviceParam[emDevNum].stStaticParam.pstSclGpioBase->BSRR = astOledDeviceParam[emDevNum].stStaticParam.usSclGpioPin;
}

///	@brief				SCL引脚置低
///
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledSclReset(emOledDevNumTdf emDevNum)
{
	astOledDeviceParam[emDevNum].stStaticParam.pstSclGpioBase->BSRR = (uint32_t)astOledDeviceParam[emDevNum].stStaticParam.usSclGpioPin << 16u;
}

///	@brief				IIC起始信号
///
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledIicStart(emOledDevNumTdf emDevNum)
{
	s_vOledSdaSet(emDevNum);
	s_vOledSclSet(emDevNum);
	s_vOledSdaReset(emDevNum);
	s_vOledSclReset(emDevNum);
}

///	@brief				IIC停止信号
///
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledIicStop(emOledDevNumTdf emDevNum)
{
	s_vOledSclReset(emDevNum);		//确保SCL为低
	s_vOledSdaReset(emDevNum);		//SDA拉低
	s_vOledSclSet(emDevNum);		//SCL拉高
	s_vOledSdaSet(emDevNum);		//SDA拉高→停止条件
}

///	@brief				IIC等待应答
///
///	@param			emDevNum		:	设备编号
///
///	@note				主机产生第9个时钟，不读取从机应答
static void s_vOledIicWaitAck(emOledDevNumTdf emDevNum)
{
	s_vOledSclReset(emDevNum);
	s_vOledSdaReset(emDevNum);
	s_vOledSclSet(emDevNum);
	s_vOledSclReset(emDevNum);
}

///	@brief				IIC发送一字节
///
///	@param			ucData			:	待发送数据
///	@param			emDevNum		:	设备编号
///
///	@note
static void s_vOledIicSendByte(uint8_t ucData, emOledDevNumTdf emDevNum)
{
	uint8_t i;

	for(i = 0; i < 8; i++)
	{
		s_vOledSclReset(emDevNum);
		if((ucData & 0x80) != 0)
		{
			s_vOledSdaSet(emDevNum);
		}
		else
		{
			s_vOledSdaReset(emDevNum);
		}
		s_vOledSclSet(emDevNum);
		s_vOledSclReset(emDevNum);
		ucData <<= 1;
	}
}

///	@brief				向OLED写一字节命令
///
///	@param			ucCmd			:	命令字节
///	@param			emDevNum		:	设备编号
///
///	@note				控制字节0x00表示后续为命令
static void s_vOledWriteOneByteCmd(uint8_t ucCmd, emOledDevNumTdf emDevNum)
{
	s_vOledIicStart(emDevNum);
	s_vOledIicSendByte(0x78, emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(0x00, emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(ucCmd, emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicStop(emDevNum);
}

///	@brief				向OLED写多字节数据
///
///	@param			pucData			:	数据首地址
///	@param			ulLength		:	数据长度
///	@param			emDevNum		:	设备编号
///
///	@note				控制字节0x40表示后续为显存数据
static void s_vOledWriteFewBytesData(uint8_t *pucData, uint32_t ulLength, emOledDevNumTdf emDevNum)
{
	uint32_t i;

	s_vOledIicStart(emDevNum);
	s_vOledIicSendByte(0x78, emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(0x40, emDevNum);
	s_vOledIicWaitAck(emDevNum);
	for(i = 0; i < ulLength; i++)
	{
		s_vOledIicSendByte(*(pucData + i), emDevNum);
		s_vOledIicWaitAck(emDevNum);
	}
	s_vOledIicStop(emDevNum);
}

///	@brief				SSD1306命令初始化序列
///
///	@param			emDevNum		:	设备编号
///
///	@note				上电后执行一次
static void s_vOledCmdInit(emOledDevNumTdf emDevNum)
{
	s_vOledWriteOneByteCmd(0xAE, emDevNum);
	s_vOledWriteOneByteCmd(0x00, emDevNum);
	s_vOledWriteOneByteCmd(0x10, emDevNum);
	s_vOledWriteOneByteCmd(0x40, emDevNum);
	s_vOledWriteOneByteCmd(0x81, emDevNum);
	s_vOledWriteOneByteCmd(0xCF, emDevNum);
	s_vOledWriteOneByteCmd(0xA1, emDevNum);
	s_vOledWriteOneByteCmd(0xC8, emDevNum);
	s_vOledWriteOneByteCmd(0xA6, emDevNum);
	s_vOledWriteOneByteCmd(0xA8, emDevNum);
	s_vOledWriteOneByteCmd(0x3F, emDevNum);
	s_vOledWriteOneByteCmd(0xD3, emDevNum);
	s_vOledWriteOneByteCmd(0x00, emDevNum);
	s_vOledWriteOneByteCmd(0xD5, emDevNum);
	s_vOledWriteOneByteCmd(0x80, emDevNum);
	s_vOledWriteOneByteCmd(0xD9, emDevNum);
	s_vOledWriteOneByteCmd(0xF1, emDevNum);
	s_vOledWriteOneByteCmd(0xDA, emDevNum);
	s_vOledWriteOneByteCmd(0x12, emDevNum);
	s_vOledWriteOneByteCmd(0xDB, emDevNum);
	s_vOledWriteOneByteCmd(0x40, emDevNum);
	s_vOledWriteOneByteCmd(0x20, emDevNum);
	s_vOledWriteOneByteCmd(0x02, emDevNum);
	s_vOledWriteOneByteCmd(0x8D, emDevNum);
	s_vOledWriteOneByteCmd(0x14, emDevNum);
	s_vOledWriteOneByteCmd(0xA4, emDevNum);
	s_vOledWriteOneByteCmd(0xA6, emDevNum);
	s_vOledWriteOneByteCmd(0xAF, emDevNum);
}

///	@brief				向显存指定位置画点
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			emDevNum		:	设备编号
///
///	@note				显存按page(8行)×列组织
void vOledDrawOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum)
{
	uint16_t usPage;
	uint16_t usSeg;
	uint16_t usMask;

	//计算目标page
	usPage = ulY >> 3;
	//计算目标seg
	usSeg = ulY & 0x07;
	//计算对应seg掩码
	usMask = 0x01 << usSeg;

	astOledDeviceParam[emDevNum].stRunningParam.aucOledBuffer[usPage * OLED_BUFFER_WIDTH + ulX] |= usMask;
}

///	@brief				向显存指定位置清点
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			emDevNum		:	设备编号
void vOledClearOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum)
{
	uint16_t usPage;
	uint16_t usSeg;
	uint16_t usMask;

	usPage = ulY >> 3;
	usSeg = ulY & 0x07;
	usMask = 0x01 << usSeg;

	astOledDeviceParam[emDevNum].stRunningParam.aucOledBuffer[usPage * OLED_BUFFER_WIDTH + ulX] &= ~usMask;
}

///	@brief				清空显存
///
///	@param			emDevNum		:	设备编号
void vOledClearBuffer(emOledDevNumTdf emDevNum)
{
	memset(astOledDeviceParam[emDevNum].stRunningParam.aucOledBuffer, 0,
			OLED_BUFFER_WIDTH * OLED_BUFFER_HEIGHT);
}

///	@brief				向显存写入一个字符
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			ucChar			:	ASCII字符
///	@param			emFontSize		:	字号
///	@param			emMode			:	正显/反显
///	@param			emDevNum		:	设备编号
///
///	@note				按列扫描字模，逐点写入显存
void vOledWriteOneCharToBuffer(uint32_t ulX, uint32_t ulY, uint8_t ucChar,
		emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode, emOledDevNumTdf emDevNum)
{
	uint16_t i;
	uint16_t j;
	uint16_t usY0;
	uint32_t ulSizeTemp;
	uint32_t ulCharIndex;
	uint8_t ucData;
	const uint8_t *p;

	ulSizeTemp = ((emFontSize >> 3) + ((emFontSize & 0x07) ? 1 : 0)) * (emFontSize >> 1);
	ulCharIndex = ucChar - ' ';
	usY0 = ulY;

	//匹配字模
	if(emFontSize == emOledFontSize_6x12)
	{
		p = c_a2ucOledFont_Ascii_1206[0];
	}
	else if(emFontSize == emOledFontSize_8x16)
	{
		p = c_a2ucOledFont_Ascii_1608[0];
	}
	else if(emFontSize == emOledFontSize_12x24)
	{
		p = c_a2ucOledFont_Ascii_2412[0];
	}
	else
	{
		return;
	}

	for(i = 0; i < ulSizeTemp; i++)
	{
		ucData = p[ulCharIndex * ulSizeTemp + i];

		for(j = 0; j < 8; j++)
		{
			if((ucData & 0x80) != 0)
			{
				if(emMode == emOledPixelShowMode_Positive)
				{
					vOledDrawOnePointToBuffer(ulX, ulY, emDevNum);
				}
				else
				{
					vOledClearOnePointToBuffer(ulX, ulY, emDevNum);
				}
			}
			else
			{
				if(emMode == emOledPixelShowMode_Positive)
				{
					vOledClearOnePointToBuffer(ulX, ulY, emDevNum);
				}
				else
				{
					vOledDrawOnePointToBuffer(ulX, ulY, emDevNum);
				}
			}

			ucData <<= 1;
			ulY++;
			if((ulY - usY0) == (uint16_t)emFontSize)
			{
				ulY = usY0;
				ulX++;
				break;
			}
		}
	}
}

///	@brief				向显存写入字符串
///
///	@param			usX				:	x坐标
///	@param			usY				:	y坐标
///	@param			c_pucChar		:	字符串首地址
///	@param			emFontSize		:	字号
///	@param			emMode			:	正显/反显
///	@param			emDevNum		:	设备编号
///
///	@note				自动换行
void vOledWriteStringToBuffer(uint16_t usX, uint16_t usY, const uint8_t *c_pucChar,
		emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode, emOledDevNumTdf emDevNum)
{
	while((*c_pucChar >= ' ') && (*c_pucChar <= '~'))
	{
		vOledWriteOneCharToBuffer(usX, usY, *c_pucChar, emFontSize, emMode, emDevNum);

		usX += emFontSize >> 1;

		if(usX > (OLED_POINT_WIDTH - emFontSize))
		{
			usX = 0;
			usY += emFontSize;
		}

		c_pucChar++;
	}
}

///	@brief				画一条水平虚线入显存
///
///	@param			usX				:	起始x坐标
///	@param			usY				:	y坐标
///	@param			ulLength		:	虚线长度（像素）
///	@param			emDevNum		:	设备编号
void vOledDrawDotLine(uint16_t usX, uint16_t usY, uint32_t ulLength, emOledDevNumTdf emDevNum)
{
	uint32_t i;

	for(i = 0; i < ulLength; i++)
	{
		if((i & 0x01) == 0)
		{
			vOledDrawOnePointToBuffer(usX + i, usY, emDevNum);
		}
		else
		{
			vOledClearOnePointToBuffer(usX + i, usY, emDevNum);
		}
	}
}

///	@brief				将显存刷新到OLED
///
///	@param			emDevNum		:	设备编号
///
///	@note				按page逐行写入
void vOledRefreshFromBuffer(emOledDevNumTdf emDevNum)
{
	uint8_t i;

	for(i = 0; i < OLED_BUFFER_HEIGHT; i++)
	{
		s_vOledWriteOneByteCmd(0xB0 + i, emDevNum);		//设置页起始地址
		s_vOledWriteOneByteCmd(0x00, emDevNum);			//设置低列起始地址
		s_vOledWriteOneByteCmd(0x10, emDevNum);			//设置高列起始地址

		s_vOledWriteFewBytesData(&(astOledDeviceParam[emDevNum].stRunningParam.aucOledBuffer[i * OLED_BUFFER_WIDTH]),
				OLED_BUFFER_WIDTH, emDevNum);
	}
}

///	@brief				获取OLED设备参数（只读）
///
///	@param			emDevNum		:	设备编号
///
///	@return			设备参数const指针
const stOledDeviceParamTdf *c_pstGetOledDeviceParam(emOledDevNumTdf emDevNum)
{
	return &astOledDeviceParam[emDevNum];
}

///	@brief				OLED运行参数初始化
///
///	@param			pstInit			:	运行参数结构体首地址
///	@param			emDevNum		:	设备编号
///
///	@note
void vOledDeviceRunningParamInit(stOledRunningParamTdf *pstInit, emOledDevNumTdf emDevNum)
{
	memcpy(&astOledDeviceParam[emDevNum].stRunningParam, pstInit, sizeof(stOledRunningParamTdf) / sizeof(uint8_t));
}

///	@brief				OLED设备初始化
///
///	@param			pstInit			:	静态参数结构体首地址
///	@param			emDevNum		:	设备编号
///
///	@note				登记GPIO并执行SSD1306命令初始化
void vOledDeviceInit(stOledStaticParamTdf *pstInit, emOledDevNumTdf emDevNum)
{
	memcpy(&astOledDeviceParam[emDevNum].stStaticParam, pstInit, sizeof(stOledStaticParamTdf) / sizeof(uint8_t));

	s_vOledCmdInit(emDevNum);
}
