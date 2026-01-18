
#include "font_template.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

static const FontChar_t FONT_NAME_chars[] = {
FONT_CHAR_INSERT
};

static const uint8_t FONT_NAME_data[] = {
FONT_DATA_INSERT
};

const Font_t FONT_NAME = {
	.height = FONT_HEIGHT,
	.ascii_start = FONT_ASCII_START,
	.char_count = LENGTH(FONT_NAME_chars),
	.chars = FONT_NAME_chars,
	.data = FONT_NAME_data,
};

/*
 * PUBLIC FUNCTIONS
 */

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

