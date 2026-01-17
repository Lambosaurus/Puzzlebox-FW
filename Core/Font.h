#ifndef FONT_H
#define FONT_H

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

typedef struct {
	uint8_t width;
	uint8_t pad;
	uint16_t data_offset;
} FontChar_t;

typedef struct {
	uint8_t height;
	uint8_t ascii_start;
	uint8_t char_count;
	const FontChar_t * chars;
	const uint8_t * data;
} Font_t;

/*
 * EXTERN DECLARATIONS
 */

#endif //FONT_H
