#ifndef _INTERPRETER_DEVICE_H_
#define _INTERPRETER_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"



///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emInterpreterDevNum0				=	0,
	emInterpreterDevNum1,
	emInterpreterDevNum2,
	emInterpreterDevNum3,
	emInterpreterDevNum4,
	emInterpreterDevNum5,
	emInterpreterDevNum6,
	emInterpreterDevNum7,
	emInterpreterDevNum8,
}emInterpreterDevNumTdf;

#define INTERPRETER_DEV_NUM		1

#define INTERPRETER_BOARD			emInterpreterDevNum0
#define INTERPRETER1						emInterpreterDevNum1
#define INTERPRETER2						emInterpreterDevNum2
#define INTERPRETER3						emInterpreterDevNum3
#define INTERPRETER4						emInterpreterDevNum4
#define INTERPRETER5						emInterpreterDevNum5
#define INTERPRETER6						emInterpreterDevNum6
#define INTERPRETER7						emInterpreterDevNum7
#define INTERPRETER8						emInterpreterDevNum8

///	@brief			命令结构表定义
///	
///	@note
typedef struct
{
	const uint8_t					*c_pucObject;			//对象
	const uint8_t					*c_pucCmd;			//命令
	const uint8_t					*c_pucParam;			//参数
	const uint8_t					c_ucParamNum;		//参数数量
}
stInterpreterStaticParamTdf;

///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	uint8_t			i;
}
stInterpreterStaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	uint8_t			i;
}
stInterpreterRunningParamTdf;

typedef struct
{
	stInterpreterRunningParamTdf stRunningParam;		//静态参数
	stInterpreterStaticParamTdf stStaticParam;					//动态参数
}stInterpreterDeviceParamTdf;


#endif
