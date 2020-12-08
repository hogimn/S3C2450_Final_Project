#ifndef __MAGNETIC_H__
#define __MAGNETIC_H__

#define MAGNETIC_NULL     0
#define MAGNETIC_DETECTED 1

void magnetic_init(void);
int magnetic_is_detected(void);
void magnetic_deinit(void);

#endif
