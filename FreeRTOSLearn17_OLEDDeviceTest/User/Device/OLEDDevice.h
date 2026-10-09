#ifndef _OLED_DEVICE_H_
#define _OLED_DEVICE_H_

#include "stm32f1xx_hal.h"
#include "math.h"

#define	PI	3.141592653


///	@brief					设备号枚举
///
///	@note
typedef enum
{
	emOledDevNum0				=	0,
	emOledDevNum1,
	emOledDevNum2,
	emOledDevNum3,
	emOledDevNum4,
	emOledDevNum5,
	emOledDevNum6,
	emOledDevNum7,
	emOledDevNum8,
}emOledDevNumTdf;

#define OLED_DEV_NUM				1
#define OLED									emOledDevNum0
#define OLED_POINT_WIDTH		128
#define OLED_POINT_HEIGHT		64
#define OLED_BUFFER_WIDTH		OLED_POINT_WIDTH			//OELD一帧数据的行宽
#define OLED_BUFFER_HEIGHT		OLED_POINT_HEIGHT / 8	//OLED一帧数据的列高


///	@brief			静态参数定义
///	
///	@note
typedef struct
{
	GPIO_TypeDef								*pstSclGpioBase;		//	SCL GPIOx
	uint16_t										usSclGpioPin;			//	SCL GPIO_PIN_x
	GPIO_TypeDef								*pstSdaGpioBase;		//	SDA GPIOx
	uint16_t										usSdaGpioPin;			//	SDA GPIO_PIN_x
}
stOledStaticParamTdf;

///	@brief			运行参数定义
///	
///	@note
typedef struct
{
	uint8_t										aucOledBuffer[OLED_BUFFER_WIDTH * OLED_BUFFER_HEIGHT];//显存
}
stOledRunningParamTdf;

typedef struct
{
	stOledRunningParamTdf stRunningParam;		//静态参数
	stOledStaticParamTdf stStaticParam;					//动态参数
}stOledDeviceParamTdf;

void vOledDeviceInit(stOledStaticParamTdf *pstInit,emOledDevNumTdf emDevNum);
void vOledRefreshFromBuffer( emOledDevNumTdf	emDevNum);


#endif
