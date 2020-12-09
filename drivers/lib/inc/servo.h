#ifndef __SERVO_H__
#define __SERVO_H__

#include "mds2450_mg995.h"

#define	SERVO_INIT_OK	1
#define SERVO_ROTATE_OK 1

#define SERVO_180_DEGREE   	5
#define SERVO_135_DEGREE   	4
#define SERVO_90_DEGREE		3
#define SERVO_45_DEGREE   	2
#define SERVO_0_DEGREE   	1


int servo_init(void);
int servo_rotate(int rotate_amount);
void servo_deinit(void);

#endif
