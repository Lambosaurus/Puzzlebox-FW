#ifndef BOARD_H
#define BOARD_H

#define STM32G0

// Core config
//#define CORE_USE_TICK_IRQ

// CLK config
//#define CLK_USE_HSE
//#define CLK_USE_LSE
//#define CLK_LSE_BYPASS
//#define CLK_LSE_FREQ		32768
//#define CLK_SYSCLK_FREQ	32000000

// RTC config
//#define RTC_USE_IRQS

// US config
//#define US_TIM			TIM_22
//#define US_RES			1

// ADC config
#define ADC_VREF	        3000

// GPIO config
#define GPIO_USE_IRQS
#define GPIO_IRQ4_ENABLE 	// LIS2_INT_PIN

// TIM config
//#define TIM_USE_IRQS
#define TIM2_ENABLE
#define TIM16_ENABLE

// UART config
#define UART1_PINS			(PA9 | PA10)
#define UART1_AF		    GPIO_AF1_USART1
#define UART_BFR_SIZE     	128

// SPI config
#define SPI1_PINS		    (PA5 | PA7)
#define SPI1_AF				GPIO_AF0_SPI1

// I2C config
#define I2C1_PINS			(PB6 | PB7)
#define I2C1_AF				GPIO_AF6_I2C1
//#define I2C_USE_FASTMODEPLUS
//#define I2C_USE_LONG_TRANSFER

// USB config
#define USB_ENABLE
#define USB_CLASS_CDC
#define USB_CDC_BFR_SIZE	512
#define USB_PD				2
#define USB_PD_SINK

// IO config
#define VBATT_SNS_EN_PIN	PA0
#define VBATT_SNS_AIN		ADC_Channel_1

#define MIC_REF_DAC			DAC_CH1
#define MIC_AIN				ADC_Channel_16

#define BOARD_PWR_PIN		PA15
#define BOARD_I2C			I2C_1
#define LED_PIN				PB9
#define BUTTON_PIN			PC13

#define	TOUCH_KEY_0			PB13
#define TOUCH_KEY_1			PB14
#define TOUCH_KEY_2			PB15
#define TOUCH_KEY_3			PA2

#define BUZZER_PIN			PB8
#define BUZZER_TIM			TIM_16
#define BUZZER_TIM_CH		TIM_CH1
#define BUZZER_PIN_AF		GPIO_AF2_TIM16

#define BACKLIGHT_PIN		PA3
#define BACKLIGHT_TIM		TIM_2
#define BACKLIGHT_TIM_CH	TIM_CH4
#define BACKLIGHT_PIN_AF	GPIO_AF2_TIM2

#define CONSOLE_UART		UART_1
#define CONSOLE_BAUD		115200
#define CONSOLE_DET_PIN		PC7
#define LOG_PRINT_COLOR
#define LOG_PRINT_TIMESTAMP

#define LIS2DT_INT_PIN		PB4
#define LIS2DT_I2C			BOARD_I2C

#define VL53L3_XSHUT_PIN	PB5
#define VL53L3_I2C			BOARD_I2C

#define VEML3328_I2C		BOARD_I2C

#define BME280_I2C			BOARD_I2C

#define LIS2MD_I2C			BOARD_I2C

#define M24XX_SERIES		512
#define M24XX_I2C			BOARD_I2C

#define ST7571_WIDTH		128
#define ST7571_HEIGHT		128
#define ST7571_SPI			SPI_1
#define ST7571_CS_PIN		PB1
#define ST7571_A0_PIN		PA6
#define ST7571_RST_PIN		PB0


#endif /* BOARD_H */
