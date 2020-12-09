#include <stdio.h>
#include <unistd.h>	// sleep
#include "servo.h"

int main(void)
{
    servo_init();

	servo_rotate(SERVO_180_DEGREE);
	sleep(1);
	
	servo_rotate(SERVO_135_DEGREE);
	sleep(1);
	
	servo_rotate(SERVO_90_DEGREE);
	sleep(1);
	
	servo_rotate(SERVO_45_DEGREE);
	sleep(1);
	
	servo_rotate(SERVO_0_DEGREE);
	sleep(1);


    servo_deinit();

    return 0;                  
}
