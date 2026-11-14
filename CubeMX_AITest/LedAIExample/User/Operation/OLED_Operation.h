#ifndef _OLED_OPERATION_H_
#define _OLED_OPERATION_H_

#include "OLEDDevice.h"
#include "Project_Config.h"
#include "Calculator_Operation.h"


void vOledInitAll(void);
void vOledTest(void);

///	@brief			渲染一帧计算器界面
///
///	@param			pstMsg			:	已格式化的显示帧
///
///	@note			布局：标题6x12@(0,0)、表达式8x16@(0,16)、数值12x24@(0,36)
void vOledShowCalcFrame(const stCalcDisplayMsgTdf *pstMsg);

///	@brief					显示帧类型枚举
///
///	@note
typedef enum
{
	emDisplayFrameType_Calc		=	0,
	emDisplayFrameType_List,
}emDisplayFrameTypeTdf;

///	@brief					列表显示帧
///
///	@note					标题6x12@(0,0)，最多3行8x16@(0,16/32/48)
typedef struct
{
	uint8_t	ucTitleLength;											//	标题有效字符数
	uint8_t	ucLineCount;											//	有效行数1~3
	uint8_t	ucSelectedIndex;										//	选中行号，0xFF表示无选中
	uint8_t	aucReserved;											//	保留
	uint8_t	aucTitle[DISPLAY_LINE_LENGTH];							//	标题文本
	uint8_t	aucLine[DISPLAY_LIST_LINE_NUM][DISPLAY_LINE_LENGTH];	//	行文本
}stListDisplayMsgTdf;

///	@brief					统一显示帧
///
///	@note
typedef struct
{
	emDisplayFrameTypeTdf	emType;									//	帧类型
	union
	{
		stCalcDisplayMsgTdf	stCalc;									//	计算器帧
		stListDisplayMsgTdf	stList;									//	列表帧
	}unFrame;
}stDisplayMsgTdf;

///	@brief			渲染一帧列表界面
///
///	@param			pstMsg			:	已格式化的列表显示帧
///
///	@note			标题6x12@(0,0)，行8x16@(0,16/32/48)；
///					选中行加'>'前缀并反显，超长行按屏宽截断
void vOledShowListFrame(const stListDisplayMsgTdf *pstMsg);

///	@brief			渲染一帧统一显示帧
///
///	@param			pstMsg			:	已格式化的显示帧
///
///	@note			按帧类型分发到计算器帧或列表帧渲染
void vOledShowFrame(const stDisplayMsgTdf *pstMsg);


#endif
