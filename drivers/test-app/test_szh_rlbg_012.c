#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

#include "mds2450_szh_rlbg_012.h"

int main(void) 
{
	int flag =0;
    int fd;
 
    fd = open("/dev/mds2450_relay", O_RDWR,0);
	printf("fd = %d\n", fd);
	
	if (fd<0) {
        perror("/dev/mds2450_relay error");
        exit(-1);
    }


    ioctl(fd, IN1_LOW, flag); getchar();
	ioctl(fd, IN1_HIGH, flag); getchar();
	
	ioctl(fd, IN2_LOW, flag); getchar();
	ioctl(fd, IN2_HIGH, flag); getchar();
	
	ioctl(fd, IN3_LOW, flag); getchar();
	ioctl(fd, IN3_HIGH, flag); getchar();
	
	ioctl(fd, IN4_LOW, flag); getchar();
	ioctl(fd, IN4_HIGH, flag); getchar();
	
	close(fd);
}
