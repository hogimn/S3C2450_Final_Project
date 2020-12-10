#include <stdio.h>
#include "magnetic.h"

int main(void)
{
    magnetic_init();

    while (1)
    {
		if(magnetic_is_detected())
		{
			printf("Stop Watering\n");
		}
    }

    magnetic_deinit();
    
	return 0;                  
}
