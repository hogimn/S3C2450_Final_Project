#ifndef __FAN_H__
#define __FAN_H__

#include "relay.h"
#include "mds2450_leadcool120.h"

#define FAN_SPEED_NORMAL    1
#define FAN_SPEED_FAST      2
#define FAN_SPEED_VERY_FAST 3

void fan_init(void);
int fan_rotate(int speed);
void fan_deinit(void);

#endif
