#ifndef RBX_CONFIG
#define RBX_CONFIG

#include "gen/config.h"
#include "detection.hpp"

#define RBX_STRERROR_BUFSIZE 256
// This is just so annoying. strerror_r has different signatures
// on GNU and XSI, don't ask why. Also the GNU version has an
// unused warning that can't be worked around easier in another way than
// just putting an if() around and don't do anything with it.
#define RBX_STRERROR(errno, buf, size) if(strerror_r(errno, buf, size))

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
