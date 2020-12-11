#include "humidifier.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

static int fd_humidifier;
static int cnt = 0;

int humidifier_init(void)
{
	fd_humidifier = open(HUMIDIFIER_PATH, O_RDWR,0);
	
	if (fd_humidifier<0) {
       // file open error
       return !HUMIDIFIER_INIT_OK;
    }
    
	ioctl(fd_humidifier, HUMIDIFIER_HIGH, 0);
	
	return HUMIDIFIER_INIT_OK;
}

void humidifier_push(void)
{
	cnt++;
	ioctl(fd_humidifier, HUMIDIFIER_LOW, 0);
    usleep(200000);
    ioctl(fd_humidifier, HUMIDIFIER_HIGH, 0);
}

void humidifier_push_2(void)
{
	humidifier_push();
    
    usleep(500000);
    
    humidifier_push();
}

void humidifier_on(void)
{
	printf("humidifier_on :: cnt = %d\n",cnt);
	if(cnt%3==0)		
	{
		printf("1 push\n");
		humidifier_push();	// 한번도 눌리지 않았다면 한 번 눌러야 켜짐
	}
	
	else if(cnt%3==1)
	{
		printf("do nothing\n");
			// 1 push -> do nothing already on
	}
	else		
	{
		printf("2 push\n");
		humidifier_push_2(); // 2 push(연속 분사) -> 일반 분사로
	}
	usleep(100000);
}

void humidifier_off(void)
{
	printf("humidifier_off :: cnt = %d\n",cnt);
	if(cnt%3==0)		
	{
		printf("do nothing\n");	// 한번도 눌리지 않았다면 꺼져있는 상태 do nothing
		
	}
	else if(cnt%3==1)
	{
		printf("2 push\n");
		humidifier_push_2();	// 1 push -> 2push(off)
	}	
	else
	{
		printf("1 push\n");
		humidifier_push();
	}
	usleep(100000);
}

void humidifier_deinit(void)
{
    humidifier_off();
    
    close(fd_humidifier);
}