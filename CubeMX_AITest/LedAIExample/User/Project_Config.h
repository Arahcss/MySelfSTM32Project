#ifndef _PROJECT_CONFIG_H_
#define _PROJECT_CONFIG_H_

#define TRUE				1
#define FALSE				0

#define	STM32_IC_HAL			"stm32f1xx_hal.h"

#define	BTN_DEV_NUM					4
#define	BUTTON_UP					emBtnDevNum0
#define	BUTTON_DOWN					emBtnDevNum1
#define	BUTTON_ENTER				emBtnDevNum2
#define	BUTTON_CANCEL				emBtnDevNum3

#define	KEYPAD_ROW_NUM				5
#define	KEYPAD_COL_NUM				4
#define	KEYPAD_DEV_NUM				(KEYPAD_ROW_NUM * KEYPAD_COL_NUM)
#define	KEYPAD_DEBOUNCE_THRESHOLD	2

#define	CALC_INPUT_MAX_VALUE		9999
#define	CALC_INPUT_MAX_DIGIT		4
#define	CALC_LINE_BUFFER_LENGTH		17

#endif
