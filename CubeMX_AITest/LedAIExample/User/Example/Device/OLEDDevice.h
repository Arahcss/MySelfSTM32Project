#ifndef _OLED_DEVICE_H_
#define _OLED_DEVICE_H_

#include "stm32f1xx_hal.h"

///	@brief				设备号枚举
///
///	@note
typedef enum
{
	emOledDevNum0			=	0,
	emOledDevNum1,
}emOledDevNumTdf;

///	@brief				字号枚举（值为字符高度，单位像素）
///
///	@note
typedef enum
{
	emOledFontSize_6x12		=	12,
	emOledFontSize_8x16		=	16,
	emOledFontSize_12x24	=	24,
}emOledFontSizeTdf;

///	@brief				像素显示模式枚举
///
///	@note
typedef enum
{
	emOledPixelShowMode_Positive	=	0,
	emOledPixelShowMode_Negative,
}emOledPixelShowModeTdf;

#define	OLED_DEV_NUM			1
#define	OLED					emOledDevNum0
#define	OLED_POINT_WIDTH		128
#define	OLED_POINT_HEIGHT		64
#define	OLED_BUFFER_WIDTH		OLED_POINT_WIDTH			//一帧数据的行宽
#define	OLED_BUFFER_HEIGHT		(OLED_POINT_HEIGHT / 8)	//一帧数据的列高

///	@brief			静态参数定义
///
///	@note			初始化后不变的硬件连接信息
typedef struct
{
	GPIO_TypeDef	*pstSclGpioBase;		//	SCL GPIOx
	uint16_t		 usSclGpioPin;			//	SCL GPIO_PIN_x
	GPIO_TypeDef	*pstSdaGpioBase;		//	SDA GPIOx
	uint16_t		 usSdaGpioPin;			//	SDA GPIO_PIN_x
}stOledStaticParamTdf;

///	@brief			运行参数定义
///
///	@note			运行时变化的显存数据
typedef struct
{
	uint8_t	aucOledBuffer[OLED_BUFFER_WIDTH * OLED_BUFFER_HEIGHT];	//	显存
}stOledRunningParamTdf;

///	@brief			OLED设备参数总结构
///
///	@note
typedef struct
{
	stOledRunningParamTdf	stRunningParam;		//	运行参数
	stOledStaticParamTdf	stStaticParam;		//	静态参数
}stOledDeviceParamTdf;

///	@brief				OLED设备初始化
///
///	@param			pstInit			:	静态参数结构体首地址
///	@param			emDevNum		:	设备编号
///
///	@note				登记GPIO并执行SSD1306命令初始化
void vOledDeviceInit(stOledStaticParamTdf *pstInit, emOledDevNumTdf emDevNum);

///	@brief				OLED运行参数初始化
///
///	@param			pstInit			:	运行参数结构体首地址
///	@param			emDevNum		:	设备编号
///
///	@note
void vOledDeviceRunningParamInit(stOledRunningParamTdf *pstInit, emOledDevNumTdf emDevNum);

///	@brief				获取OLED设备参数（只读）
///
///	@param			emDevNum		:	设备编号
///
///	@return			设备参数const指针
const stOledDeviceParamTdf *c_pstGetOledDeviceParam(emOledDevNumTdf emDevNum);

///	@brief				向显存指定位置画点
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			emDevNum		:	设备编号
void vOledDrawOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum);

///	@brief				向显存指定位置清点
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			emDevNum		:	设备编号
void vOledClearOnePointToBuffer(uint32_t ulX, uint32_t ulY, emOledDevNumTdf emDevNum);

///	@brief				向显存写入一个字符
///
///	@param			ulX				:	x坐标
///	@param			ulY				:	y坐标
///	@param			ucChar			:	ASCII字符
///	@param			emFontSize		:	字号
///	@param			emMode			:	正显/反显
///	@param			emDevNum		:	设备编号
void vOledWriteOneCharToBuffer(uint32_t ulX, uint32_t ulY, uint8_t ucChar,
		emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode, emOledDevNumTdf emDevNum);

///	@brief				向显存写入字符串
///
///	@param			usX				:	x坐标
///	@param			usY				:	y坐标
///	@param			c_pucChar		:	字符串首地址
///	@param			emFontSize		:	字号
///	@param			emMode			:	正显/反显
///	@param			emDevNum		:	设备编号
void vOledWriteStringToBuffer(uint16_t usX, uint16_t usY, const uint8_t *c_pucChar,
		emOledFontSizeTdf emFontSize, emOledPixelShowModeTdf emMode, emOledDevNumTdf emDevNum);

///	@brief				画一条水平虚线入显存
///
///	@param			usX				:	起始x坐标
///	@param			usY				:	y坐标
///	@param			ulLength		:	虚线长度（像素）
///	@param			emDevNum		:	设备编号
void vOledDrawDotLine(uint16_t usX, uint16_t usY, uint32_t ulLength, emOledDevNumTdf emDevNum);

///	@brief				清空显存
///
///	@param			emDevNum		:	设备编号
void vOledClearBuffer(emOledDevNumTdf emDevNum);

///	@brief				将显存刷新到OLED
///
///	@param			emDevNum		:	设备编号
void vOledRefreshFromBuffer(emOledDevNumTdf emDevNum);


#endif
