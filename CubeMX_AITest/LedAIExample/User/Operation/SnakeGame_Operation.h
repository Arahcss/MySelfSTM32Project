#ifndef _SNAKEGAME_OPERATION_H_
#define _SNAKEGAME_OPERATION_H_

#include <stdint.h>

#define	SNAKE_GRID_COL_NUM			16		/* 网格列数 */
#define	SNAKE_GRID_ROW_NUM			6		/* 网格行数 */
#define	SNAKE_CELL_NUM				(SNAKE_GRID_COL_NUM * SNAKE_GRID_ROW_NUM)
#define	SNAKE_STEP_TICK_NUM			4		/* 每4个MENU_TICK_PERIOD_MS走一步，约200ms */

///	@brief					贪吃蛇运行状态枚举
///
///	@note
typedef enum
{
	emSnakeState_Running		=	0,
	emSnakeState_Paused,
	emSnakeState_GameOver,
	emSnakeState_Win,
}emSnakeStateTdf;

///	@brief					贪吃蛇方向枚举
///
///	@note
typedef enum
{
	emSnakeDirection_Up			=	0,
	emSnakeDirection_Down,
	emSnakeDirection_Left,
	emSnakeDirection_Right,
}emSnakeDirectionTdf;

///	@brief					贪吃蛇格子内容枚举
///
///	@note					数值即网格帧aucCell的取值，显示层按值渲染
typedef enum
{
	emSnakeCell_Empty			=	0,
	emSnakeCell_Body,
	emSnakeCell_Head,
	emSnakeCell_Food,
}emSnakeCellTdf;

///	@brief					重开一局
///
///	@param					None
///
///	@note					长度3、蛇头(5,3)、方向Right、得分0、状态Running，并生成首个食物
void vSnakeGameInit(void);

///	@brief					设置运动方向
///
///	@param			emDir			:	目标方向
///
///	@note					忽略180°反向；暂停/结束状态忽略
void vSnakeGameSetDirection(emSnakeDirectionTdf emDir);

///	@brief					切换暂停/继续
///
///	@param					None
///
///	@note					Running↔Paused，其它状态不动作
void vSnakeGameTogglePause(void);

///	@brief					走一步
///
///	@param					None
///
///	@return					1表示显示帧需要刷新，0表示无变化
///
///	@note					仅Running时推进；撞墙/撞自身转GameOver，吃满转Win
uint8_t ucSnakeGameStep(void);

///	@brief					获取当前得分
///
///	@param					None
///
///	@return					已吃到的食物数量
///
///	@note
uint8_t ucSnakeGameGetScore(void);

///	@brief					获取当前状态
///
///	@param					None
///
///	@return					当前运行状态
///
///	@note
emSnakeStateTdf emSnakeGameGetState(void);

///	@brief					填充网格显示数据
///
///	@param			pucCell			:	目标缓冲区，长度SNAKE_CELL_NUM
///
///	@note					先全部写Empty，再按蛇头/蛇身/食物写入；行优先row*ucColNum+col
void vSnakeGameFillGrid(uint8_t *pucCell);

#endif
