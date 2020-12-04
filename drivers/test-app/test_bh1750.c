/*
	################################################
	#        AMBIENT LIGHT SENSOR : BH1750 	  	   #
	################################################
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <termio.h>

#if 0
	#define gprintk(fmt, x... ) printf( "%s: " fmt, __FUNCTION__ , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

int main(void) {

	int fd;
	int intensity=0;
	
	unsigned char temp[2];
	uint16_t raddr=0;
	uint16_t addr = 0x23;
	
	int ret;

	fd = open("/dev/i2c-0", O_RDWR);
	printf("fd = %d\n", fd);
	
	if (fd < 0) {
		printf("file open error!\n");
		return -1;
	}

	// slave 주소 지정
	if (ioctl(fd, I2C_SLAVE, addr) < 0)
	{
		printf("address error!\n");
		return -1;
	}

	
	while(1) {
		
		usleep(1000000); // Option
		
		/*---------- power on ------------*/
		raddr=0x01;  // 0000 0001
		ret = write(fd,&raddr,1);
		gprintk("ret : %d\n",ret);
		
		
		/*------------ Reset -------------*/
		raddr=0x07;  // 0000 0111
		ret = write(fd,&raddr,1);
		gprintk("ret : %d\n",ret);
		
		
		/*---------- Send Mode instr. ------------*/
		raddr = (0x23<<1)|(0x00); //010 0011 + 0(write)
		ret = write(fd,&raddr,1);
		gprintk("ret : %d\n",ret);

		//raddr = 0x23; //0010 0011, OneTime_L_resolution_Mode
		raddr = 0x20; //0010 0000, OneTime_H_resolution_Mode

		ret = write(fd,&raddr,1);
		gprintk("ret : %d\n",ret);


		/*---------- Wait to complete measurement (max 180ms) ------------*/		
		//usleep(100000); // low resolution
		usleep(360000); // high resolution

		
		/*---------- Read Measurement Result ------------*/
		raddr = (0x23<<1)|(0x01); //010 0011 + 0(read)
		gprintk(" raddr is : 0x%x \n", raddr>>1 );
		
		ret = write(fd,&raddr,1);
		gprintk("ret : %d\n",ret);
		

		ret = read(fd, temp, 2);
		gprintk("ret : %d  , L temp : %d, L temp : %d \n",ret, temp[0],temp[1]);
		
		
		/*---------- Calculate Intensity ------------*/
		intensity = temp[0] << 8 | temp[1]  ;
		
		intensity  = (int) (intensity / 1.2);
		
		printf("intensity : %d lux\n",intensity);
		
	}
	
	close(fd);

	return 0;
}
