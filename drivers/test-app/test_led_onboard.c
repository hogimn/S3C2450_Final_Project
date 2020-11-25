#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termio.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <signal.h>
#include <linux/kdev_t.h>
#include <sys/stat.h>
#include "mds2450_led_onboard.h"

#define GPG4    4
#define GPG7    7

void interval_expired(int id);
void stop_handler(int signum);

static int count = 0; 
static timer_t timer_1; 
static struct itimerspec itime = {{1,0}, {1,0}};
static struct itimerspec last_itime;

static int fd;
static int cur_led = GPG4;

int main(void)
{
    int i;
    int flags = 0;

    mknod(LED_ONBOARD_DEV_PATH, S_IRWXU|S_IRWXG|S_IFCHR,
          MKDEV(LED_ONBOARD_MAJOR, 0));

    fd = open(LED_ONBOARD_DEV_PATH, O_RDWR);
    if (fd < 0)
    {
        perror(LED_ONBOARD_DEV_PATH" failed");
        exit(-1);
    }

    /* set up to signal SIGALRM if timer expires */
    timer_create(CLOCK_REALTIME, 0, &timer_1);

    signal(SIGALRM, (void(*)())interval_expired);
    signal(SIGSTOP, stop_handler);

    /* period: 1 sec */
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = 100000000;
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 100000000;

    timer_settime(timer_1, flags, &itime, &last_itime);

    for(;;)
        pause();
}

void stop_handler(int signum)
{
    close(fd); 
}

void interval_expired(int id)
{
    int flags = 0;
    struct timespec rtclock_time;

    clock_gettime(CLOCK_REALTIME, &rtclock_time);
    count++;

    if(count % 2)
        printf("tick @ %ld sec, %ld nsec\n", rtclock_time.tv_sec, rtclock_time.tv_nsec);
    else
        printf("tock @ %ld sec, %ld nsec\n", rtclock_time.tv_sec, rtclock_time.tv_nsec);
    fflush(stdout);

    ioctl(fd, LED_ONBOARD_TOGGLE, cur_led);
    cur_led = (cur_led==GPG7)? GPG4 : cur_led+1;
}
