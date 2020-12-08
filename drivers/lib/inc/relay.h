#ifndef __RELAY_H__
#define __RELAY_H__

#include "mds2450_szh_rlbg_012.h"

#define RELAY_CHANNEL_1  1
#define RELAY_CHANNEL_2  2
#define RELAY_CHANNEL_3  3
#define RELAY_CHANNEL_4  4

void relay_init(void);
int relay_connect(int channel);
int relay_disconnect(int channel);
void relay_deinit(void);

#endif
