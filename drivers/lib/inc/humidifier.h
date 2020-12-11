#ifndef __HUMIDIFIER_H__
#define __HUMIDIFIER_H__

#include "mds2450_humidifier.h"

#define HUMIDIFIER_INIT_OK 1

int humidifier_init(void);
void humidifier_push(void);
void humidifier_push_2(void);
void humidifier_on(void);
void humidifier_off(void);
void humidifier_deinit(void);


#endif
