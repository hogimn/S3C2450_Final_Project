#include "relay.h"
#include <stdio.h>

int main(void)
{

	relay_init();
	
	getchar();
	
    relay_connect(RELAY_CHANNEL_1); getchar();
	relay_connect(RELAY_CHANNEL_2); getchar();
	relay_connect(RELAY_CHANNEL_3); getchar();
	relay_connect(RELAY_CHANNEL_4); getchar();
	
	relay_disconnect(RELAY_CHANNEL_1); getchar();
	relay_disconnect(RELAY_CHANNEL_2); getchar();
	relay_disconnect(RELAY_CHANNEL_3); getchar();
	relay_disconnect(RELAY_CHANNEL_4); getchar();

    relay_deinit();
    return 0;                  
}
