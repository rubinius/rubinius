#ifdef ENABLE_LLVM

#include "defines.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "primitives.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "instruments/tooling.hpp"

#include <iostream>

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {

#include "gen/jit_primitives.cpp"
#include "gen/jit_resolver.cpp"
}
#endif
