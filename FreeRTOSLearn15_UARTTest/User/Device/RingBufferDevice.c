#include "RingBufferDevice.h"
#include "string.h"

stRingBufferDeviceParamTdf	astRingBufferDeviceParam[RING_BUFFER_DEV_NUM];

const stRingBufferDeviceParamTdf	*c_pstGetRingBufferDeviceParam(emRingBufferDevNumTdf emDevNum)
{
	return &astRingBufferDeviceParam[emDevNum];
}

void vRingBufferDeviceRunningParamInit(stRingBufferRunningParamTdf *pstInit,emRingBufferDevNumTdf emDevNum)
{
		memcpy(&astRingBufferDeviceParam[emDevNum].stRunningParam,pstInit,sizeof(stRingBufferRunningParamTdf)	/	sizeof(uint8_t));
}


