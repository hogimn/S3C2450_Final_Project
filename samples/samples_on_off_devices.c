#include <stdio.h>
#include <unistd.h>	// sleep
#include "dryer.h"
#include "solenoid.h"
#include "led.h"

int main(void)
{
    relay_init();
	
	
	led_on();
	solenoid_open();
	dryer_on();
	
	sleep(3);
	
	led_off();
	solenoid_close();
	dryer_off();
	
	relay_deinit();
	
    return 0;                  
}
