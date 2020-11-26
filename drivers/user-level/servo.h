#ifndef __SERVO_H__
#define __SERVO_H__

#include "mds2450_sg90.h"

#define SERVO_90_DEGREE  SG90_PWM_DEGREE_90
#define SERVO_0_DEGREE   SG90_PWM_DEGREE_0
#define SERVO_PWM_PERIOD SG90_PWM_PERIOD
#define SERVO_DEV_PATH   SG90_PWM_PATH
#define SERVO_ENABLE     SG90_PWM_ENABLE
#define SERVO_DISABLE    SG90_PWM_DISABLE
#define SERVO_DUTYRATE   SG90_PWM_DUTYRATE

void servo_init(void);
void servo_rotate(int rotate_amount);
void servo_deinit(void);

#endif
