#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

#include "mds2450_humidifier.h"

int main(void) 
{
	int flag =0;
    int fd;
 
    fd = open(HUMIDIFIER_PATH, O_RDWR,0);
	printf("fd = %d\n", fd);
	
	if (fd<0) {
        perror(HUMIDIFIER_PATH "error");
        exit(-1);
    }


    ioctl(fd, HUMIDIFIER_LOW, flag); getchar();
	ioctl(fd, HUMIDIFIER_HIGH, flag); getchar();
	
	
	close(fd);
	return 0;
}
