#ifndef ST7571_H
#define ST7571_H

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

void ST7571_Init(void);
void ST7571_Deinit(void);

void ST7571_SetContrast(uint8_t contrast);

// All writes are buffered, and this causes the buffer to be written to screen.
void ST7571_Display(void);

void ST7571_Clear(void);
void ST7571_Fill(uint8_t color);
void ST7571_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void ST7571_DrawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

void ST7571_DrawImage(uint8_t x, uint8_t y, const uint8_t * img, uint8_t width, uint8_t height);

/*
 * EXTERN DECLARATIONS
 */

#endif //ST7571_H
