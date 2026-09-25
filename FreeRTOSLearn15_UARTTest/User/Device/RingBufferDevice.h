#ifndef _RING_BUFFER_DEVICE_H_
#define _RING_BUFFER_DEVICE_H_

#include "stm32f1xx_hal.h"

#define RING_BUFFER_DEV_NUM		1
#define UART_RX_BUFFER					emRingBufferDevNum0

///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emRingBufferDevNum0				=	0,
	emRingBufferDevNum1,
	emRingBufferDevNum2,
	emRingBufferDevNum3,
	emRingBufferDevNum4,
	emRingBufferDevNum5,
	emRingBufferDevNum6,
	emRingBufferDevNum7,
	emRingBufferDevNum8,
}emRingBufferDevNumTdf;

///	@brief			状态定义
///	
///	@note
typedef enum
{
	emRingBufferStatus_Empty						=0,	//空
	emRingBufferStatus_Full							=1,	//满
	emRingBufferStatus_NotEmptyNotFull	=2,	//非空非满
}emRingBufferStatusTdf;

///	@brief			错误编码定义
///	
///	@note
typedef enum
{
	emRingBufferError_None						=0,	//无错误
	emRingBufferError_WriteFull					=1,	//已写入满
	emRingBufferError_ReadEmpty				=2,	//读取空
}
emRingBufferErrorCodeTdf;

///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	void					*pvHead;				//缓冲区头部
	void					*pvTail;				//缓冲区尾部
	uint32_t			ulElementLength;	//元素长度
}
stRingBufferStaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	void									*pvWrite;				//写指针
	void									*pvRead;				//读指针
	emRingBufferStatusTdf	emStatus;				//缓冲区状态
}
stRingBufferRunningParamTdf;

typedef struct
{
	stRingBufferRunningParamTdf stRunningParam;		//静态参数
	stRingBufferStaticParamTdf stStaticParam;					//动态参数
}stRingBufferDeviceParamTdf;


#endif
