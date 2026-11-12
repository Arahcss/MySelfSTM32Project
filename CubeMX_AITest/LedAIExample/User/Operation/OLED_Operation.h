#ifndef _OLED_OPERATION_H_
#define _OLED_OPERATION_H_

#include "OLEDDevice.h"
#include "Calculator_Operation.h"


void vOledInitAll(void);
void vOledTest(void);

///	@brief			渲染一帧计算器界面
///
///	@param			pstMsg			:	已格式化的显示帧
///
///	@note			布局：标题6x12@(0,0)、表达式8x16@(0,16)、数值12x24@(0,36)
void vOledShowCalcFrame(const stCalcDisplayMsgTdf *pstMsg);


#endif
