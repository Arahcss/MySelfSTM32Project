#include "OledDevice.h"
#include "string.h"

stOledDeviceParamTdf	astOledDeviceParam[OLED_DEV_NUM];


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

