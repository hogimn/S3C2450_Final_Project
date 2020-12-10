#include "solenoid.h"

void solenoid_init(void)
{

}

void solenoid_open(void)
{
	relay_connect(RELAY_CHANNEL_2);
}

void solenoid_close(void)
{
	relay_disconnect(RELAY_CHANNEL_2);
}

void solenoid_deinit(void)
{

}
