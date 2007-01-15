#ifndef __SHOTGUN__
#define __SHOTGUN__ 1
#define INTERNAL_MACROS 1
#define INTERNAL_DEBUG 1

#ifndef swap32
#define swap32(x)       ((((x)&0xFF)<<24)       \
                        |(((x)>>24)&0xFF)       \
                        |(((x)&0x0000FF00)<<8)  \
                        |(((x)&0x00FF0000)>>8)  )
#endif

#ifndef MAIN
extern void *__main_address;
#endif

#include "rubinius.h"
#include "state.h"
#include "object.h"
#include "auto.h"
#include "log.h"
#endif
