#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termio.h>

#include "fan.h"

static int fd_fan;

struct pwm_duty_t duty;

int fan_init(void)
{	
	fd_fan = open(PWM_FAN_PATH, O_RDWR,0);
	
	if (fd_fan<0) {
        // file open error
        return !FAN_INIT_OK;
    }
	
	duty.period = PWM_FAN_PERIOD;
	
	ioctl(fd_fan, PWM_FAN_ENABLE);
	sleep(1);
	
	return FAN_INIT_OK;
}

int fan_rotate(int speed)
{
	relay_connect(3);
	if(speed>=0&&speed<11)
	{
		duty.pulse_width = PWM_FAN_SPEED(speed);
		ioctl(fd_fan, PWM_FAN_DUTYRATE, &duty);
		return FAN_ROTATE_OK;
	}
	else 
	{
		// Unknown command
		return !FAN_ROTATE_OK;
	}
	
}

void fan_off(void)
{
	relay_disconnect(RELAY_CHANNEL_3);
}

void fan_deinit(void)
{
	relay_disconnect(3);
	
	ioctl(fd_fan, PWM_FAN_DISABLE);
	close(fd_fan);
}
