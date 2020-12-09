#include <stdio.h>
#include <unistd.h>	// sleep
#include "photo.h"

int main(void)
{
	int ret;

    photo_init();

    while (1)
    {
		ret = photo_get_intensity();

		printf("intensity : %d\n",ret);
		sleep(1);
    }

    photo_deinit();
    
	return 0;                  
}
