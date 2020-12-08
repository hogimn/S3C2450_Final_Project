#ifndef __MOISTURE_H__
#define __MOISTURE_H__

#define MOISTURE_LACK    0
#define MOISTURE_FULL    1

void moisture_init(void);
int moisture_is_full(void);
void moisture_deinit(void);

#endif
