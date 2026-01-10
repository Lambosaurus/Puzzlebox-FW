
#include "Core.h"
#include "GPIO.h"


int main(void)
{
	CORE_Init();
	GPIO_EnableOutput(LED_PIN, false);


	while(1)
	{
		GPIO_Write(LED_PIN, true);
		CORE_Delay(500);
		GPIO_Write(LED_PIN, false);
		CORE_Delay(500);
	}
}

