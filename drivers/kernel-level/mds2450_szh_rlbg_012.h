#ifndef _RELAY_H_
#define _RELAY_H_

#define RELAY_MAGIC    'k'
#define RELAY_MAXNR    8

#define IN1_LOW     	_IO(RELAY_MAGIC, 0)	
#define IN1_HIGH       	_IO(RELAY_MAGIC, 1)
#define IN2_LOW     	_IO(RELAY_MAGIC, 2)	
#define IN2_HIGH       	_IO(RELAY_MAGIC, 3)	
#define IN3_LOW     	_IO(RELAY_MAGIC, 4)	
#define IN3_HIGH      	_IO(RELAY_MAGIC, 5)	
#define IN4_LOW     	_IO(RELAY_MAGIC, 6)	
#define IN4_HIGH      	_IO(RELAY_MAGIC, 7)	  

#endif 
