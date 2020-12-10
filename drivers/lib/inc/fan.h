#ifndef __FAN_H__
#define __FAN_H__

#include "relay.h"
#include "mds2450_leadcool120.h"

#define FAN_SPEED_NORMAL    0
#define FAN_SPEED_FAST      6
#define FAN_SPEED_VERY_FAST 10

#define FAN_INIT_OK			1
#define	FAN_ROTATE_OK 		1

int fan_init(void);
int fan_rotate(int speed);
void fan_off(void);
void fan_deinit(void);


#endif
