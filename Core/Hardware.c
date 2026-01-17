#include "Hardware.h"

#include "GPIO.h"
#include "ADC.h"
#include "US.h"
#include "I2C.h"
#include "TIM.h"
#include "SPI.h"

#include "Console.h"
#include "Logging.h"

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

static struct {
	bool backlight_on;
} gHW;

/*
 * PUBLIC FUNCTIONS
 */

void HW_Init(void)
{
	Console_Init();
	Log_Info("Booting...");

	GPIO_EnableOutput(LED_PIN, false);
	GPIO_EnableInput(BUTTON_PIN, GPIO_Pull_Up);
	GPIO_EnableOutput(BOARD_PWR_PIN, false);
	GPIO_EnableOutput(VBATT_SNS_EN_PIN, false);

	GPIO_EnableInput(TOUCH_KEY_0, GPIO_Pull_Down);
	GPIO_EnableInput(TOUCH_KEY_1, GPIO_Pull_Down);
	GPIO_EnableInput(TOUCH_KEY_2, GPIO_Pull_Down);
	GPIO_EnableInput(TOUCH_KEY_3, GPIO_Pull_Down);

	I2C_Init(BOARD_I2C, I2C_Mode_Fast);

	gHW.backlight_on = false;
}

void HW_Deinit(void)
{
	HW_SetBacklight(0);

	I2C_Deinit(BOARD_I2C);

	GPIO_Deinit(TOUCH_KEY_0);
	GPIO_Deinit(TOUCH_KEY_1);
	GPIO_Deinit(TOUCH_KEY_2);
	GPIO_Deinit(TOUCH_KEY_3);

	GPIO_Deinit(LED_PIN);
	GPIO_Deinit(VBATT_SNS_EN_PIN);
	GPIO_Deinit(BOARD_PWR_PIN);
}

void HW_SetLed(bool enable)
{
	GPIO_Write(LED_PIN, enable);
}

bool HW_ReadButton(void)
{
	return !GPIO_Read(BUTTON_PIN);
}

void HW_SetPower(bool enable)
{
	GPIO_Write(BOARD_PWR_PIN, enable);
}

uint32_t HW_ReadVBatt(void)
{
	ADC_Init();

	GPIO_Write(VBATT_SNS_EN_PIN, true);
	US_Delay(10);
	uint32_t mv = AIN_AinToDivider(ADC_Read(VBATT_SNS_AIN), 10, 10);
	GPIO_Write(VBATT_SNS_EN_PIN, false);

	ADC_Deinit();

	return mv;
}

uint8_t HW_ReadKeys(void)
{
	uint8_t keys = 0;
	if (GPIO_Read(TOUCH_KEY_0))
		keys |= 0x01;
	if (GPIO_Read(TOUCH_KEY_1))
		keys |= 0x02;
	if (GPIO_Read(TOUCH_KEY_2))
		keys |= 0x04;
	if (GPIO_Read(TOUCH_KEY_3))
		keys |= 0x08;
	return keys;
}

void HW_SetBacklight(uint8_t pct)
{
	if (pct > 0)
	{
		if (!gHW.backlight_on)
		{
			TIM_Init(BACKLIGHT_TIM, 100 * 10000, 99);
			TIM_EnablePwm(BACKLIGHT_TIM, BACKLIGHT_TIM_CH, BACKLIGHT_PIN, BACKLIGHT_PIN_AF);
			TIM_Start(BACKLIGHT_TIM);
			gHW.backlight_on = true;
		}
		TIM_SetPulse(BACKLIGHT_TIM, BACKLIGHT_TIM_CH, pct);
	}
	else
	{
		GPIO_Deinit(BACKLIGHT_PIN);
		TIM_Deinit(BACKLIGHT_TIM);
		gHW.backlight_on = false;
	}
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
