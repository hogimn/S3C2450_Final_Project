#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "mds2450_sg90.h"

int main(void)
{
    int fd = open("/dev/mds2450_sg90", O_RDWR);
    
    struct pwm_duty_t duty;

    duty.period = SG90_PWM_PERIOD; 

    ioctl(fd, SG90_PWM_ENABLE);

    sleep(1);

    duty.pulse_width = SG90_PWM_DEGREE_90;
    ioctl(fd, SG90_PWM_DUTYRATE, &duty);

    sleep(1);


    duty.pulse_width = SG90_PWM_DEGREE_0;
    ioctl(fd, SG90_PWM_DUTYRATE, &duty);

    sleep(1);

    ioctl(fd, SG90_PWM_DISABLE);

    close(fd);

    return 0;
}
