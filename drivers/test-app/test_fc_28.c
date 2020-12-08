#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

// 물에 담그면 0 됨.
int main(void)
{
	int fd;
	int ret;
	int prevRet = -1 ;

	printf("\nStarting Soil Moisture sensor test\n");
	
	fd = open("/dev/mds2450_fc_28", O_RDWR, 0);
	
	printf("fd = %d\n", fd);
	
	if(fd < 0) {
		perror("/dev/mds2450_fc_28 open error\n");
		exit(-1);
	}
	
	while(1)
	{
		ret = read(fd,NULL,0);
		if(ret != prevRet)	printf("\nSoil Moisture sensor data : %d\n",ret);
		prevRet = ret;
	}
	

	close(fd);

	return 0;
}
