#ifndef _INPUT_OPERATION_H_
#define _INPUT_OPERATION_H_

#include <stdint.h>

///	@brief					输入来源枚举
///
///	@note					板载4键与矩阵键盘共用同一个队列，用高字节区分来源
typedef enum
{
	emInputSource_Btn			=	0,
	emInputSource_KeyPad,
}emInputSourceTdf;

///	@brief					归一化按键枚举
///
///	@note					矩阵键盘键号统一映射为 emInputKey_KeyPadBase + 键号（0~19）
typedef enum
{
	emInputKey_Up				=	0,
	emInputKey_Down,
	emInputKey_Enter,
	emInputKey_Cancel,
	emInputKey_KeyPadBase		=	16,
}emInputKeyTdf;

#define INPUT_EVENT_MAKE(emSource, ucCode)	(((uint32_t)(emSource) << 8) | (uint32_t)(uint8_t)(ucCode))
#define	INPUT_EVENT_SOURCE(ulMsg)			((uint32_t)(ulMsg) >> 8)
#define	INPUT_EVENT_CODE(ulMsg)				((uint32_t)(ulMsg) & 0xFFu)

#endif
