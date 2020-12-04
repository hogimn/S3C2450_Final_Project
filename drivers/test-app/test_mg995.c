#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termio.h>

#include "mds2450_mg995.h"

int main(void)
{
    int fd;
	
	fd = open(MG995_PWM_PATH, O_RDWR,0);
    
	printf("fd = %d\n", fd);
    
	if (fd<0) {
        perror(MG995_PWM_PATH "error");
        exit(-1);
    }
   
    struct pwm_duty_t duty;
    duty.period = MG995_PWM_PERIOD; 

    ioctl(fd, MG995_PWM_ENABLE);
    sleep(1);
	
	duty.pulse_width = MG995_PWM_DEGREE_0;
    ioctl(fd, MG995_PWM_DUTYRATE, &duty);
    sleep(1);
	
	 duty.pulse_width = MG995_PWM_DEGREE_45;
    ioctl(fd, MG995_PWM_DUTYRATE, &duty);
    sleep(1);
	
    duty.pulse_width = MG995_PWM_DEGREE_90;
    ioctl(fd, MG995_PWM_DUTYRATE, &duty);
    sleep(1);
	
	duty.pulse_width = MG995_PWM_DEGREE_135;
    ioctl(fd, MG995_PWM_DUTYRATE, &duty);
    sleep(1);
	
	
	duty.pulse_width = MG995_PWM_DEGREE_180;
    ioctl(fd, MG995_PWM_DUTYRATE, &duty);
    sleep(1);
	
	
	
	
	
	

    ioctl(fd, MG995_PWM_DISABLE);
	
    close(fd);

    return 0;
}