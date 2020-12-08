#include "relay.h"

void relay_init(void)
{

}

int relay_connect(int channel)
{
    /* detect invalid parameter */
    if (channel < RELAY_CHANNEL_1 ||
        channel > RELAY_CHANNEL_4)
    {
        return -1;    
    }

}

int relay_disconnect(int channel)
{
    /* detect invalid parameter */
    if (channel < RELAY_CHANNEL_1 ||
        channel > RELAY_CHANNEL_4)
    {
        return -1;    
    }

}

void relay_deinit(void)
{

}
