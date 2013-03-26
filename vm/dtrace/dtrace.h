#ifndef RUBINIUS_DTRACE_H
#define RUBINIUS_DTRACE_H

#ifdef HAVE_DTRACE
#include "dtrace/probes.h"

#define RUBINIUS_METHOD_HOOK(probe, state, mod, method, previous) \
{ \
  if(RUBINIUS_METHOD_##probe##_ENABLED()) { \
    const char* module_name = mod->debug_str(state).c_str(); \
    const char* code_name = method->debug_str(state).c_str(); \
    const char* file_name = "<unknown>"; \
    int line = 0; \
    if(previous) { \
      Symbol* file = previous->file(state); \
      if(!file->nil_p()) { \
        file_name = file->debug_str(state).c_str(); \
      } \
      line = previous->line(state); \
    } \
    RUBINIUS_METHOD_##probe(module_name, code_name, file_name, line); \
  } \
} \

#else
#include "dtrace/probes_dummy.h"
#define RUBINIUS_METHOD_HOOK(probe, state, mod, method, previous) do { } while(0)
#endif

#define RUBINIUS_METHOD_ENTRY_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(ENTRY, state, module, method, previous)

#define RUBINIUS_METHOD_RETURN_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(RETURN, state, module, method, previous)

#define RUBINIUS_METHOD_NATIVE_ENTRY_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(NATIVE_ENTRY, state, module, method, previous)

#define RUBINIUS_METHOD_NATIVE_RETURN_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(NATIVE_RETURN, state, module, method, previous)

#define RUBINIUS_METHOD_FFI_ENTRY_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(FFI_ENTRY, state, module, method, previous)

#define RUBINIUS_METHOD_FFI_RETURN_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(FFI_RETURN, state, module, method, previous)

#define RUBINIUS_METHOD_PRIMITIVE_ENTRY_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(PRIMITIVE_ENTRY, state, module, method, previous)

#define RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, module, method, previous) \
    RUBINIUS_METHOD_HOOK(PRIMITIVE_RETURN, state, module, method, previous)

#endif
