#ifndef __ERROR_H__
#define __ERROR_H__

//#define ERR_HANDLE fprintf(stderr, "%s[%d]: %s() failed\n", \
//        __FILE__, __LINE__, __FUNCTION__); exit(1);

#define ERR_HANDLE fprintf(stderr, "%s[%d]: %s() failed\n", \
        __FILE__, __LINE__, __FUNCTION__);

#endif
