#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include "servo.h"

int main(void)
{
    servo_init();

    servo_rotate(SERVO_90_DEGREE);

    sleep(1);

    servo_rotate(SERVO_0_DEGREE);

    servo_deinit();

    return 0;                  
}
