#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include "mds2450_leadcool120.h"

int main(void){
	int fd;
	int i;

	// 노드 생성 , 사용자 읽기쓰기 가능, 그룹 읽기쓰기 가능, 문자디바이스
	mknod(PWM_FAN_PATH, S_IRWXU | S_IRWXG | S_IFCHR,
		  MKDEV(PWM_FAN_MAJOR, 0));
	
	// 파일 디스크립터 열기, 읽기쓰기 모드
	fd = open(PWM_FAN_PATH, O_RDWR);
	
	struct pwm_duty_t duty;
	duty.period = PWM_FAN_PERIOD;
	
	ioctl(fd, PWM_FAN_ENABLE);
	sleep(1);
	
	for(i = 0;i < 11;i++){
		duty.pulse_width = PWM_FAN_SPEED(i);
		ioctl(fd, PWM_FAN_DUTYRATE, &duty);
		sleep(2);
	}
/*	
	duty.pulse_width = PWM_FAN_SPEED_00;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_01;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_02;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_03;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_04;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_05;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_06;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_07;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_08;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_09;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	duty.pulse_width = PWM_FAN_SPEED_10;
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
*/	
	duty.pulse_width = PWM_FAN_SPEED(0);
    ioctl(fd, PWM_FAN_DUTYRATE, &duty);
    sleep(2);
	
	ioctl(fd, PWM_FAN_DISABLE);
	close(fd);
	
	return 0;
}
