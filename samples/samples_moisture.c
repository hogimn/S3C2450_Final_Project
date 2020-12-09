#include <stdio.h>
#include "moisture.h"

int main(void)
{
    moisture_init();

    while (1)
    {
		if(moisture_is_full())
		{
			printf("stop watering\n");
		}
    }

    moisture_deinit();
    
	return 0;                  
}
