#include <stddef.h>
#include "SnakeGame_Operation.h"

static uint8_t				s_aucSnakeCol[SNAKE_CELL_NUM];		//	蛇身列坐标，下标0为蛇头
static uint8_t				s_aucSnakeRow[SNAKE_CELL_NUM];		//	蛇身行坐标，下标0为蛇头
static uint8_t				s_ucSnakeLength;					//	当前长度
static emSnakeDirectionTdf	s_emSnakeDirection;					//	当前方向
static uint8_t				s_ucFoodCol;						//	食物列
static uint8_t				s_ucFoodRow;						//	食物行
static uint8_t				s_ucScore;							//	得分
static emSnakeStateTdf		s_emSnakeState;						//	运行状态
static uint16_t				s_usSnakeRandom;					//	伪随机数种子

///	@brief					判断两方向是否相反
///
///	@param			emFrom			:	当前方向
///	@param			emTo			:	目标方向
///
///	@return					1表示相反，0表示不相反
///
///	@note
static uint8_t s_ucSnakeIsOpposite(emSnakeDirectionTdf emFrom, emSnakeDirectionTdf emTo)
{
	if(((emFrom == emSnakeDirection_Up) && (emTo == emSnakeDirection_Down))
			|| ((emFrom == emSnakeDirection_Down) && (emTo == emSnakeDirection_Up))
			|| ((emFrom == emSnakeDirection_Left) && (emTo == emSnakeDirection_Right))
			|| ((emFrom == emSnakeDirection_Right) && (emTo == emSnakeDirection_Left)))
	{
		return 1;
	}

	return 0;
}

///	@brief					判断指定格子是否被蛇占用（不含尾巴）
///
///	@param			ucCol			:	列坐标
///	@param			ucRow			:	行坐标
///
///	@return					1表示命中，0表示未命中
///
///	@note					不吃食物时尾巴会移开，故尾巴格不算碰撞
static uint8_t s_ucSnakeHitBody(uint8_t ucCol, uint8_t ucRow)
{
	uint8_t ucIndex;
	uint8_t ucBodyNum;

	ucBodyNum = 0;
	if(s_ucSnakeLength > 0)
	{
		ucBodyNum = (uint8_t)(s_ucSnakeLength - 1);
	}

	for(ucIndex = 0; ucIndex < ucBodyNum; ucIndex++)
	{
		if((s_aucSnakeCol[ucIndex] == ucCol) && (s_aucSnakeRow[ucIndex] == ucRow))
		{
			return 1;
		}
	}

	return 0;
}

///	@brief					生成新食物
///
///	@param					None
///
///	@note					线性同余取起点，被占用则向后线性探测；整盘无空格置Win
static void s_vSnakeGameGenerateFood(void)
{
	uint8_t ucStart;
	uint8_t ucOffset;
	uint8_t ucIndex;
	uint8_t ucCol;
	uint8_t ucRow;
	uint8_t ucBodyIndex;
	uint8_t ucOccupied;
	uint8_t ucFound;

	s_usSnakeRandom = (uint16_t)((s_usSnakeRandom * 1103u) + 12345u);
	ucStart = (uint8_t)(s_usSnakeRandom % SNAKE_CELL_NUM);

	ucFound = 0;

	for(ucOffset = 0; ucOffset < SNAKE_CELL_NUM; ucOffset++)
	{
		ucIndex = (uint8_t)((ucStart + ucOffset) % SNAKE_CELL_NUM);
		ucCol = (uint8_t)(ucIndex % SNAKE_GRID_COL_NUM);
		ucRow = (uint8_t)(ucIndex / SNAKE_GRID_COL_NUM);

		ucOccupied = 0;
		for(ucBodyIndex = 0; ucBodyIndex < s_ucSnakeLength; ucBodyIndex++)
		{
			if((s_aucSnakeCol[ucBodyIndex] == ucCol) && (s_aucSnakeRow[ucBodyIndex] == ucRow))
			{
				ucOccupied = 1;
				break;
			}
		}

		if(ucOccupied == 0)
		{
			s_ucFoodCol = ucCol;
			s_ucFoodRow = ucRow;
			ucFound = 1;
			break;
		}
	}

	if(ucFound == 0)
	{
		s_emSnakeState = emSnakeState_Win;
	}
}

///	@brief					重开一局
///
///	@param					None
///
///	@note					长度3、蛇头(5,3)、方向Right、得分0、状态Running，并生成首个食物
void vSnakeGameInit(void)
{
	uint8_t ucIndex;

	s_aucSnakeCol[0] = 5;
	s_aucSnakeRow[0] = 3;
	s_aucSnakeCol[1] = 4;
	s_aucSnakeRow[1] = 3;
	s_aucSnakeCol[2] = 3;
	s_aucSnakeRow[2] = 3;

	s_ucSnakeLength = 3;

	for(ucIndex = s_ucSnakeLength; ucIndex < SNAKE_CELL_NUM; ucIndex++)
	{
		s_aucSnakeCol[ucIndex] = 0;
		s_aucSnakeRow[ucIndex] = 0;
	}

	s_emSnakeDirection = emSnakeDirection_Right;
	s_ucFoodCol = 0;
	s_ucFoodRow = 0;
	s_ucScore = 0;
	s_emSnakeState = emSnakeState_Running;
	s_usSnakeRandom = 12345u;

	s_vSnakeGameGenerateFood();
}

///	@brief					设置运动方向
///
///	@param			emDir			:	目标方向
///
///	@note					忽略180°反向；暂停/结束状态忽略
void vSnakeGameSetDirection(emSnakeDirectionTdf emDir)
{
	switch(emDir)
	{
		case emSnakeDirection_Up:
		case emSnakeDirection_Down:
		case emSnakeDirection_Left:
		case emSnakeDirection_Right:
		{
			break;
		}
		default:
		{
			return;
		}
	}

	if(s_emSnakeState != emSnakeState_Running)
	{
		return;
	}

	if(s_ucSnakeIsOpposite(s_emSnakeDirection, emDir) != 0)
	{
		return;
	}

	s_emSnakeDirection = emDir;
}

///	@brief					切换暂停/继续
///
///	@param					None
///
///	@note					Running↔Paused，其它状态不动作
void vSnakeGameTogglePause(void)
{
	switch(s_emSnakeState)
	{
		case emSnakeState_Running:
		{
			s_emSnakeState = emSnakeState_Paused;
			break;
		}
		case emSnakeState_Paused:
		{
			s_emSnakeState = emSnakeState_Running;
			break;
		}
		default:
		{
			break;
		}
	}
}

///	@brief					走一步
///
///	@param					None
///
///	@return					1表示显示帧需要刷新，0表示无变化
///
///	@note					仅Running时推进；撞墙/撞自身转GameOver，吃满转Win
uint8_t ucSnakeGameStep(void)
{
	uint8_t ucNewCol;
	uint8_t ucNewRow;
	uint8_t ucIndex;
	uint8_t ucEatFood;

	if(s_emSnakeState != emSnakeState_Running)
	{
		return 0;
	}

	ucNewCol = s_aucSnakeCol[0];
	ucNewRow = s_aucSnakeRow[0];

	switch(s_emSnakeDirection)
	{
		case emSnakeDirection_Up:
		{
			ucNewRow--;
			break;
		}
		case emSnakeDirection_Down:
		{
			ucNewRow++;
			break;
		}
		case emSnakeDirection_Left:
		{
			ucNewCol--;
			break;
		}
		case emSnakeDirection_Right:
		{
			ucNewCol++;
			break;
		}
		default:
		{
			return 0;
		}
	}

	//撞墙：无符号回绕后越界即判负
	if((ucNewCol >= SNAKE_GRID_COL_NUM) || (ucNewRow >= SNAKE_GRID_ROW_NUM))
	{
		s_emSnakeState = emSnakeState_GameOver;
		return 1;
	}

	if(s_ucSnakeHitBody(ucNewCol, ucNewRow) != 0)
	{
		s_emSnakeState = emSnakeState_GameOver;
		return 1;
	}

	ucEatFood = 0;
	if((ucNewCol == s_ucFoodCol) && (ucNewRow == s_ucFoodRow))
	{
		ucEatFood = 1;
		if(s_ucSnakeLength < SNAKE_CELL_NUM)
		{
			s_ucSnakeLength++;
		}
		s_ucScore++;
	}

	//整体后移一格，蛇头写入新坐标；吃食物时长度已加1，尾巴保持不动
	for(ucIndex = (uint8_t)(s_ucSnakeLength - 1); ucIndex > 0; ucIndex--)
	{
		s_aucSnakeCol[ucIndex] = s_aucSnakeCol[ucIndex - 1];
		s_aucSnakeRow[ucIndex] = s_aucSnakeRow[ucIndex - 1];
	}

	s_aucSnakeCol[0] = ucNewCol;
	s_aucSnakeRow[0] = ucNewRow;

	if(ucEatFood != 0)
	{
		if(s_ucSnakeLength >= SNAKE_CELL_NUM)
		{
			s_emSnakeState = emSnakeState_Win;
			return 1;
		}

		s_vSnakeGameGenerateFood();
	}

	return 1;
}

///	@brief					获取当前得分
///
///	@param					None
///
///	@return					已吃到的食物数量
///
///	@note
uint8_t ucSnakeGameGetScore(void)
{
	return s_ucScore;
}

///	@brief					获取当前状态
///
///	@param					None
///
///	@return					当前运行状态
///
///	@note
emSnakeStateTdf emSnakeGameGetState(void)
{
	return s_emSnakeState;
}

///	@brief					填充网格显示数据
///
///	@param			pucCell			:	目标缓冲区，长度SNAKE_CELL_NUM
///
///	@note					先全部写Empty，再按蛇头/蛇身/食物写入；行优先row*ucColNum+col
void vSnakeGameFillGrid(uint8_t *pucCell)
{
	uint8_t ucIndex;
	uint8_t ucCellValue;

	if(pucCell == NULL)
	{
		return;
	}

	for(ucIndex = 0; ucIndex < SNAKE_CELL_NUM; ucIndex++)
	{
		pucCell[ucIndex] = (uint8_t)emSnakeCell_Empty;
	}

	for(ucIndex = 0; ucIndex < s_ucSnakeLength; ucIndex++)
	{
		if(ucIndex == 0)
		{
			ucCellValue = (uint8_t)emSnakeCell_Head;
		}
		else
		{
			ucCellValue = (uint8_t)emSnakeCell_Body;
		}

		pucCell[(s_aucSnakeRow[ucIndex] * SNAKE_GRID_COL_NUM) + s_aucSnakeCol[ucIndex]] = ucCellValue;
	}

	//胜利帧里食物位置已被蛇身覆盖，不再写食物
	if(s_emSnakeState != emSnakeState_Win)
	{
		pucCell[(s_ucFoodRow * SNAKE_GRID_COL_NUM) + s_ucFoodCol] = (uint8_t)emSnakeCell_Food;
	}
}
