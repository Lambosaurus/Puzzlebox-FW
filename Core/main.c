
#include "Core.h"
#include "Hardware.h"
#include "Logging.h"

#include "I2C.h"
#include "BME280.h"

int main(void)
{
	CORE_Init();
	HW_Init();

	HW_SetPower(true);
	CORE_Delay(100);

	if (!BME280_Init())
	{
		Log_Error("BME280 init error");
	}

	while(1)
	{
		uint32_t pressure;
		int16_t temp;
		uint8_t hum;
		if (!BME280_Read(&pressure, &temp, &hum))
		{
			Log_Error("BME280 read error");
		}
		Log_Info("pressure: %d, temp: %d, hum: %d", pressure, temp, hum);

		CORE_Delay(250);
		CORE_Idle();
	}
}

