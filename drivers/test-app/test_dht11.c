#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) 
{
    int fd;
    char buf[5];
    int i;

    fd = open("/dev/mds2450_dht11", O_RDWR);

    while (1)
    {
        int count = read(fd, buf, sizeof(buf)); 
		if(count<5) continue;
        printf("count: %d\n", count);
        for (i = 0; i < count; i++) 
        {
            printf("%u ", buf[i]);
        }
        printf("\n");

        sleep(1);
    }
	
	return 0;
}
