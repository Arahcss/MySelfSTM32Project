#include "InterpreterDevice.h"
#include "string.h"

stInterpreterDeviceParamTdf	astInterpreterDeviceParam[INTERPRETER_DEV_NUM];

///	@brief				INTERPRETER执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				根据模式执行不同操作
void vInterpreterDeviceExecute(char *pcCmdLine,emInterpreterDevNumTdf	emDevNum)
{
	char *apcString[CMD_INTERPRETER_DILIVERY_NUM_MAX];		//指针数组，存储分割好的字符串首地址		=	{0};
	uint8_t ucDeLiveryStringNum 	=	0;												//分割好的字符串数量
	uint32_t i;
	
	//1分割字符串
	apcString[ucDeLiveryStringNum] = strtok(pcCmdLine,CMD_INTERPRETER_DILIVER_STRING);
	
	while(1)
	{
		apcString[ucDeLiveryStringNum] = strtok(0 , CMD_INTERPRETER_DILIVER_STRING);
		if(apcString[ucDeLiveryStringNum] == 0)
		{
			break;
		}
		ucDeLiveryStringNum++;
	}
	
	//2匹配字符串
	for(i = 0 ; i < astInterpreterDeviceParam[emDevNum].stStaticParam.ulListSize;i++)
	{
		if((ucDeLiveryStringNum - 2) != astInterpreterDeviceParam[emDevNum].stStaticParam.pstList -> c_ucParamNum)
		{
			continue;
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


