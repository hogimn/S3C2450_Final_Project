#include <stdio.h>
#include <unistd.h>	// sleep
#include "fan.h"

int main(void)
{
	relay_init();
    fan_init();
	sleep(3);
	
	printf("start fan test\n");
	
	fan_rotate(FAN_SPEED_NORMAL);
	sleep(3);
	
	fan_rotate(FAN_SPEED_FAST);
	sleep(3);
	
	fan_rotate(FAN_SPEED_VERY_FAST);
	sleep(3);
	

    fan_deinit();
	relay_deinit();
    
	return 0;                  
}
