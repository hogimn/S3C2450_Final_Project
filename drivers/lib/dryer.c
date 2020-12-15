#include "dryer.h"

void dryer_init(void)
{
	
}

void dryer_on(void)
{
	relay_connect(4);
}

void dryer_off(void)
{
	relay_disconnect(4);
}

void dryer_deinit(void)
{

}
