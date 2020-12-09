#include "solenoid.h"

void solenoid_init(void)
{

}

void solenoid_open(void)
{
	relay_connect(2);
}

void solenoid_close(void)
{
	relay_disconnect(2);
}

void solenoid_deinit(void)
{

}
