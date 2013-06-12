#ifndef RBX_MISSING_UNORDERED_SET_HPP
#define RBX_MISSING_UNORDERED_SET_HPP

#include "vm/config.h"

#ifdef RBX_HAVE_TR1
#include <tr1/unordered_set>
#define std_unordered_set std::tr1::unordered_set
#else
#include <unordered_set>
#define std_unordered_set std::unordered_set
#endif

#endif
