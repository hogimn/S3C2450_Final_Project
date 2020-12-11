#include "humidifier.h"
#include <stdlib.h>
#include <stdio.h>
#define ERR_HANDLE fprintf(stderr, "%s[%d]: failed\n", \
        __FUNCTION__, __LINE__); exit(1);
		
int main(void)
{
	if(humidifier_init()!=HUMIDIFIER_INIT_OK)
	{
		ERR_HANDLE;
	}
	printf("start\n");
	humidifier_on();	getchar();
	
	humidifier_off();	getchar();
	
	humidifier_off();	getchar();
	
	humidifier_on();	getchar();
	
    humidifier_deinit();
    
    return 0;                  
}
