#include "OledDevice.h"
#include "string.h"

stOledDeviceParamTdf	astOledDeviceParam[OLED_DEV_NUM];

///	@brief				SDA引脚置高
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledSdaSet(emOledDevNumTdf	emDevNum)
{
	HAL_GPIO_WritePin(	astOledDeviceParam[emDevNum].stStaticParam.pstSdaGpioBase,
										astOledDeviceParam[emDevNum].stStaticParam.usSdaGpioPin,
										GPIO_PIN_SET);
}

///	@brief				SDA引脚置低
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledSdaReset(emOledDevNumTdf	emDevNum)
{
	HAL_GPIO_WritePin(	astOledDeviceParam[emDevNum].stStaticParam.pstSdaGpioBase,
										astOledDeviceParam[emDevNum].stStaticParam.usSdaGpioPin,
										GPIO_PIN_RESET);
}

///	@brief				SCL引脚置高
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledSclSet(emOledDevNumTdf	emDevNum)
{
	HAL_GPIO_WritePin(	astOledDeviceParam[emDevNum].stStaticParam.pstSclGpioBase,
										astOledDeviceParam[emDevNum].stStaticParam.usSclGpioPin,
										GPIO_PIN_SET);
}

///	@brief				SCL引脚置低
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledSclReset(emOledDevNumTdf	emDevNum)
{
	HAL_GPIO_WritePin(	astOledDeviceParam[emDevNum].stStaticParam.pstSclGpioBase,
										astOledDeviceParam[emDevNum].stStaticParam.usSclGpioPin,
										GPIO_PIN_RESET);
}

///	@brief				IIC起始
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledIicStart(emOledDevNumTdf	emDevNum)
{
	s_vOledSdaSet(emDevNum);
	s_vOledSclSet(emDevNum);
	s_vOledSdaReset(emDevNum);
	s_vOledSclReset(emDevNum);
}

///	@brief				IIC停止
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledIicStop(emOledDevNumTdf	emDevNum)
{
	s_vOledSclReset(emDevNum); // 确保 SCL 为低
	s_vOledSdaReset(emDevNum); // SDA 拉低
	s_vOledSclSet(emDevNum);   // SCL 拉高
	s_vOledSdaSet(emDevNum);   // SDA 拉高 → 停止条件
}

///	@brief				IIC等待应答
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledIicWaitAck(emOledDevNumTdf	emDevNum)
{
	s_vOledSclSet(emDevNum);
	s_vOledSclReset(emDevNum);
}

///	@brief				IIC发送一字节
///
///	@param			emDevNum		:	设备编号
///
///	@note				
static void s_vOledIicSendByte(uint8_t ucData , emOledDevNumTdf	emDevNum)
{
	uint8_t i;
	for(i = 0; i < 8 ;i++)
	{
		s_vOledSclReset(emDevNum);
		if((ucData & 0x80 ) != 0)
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
///	@param			ucCmd				:	要写入的命令
//							emDevNum		:	设备编号
///
///	@note				
static void s_vOledWriteOneByteCmd(uint8_t ucCmd , emOledDevNumTdf	emDevNum)
{
	s_vOledIicStart(emDevNum);
	s_vOledIicSendByte(0x78,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(0x00,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(ucCmd,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicStop(emDevNum);
}

///	@brief				向OLED写多字节命令
///
///	@param			ucCmd				:	要写入的命令
///							emDevNum		:	设备编号
///							ulLength			:	数据长度
///
///	@note				
static void s_vOledWriteFewbytesData(uint8_t *pucData ,uint32_t ulLength, emOledDevNumTdf	emDevNum)
{
	uint32_t i;
	
	
	s_vOledIicStart(emDevNum);
	s_vOledIicSendByte(0x78,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	s_vOledIicSendByte(0x40,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	for( i = 0 ; i < ulLength ; i++)
	{
		s_vOledIicSendByte(*(pucData + i),emDevNum);
		s_vOledIicWaitAck(emDevNum);
		
	}
	s_vOledIicStop(emDevNum);
}

///	@brief				更新显存显示
///
///	@param			emDevNum		:	设备编号
///
///	@note				
void vOledRefreshFromBuffer( emOledDevNumTdf	emDevNum)
{
	uint8_t i;
	
	for(i = 0 ; i < OLED_BUFFER_HEIGHT;i++ )
	{
		s_vOledWriteOneByteCmd(0xB0 + i,emDevNum);			//设置行起始地址
		s_vOledWriteOneByteCmd(0x00,emDevNum);				//设置低列起始地址
		s_vOledWriteOneByteCmd(0x10,emDevNum);			//设置高列起始地址
	
		s_vOledWriteFewbytesData(&(astOledDeviceParam[emDevNum].stRunningParam.aucOledBuffer[i * OLED_BUFFER_WIDTH]),
													OLED_BUFFER_WIDTH,
													emDevNum);
	}
}

///	@brief				OLED命令初始化
///
///	@param			emDevNum		:	设备编号
///
///	@note				
void vOledCmdInit( emOledDevNumTdf	emDevNum)
{
	s_vOledWriteOneByteCmd(0xAE,emDevNum);
	s_vOledWriteOneByteCmd(0x00,emDevNum);
	s_vOledWriteOneByteCmd(0x10,emDevNum);
	s_vOledWriteOneByteCmd(0x40,emDevNum);
	s_vOledWriteOneByteCmd(0x81,emDevNum);
	s_vOledWriteOneByteCmd(0xCF,emDevNum);
	s_vOledWriteOneByteCmd(0xA1,emDevNum);
	s_vOledWriteOneByteCmd(0xC8,emDevNum);
	s_vOledWriteOneByteCmd(0xA6,emDevNum);
	s_vOledWriteOneByteCmd(0xA8,emDevNum);
	s_vOledWriteOneByteCmd(0x3F,emDevNum);
	s_vOledWriteOneByteCmd(0xD3,emDevNum);
	s_vOledWriteOneByteCmd(0x00,emDevNum);
	s_vOledWriteOneByteCmd(0xD5,emDevNum);
	s_vOledWriteOneByteCmd(0x80,emDevNum);
	s_vOledWriteOneByteCmd(0xD9,emDevNum);
	s_vOledWriteOneByteCmd(0xF1,emDevNum);
	s_vOledWriteOneByteCmd(0xDA,emDevNum);
	s_vOledWriteOneByteCmd(0x12,emDevNum);
	s_vOledWriteOneByteCmd(0xDB,emDevNum);
	s_vOledWriteOneByteCmd(0x40,emDevNum);
	s_vOledWriteOneByteCmd(0x20,emDevNum);
	s_vOledWriteOneByteCmd(0x02,emDevNum);
	s_vOledWriteOneByteCmd(0x8D,emDevNum);
	s_vOledWriteOneByteCmd(0x14,emDevNum);
	s_vOledWriteOneByteCmd(0xA4,emDevNum);
	s_vOledWriteOneByteCmd(0xA6,emDevNum);
	s_vOledWriteOneByteCmd(0xAF,emDevNum);
}

///	@brief				OLED周期执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				根据模式执行不同操作
void vOledDevicePeriodExecute(emOledDevNumTdf	emDevNum)
{
	
}

const stOledDeviceParamTdf	*c_pstGetOledDeviceParam(emOledDevNumTdf emDevNum)
{
	return &astOledDeviceParam[emDevNum];
}

void vOledDeviceRunningParamInit(stOledRunningParamTdf *pstInit,emOledDevNumTdf emDevNum)
{
		memcpy(&astOledDeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stOledRunningParamTdf)	/	sizeof(uint8_t));
}

///	@brief				OLED设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vOledDeviceInit(stOledStaticParamTdf *pstInit,emOledDevNumTdf emDevNum)
{
	memcpy(&astOledDeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stOledStaticParamTdf)	/	sizeof(uint8_t));
	
	vOledCmdInit(emDevNum);
}

