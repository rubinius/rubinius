#ifndef __MASS_QUARK__
#define __MASS_QUARK__
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef uintptr_t quark;

quark quark_from_string(const char *string);

quark quark_from_static_string(const char *string);

const char * quark_to_string(quark q);

#ifdef __cplusplus
}
#endif

#endif /* __MASS_QUARK__ */
