#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include "mds2450_sg90.h"

int main(void)
{
    int fd;

    mknod(SG90_PWM_PATH, S_IRWXU|S_IRWXG|S_IFCHR, 
          MKDEV(SG90_PWM_MAJOR, 0));

    fd = open(SG90_PWM_PATH, O_RDWR);
    
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
