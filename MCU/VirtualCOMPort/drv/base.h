#ifndef __BASE_H__
#define __BASE_H__

#include "platform_config.h"

#include <stddef.h>

/*typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef signed short    int16_t;
typedef unsigned short  uint16_t;
typedef signed long     int32_t;
typedef unsigned long   uint32_t;*/

#ifndef INLINE
#define INLINE
#endif

#ifndef TRUE
typedef unsigned char   BOOL;
#define TRUE        1
#define FALSE       0
#endif

#ifndef NULL
#define NULL        ((void*)0)
#endif

#ifndef MIN
#define MIN(a,b)    ((a)<=(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b)    ((a)>=(b)?(a):(b))
#endif

#define BITS_SET(v,b)   ((v) |= (b))
#define BITS_CLR(v,b)   ((v) &= ~(b))
#define BITS_CHK(v,b)   (((v) & (b)) == (b))

#endif  // __BASE_H__
