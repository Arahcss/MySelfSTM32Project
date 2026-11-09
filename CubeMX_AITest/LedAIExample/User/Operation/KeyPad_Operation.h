#ifndef _KEYPAD_OPERATION_H_
#define _KEYPAD_OPERATION_H_

#include "KeyPadDevice.h"


void vKeyPadInitAll(void);
void vKeyPadExecuteAll(void);
emKeyPadDevNumTdf emKeyPadGetPressEvent(void);
uint32_t ulKeyPadGetPressedMask(void);

#endif
