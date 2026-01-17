
#include "Display.h"

#include "SPI.h"
#include "ST7571.h"
#include "Font.h"

#include "fonts/crox4h.h"
#include "fonts/crox2h.h"
#include "fonts/minicute.h"

#include <stdarg.h>
#include <stdio.h>


/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static const Font_t * Display_GetFont(Display_Font_t font);
static const FontChar_t * Display_GetChar(const Font_t * font, char ch);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Display_Init(void)
{
	SPI_Init(ST7571_SPI, 16000000, SPI_Mode_0);
	ST7571_Init();
}

void Display_Deinit(void)
{
	ST7571_Deinit();
	SPI_Deinit(ST7571_SPI);
}

void Display_Clear(void)
{
	ST7571_Clear();
}

void Display_Show(void)
{
	ST7571_Display();
}

uint8_t Display_Print(uint8_t x, uint8_t y, Display_Font_t font_size, const char * str)
{
	const Font_t * font = Display_GetFont(font_size);

	bool first_char = true;

	while (*str != 0)
	{
		if (!first_char)
		{
			x += 1;
		}
		first_char = false;

		char ch = *str++;
		const FontChar_t * char_info = Display_GetChar(font, ch);
		if (char_info != NULL)
		{
			x += char_info->pad & 0xF; // Pre pad
			ST7571_DrawImage(x, y, font->data + char_info->data_offset, char_info->width, font->height);
			x += char_info->width;
			x += char_info->pad >> 4; // Post pad
		}
	}

	return x;
}

uint8_t Display_Printf(uint8_t x, uint8_t y, Display_Font_t font, const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char bfr[64];
	vsnprintf(bfr, sizeof(bfr), fmt, va);
	va_end(va);

	return Display_Print(x, y, font, bfr);
}

void Display_DrawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
	ST7571_DrawBox(x, y, width, height, color);
}

/*
 * PRIVATE FUNCTIONS
 */

static const Font_t * Display_GetFont(Display_Font_t font)
{
	switch (font)
	{
	default:
	case Display_Font_8px:
		return &minicute;
	case Display_Font_10px:
		return &crox2h;
	case Display_Font_14px:
		return &crox4h;
	}
}

static const FontChar_t * Display_GetChar(const Font_t * font, char ch)
{
	// First check we have info for these characters.
	if (ch >= font->ascii_start)
	{
		uint8_t char_index = ch - font->ascii_start;
		if (char_index < font->char_count)
		{
			return &font->chars[char_index];
		}

	}
	return NULL;
}

/*
 * INTERRUPT ROUTINES
 */

