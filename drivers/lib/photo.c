#include "photo.h"
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
#include <time.h>

#if 0
	#define gprintk(fmt, x... ) printf(fmt , ## x)
#else
	#define gprintk(x...) do { } while (0)
#endif

static int fd_photo;

int photo_init(void)
{
	uint16_t addr = 0x23;
	
	fd_photo = open("/dev/i2c-0", O_RDWR);
	
	if (fd_photo < 0) {
		// file open error
		return PHOTO_INIT_FILE_OPEN_FAIL;
	}
	
	if (ioctl(fd_photo, I2C_SLAVE, addr) < 0)
	{
		// address error
		return PHOTO_INIT_I2C_FAIL;
	}
	
	return PHOTO_INIT_OK;
}

int photo_get_intensity(void)
{
	uint16_t raddr=0;	
	unsigned char temp[2];
	int intensity;
	int ret;
		
	/*---------- power on ------------*/
	raddr=0x01;  // 0000 0001
	ret = write(fd_photo,&raddr,1);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d\n",ret);
	
	
	/*------------ Reset -------------*/
	raddr=0x07;  // 0000 0111
	ret = write(fd_photo,&raddr,1);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d\n",ret);
	
	
	/*---------- Send Mode instr. ------------*/
	raddr = (0x23<<1)|(0x00); //010 0011 + 0(write)
	ret = write(fd_photo,&raddr,1);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d\n",ret);

	//raddr = 0x23; //0010 0011, OneTime_L_resolution_Mode
	raddr = 0x20; //0010 0000, OneTime_H_resolution_Mode

	ret = write(fd_photo,&raddr,1);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d\n",ret);


	/*---------- Wait to complete measurement (max 180ms) ------------*/		
	//usleep(100000); // low resolution
	usleep(360000); // high resolution

	
	/*---------- Read Measurement Result ------------*/
	raddr = (0x23<<1)|(0x01); //010 0011 + 0(read)
	gprintk(" raddr is : 0x%x \n", raddr>>1 );

	ret = write(fd_photo,&raddr,1);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d\n",ret);
	

	ret = read(fd_photo, temp, 2);
	if (ret == -1)
	{
        return ret;	
	}
	gprintk("ret : %d  , L temp : %d, L temp : %d \n",ret, temp[0],temp[1]);
	
	
	/*---------- Calculate Intensity ------------*/
	intensity = temp[0] << 8 | temp[1]  ;
	
	intensity  = (int) (intensity / 1.2);
	
	return intensity;
}

int check_night(void)
{
	time_t timer;
	struct tm*t;
	while(1)
	{
    timer = time(NULL);
	t=localtime(&timer);
	printf("%d\n\n",t->tm_hour);
	if(t->tm_hour>=8 && t->tm_hour<18)	return !NIGHT_TIME;
	//if(t->tm_min>=22 && t->tm_min<23)	return !NIGHT_TIME;
	else return NIGHT_TIME;
	}
}

void photo_deinit(void)
{
	close(fd_photo);
}
