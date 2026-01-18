
#include "ST7571.h"

#include "SPI.h"
#include "GPIO.h"
#include "Core.h"
#include "US.h"

/*
 * PRIVATE DEFINITIONS
 */

#define ST7571_CMD_RESET					0xE2

#define ST7571_CMD_DISPLAY_ON    			0xAF
#define ST7571_CMD_DISPLAY_OFF   			0xAE

#define ST7571_CMD_MODE						0x38
#define ST7571_CMD_MODE_DEFAULT				0x54

#define ST7571_CMD_CONTRAST					0x81
#define ST7571_CMD_CONTRAST_DEFAULT			20

#define ST7571_CMD_PWRCTL_ON1				0x2C
#define ST7571_CMD_PWRCTL_ON2				0x2E
#define ST7571_CMD_PWRCTL_ON3				0x2F

#define ST7571_CMD_REGULATOR				0x26
#define ST7571_CMD_LCD_BIAS					0x50
#define ST7571_CMD_OSC_ON					0xAB
#define ST7571_CMD_BOOSTLEVEL_6				0x67
#define ST7571_CMD_BOOSTLEVEL_3				0x64
#define ST7571_CMD_SET_FRC_PWM				0x93
#define ST7571_CMD_LINE_INVERSION_ON		0x4C
#define ST7571_CMD_DUTY						0x48
#define ST7571_CMD_DUTY_DEFAULT				0x80
#define ST7571_CMD_SET_NLINE_INV			0x5C
#define ST7571_CMD_SET_NLINE_INV_DEFAULT	12

#define ST7571_CMD_SET_ALLPTS_NORMAL 		0xA4
#define ST7571_CMD_SET_ALLPTS_ON  			0xA5

#define ST7571_CMD_SET_COLUMN_UPPER  		0x10
#define ST7571_CMD_SET_COLUMN_LOWER  		0x00

#define ST7571_CMD_SET_DISP_START_LINE  	0x40
#define ST7571_CMD_SET_PAGE  				0xB0

#define ST7571_CMD_SET_ADC_NORMAL  			0xA0
#define ST7571_CMD_SET_ADC_REVERSE 			0xA1

#define ST7571_CMD_SET_DISP_NORMAL 			0xA6
#define ST7571_CMD_SET_DISP_REVERSE 		0xA7

#define ST7571_CMD_SET_COM_NORMAL  			0xC0
#define ST7571_CMD_SET_COM_REVERSE  		0xC8

#define ST7571_CMD_POWERSAVE_ON				0xA9
#define ST7571_CMD_POWERSAVE_OFF			0xE1

#define ST7571_CMD_EXTENTION_SET_3			0x7B
#define ST7571_CMD_EXTENTION_SET_EXIT		0x00
#define ST7571_CMD_COLORMODE_BW				0x11
#define ST7571_CMD_COLORMDOE_GREY			0x10

#ifndef ST7571_BPP
#define ST7571_BPP							1 // Bits per pixel
#endif

#define ST7571_COL_COUNT					(ST7571_WIDTH)
#define ST7571_PAGE_COUNT					(ST7571_HEIGHT / 8)
#define ST7571_COL_OFFSET					0

#define ABS_DIF(a,b)						((a)>(b) ? (a)-(b) : (b)-(a))
#define SET_BITS(byte, mask, set)			((byte) = (set) ? ((byte) | (mask)) : ((byte) & ~(mask)))

#if (ST7571_BPP == 1)
#elif (ST7571_BPP == 2)
#else
#error "Unsupported bits-per-pixel"
#endif

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static inline void ST7571_Select(void);
static inline void ST7571_Deselect(void);

static void ST7571_WriteCommand(uint8_t cmd);
static void ST7571_WriteCommands(const uint8_t * cmds, uint32_t size);
static void ST7571_WriteData(const uint8_t * data, uint32_t size);
static void ST7571_WritePage(uint8_t page, uint8_t offset, uint8_t length);
static void ST7571_FloodPage(uint8_t page, uint8_t x, uint8_t width, uint8_t mask, uint8_t color);

static void ST7571_SetPixel(uint8_t x, uint8_t y, uint8_t color);

static inline uint8_t * ST7571_GetPage(uint8_t page, uint8_t column);
static inline void ST7571_SwapBytes(uint8_t * a, uint8_t * b);

#ifdef ST7571_TRACK_PAGE_EDITS
static void ST7571_ClearPageEdits(void);
static void ST7571_SetPageEdits(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
#endif

/*
 * PRIVATE VARIABLES
 */

// Memory is arranged in pages - which traverse the display width.
// Each byte is one column in the page, 8 bits tall.
static uint8_t gDisplayBuffer[ST7571_COL_COUNT * ST7571_BPP * ST7571_PAGE_COUNT];

#ifdef ST7571_TRACK_PAGE_EDITS
// Record the affected regions
static struct {
	struct {
		uint8_t col_min;
		uint8_t col_max;
	} page_edits[ST7571_PAGE_COUNT];
} gST75xx;
#endif

/*
 * PUBLIC FUNCTIONS
 */

void ST7571_Init(void)
{
	GPIO_EnableOutput(ST7571_A0_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(ST7571_RST_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(ST7571_CS_PIN, GPIO_PIN_SET);

	// Make sure the reset is held for a little bit to get the display to reset.
	US_Delay(10);
	GPIO_Set(ST7571_RST_PIN);
	US_Delay(10);

	CORE_Delay(200);

	const uint8_t cfg_cmds[] = {
		ST7571_CMD_MODE,
		ST7571_CMD_MODE_DEFAULT,
		ST7571_CMD_OSC_ON,
		ST7571_CMD_LCD_BIAS | 7,
		ST7571_CMD_DUTY,
		ST7571_CMD_DUTY_DEFAULT,
		ST7571_CMD_REGULATOR,
		ST7571_CMD_CONTRAST,
		ST7571_CMD_CONTRAST_DEFAULT,

#if (ST7571_BPP == 1)
		// Enter B/W mode
		ST7571_CMD_EXTENTION_SET_3,
		ST7571_CMD_COLORMODE_BW,
		ST7571_CMD_EXTENTION_SET_EXIT,
#endif
		ST7571_CMD_SET_FRC_PWM,
		ST7571_CMD_SET_ADC_NORMAL,
		ST7571_CMD_SET_COM_NORMAL,

		ST7571_CMD_BOOSTLEVEL_6,
		ST7571_CMD_PWRCTL_ON1,

	};
	ST7571_WriteCommands(cfg_cmds, sizeof(cfg_cmds));

	// This seems to be some kind of power enable sequence....
	CORE_Delay(100);
	ST7571_WriteCommand(ST7571_CMD_PWRCTL_ON2);
	CORE_Delay(100);
	ST7571_WriteCommand(ST7571_CMD_PWRCTL_ON3);
	CORE_Delay(10);
	ST7571_WriteCommand(ST7571_CMD_DISPLAY_ON);

	ST7571_Clear();
}

void ST7571_Deinit(void)
{
	ST7571_WriteCommand(ST7571_CMD_DISPLAY_OFF);

	GPIO_Deinit(ST7571_A0_PIN);
	GPIO_Deinit(ST7571_RST_PIN);
	GPIO_Deinit(ST7571_CS_PIN);
}

void ST7571_SetContrast(uint8_t contrast)
{
	const uint8_t cmd[] = {
		ST7571_CMD_CONTRAST,
		contrast & 0x3f
	};
	ST7571_WriteCommands(cmd, sizeof(cmd));
}

void ST7571_Clear(void)
{
	bzero(gDisplayBuffer, sizeof(gDisplayBuffer));
#ifdef ST7571_TRACK_PAGE_EDITS
	ST7571_SetPageEdits(0, 0, ST7571_WIDTH-1, ST7571_HEIGHT-1);
#endif
}

void ST7571_Fill(uint8_t color)
{
	for (uint32_t page = 0; page < ST7571_PAGE_COUNT; page++)
	{
		ST7571_FloodPage(page, 0, ST7571_WIDTH, 0xFF, color);
	}
#ifdef ST7571_TRACK_PAGE_EDITS
	ST7571_SetPageEdits(0, 0, ST7571_WIDTH-1, ST7571_HEIGHT-1);
#endif
}

void ST7571_Display(void)
{
#ifdef ST7571_TRACK_PAGE_EDITS
	for (uint32_t page = 0; page < ST7571_PAGE_COUNT; page++)
	{
		uint8_t col_min = gST75xx.page_edits[page].col_min;
		uint8_t col_max = gST75xx.page_edits[page].col_max;
		if ( col_max >= col_min )
		{
			ST7571_WritePage(page, col_min, col_max + 1 - col_min);
		}
	}
	ST7571_ClearPageEdits();
#else
	for (uint32_t page = 0; page < ST7571_PAGE_COUNT; page++)
	{
		ST7571_WritePage(page, 0, ST7571_WIDTH);
	}
#endif
}

void ST7571_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
	// Bresenhams algorithm

	// If its steep, iterate over Y instead.
	uint8_t steep = ABS_DIF(y1, y0) > ABS_DIF(x1, x0);
	if (steep) {
		ST7571_SwapBytes(&x0, &y0);
		ST7571_SwapBytes(&x1, &y1);
	}

	// Make sure x is ordered.
	if (x0 > x1) {
		ST7571_SwapBytes(&x0, &x1);
		ST7571_SwapBytes(&y0, &y1);
	}

	uint8_t dx = x1 - x0;
	uint8_t dy = ABS_DIF(y1, y0);

	int8_t err = dx / 2;
	int8_t ystep = (y0 < y1) ? 1 : -1;

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			ST7571_SetPixel(y0, x0, color);
		}
		else
		{
			ST7571_SetPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
		  y0 += ystep;
		  err += dx;
		}
	}

#ifdef ST7571_TRACK_PAGE_EDITS
	ST7571_SetPageEdits(x0, y0, x1, y1);
#endif
}

// Image must match the display format. Ie, rows of 8 bit columns.
void ST7571_DrawImage(uint8_t x, uint8_t y, const uint8_t * img, uint8_t width, uint8_t height)
{
	if (x > ST7571_WIDTH || y > ST7571_HEIGHT)
	{
		return;
	}

	uint32_t row_bytes = width * ST7571_BPP;

	if (x + width > ST7571_WIDTH)
		width = ST7571_WIDTH - x;
	if (y + height > ST7571_HEIGHT)
		height = ST7571_HEIGHT - y;

	uint8_t row0_offset = y & 0x07;
	uint8_t row1_offset = 8 - row0_offset;
	uint8_t last_page = (y + height - 1) / 8;

	if (row0_offset == 0)
	{
		// Writes are page aligned.
		for (uint8_t page = y / 8; page <= last_page; page++)
		{
			uint8_t * dst = ST7571_GetPage(page, x);
			const uint8_t * src = img;
			img += row_bytes;

			for (uint8_t ix = 0; ix < (width * ST7571_BPP); ix++)
			{
				*dst++ |= *src++;
			}
		}

	}
	else
	{
		// Writes not page aligned. Need to hit two rows.
		for (uint8_t page = y / 8; page < last_page; page++)
		{
			uint8_t * dst0 = ST7571_GetPage(page, x);
			uint8_t * dst1 = ST7571_GetPage(page+1, x);
			const uint8_t * src = img;
			img += row_bytes;

			for (uint8_t ix = 0; ix < (width * ST7571_BPP); ix++)
			{
				*dst0++ |= (*src) << row0_offset;
				*dst1++ |= (*src++) >> row1_offset;
			}
		}

		// last page
		uint8_t * dst = ST7571_GetPage(last_page, x);
		for (uint8_t ix = 0; ix < (width * ST7571_BPP); ix++)
		{
			*dst++ |= (*img++) << row0_offset;
		}

	}

#ifdef ST7571_TRACK_PAGE_EDITS
	ST7571_SetPageEdits(x, y, x + width - 1, y + height - 1);
#endif
}

void ST7571_DrawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
	if (x > ST7571_WIDTH || y > ST7571_HEIGHT)
	{
		return;
	}
	if (x + width > ST7571_WIDTH)
	{
		width = ST7571_WIDTH - x;
	}
	if (y + height > ST7571_HEIGHT)
	{
		height = ST7571_HEIGHT - y;
	}

	uint8_t y_end = y + height - 1;
	uint8_t page = y / 8;
	uint8_t page_end;
	do
	{
		uint8_t mask = 0xFF;

		uint8_t page_start = page * 8;
		if (y > page_start)
		{
			mask <<= y - page_start;
		}

		page_end = page_start + 7;

		if (y_end < page_end)
		{
			mask &= 0xFF >> (page_end - y_end);
		}

		ST7571_FloodPage(page, x, width, mask, color);
		page++;
	}
	while ( y_end > page_end );

#ifdef ST7571_TRACK_PAGE_EDITS
	ST7571_SetPageEdits(x, y, x + width - 1, y + height - 1);
#endif
}

/*
 * PRIVATE FUNCTIONS
 */

static void ST7571_SetPixel(uint8_t x, uint8_t y, uint8_t color)
{
	uint8_t page = y / 8;
	uint8_t row = y - (page * 8);
	uint8_t mask = 1 << row;
	uint8_t * dst = ST7571_GetPage(page, x);

#if (ST7571_BPP == 2)
	SET_BITS(*dst, mask, color & 0x02);
	dst++;
#endif
	SET_BITS(*dst, mask, color & 0x01);
}

static void ST7571_FloodPage(uint8_t page, uint8_t x, uint8_t width, uint8_t mask, uint8_t color)
{
	uint8_t * dst = ST7571_GetPage(page, x);

#if (ST7571_BPP == 1)
	if (mask == 0xFF)
	{
		// We can optimize for aligned pages.
		memset(dst, color & 0x01 ? 0xFF : 0x00, width);
		return;
	}
#endif

	while (width--)
	{
#if (ST7571_BPP == 2)
		SET_BITS(*dst, mask, color & 0x02);
		dst++;
#endif
		SET_BITS(*dst, mask, color & 0x01);
		dst++;
	}
}

static void ST7571_WritePage(uint8_t page, uint8_t col_start, uint8_t col_count)
{
	uint8_t col_addr = col_start + ST7571_COL_OFFSET;
	const uint8_t cmd[] = {
		ST7571_CMD_SET_PAGE 		| page,
		ST7571_CMD_SET_COLUMN_LOWER | (col_addr & 0x0F),
		ST7571_CMD_SET_COLUMN_UPPER | (col_addr >> 4),
	};
	ST7571_WriteCommands(cmd, sizeof(cmd));
	ST7571_WriteData(ST7571_GetPage(page, col_start), col_count * ST7571_BPP);
}


#ifdef ST7571_TRACK_PAGE_EDITS
static void ST7571_SetPageEdits(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t page_start = y0 / 8;
	uint8_t page_end = y1 / 8;

	for (uint32_t p = page_start; p <= page_end; p++)
	{
		if (gST75xx.page_edits[p].col_max < x1) {
			gST75xx.page_edits[p].col_max = x1;
		}
		if (gST75xx.page_edits[p].col_min > x0) {
			gST75xx.page_edits[p].col_min = x0;
		}
	}
}

static void ST7571_ClearPageEdits(void)
{
	for (uint32_t i = 0; i < ST7571_PAGE_COUNT; i++)
	{
		gST75xx.page_edits[i].col_min = ST7571_COL_COUNT - 1;
		gST75xx.page_edits[i].col_max = 0;
	}
}
#endif //ST7571_TRACK_PAGE_EDITS

static inline void ST7571_SwapBytes(uint8_t * a, uint8_t * b)
{
	uint8_t tmp = *a;
	*a = *b;
	*b = tmp;
}


/*
 * PRIVATE FUNCTIONS: INTERFACE
 */

static inline void ST7571_Select(void)
{
	GPIO_Reset(ST7571_CS_PIN);
}

static inline void ST7571_Deselect(void)
{
	GPIO_Set(ST7571_CS_PIN);
}

static inline uint8_t * ST7571_GetPage(uint8_t page, uint8_t column)
{
	return gDisplayBuffer + ((page * ST7571_COL_COUNT) + column) * ST7571_BPP;
}

static void ST7571_WriteCommand(uint8_t cmd)
{
	ST7571_WriteCommands(&cmd, 1);
}

static void ST7571_WriteCommands(const uint8_t * cmds, uint32_t size)
{
	ST7571_Select();
	SPI_Write(ST7571_SPI, cmds, size);
	ST7571_Deselect();
}

static void ST7571_WriteData(const uint8_t * data, uint32_t size)
{
	GPIO_Set(ST7571_A0_PIN);
	ST7571_Select();
	SPI_Write(ST7571_SPI, data, size);
	ST7571_Deselect();
	GPIO_Reset(ST7571_A0_PIN);
}



