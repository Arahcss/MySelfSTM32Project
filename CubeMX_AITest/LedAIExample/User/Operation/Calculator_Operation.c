#include "Calculator_Operation.h"

///	@brief					按键功能枚举
///
///	@note
typedef enum
{
	emCalcKeyFunction_None			=	0,		//	未使用键
	emCalcKeyFunction_Digit,				//	数字键
	emCalcKeyFunction_Operator,				//	运算符键
	emCalcKeyFunction_Equal,				//	等号键
	emCalcKeyFunction_ClearAll,				//	全清键
	emCalcKeyFunction_Backspace,			//	退格键
}emCalcKeyFunctionTdf;

///	@brief			按键映射项
///
///	@note			ucValue对数字键为0~9，对运算符键为运算符枚举值
typedef struct
{
	emCalcKeyFunctionTdf		emFunction;			//	按键功能
	uint8_t						ucValue;			//	功能附带数值
}stCalcKeyMapTdf;

///	@brief			按键映射表
///
///	@note			下标为键号（=行索引*KEYPAD_COL_NUM+列索引）；
///					改键位只改本表，表长必须等于KEYPAD_DEV_NUM
static const stCalcKeyMapTdf s_astCalcKeyMap[KEYPAD_DEV_NUM] =
{
	{emCalcKeyFunction_None, 0},						//	键0  (0,0) 未使用
	{emCalcKeyFunction_Digit, 0},						//	键1  (0,1) 数字0
	{emCalcKeyFunction_None, 0},						//	键2  (0,2) 未使用
	{emCalcKeyFunction_ClearAll, 0},					//	键3  (0,3) 全清
	{emCalcKeyFunction_Digit, 7},						//	键4  (1,0) 数字7
	{emCalcKeyFunction_Digit, 8},						//	键5  (1,1) 数字8
	{emCalcKeyFunction_Digit, 9},						//	键6  (1,2) 数字9
	{emCalcKeyFunction_Backspace, 0},					//	键7  (1,3) 退格
	{emCalcKeyFunction_Digit, 4},						//	键8  (2,0) 数字4
	{emCalcKeyFunction_Digit, 5},						//	键9  (2,1) 数字5
	{emCalcKeyFunction_Digit, 6},						//	键10 (2,2) 数字6
	{emCalcKeyFunction_Equal, 0},						//	键11 (2,3) 等号
	{emCalcKeyFunction_Digit, 1},						//	键12 (3,0) 数字1
	{emCalcKeyFunction_Digit, 2},						//	键13 (3,1) 数字2
	{emCalcKeyFunction_Digit, 3},						//	键14 (3,2) 数字3
	{emCalcKeyFunction_None, 0},						//	键15 (3,3) 未使用
	{emCalcKeyFunction_Operator, emCalcOperator_Add},	//	键16 (4,0) 加
	{emCalcKeyFunction_Operator, emCalcOperator_Sub},	//	键17 (4,1) 减
	{emCalcKeyFunction_Operator, emCalcOperator_Mul},	//	键18 (4,2) 乘
	{emCalcKeyFunction_Operator, emCalcOperator_Div},	//	键19 (4,3) 除
};

///	@brief			计算器私有上下文
///
///	@note			ucDigitCount为当前正在输入的操作数位数，用于4位上限与退格
typedef struct
{
	emCalcStateTdf				emState;			//	当前状态
	emCalcOperatorTdf			emOperator;			//	待运算的运算符
	int32_t						slOperand1;			//	操作数1
	int32_t						slOperand2;			//	操作数2
	int32_t						slResult;			//	运算结果
	uint8_t						ucDigitCount;		//	当前输入位数
	emCalcErrorTdf				emError;			//	错误码
}stCalcContextTdf;

static stCalcContextTdf			s_stCalcContext;		//	计算器上下文
static stCalcDisplayMsgTdf		s_stCalcDisplayMsg;		//	当前显示帧

///	@brief					复位上下文到初始状态
///
///	@param					None
///
///	@note
static void s_vCalcContextReset(void)
{
	s_stCalcContext.emState = emCalcState_InputOperand1;
	s_stCalcContext.emOperator = emCalcOperator_None;
	s_stCalcContext.slOperand1 = 0;
	s_stCalcContext.slOperand2 = 0;
	s_stCalcContext.slResult = 0;
	s_stCalcContext.ucDigitCount = 0;
	s_stCalcContext.emError = emCalcError_None;
}

///	@brief					获取字符串结尾地址
///
///	@param			pucText			:	字符串首地址
///
///	@return					指向结束符'\0'的指针
///
///	@note
static uint8_t *s_pucCalcGetStringEnd(uint8_t *pucText)
{
	while(*pucText != '\0')
	{
		pucText++;
	}

	return pucText;
}

///	@brief					写入字符串（覆盖）
///
///	@param			pucDest			:	目标缓冲区
///	@param			c_pucSrc		:	源字符串
///
///	@note					目标缓冲区需足以容纳源字符串及结束符
static void s_vCalcWriteText(uint8_t *pucDest, const uint8_t *c_pucSrc)
{
	while(*c_pucSrc != '\0')
	{
		*pucDest = *c_pucSrc;
		pucDest++;
		c_pucSrc++;
	}

	*pucDest = '\0';
}

///	@brief					追加一个字符
///
///	@param			pucDest			:	以'\0'结尾的目标缓冲区
///	@param			ucChar			:	待追加字符
///
///	@note
static void s_vCalcAppendChar(uint8_t *pucDest, uint8_t ucChar)
{
	uint8_t *pucEnd;

	pucEnd = s_pucCalcGetStringEnd(pucDest);
	*pucEnd = ucChar;
	pucEnd++;
	*pucEnd = '\0';
}

///	@brief					整数转十进制字符串
///
///	@param			slValue			:	待转换数值
///	@param			pucBuffer		:	输出缓冲区，至少CALC_LINE_BUFFER_LENGTH字节
///
///	@note					支持负数；不依赖sprintf
static void s_vCalcNumberToString(int32_t slValue, uint8_t *pucBuffer)
{
	uint8_t aucTemp[CALC_LINE_BUFFER_LENGTH];
	uint8_t ucCount;
	uint32_t ulValue;
	uint8_t *pucWrite;

	if(slValue < 0)
	{
		*pucBuffer = '-';
		pucBuffer++;
		ulValue = (uint32_t)(-slValue);
	}
	else
	{
		ulValue = (uint32_t)slValue;
	}

	//逆序取十进制位
	ucCount = 0;
	do
	{
		aucTemp[ucCount] = (uint8_t)('0' + (ulValue % 10));
		ucCount++;
		ulValue /= 10;
	}while(ulValue != 0);

	pucWrite = pucBuffer;
	while(ucCount > 0)
	{
		ucCount--;
		*pucWrite = aucTemp[ucCount];
		pucWrite++;
	}

	*pucWrite = '\0';
}

///	@brief					追加一个整数
///
///	@param			pucDest			:	以'\0'结尾的目标缓冲区
///	@param			slValue			:	待追加数值
///
///	@note
static void s_vCalcAppendNumber(uint8_t *pucDest, int32_t slValue)
{
	uint8_t aucTemp[CALC_LINE_BUFFER_LENGTH];

	s_vCalcNumberToString(slValue, aucTemp);
	s_vCalcWriteText(s_pucCalcGetStringEnd(pucDest), aucTemp);
}

///	@brief					获取运算符显示字符
///
///	@param			emOperator		:	运算符
///
///	@return					显示字符
///
///	@note
static uint8_t s_ucCalcGetOperatorChar(emCalcOperatorTdf emOperator)
{
	uint8_t ucChar;

	switch(emOperator)
	{
		case emCalcOperator_Add:
		{
			ucChar = '+';
			break;
		}
		case emCalcOperator_Sub:
		{
			ucChar = '-';
			break;
		}
		case emCalcOperator_Mul:
		{
			ucChar = '*';
			break;
		}
		case emCalcOperator_Div:
		{
			ucChar = '/';
			break;
		}
		default:
		{
			ucChar = '?';
			break;
		}
	}

	return ucChar;
}

///	@brief					根据上下文生成显示帧文本
///
///	@param					None
///
///	@note					表达式行最多16字符，数值行最多10字符，均在缓冲区内
static void s_vCalcBuildDisplay(void)
{
	uint8_t *pucExpression;

	pucExpression = s_stCalcDisplayMsg.aucExpression;

	switch(s_stCalcContext.emState)
	{
		case emCalcState_InputOperand1:
		{
			s_vCalcNumberToString(s_stCalcContext.slOperand1, s_stCalcDisplayMsg.aucExpression);
			s_vCalcNumberToString(s_stCalcContext.slOperand1, s_stCalcDisplayMsg.aucValue);
			break;
		}
		case emCalcState_InputOperand2:
		{
			s_vCalcNumberToString(s_stCalcContext.slOperand1, s_stCalcDisplayMsg.aucExpression);
			s_vCalcAppendChar(pucExpression, s_ucCalcGetOperatorChar(s_stCalcContext.emOperator));
			s_vCalcNumberToString(s_stCalcContext.slOperand2, s_stCalcDisplayMsg.aucValue);
			break;
		}
		case emCalcState_Result:
		{
			s_vCalcNumberToString(s_stCalcContext.slOperand1, s_stCalcDisplayMsg.aucExpression);
			s_vCalcAppendChar(pucExpression, s_ucCalcGetOperatorChar(s_stCalcContext.emOperator));
			s_vCalcAppendNumber(pucExpression, s_stCalcContext.slOperand2);

			s_vCalcWriteText(s_stCalcDisplayMsg.aucValue, (const uint8_t *)"=");
			s_vCalcAppendNumber(s_stCalcDisplayMsg.aucValue, s_stCalcContext.slResult);
			break;
		}
		default:
		{
			//错误态：显示出错表达式与错误提示
			s_vCalcNumberToString(s_stCalcContext.slOperand1, s_stCalcDisplayMsg.aucExpression);
			s_vCalcAppendChar(pucExpression, s_ucCalcGetOperatorChar(s_stCalcContext.emOperator));
			s_vCalcAppendNumber(pucExpression, s_stCalcContext.slOperand2);

			s_vCalcWriteText(s_stCalcDisplayMsg.aucValue, (const uint8_t *)"DIV/0");
			break;
		}
	}
}

///	@brief					向当前操作数追加一位数字
///
///	@param			pslOperand		:	操作数指针
///	@param			ucDigit			:	数字0~9
///
///	@note					满CALC_INPUT_MAX_DIGIT位或超过CALC_INPUT_MAX_VALUE时忽略；
///							首位为0时用新数字替换，避免前导零
static void s_vCalcAppendDigit(int32_t *pslOperand, uint8_t ucDigit)
{
	int32_t slNewValue;

	//已满4位
	if(s_stCalcContext.ucDigitCount >= CALC_INPUT_MAX_DIGIT)
	{
		return;
	}

	//首位或当前值仍为0时直接替换
	if((s_stCalcContext.ucDigitCount == 0) || (*pslOperand == 0))
	{
		*pslOperand = (int32_t)ucDigit;

		if(s_stCalcContext.ucDigitCount == 0)
		{
			s_stCalcContext.ucDigitCount = 1;
		}

		return;
	}

	slNewValue = *pslOperand * 10 + (int32_t)ucDigit;
	if(slNewValue > CALC_INPUT_MAX_VALUE)
	{
		return;
	}

	*pslOperand = slNewValue;
	s_stCalcContext.ucDigitCount++;
}

///	@brief					删除当前操作数末位数字
///
///	@param			pslOperand		:	操作数指针
///
///	@note					无输入时忽略
static void s_vCalcBackspace(int32_t *pslOperand)
{
	if(s_stCalcContext.ucDigitCount == 0)
	{
		return;
	}

	*pslOperand = *pslOperand / 10;
	s_stCalcContext.ucDigitCount--;
}

///	@brief					处理运算符按键
///
///	@param			emOperator		:	新运算符
///
///	@note					输入操作数1：记录运算符转输入操作数2；
///							输入操作数2：连算（结果作新操作数1），无输入时只替换运算符；
///							结果态：以结果作操作数1继续；错误态：忽略
static void s_vCalcPressOperator(emCalcOperatorTdf emOperator)
{
	int32_t slResult;
	emCalcErrorTdf emError;

	switch(s_stCalcContext.emState)
	{
		case emCalcState_InputOperand1:
		{
			//未输入操作数1时忽略
			if(s_stCalcContext.ucDigitCount == 0)
			{
				break;
			}

			s_stCalcContext.emOperator = emOperator;
			s_stCalcContext.slOperand2 = 0;
			s_stCalcContext.ucDigitCount = 0;
			s_stCalcContext.emState = emCalcState_InputOperand2;
			break;
		}
		case emCalcState_InputOperand2:
		{
			//未输入操作数2时只替换运算符
			if(s_stCalcContext.ucDigitCount == 0)
			{
				s_stCalcContext.emOperator = emOperator;
				break;
			}

			//连算：先结算当前表达式，结果作为新的操作数1
			emError = emCalcError_None;
			slResult = slCalcArithmetic(s_stCalcContext.slOperand1, s_stCalcContext.slOperand2,
					s_stCalcContext.emOperator, &emError);

			if(emError != emCalcError_None)
			{
				s_stCalcContext.emError = emError;
				s_stCalcContext.emState = emCalcState_Error;
				break;
			}

			s_stCalcContext.slOperand1 = slResult;
			s_stCalcContext.slOperand2 = 0;
			s_stCalcContext.ucDigitCount = 0;
			s_stCalcContext.emOperator = emOperator;
			break;
		}
		case emCalcState_Result:
		{
			//结果作为下一个表达式的操作数1
			s_stCalcContext.slOperand1 = s_stCalcContext.slResult;
			s_stCalcContext.slOperand2 = 0;
			s_stCalcContext.ucDigitCount = 0;
			s_stCalcContext.emOperator = emOperator;
			s_stCalcContext.emState = emCalcState_InputOperand2;
			break;
		}
		default:
		{
			//错误态忽略
			break;
		}
	}
}

///	@brief					处理等号按键
///
///	@param					None
///
///	@note					仅"输入操作数2且已输入操作数2"时有效，结果转结果态或错误态
static void s_vCalcPressEqual(void)
{
	int32_t slResult;
	emCalcErrorTdf emError;

	if(s_stCalcContext.emState != emCalcState_InputOperand2)
	{
		return;
	}

	if(s_stCalcContext.ucDigitCount == 0)
	{
		return;
	}

	emError = emCalcError_None;
	slResult = slCalcArithmetic(s_stCalcContext.slOperand1, s_stCalcContext.slOperand2,
			s_stCalcContext.emOperator, &emError);

	if(emError != emCalcError_None)
	{
		s_stCalcContext.emError = emError;
		s_stCalcContext.emState = emCalcState_Error;
		return;
	}

	s_stCalcContext.slResult = slResult;
	s_stCalcContext.emState = emCalcState_Result;
}

///	@brief					整数四则运算内核
///
///	@param			slOperand1		:	操作数1
///	@param			slOperand2		:	操作数2
///	@param			emOperator		:	运算符
///	@param			pemError		:	错误码输出
///
///	@return					运算结果
///
///	@note					整数除法向零截断；除零返回0并输出错误码
int32_t slCalcArithmetic(int32_t slOperand1, int32_t slOperand2,
		emCalcOperatorTdf emOperator, emCalcErrorTdf *pemError)
{
	int32_t slResult;

	slResult = 0;
	*pemError = emCalcError_None;

	switch(emOperator)
	{
		case emCalcOperator_Add:
		{
			slResult = slOperand1 + slOperand2;
			break;
		}
		case emCalcOperator_Sub:
		{
			slResult = slOperand1 - slOperand2;
			break;
		}
		case emCalcOperator_Mul:
		{
			slResult = slOperand1 * slOperand2;
			break;
		}
		case emCalcOperator_Div:
		{
			if(slOperand2 == 0)
			{
				*pemError = emCalcError_DivideByZero;
			}
			else
			{
				slResult = slOperand1 / slOperand2;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return slResult;
}

///	@brief					计算器初始化
///
///	@param					None
///
///	@note					复位上下文并生成首帧显示数据
void vCalcInit(void)
{
	s_vCalcContextReset();
	s_vCalcBuildDisplay();
}

///	@brief					处理一次按键
///
///	@param			ucKeyIndex		:	键号0~KEYPAD_DEV_NUM-1
///
///	@note					错误态下仅全清有效；结果态按数字键开始新计算
void vCalcInputKey(uint8_t ucKeyIndex)
{
	const stCalcKeyMapTdf *pstKeyMap;

	if(ucKeyIndex >= KEYPAD_DEV_NUM)
	{
		return;
	}

	pstKeyMap = &s_astCalcKeyMap[ucKeyIndex];

	switch(pstKeyMap->emFunction)
	{
		case emCalcKeyFunction_Digit:
		{
			if(s_stCalcContext.emState == emCalcState_InputOperand1)
			{
				s_vCalcAppendDigit(&s_stCalcContext.slOperand1, pstKeyMap->ucValue);
			}
			else if(s_stCalcContext.emState == emCalcState_InputOperand2)
			{
				s_vCalcAppendDigit(&s_stCalcContext.slOperand2, pstKeyMap->ucValue);
			}
			else if(s_stCalcContext.emState == emCalcState_Result)
			{
				//结果态按数字键开始新计算
				s_vCalcContextReset();
				s_vCalcAppendDigit(&s_stCalcContext.slOperand1, pstKeyMap->ucValue);
			}
			else
			{
				//错误态忽略
			}
			break;
		}
		case emCalcKeyFunction_Operator:
		{
			s_vCalcPressOperator((emCalcOperatorTdf)pstKeyMap->ucValue);
			break;
		}
		case emCalcKeyFunction_Equal:
		{
			s_vCalcPressEqual();
			break;
		}
		case emCalcKeyFunction_ClearAll:
		{
			s_vCalcContextReset();
			break;
		}
		case emCalcKeyFunction_Backspace:
		{
			if(s_stCalcContext.emState == emCalcState_InputOperand1)
			{
				s_vCalcBackspace(&s_stCalcContext.slOperand1);
			}
			else if(s_stCalcContext.emState == emCalcState_InputOperand2)
			{
				s_vCalcBackspace(&s_stCalcContext.slOperand2);
			}
			else
			{
				//结果态/错误态退格忽略
			}
			break;
		}
		default:
		{
			break;
		}
	}

	s_vCalcBuildDisplay();
}

///	@brief					获取当前显示帧
///
///	@param					None
///
///	@return					显示帧const指针
///
///	@note					返回内部静态数据，可直接作为队列投递源
const stCalcDisplayMsgTdf *c_pstGetCalcDisplayMsg(void)
{
	return &s_stCalcDisplayMsg;
}
