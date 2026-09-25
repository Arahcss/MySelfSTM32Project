#include "RingBufferDevice.h"
#include "string.h"

stRingBufferDeviceParamTdf	astRingBufferDeviceParam[RING_BUFFER_DEV_NUM];

const stRingBufferDeviceParamTdf	*c_pstGetRingBufferDeviceParam(emRingBufferDevNumTdf emDevNum)
{
	return &astRingBufferDeviceParam[emDevNum];
}
///	@brief			拷贝运行参数
///
///	@param		emDevNum :设备号
///
///	@note
void vRingBufferDeviceRunningParamInit(stRingBufferRunningParamTdf *pstInit,emRingBufferDevNumTdf emDevNum)
{
		memcpy(&astRingBufferDeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stRingBufferRunningParamTdf)	/	sizeof(uint8_t));
}

///	@brief			写单个元素
///
///	@param		pvElement	:要写入的首元素地址
///						emDevNum :设备号
///
///	@note			为了能在中断中使用，此函数不用memcpy
emRingBufferErrorCodeTdf emRingBufferWriteSingleElement(void *pvElement , emRingBufferDevNumTdf emDevNum)
{
	uint32_t i;
	
	//如果满了返回错误
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus == emRingBufferStatus_Full)
	{
		return emRingBufferError_WriteFull;
	}
	
	for(i = 0;i < astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength ; i++)
	{
		*((uint8_t *) (astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite) + i)   =   *((uint8_t *)(pvElement) + i) ;
	}
	
	//如果超出尾指针则更新尾指针
	astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite =	(void *) ((uint8_t * )astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite + 
																												astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength);
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite > astRingBufferDeviceParam[emDevNum].stStaticParam.pvTail)
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite = astRingBufferDeviceParam[emDevNum].stStaticParam.pvHead;
	}
	
	//更新状态
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite == astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead)
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus = emRingBufferStatus_Full;
	}
	else
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus = emRingBufferStatus_NotEmptyNotFull;		
	}
	
	return emRingBufferError_None;
}

///	@brief				RingBuffer初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vRingBufferDeviceInit(stRingBufferStaticParamTdf *pstInit,emRingBufferDevNumTdf emDevNum)
{
	memcpy(&astRingBufferDeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stRingBufferStaticParamTdf)	/	sizeof(uint8_t));
}
