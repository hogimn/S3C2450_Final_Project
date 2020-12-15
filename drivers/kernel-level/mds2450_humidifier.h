#ifndef _HUMIDIFIER_H_
#define _HUMIDIFIER_H_

#define HUMIDIFIER_PATH "/dev/mds2450_humidifier"

#define HUMIDIFIER_MAGIC    'k'
#define HUMIDIFIER_MAXNR    2

#define HUMIDIFIER_LOW     	_IO(HUMIDIFIER_MAGIC, 0)	
#define HUMIDIFIER_HIGH       	_IO(HUMIDIFIER_MAGIC, 1)
	  

#endif 
