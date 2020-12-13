#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) 
{
    int fd;
    char buf[5];
    int count;
    int i;

    fd = open("/dev/mds2450_dht11", O_RDWR);

    while (1)
    {
        count = read(fd, buf, sizeof(buf)); 
        if (count == -1)
        {
            printf("read() failed");
            sleep(2);
        }

        for (i = 0; i < count; i++) 
        {
            printf("%u ", buf[i]);
        }
        printf("\n");

        sleep(2);
    }

    return 0;
}
