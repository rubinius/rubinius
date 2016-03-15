#ifndef RBX_CONFIG
#define RBX_CONFIG

#include "gen/config.h"
#include "detection.hpp"

#define RBX_STRERROR_BUFSIZE 256

// strerror_r has different signatures on GNU and XSI.
// - The GNU version returns a pointer to a string, which may be the one passed
//   to the function as 'buf', or some immutable static string, in which case
//   'buf' is unused.
// - The XSI version always stores the error message in 'buf' and returns 0 on
//   success.
// This macro makes sure that the error message is returned either way.
#ifdef STRERROR_R_CHAR_P
#define RBX_STRERROR(errno, buf, size) strerror_r(errno, buf, size)
#else
#define RBX_STRERROR(errno, buf, size) (strerror_r(errno, buf, size), buf)
#endif

// Enable this define for some minimal GC debugging
// #define RBX_GC_DEBUG

// Enable for GC stress. This only ensures that the interrupts
// for a GC are set. Use RBX_GC_STRESS_YOUNG and / or RBX_GC_STRESS_MATURE
// to run either the young or mature gen on each possibility
// #define RBX_GC_STRESS

// When stress testing is enabled, forces a young collection every time it
// is possible. This can be useful to flush out bugs because of moving objects.
// #define RBX_GC_STRESS_YOUNG

// When stress testing is enabled, forces a mature collection every time it
// is possible. This can be useful to flush out bugs with reachability etc.
// #define RBX_GC_STRESS_MATURE

#endif
