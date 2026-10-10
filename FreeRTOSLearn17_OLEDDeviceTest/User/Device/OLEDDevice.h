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
}emOledDevNumTdf;

///	@brief					字号枚举
///
///	@note
typedef enum
{
	emOledFontSize_6x12				=	12,
	emOledFontSize_6x16				=	16,
	emOledFontSize_12x24				=	24,
}emOledFontSizeTdf;

///	@brief					像素显示模式枚举
///
///	@note
typedef enum
{
	emOledPixelShowMode_Positive				=	0,
	emOledPixelShowMode_Negative			=	1,
}emOledPixelShowModeTdf;

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
void vOledClearOnePointToBuffer( uint32_t x ,uint32_t y,emOledDevNumTdf	emDevNum);
void vOledDrawOnePointToBuffer( uint32_t x ,uint32_t y,emOledDevNumTdf	emDevNum);


#endif
