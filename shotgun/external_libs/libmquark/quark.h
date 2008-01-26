#ifndef __MASS_QUARK__
#define __MASS_QUARK__
#include <stdint.h>
typedef uintptr_t quark;

quark quark_from_string(const char *string);

quark quark_from_static_string(const char *string);

const char * quark_to_string(quark q);
#endif /* __MASS_QUARK__ */
