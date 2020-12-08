#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

// 자석을 감지하면 값이 0 됨.
int main(void)
{
	int fd;
	int ret;
	int prevRet = -1 ;

	printf("\nStarting Magnetic sensor test\n");
	
	fd = open("/dev/mds2450_szh_ssbh_040", O_RDWR, 0);
	
	printf("fd = %d\n", fd);
	
	if(fd < 0) {
		perror("/dev/mds2450_szh_ssbh_040 open error\n");
		exit(-1);
	}
	
	while(1)
	{
		ret = read(fd,NULL,0);
		if(ret != prevRet)	printf("\nMagnetic sensor data : %d\n",ret);
		prevRet = ret;
	}
	

	close(fd);

	return 0;
}
