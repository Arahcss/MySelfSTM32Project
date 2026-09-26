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
	
	//如果为满返回错误
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus == emRingBufferStatus_Empty)
	{
		return emRingBufferError_WriteFull;
	}
	
	//读取数据
	for(i = 0;i < astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength ; i++)
	{
		*((uint8_t *) (astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite) + i)   =   *((uint8_t *)(pvElement) + i) ;
	}
	
	//如果超出尾指针则更新头指针
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

///	@brief			读取单个元素
///
///	@param		pvElement	:要写入的首元素地址
///						emDevNum :设备号
///
///	@note			
emRingBufferErrorCodeTdf emRingBufferReadSingleElement(void *pvElement , emRingBufferDevNumTdf emDevNum)
{
	uint32_t i;
	
	//如果为空返回错误
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus == emRingBufferStatus_Empty)
	{
		return emRingBufferError_ReadEmpty;
	}
	
	//读取数据
	for(i = 0;i < astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength ; i++)
	{
		*((uint8_t *)(pvElement) + i)   =   *((uint8_t *) (astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite) + i) ;
	}
	
	//如果超出尾指针则更新头指针
	astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead  =	(void *) ((uint8_t * )astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead + 
																												astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength);
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead  > astRingBufferDeviceParam[emDevNum].stStaticParam.pvTail)
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead  = astRingBufferDeviceParam[emDevNum].stStaticParam.pvHead;
	}
	
	//更新状态
	if(astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite == astRingBufferDeviceParam[emDevNum].stRunningParam.pvRead)
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus = emRingBufferStatus_Empty;
	}
	else
	{
		astRingBufferDeviceParam[emDevNum].stRunningParam.emStatus = emRingBufferStatus_NotEmptyNotFull;		
	}
	
	return emRingBufferError_None;
}

///	@brief				查找单个元素首次出现位置
///
///	@param			c_pvTargetElement		:	要匹配的目标元素地址
///	@param			emDevNum					:	设备编号
///
///	@retval			元素首次出现的位置
///
///	@note			
void *pvRingBufferFindElementFirstPosition(void *c_pvTargetElement,emRingBufferDevNumTdf emDevNum)
{
	void *p = astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite;
	while(p != astRingBufferDeviceParam[emDevNum].stRunningParam.pvWrite)
	{
		if(memcmp(p,c_pvTargetElement,astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength) == 0)
		{
			return p;
		}
		
		p = (void *) ((uint8_t *)p+ astRingBufferDeviceParam[emDevNum].stStaticParam.ulElementLength);
		if(p > astRingBufferDeviceParam[emDevNum].stStaticParam.pvTail)
		{
			p = astRingBufferDeviceParam[emDevNum].stStaticParam.pvHead;
		}
	}
	
	return NULL;
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
