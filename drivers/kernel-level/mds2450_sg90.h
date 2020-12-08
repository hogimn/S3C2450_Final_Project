#ifndef __MDS2450_SG90__
#define __MDS2450_SG90__

#include "mds2450_pwm.h"

#define SG90_PWM_IOCTL_MAGIC 'p'
#define SG90_PWM_PERIOD      20000000
#define SG90_PWM_DEGREE_0    650000
#define SG90_PWM_DEGREE_90   1650000

#define SG90_PWM_ENABLE   _IO(SG90_PWM_IOCTL_MAGIC, 0)
#define SG90_PWM_DISABLE  _IO(SG90_PWM_IOCTL_MAGIC, 1)
#define SG90_PWM_DUTYRATE _IOW(SG90_PWM_IOCTL_MAGIC, 2, struct pwm_duty_t)
#define SG90_PWM_IOCTL_MAXNR 3

#define SG90_PWM_PATH "/dev/mds2450_sg90"
#define SG90_PWM_MAJOR 30

#endif

