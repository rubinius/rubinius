#ifndef RUBINIUS_DTRACE_H
#define RUBINIUS_DTRACE_H

#define RBX_DTRACE_CHAR_P RBX_DTRACE_CONST char*

#ifdef HAVE_DTRACE
#include "dtrace/probes.h"

#define RUBINIUS_METHOD_HOOK(probe, state, mod, method) \
{ \
  if(RUBINIUS_METHOD_##probe##_ENABLED()) { \
    RBX_DTRACE_CHAR_P module_name = \
        const_cast<RBX_DTRACE_CHAR_P>(mod->debug_str(state).c_str()); \
    RBX_DTRACE_CHAR_P code_name = \
        const_cast<RBX_DTRACE_CHAR_P>(method->debug_str(state).c_str()); \
    RBX_DTRACE_CHAR_P file_name = \
        const_cast<RBX_DTRACE_CHAR_P>("<unknown>"); \
    int line = 0; \
    if(CallFrame* frame = state->get_ruby_frame()) { \
      Symbol* file = frame->file(state); \
      if(!file->nil_p()) { \
        file_name = const_cast<RBX_DTRACE_CHAR_P>(file->debug_str(state).c_str()); \
      } \
      line = frame->line(state); \
    } \
    RUBINIUS_METHOD_##probe(module_name, code_name, file_name, line); \
  } \
} \

#else
#include "dtrace/probes_dummy.h"
#define RUBINIUS_METHOD_HOOK(probe, state, mod, method) do { } while(0)
#endif

#define RUBINIUS_METHOD_ENTRY_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(ENTRY, state, module, method)

#define RUBINIUS_METHOD_RETURN_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(RETURN, state, module, method)

#define RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(NATIVE_ENTRY, state, module, method)

#define RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(NATIVE_RETURN, state, module, method)

#define RUBINIUS_METHOD_FFI_ENTRY_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(FFI_ENTRY, state, module, method)

#define RUBINIUS_METHOD_FFI_RETURN_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(FFI_RETURN, state, module, method)

#define RUBINIUS_METHOD_PRIMITIVE_ENTRY_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(PRIMITIVE_ENTRY, state, module, method)

#define RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, module, method) \
    RUBINIUS_METHOD_HOOK(PRIMITIVE_RETURN, state, module, method)

#endif
