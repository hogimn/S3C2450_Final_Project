#include "led.h"

void led_init(void)
{
	
}

void led_on(void)
{
	relay_connect(RELAY_CHANNEL_1);
}

void led_off(void)
{
	relay_disconnect(RELAY_CHANNEL_1);
}

void led_deinit(void)
{

}
