#include "InterpreterDevice.h"
#include "string.h"

stInterpreterDeviceParamTdf	astInterpreterDeviceParam[INTERPRETER_DEV_NUM];

///	@brief				INTERPRETER执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				根据模式执行不同操作
void vInterpreterDeviceExecute(emInterpreterDevNumTdf	emDevNum)
{
	vInterpreterDeviceBlinkExecute(emDevNum);
	switch(emDevNum)
	{
		
		default:
		{
			;
		}
	}
}

const stInterpreterDeviceParamTdf	*c_pstGetInterpreterDeviceParam(emInterpreterDevNumTdf emDevNum)
{
	return &astInterpreterDeviceParam[emDevNum];
}

void vInterpreterDeviceRunningParamInit(stInterpreterRunningParamTdf *pstInit,emInterpreterDevNumTdf emDevNum)
{
		memcpy(&astInterpreterDeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stInterpreterRunningParamTdf)	/	sizeof(uint8_t));
}


///	@brief				INTERPRETER设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vInterpreterDeviceInit(stInterpreterStaticParamTdf *pstInit,emInterpreterDevNumTdf emDevNum)
{
	memcpy(&astInterpreterDeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stInterpreterStaticParamTdf)	/	sizeof(uint8_t));
}


