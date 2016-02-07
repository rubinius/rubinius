#ifndef _PROBES_H
#define _PROBES_H

#ifdef __cplusplus
extern "C" {
#endif

#define RUBINIUS_METHOD_ENTRY_ENABLED() 0
#define RUBINIUS_METHOD_ENTRY(arg0, arg1, arg2, arg3) do { } while(0)

#define RUBINIUS_METHOD_RETURN_ENABLED() 0
#define RUBINIUS_METHOD_RETURN(arg0, arg1, arg2, arg3) do { } while(0)


#define RUBINIUS_METHOD_NATIVE_ENTRY_ENABLED() 0
#define RUBINIUS_METHOD_NATIVE_ENTRY(arg0, arg1, arg2, arg3) do { } while(0)

#define RUBINIUS_METHOD_NATIVE_RETURN_ENABLED() 0
#define RUBINIUS_METHOD_NATIVE_RETURN(arg0, arg1, arg2, arg3) do { } while(0)


#define RUBINIUS_METHOD_FFI_ENTRY_ENABLED() 0
#define RUBINIUS_METHOD_FFI_ENTRY(arg0, arg1, arg2, arg3) do { } while(0)

#define RUBINIUS_METHOD_FFI_RETURN_ENABLED() 0
#define RUBINIUS_METHOD_FFI_RETURN(arg0, arg1, arg2, arg3) do { } while(0)


#define RUBINIUS_METHOD_PRIMITIVE_ENTRY_ENABLED() 0
#define RUBINIUS_METHOD_PRIMITIVE_ENTRY(arg0, arg1, arg2, arg3) do { } while(0)

#define RUBINIUS_METHOD_PRIMITIVE_RETURN_ENABLED() 0
#define RUBINIUS_METHOD_PRIMITIVE_RETURN(arg0, arg1, arg2, arg3) do { } while(0)


#define RUBINIUS_GC_BEGIN_ENABLED() 0
#define RUBINIUS_GC_BEGIN(arg0) do { } while(0)

#define RUBINIUS_GC_END_ENABLED() 0
#define RUBINIUS_GC_END(arg0) do { } while(0)


#define RUBINIUS_THREAD_START_ENABLED() 0
#define RUBINIUS_THREAD_START(arg0, arg1, arg2) do { } while(0)

#define RUBINIUS_THREAD_STOP_ENABLED() 0
#define RUBINIUS_THREAD_STOP(arg0, arg1, arg2) do { } while(0)


#define RUBINIUS_JIT_FUNCTION_BEGIN_ENABLED() 0
#define RUBINIUS_JIT_FUNCTION_BEGIN(arg0, arg1, arg2, arg3) do { } while(0)

#define RUBINIUS_JIT_FUNCTION_END_ENABLED() 0
#define RUBINIUS_JIT_FUNCTION_END(arg0, arg1, arg2, arg3) do { } while(0)

#ifdef __cplusplus
}
#endif

#endif /* _PROBES_H */
