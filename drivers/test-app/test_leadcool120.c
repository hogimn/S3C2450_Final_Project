#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termio.h>

#include "mds2450_leadcool120.h"

int main(void){
	int fd;
	int i;
	
	// 파일 디스크립터 열기, 읽기쓰기 모드
	fd = open(PWM_FAN_PATH, O_RDWR,0);
	
	printf("fd = %d\n", fd);
	
	if (fd<0) {
        perror(PWM_FAN_PATH " error");
        exit(-1);
    }
	
	struct pwm_duty_t duty;
	duty.period = PWM_FAN_PERIOD;
	
	ioctl(fd, PWM_FAN_ENABLE);
	sleep(1);
	
	for(i = 0;i < 11;i++){
		duty.pulse_width = PWM_FAN_SPEED(i);
		ioctl(fd, PWM_FAN_DUTYRATE, &duty);
		sleep(2);
	}
	
	duty.pulse_width = PWM_FAN_SPEED(0);
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	ioctl(fd, PWM_FAN_DISABLE);
	close(fd);
	
	return 0;
}
