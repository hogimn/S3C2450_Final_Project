#ifndef __MDS2450_PWM_H__
#define __MDS2450_PWM_H__

struct pwm_duty_t 
{
    int pulse_width; // nsec
    int period;      // nsec
};

#endif
