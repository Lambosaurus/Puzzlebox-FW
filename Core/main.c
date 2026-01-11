
#include "Core.h"
#include "Hardware.h"
#include "Logging.h"

int main(void)
{
	CORE_Init();
	HW_Init();

	HW_SetPower(true);

	while(1)
	{
		CORE_Idle();
	}
}

