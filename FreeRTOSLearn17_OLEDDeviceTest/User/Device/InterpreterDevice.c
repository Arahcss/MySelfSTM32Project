#include "InterpreterDevice.h"
#include "string.h"

stInterpreterDeviceParamTdf	astInterpreterDeviceParam[INTERPRETER_DEV_NUM];

///	@brief				字符串转换为uint32_t型
///
///	@note		
static uint32_t s_ulStringToUint32(char *str)
{
	uint32_t ulResult = 0;
	
	while((*str >= '0') && (*str <= '9'))
	{
		ulResult *= 10;
		ulResult += *str - '0';
		str++;
	}
	return  ulResult;
}

///	@brief				INTERPRETER执行
///
///	@param			emDevNum		:	设备编号
///
///	@note				根据模式执行不同操作
void vInterpreterDeviceExecute(char *pcCmdLine,emInterpreterDevNumTdf	emDevNum)
{
	char *apcString[CMD_INTERPRETER_DILIVERY_NUM_MAX] = {0};		//指针数组，存储分割好的字符串首地址		=	{0};
	char *apcParam[(CMD_INTERPRETER_DILIVERY_NUM_MAX - 2)*2] = {0};
	uint8_t ucDeLiveryStringNum 	=	0;												//分割好的字符串数量
	uint32_t i,j;
	uint32_t ulParam,ulParamMax,ulParamMin;
	char acParamStringBuffer[50];
	char *p;
	
	//1分割字符串
	apcString[ucDeLiveryStringNum] = strtok(pcCmdLine,CMD_INTERPRETER_DILIVER_STRING);
	
	while(1)
	{
		ucDeLiveryStringNum++;
		
		p = strtok(0,CMD_INTERPRETER_DILIVER_STRING);
		if(p == 0)
		{
			break;
		}
		
		apcString[ucDeLiveryStringNum] = p;
	}
	
	//2匹配字符串
	for(i = 0 ; i < astInterpreterDeviceParam[emDevNum].stStaticParam.ulListSize;i++)
	{
		//匹配参数个数
		if((ucDeLiveryStringNum - 2) != astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].c_ucParamNum)
		{
			continue;
		}
		//匹配对象
		if(strcmp(apcString[0], astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].c_pcObject) != 0 )
		{
			continue;
		}
		//匹配命令
		if(strcmp(apcString[1], astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].c_pcCmd) != 0 )
		{
			continue;
		}
		
		break;
	}
	
	//i等于字符串大小说明没走到break跳出，所以出错
	if(i > astInterpreterDeviceParam[emDevNum].stStaticParam.ulListSize)
	{
		return;
	}
	
	//判断参数是否合法
	strcpy(acParamStringBuffer,astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].c_pcParam);
	
	//分割字符串
	ucDeLiveryStringNum = 0;
	apcParam[ucDeLiveryStringNum] = strtok(acParamStringBuffer,CMD_INTERPRETER_DILIVER_STRING);
	while(1)
	{
		ucDeLiveryStringNum++;
		
		p = strtok(0,CMD_INTERPRETER_DILIVER_STRING);
		if(p == 0)
		{
			break;
		}
		
		apcParam[ucDeLiveryStringNum] = p;
	}
	
	for(j = 0 ; j < astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].c_ucParamNum;j++)
	{
		ulParam = s_ulStringToUint32(apcString[j + 2]);
		ulParamMin = s_ulStringToUint32(apcParam[ j * 2]);
		ulParamMax = s_ulStringToUint32(apcParam[ j * 2+1]);
		
		if(ulParam < ulParamMin || ulParam > ulParamMax)
		{
			return;
		}
	}
	
	//无出错转到回调函数
	astInterpreterDeviceParam[emDevNum].stStaticParam.pstList[i].pvfCallBack(apcString);
	
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


