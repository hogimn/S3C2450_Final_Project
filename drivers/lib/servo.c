#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termio.h>
#include "servo.h"

static int fd_servo;
static struct pwm_duty_t duty;

int servo_init(void)
{
	fd_servo = open(MG995_PWM_PATH, O_RDWR,0);
    
	if (fd_servo<0) {
        // file open error
        return !SERVO_INIT_OK;
    }

    duty.period = MG995_PWM_PERIOD; 
    ioctl(fd_servo, MG995_PWM_ENABLE);
	
	return SERVO_INIT_OK;
}

int servo_rotate(int rotate_amount)
{
	switch(rotate_amount)
	{
		case 1:
			duty.pulse_width = MG995_PWM_DEGREE_0;
			break;
		
		case 2:
			duty.pulse_width = MG995_PWM_DEGREE_45;
			break;
		
		case 3:
			duty.pulse_width = MG995_PWM_DEGREE_90;
			break;
		
		case 4:
			duty.pulse_width = MG995_PWM_DEGREE_135;
			break;
		
		case 5:
			duty.pulse_width = MG995_PWM_DEGREE_180;
			break;
		default :
			// Unknown command
			return !SERVO_ROTATE_OK;
	}
	
    ioctl(fd_servo, MG995_PWM_DUTYRATE, &duty);
	sleep(1);
	
	return SERVO_ROTATE_OK;
}

void servo_deinit(void)
{
	ioctl(fd_servo, MG995_PWM_DISABLE);
	close(fd_servo);
}
