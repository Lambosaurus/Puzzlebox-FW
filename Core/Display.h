#ifndef DISPLAY_H
#define DISPLAY_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef enum {
	Display_Font_8px,
	Display_Font_10px,
	Display_Font_14px,
} Display_Font_t;

/*
 * PUBLIC FUNCTIONS
 */

void Display_Init(void);
void Display_Deinit(void);

void Display_Clear(void);
void Display_Show(void);
uint8_t Display_Printf(uint8_t, uint8_t y, Display_Font_t font, const char * fmt, ...);
uint8_t Display_Print(uint8_t x, uint8_t y, Display_Font_t font, const char * str);
void Display_DrawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

/*
 * EXTERN DECLARATIONS
 */

#endif //DISPLAY_H
