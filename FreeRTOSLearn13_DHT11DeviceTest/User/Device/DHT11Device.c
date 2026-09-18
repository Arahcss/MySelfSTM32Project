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


///	@brief				DHT11设备初始化
///
///	@param			psInit				:	初始化参数结构体的首地址
///	@param			emDevNum		:	设备编号
///	@note
void vDht11DeviceInit(stDht11StaticParamTdf *pstInit,emDht11DevNumTdf emDevNum)
{
	memcpy(&astDht11DeviceParam[emDevNum].stStaticParam,pstInit,sizeof(stDht11StaticParamTdf)	/	sizeof(uint8_t));
}

