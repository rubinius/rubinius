#ifndef RBX_CONFIG
#define RBX_CONFIG

#include "detection.hpp"

// Currently, these are only support on x86-32

#ifdef IS_X8632

// Add support for the new dynamic interpreter
#define USE_DYNAMIC_INTERPRETER

// Add support for the usage based JIT (experimental)
#define USE_USAGE_JIT

#endif

#endif
