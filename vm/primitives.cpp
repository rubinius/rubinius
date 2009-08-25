#include "prelude.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "event.hpp"
#include "gen/includes.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "instruments/profiler.hpp"

#include <iostream>

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#endif

namespace rubinius {

#ifdef ENABLE_LLVM
  void Primitives::queue_for_jit(STATE, CallFrame* call_frame) {
    LLVMState* ls = LLVMState::get(state);
    VMMethod* vmm = call_frame->cm->backend_method();

    VMMethod* candidate = ls->find_candidate(vmm, call_frame);
    if(candidate->call_count < 0) return;

    candidate->call_count = -1; // So we don't try and jit twice at the same time
    state->stats.jitted_methods++;

    ls->compile_soon(state, candidate);

    if(state->shared.config.jit_show_compiling) {
      std::cout << "[[[ JIT Queued method "
        << ls->queued_methods() << "/"
        << ls->jitted_methods() << " ]]]\n";
    }
  }
#endif

  Object* Primitives::unknown_primitive(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args) {
    std::string message = std::string("Called unbound or invalid primitive from method name: ");
    message += msg.name->to_str(state)->c_str();

    Exception::assertion_error(state, message.c_str());

    return Qnil;
  }

  static inline void check_counter(STATE, CallFrame* call_frame, int which) {
#ifdef ENABLE_LLVM
    int& hits = state->shared.primitive_hits(which);
    if(hits >= 0 && ++hits >= state->shared.config.jit_call_til_compile) {
      hits = -1;
      Primitives::queue_for_jit(state, call_frame);
    }
#endif
  }

#include "gen/primitives_glue.gen.cpp"
}
