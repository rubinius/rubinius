#ifndef RBX_MISSING_UNORDERED_MAP_HPP
#define RBX_MISSING_UNORDERED_MAP_HPP

#include "vm/config.h"

#ifdef RBX_HAVE_TR1
#include <tr1/unordered_map>
#define std_unordered_map std::tr1::unordered_map
#else
#include <unordered_map>
#define std_unordered_map std::unordered_map
#endif

#endif
