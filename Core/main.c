
#include "Core.h"
#include "Hardware.h"
#include "Logging.h"

#include "Display.h"

int main(void)
{
	CORE_Init();
	HW_Init();

	HW_SetPower(true);
	CORE_Delay(100);

	Display_Init();
	Display_Printf(0, 0, Display_Font_8px, "Yo dog");
	Display_Show();

	HW_SetBacklight(30);

	while(1)
	{
		CORE_Idle();
	}
}

