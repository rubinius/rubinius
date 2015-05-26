#include "vm/config.h"

#include <unistd.h>

#ifdef HAVE_GETTID
#include <sys/types.h>
#else
extern "C" {
pid_t gettid(void);
}
#endif
