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
	s_vOledSclSet(emDevNum);
	s_vOledSdaReset(emDevNum);
	s_vOledSdaSet(emDevNum);
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
	s_vOledIicSendByte(0x0C,emDevNum);
	s_vOledIicWaitAck(emDevNum);
	for( i = 0 ; i < ulLength ; i++)
	{
		s_vOledIicSendByte(*(pucData + i),emDevNum);
		s_vOledIicWaitAck(emDevNum);
		
	}
	s_vOledIicStop(emDevNum);
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
}

