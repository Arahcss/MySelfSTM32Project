#include <stddef.h>
#include "GameApp_Operation.h"
#include "SnakeGame_Operation.h"

#define	GAME_GRID_CELL_SIZE		8					//网格每格边长（像素）

static stDisplayMsgTdf		s_stGameFrame;			//	游戏应用显示帧
static uint8_t				s_ucGameTickCounter;	//	走步计数，达到SNAKE_STEP_TICK_NUM走一步

///	@brief					写入文本并清零剩余字节
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcText		:	源字符串
///
///	@note					最多拷贝DISPLAY_LINE_LENGTH-1个字符，其余字节清0
static void s_vGameAppSetText(uint8_t *pucDst, const char *c_pcText)
{
	uint8_t ucIndex = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucIndex] != '\0'))
	{
		pucDst[ucIndex] = (uint8_t)c_pcText[ucIndex];
		ucIndex++;
	}

	while(ucIndex < DISPLAY_LINE_LENGTH)
	{
		pucDst[ucIndex] = 0;
		ucIndex++;
	}
}

///	@brief					计算文本截断后的有效字符数
///
///	@param			c_pcText		:	源字符串
///
///	@return					有效字符数，最大DISPLAY_LINE_LENGTH-1
///
///	@note
static uint8_t s_ucGameAppTextLength(const char *c_pcText)
{
	uint8_t ucIndex = 0;

	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (c_pcText[ucIndex] != '\0'))
	{
		ucIndex++;
	}

	return ucIndex;
}

///	@brief					十进制得分转字符
///
///	@param			ucScore			:	得分
///	@param			pucDigit		:	输出缓冲区，至少3字节
///
///	@return					实际写入的字符数1~3
///
///	@note					不补前导0，得分0输出"0"
static uint8_t s_ucGameAppScoreToDigits(uint8_t ucScore, uint8_t *pucDigit)
{
	uint8_t ucDigitNum;
	uint8_t ucHundreds;
	uint8_t ucTens;
	uint8_t ucOnes;

	ucHundreds = (uint8_t)(ucScore / 100u);
	ucTens = (uint8_t)((ucScore / 10u) % 10u);
	ucOnes = (uint8_t)(ucScore % 10u);

	if(ucHundreds != 0)
	{
		pucDigit[0] = (uint8_t)('0' + ucHundreds);
		pucDigit[1] = (uint8_t)('0' + ucTens);
		pucDigit[2] = (uint8_t)('0' + ucOnes);
		ucDigitNum = 3;
	}
	else if(ucTens != 0)
	{
		pucDigit[0] = (uint8_t)('0' + ucTens);
		pucDigit[1] = (uint8_t)('0' + ucOnes);
		ucDigitNum = 2;
	}
	else
	{
		pucDigit[0] = (uint8_t)('0' + ucOnes);
		ucDigitNum = 1;
	}

	return ucDigitNum;
}

///	@brief					把前缀与得分拼接到文本缓冲区
///
///	@param			pucDst			:	目标缓冲区，长度DISPLAY_LINE_LENGTH
///	@param			c_pcPrefix		:	前缀文本
///	@param			ucScore			:	得分
///
///	@note					不补前导0；超长部分截断，剩余字节保持0
static void s_vGameAppSetScoreText(uint8_t *pucDst, const char *c_pcPrefix, uint8_t ucScore)
{
	uint8_t ucIndex;
	uint8_t ucSrc;
	uint8_t ucDigitNum;
	uint8_t aucDigit[3];

	ucDigitNum = s_ucGameAppScoreToDigits(ucScore, aucDigit);

	s_vGameAppSetText(pucDst, c_pcPrefix);

	ucIndex = s_ucGameAppTextLength(c_pcPrefix);
	ucSrc = 0;
	while((ucIndex < (DISPLAY_LINE_LENGTH - 1)) && (ucSrc < ucDigitNum))
	{
		pucDst[ucIndex] = aucDigit[ucSrc];
		ucIndex++;
		ucSrc++;
	}
}

///	@brief					重建游戏网格帧
///
///	@param					None
///
///	@note					标题按状态选前缀并附得分，格子由贪吃蛇模块填充
static void s_vGameAppBuildGridFrame(void)
{
	stGridDisplayMsgTdf *pstGrid = &s_stGameFrame.unFrame.stGrid;
	const char *c_pcPrefix = NULL;

	if(emSnakeGameGetState() == emSnakeState_Paused)
	{
		c_pcPrefix = "PAUSE S:";
	}
	else
	{
		c_pcPrefix = "SNAKE S:";
	}

	s_stGameFrame.emType = emDisplayFrameType_Grid;
	pstGrid->ucColNum = SNAKE_GRID_COL_NUM;
	pstGrid->ucRowNum = SNAKE_GRID_ROW_NUM;
	pstGrid->ucCellSize = GAME_GRID_CELL_SIZE;
	pstGrid->ucOriginY = DISPLAY_GRID_ORIGIN_Y;

	s_vGameAppSetScoreText(pstGrid->aucTitle, c_pcPrefix, ucSnakeGameGetScore());

	vSnakeGameFillGrid(pstGrid->aucCell);
}

///	@brief					重建游戏结束帧
///
///	@param					None
///
///	@note					三行内容：得分 / 重开提示 / 返回提示
static void s_vGameAppBuildEndFrame(void)
{
	stListDisplayMsgTdf *pstList = &s_stGameFrame.unFrame.stList;
	const char *c_pcTitle = NULL;

	if(emSnakeGameGetState() == emSnakeState_Win)
	{
		c_pcTitle = "YOU WIN";
	}
	else
	{
		c_pcTitle = "GAME OVER";
	}

	s_stGameFrame.emType = emDisplayFrameType_List;
	pstList->ucTitleLength = s_ucGameAppTextLength(c_pcTitle);
	s_vGameAppSetText(pstList->aucTitle, c_pcTitle);
	pstList->ucLineCount = DISPLAY_LIST_LINE_NUM;
	pstList->ucSelectedIndex = 0xFF;
	pstList->aucReserved = 0;

	s_vGameAppSetScoreText(pstList->aucLine[0], "SCORE: ", ucSnakeGameGetScore());
	s_vGameAppSetText(pstList->aucLine[1], "ENTER: RESTART");
	s_vGameAppSetText(pstList->aucLine[2], "CANCEL: BACK");
}

///	@brief					进入游戏应用
///
///	@param			ucParam			:	菜单项携带的参数，本应用忽略
///
///	@note					重开一局贪吃蛇并构建网格帧
void vGameAppEnter(uint8_t ucParam)
{
	(void)ucParam;

	vSnakeGameInit();
	s_ucGameTickCounter = 0;
	s_vGameAppBuildGridFrame();
}

///	@brief					退出游戏应用
///
///	@param					None
///
///	@note					无操作，蛇局状态保留到下次进入时重置
void vGameAppExit(void)
{
	/*	无操作	*/
}

///	@brief					处理一次归一化按键
///
///	@param			emKey			:	归一化按键值
///
///	@note					矩阵键盘2/4/6/8转向、11暂停、3重开；
///							Enter在结束/胜利时重开；Cancel由菜单框架处理
void vGameAppHandleKey(emInputKeyTdf emKey)
{
	uint8_t ucKeyIndex;
	emSnakeStateTdf emState;

	if(emKey == emInputKey_Enter)
	{
		emState = emSnakeGameGetState();
		if((emState == emSnakeState_GameOver) || (emState == emSnakeState_Win))
		{
			vSnakeGameInit();
			s_ucGameTickCounter = 0;
			s_vGameAppBuildGridFrame();
		}

		return;
	}

	if(emKey == emInputKey_Cancel)
	{
		return;
	}

	if(emKey < emInputKey_KeyPadBase)
	{
		return;
	}

	ucKeyIndex = (uint8_t)(emKey - emInputKey_KeyPadBase);

	switch(ucKeyIndex)
	{
		case 2:		//	上
		{
			vSnakeGameSetDirection(emSnakeDirection_Up);
			break;
		}
		case 4:		//	左
		{
			vSnakeGameSetDirection(emSnakeDirection_Left);
			break;
		}
		case 6:		//	右
		{
			vSnakeGameSetDirection(emSnakeDirection_Right);
			break;
		}
		case 8:		//	下
		{
			vSnakeGameSetDirection(emSnakeDirection_Down);
			break;
		}
		case 3:		//	AC：重开一局
		{
			vSnakeGameInit();
			s_ucGameTickCounter = 0;
			s_vGameAppBuildGridFrame();
			break;
		}
		case 11:	//	=：暂停/继续
		{
			vSnakeGameTogglePause();
			s_vGameAppBuildGridFrame();
			break;
		}
		default:
		{
			break;
		}
	}
}

///	@brief					周期回调
///
///	@param					None
///
///	@return					1表示显示帧已变化需要刷新，0表示无变化
///
///	@note					每SNAKE_STEP_TICK_NUM个周期走一步，按状态重建帧；
///							暂停时走步返回0，不刷新
uint8_t ucGameAppTick(void)
{
	emSnakeStateTdf emState;
	uint8_t ucChanged = 0;

	s_ucGameTickCounter++;
	if(s_ucGameTickCounter >= SNAKE_STEP_TICK_NUM)
	{
		s_ucGameTickCounter = 0;
		ucChanged = ucSnakeGameStep();
	}

	if(ucChanged == 0)
	{
		return 0;
	}

	emState = emSnakeGameGetState();
	if((emState == emSnakeState_GameOver) || (emState == emSnakeState_Win))
	{
		s_vGameAppBuildEndFrame();
	}
	else
	{
		s_vGameAppBuildGridFrame();
	}

	return 1;
}

///	@brief					获取当前显示帧（只读）
///
///	@param					None
///
///	@return					网格帧或列表帧const指针
const stDisplayMsgTdf *c_pstGameAppRender(void)
{
	return &s_stGameFrame;
}
