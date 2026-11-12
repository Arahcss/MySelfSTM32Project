#ifndef _CALCULATOR_OPERATION_H_
#define _CALCULATOR_OPERATION_H_

#include <stdint.h>
#include "Project_Config.h"

///	@brief					计算器状态枚举
///
///	@note
typedef enum
{
	emCalcState_InputOperand1		=	0,		//	输入第一个操作数
	emCalcState_InputOperand2,				//	输入第二个操作数
	emCalcState_Result,						//	显示结果
	emCalcState_Error,						//	错误（除零）
}emCalcStateTdf;

///	@brief					运算符枚举
///
///	@note
typedef enum
{
	emCalcOperator_None				=	0,
	emCalcOperator_Add,						//	加
	emCalcOperator_Sub,						//	减
	emCalcOperator_Mul,						//	乘
	emCalcOperator_Div,						//	除
}emCalcOperatorTdf;

///	@brief					错误码枚举
///
///	@note
typedef enum
{
	emCalcError_None				=	0,
	emCalcError_DivideByZero,				//	除数为0
}emCalcErrorTdf;

///	@brief			计算器显示帧
///
///	@note			两行均为已格式化文本，显示模块不做数值转换；
///					队列项尺寸即本结构体尺寸（sizeof = 34 字节）
typedef struct
{
	uint8_t						aucExpression[CALC_LINE_BUFFER_LENGTH];	//	表达式行文本
	uint8_t						aucValue[CALC_LINE_BUFFER_LENGTH];		//	数值行文本
}stCalcDisplayMsgTdf;

///	@brief					计算器初始化
///
///	@param					None
///
///	@note					复位上下文并生成首帧显示数据（操作数0）
void vCalcInit(void);

///	@brief					处理一次按键
///
///	@param			ucKeyIndex		:	键号0~KEYPAD_DEV_NUM-1，由键盘队列给出
///
///	@note					内部按键位映射表解析为数字/运算符/全清/退格/等号
void vCalcInputKey(uint8_t ucKeyIndex);

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					显示帧const指针，可直接投递到显示队列
const stCalcDisplayMsgTdf *c_pstGetCalcDisplayMsg(void);

///	@brief					整数四则运算内核
///
///	@param			slOperand1		:	操作数1
///	@param			slOperand2		:	操作数2
///	@param			emOperator		:	运算符
///	@param			pemError		:	错误码输出，调用方必须传入有效指针
///
///	@return					运算结果；除零时返回0并把错误码置为emCalcError_DivideByZero
///
///	@note					整数除法向零截断；操作数上限9999，结果在int32范围内
int32_t slCalcArithmetic(int32_t slOperand1, int32_t slOperand2,
		emCalcOperatorTdf emOperator, emCalcErrorTdf *pemError);

#endif
