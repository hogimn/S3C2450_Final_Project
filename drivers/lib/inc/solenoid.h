#ifndef __SOLENOID_H__
#define __SOLENOID_H__

#include "relay.h"

void solenoid_init(void);
void solenoid_open(void);
void solenoid_close(void);
void solenoid_deinit(void);

#endif
