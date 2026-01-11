#ifndef HARDWARE_H
#define HARDWARE_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void HW_Init(void);
void HW_Deinit(void);

void HW_SetLed(bool enable);
bool HW_ReadButton(void);
void HW_SetPower(bool enable);
uint32_t HW_ReadVBatt(void);

/*
 * EXTERN DECLARATIONS
 */


#endif // HARDWARE_H
