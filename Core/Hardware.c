#include "Hardware.h"

#include "GPIO.h"
#include "ADC.h"
#include "US.h"
#include "I2C.h"

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

	I2C_Init(BOARD_I2C, I2C_Mode_Fast);
}

void HW_Deinit(void)
{
	GPIO_Deinit(LED_PIN);
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

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
