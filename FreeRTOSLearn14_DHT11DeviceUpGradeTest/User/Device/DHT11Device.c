#include "Dht11Device.h"
#include "string.h"

stDht11DeviceParamTdf	astDht11DeviceParam[DHT11_DEV_NUM];

///	@brief				DHT11设备参数获取
///
///	@param			emDevNum		:	设备编号
///
///	@note
const stDht11DeviceParamTdf	*c_pstGetDht11DeviceParam(emDht11DevNumTdf emDevNum)
{
	return &astDht11DeviceParam[emDevNum];
}

///	@brief				拷贝运行参数
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vDht11DeviceRunningParamInit(stDht11RunningParamTdf *pstInit,emDht11DevNumTdf emDevNum)
{
		memcpy(&astDht11DeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stDht11RunningParamTdf)	/	sizeof(uint8_t));
}

///	@brief				设置引脚为输入模式
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vDht11SetPinDirInput(emDht11DevNumTdf emDevNum)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin			=	astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin;
	GPIO_InitStruct.Mode		=	GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull			=	GPIO_NOPULL;
	GPIO_InitStruct.Speed		=	GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,&GPIO_InitStruct);
}

///	@brief				设置引脚为输出模式
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vDht11SetPinDirOutput(emDht11DevNumTdf emDevNum)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin			=	astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin;
	GPIO_InitStruct.Mode		=	GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull			=	GPIO_NOPULL;
	GPIO_InitStruct.Speed		=	GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,&GPIO_InitStruct);
}

///	@brief				设置引脚为高电平
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vDht11SetPinHigh(emDht11DevNumTdf emDevNum)
{
	HAL_GPIO_WritePin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin,
										GPIO_PIN_SET);
}

///	@brief				设置引脚为低电平
///
///	@param			emDevNum		:	设备编号
///
///	@note
void vDht11SetPinLow(emDht11DevNumTdf emDevNum)
{
	HAL_GPIO_WritePin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin,
										GPIO_PIN_RESET);
}

///	@brief				延时2us
///
///	@note				仅在此文件生效
static void s_vDelay2Us(uint32_t n)
{
	uint32_t volatile i;
	for(i=0 ;i <n;i++)
	{
		__nop();
	}
}

///	@brief				发送开始信号
///
///	@param			emDevNum		:	设备编号
///
///	@note				仅在此文件生效
void vDht11Start(emDht11DevNumTdf emDevNum)
{
	vDht11SetPinDirOutput(emDevNum);
	
	vDht11SetPinLow(emDevNum);
	HAL_Delay(19);
	
	vDht11SetPinHigh(emDevNum);
	s_vDelay2Us(7);
}

///	@brief				等待设备回复
///
///	@note				
uint8_t ucDht11WaitForAck(emDht11DevNumTdf emDevNum)
{
	uint8_t ucCount;
	uint8_t ucPinValue;
	
	vDht11SetPinDirInput(emDevNum);
	
	//等待引脚被拉低
	ucCount = 0;
	ucPinValue = 1;
	while(ucPinValue == 1)
	{
		ucPinValue = HAL_GPIO_ReadPin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin);
		ucCount++;
		if(ucCount >= 1000)
		{
			return 0xFF;
		}
		s_vDelay2Us(1);
	}
	
	//等待引脚被拉高
	ucCount = 0;
	ucPinValue = 0;
	while(ucPinValue == 0)
	{
		ucPinValue = HAL_GPIO_ReadPin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin);
		ucCount++;
		if(ucCount >= 1000)
		{
			return 0xFF;
		}
		s_vDelay2Us(1);
	}
	
	//等待引脚被拉低
	ucCount = 0;
	ucPinValue = 1;
	while(ucPinValue == 1)
	{
		ucPinValue = HAL_GPIO_ReadPin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin);
		ucCount++;
		if(ucCount >= 1000)
		{
			return 0xFF;
		}
		s_vDelay2Us(1);
	}
	return 0;
}

///	@brief				读取单个Bit
///
///	@note				
uint8_t ucDht11ReadBit(emDht11DevNumTdf emDevNum)
{
	uint8_t ucLowLevelCount 		= 0;
	uint8_t ucHighLevelCount 	= 0;
	uint8_t ucPinValue	;
	
	vDht11SetPinDirInput(emDevNum);
	
	//等待引脚被拉低
	ucPinValue = 0;
	while(ucPinValue == 0)
	{
		ucPinValue = HAL_GPIO_ReadPin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin);
		ucLowLevelCount++;
		if(ucLowLevelCount >= 1000)
		{
			return 0xFF;
		}
		s_vDelay2Us(1);
	}
	//等待引脚被拉高
	ucPinValue = 1;
	while(ucPinValue == 1)
	{
		ucPinValue = HAL_GPIO_ReadPin(astDht11DeviceParam[emDevNum].stStaticParam.pstGpioBase,
										astDht11DeviceParam[emDevNum].stStaticParam.usGpioPin);
		ucHighLevelCount++;
		if(ucHighLevelCount >= 1000)
		{
			return 0xFF;
		}
		s_vDelay2Us(1);
	}
	
	if(ucLowLevelCount > ucHighLevelCount)
	{
		return  0;
	}
	else
	{
		return 1;
	}
}

///	@brief				读取结束
///
///	@note	
void vDht11Stop(emDht11DevNumTdf emDevNum)
{
	vDht11SetPinDirOutput(emDevNum);
	vDht11SetPinHigh(emDevNum);
}

///	@brief				读取数据
///
///	@note	
uint8_t ucDht11ReadData(emDht11DevNumTdf emDevNum)
{
	uint8_t ucError;
	uint8_t aucData[5] = {0};
	uint8_t ucTempBit;
	uint8_t i,j;
	
	vDht11Start(emDevNum);
	
	ucError = ucDht11WaitForAck(emDevNum);
	if(ucError != 0)
	{
		return ucError;
	}
	
	for(i = 0; i <5 ; i++)
	{
		for(j = 0; j< 8 ;j++)
		{
			ucTempBit = ucDht11ReadBit(emDevNum);
			if(ucTempBit == 0xFF)
			{
				return 0xFF;
			}
			aucData[i] <<= 1;
			aucData[i] += ucTempBit;
		}
	}
	
	//发送读取结束
	vDht11Stop(emDevNum);
	
	if(aucData[4] != (aucData[0] + aucData[1] + aucData[2] + aucData[3] ))
	{
		return 0xFF;
	}
	
	astDht11DeviceParam[emDevNum].stRunningParam.acHumidity[0] = aucData[0];
	astDht11DeviceParam[emDevNum].stRunningParam.acHumidity[1] = aucData[1];
	
	astDht11DeviceParam[emDevNum].stRunningParam.acTemperature[0] = aucData[2];
	astDht11DeviceParam[emDevNum].stRunningParam.acTemperature[1] = aucData[3];

	return 0 ;
}

///	@brief				执行 空闲状态
///
///	@param			emDevNum		:	设备编号
///
///	@note	
void vDht11StatusExecute_Idle(emDht11DevNumTdf emDevNum)
{
	const uint32_t c_ulCountThreshold = 2 * 1000 * 1000 / astDht11DeviceParam[emDevNum].stStaticParam.ulTimerPeriorUs;
	stDht11FsmParamTdf *pstFsmParam = &astDht11DeviceParam[emDevNum].stRunningParam.stFsmParam;
	
	pstFsmParam -> ulTimerCount++;
	
	if(pstFsmParam -> ulTimerCount >= c_ulCountThreshold)
	{
		//计数器清零
		pstFsmParam->ulTimerCount = 0;
		
		//状态转换
		pstFsmParam->emState = emDht11FsmState_StartLow;
		
		vDht11SetPinDirOutput(emDevNum);
		vDht11SetPinLow(emDevNum);
	}
}

///	@brief				状态机回调参数
///
///	@param			emDevNum		:	设备编号
///
///	@note				
void vDht11FsmCallback(emDht11DevNumTdf emDevNum)
{
	stDht11FsmParamTdf *pstFsmParam = &astDht11DeviceParam[emDevNum].stRunningParam.stFsmParam;
	
	switch(pstFsmParam -> emState)
	{
		case emDht11FsmState_Idle:
		{
			vDht11StatusExecute_Idle();
			break;
		}
	}
}

///	@brief				DHT11设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vDht11DeviceInit(stDht11StaticParamTdf *pstInit,emDht11DevNumTdf emDevNum)
{
	memcpy(&astDht11DeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stDht11StaticParamTdf)	/	sizeof(uint8_t));
}

