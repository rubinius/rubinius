#ifndef RUBINIUS_DTRACE_H
#define RUBINIUS_DTRACE_H

#ifdef HAVE_DTRACE
#include "dtrace/probes.h"

#define RUBINIUS_METHOD_HOOK(probe, state, mod, method, previous) \
{ \
  if(RUBINIUS_METHOD_##probe##_ENABLED()) { \
    RBX_DTRACE_CONST char* module_name = \
        const_cast<RBX_DTRACE_CONST char*>(mod->debug_str(state).c_str()); \
    RBX_DTRACE_CONST char* code_name = \
        const_cast<RBX_DTRACE_CONST char*>(method->debug_str(state).c_str()); \
    RBX_DTRACE_CONST char* file_name = \
        const_cast<RBX_DTRACE_CONST char*>("<unknown>"); \
    int line = 0; \
    if(previous) { \
      Symbol* file = previous->file(state); \
      if(!file->nil_p()) { \
        file_name = const_cast<RBX_DTRACE_CONST char*>(file->debug_str(state).c_str()); \
      } \
      line = previous->line(state); \
    } \
    RUBINIUS_METHOD_##probe(module_name, code_name, file_name, line); \
  } \
} \

#define RUBINIUS_OBJECT_ALLOCATE_HOOK(state, obj, frame) \
{ \
  if(RUBINIUS_OBJECT_ALLOCATE_ENABLED()) { \
    Class* mod = obj->direct_class(state); \
    RBX_DTRACE_CONST char* module_name = \
      const_cast<RBX_DTRACE_CONST char*>(mod->debug_str(state).c_str()); \
    RBX_DTRACE_CONST char* file_name = \
      const_cast<RBX_DTRACE_CONST char *>("<unknown>"); \
    int line = 0; \
    if(frame) { \
      Symbol* file = frame->file(state); \
      if(!file->nil_p()) { \
        file_name = const_cast<RBX_DTRACE_CONST char*>(file->debug_str(state).c_str()); \
      } \
      line = frame->line(state); \
    } \
    RUBINIUS_OBJECT_ALLOCATE(module_name, file_name, line); \
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
