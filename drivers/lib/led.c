#include "led.h"

void led_init(void)
{
	
}

void led_on(void)
{
	relay_connect(1);
}

void led_off(void)
{
	relay_disconnect(1);
}

void led_deinit(void)
{

}
