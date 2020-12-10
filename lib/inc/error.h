#ifndef __ERROR_H__
#define __ERROR_H__

#define ERR_HANDLE fprintf(stderr, "%s[%d]: failed\n", \
        __FUNCTION__, __LINE__); exit(1);

#endif
