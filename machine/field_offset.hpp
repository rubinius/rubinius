#ifndef RBX_FIELD_OFFSET
#define RBX_FIELD_OFFSET

#include <stdint.h>

// Stolen from Squirrelfish Extreme's JIT.cpp
// FIELD_OFFSET: Like the C++ offsetof macro, but you can use it with classes.
// The magic number 0x4000 is insignificant. We use it to avoid using NULL, since
// NULL can cause compiler problems, especially in cases of multiple inheritance.
#ifdef FIELD_OFFSET
#undef FIELD_OFFSET // Fix conflict with winnt.h.
#endif
#define FIELD_OFFSET(class, field) (reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<class*>(0x4000)->field)) - 0x4000)

#endif
