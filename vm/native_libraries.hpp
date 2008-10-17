#ifndef RBX_VM_NATIVE_LIBRARIES_HPP
#define RBX_VM_NATIVE_LIBRARIES_HPP


#ifdef _WIN32
  #define RBX_LIBSUFFIX ".dll"
#else
  #ifdef __APPLE_CC__
    #define RBX_LIBSUFFIX ".bundle"
    #define RBX_LIBSUFFIX2 ".dylib"
  #else
    #define RBX_LIBSUFFIX ".so"
  #endif
#endif


#ifdef CONFIG_USE_LTDL

#error

  #include "ltdl.h"

  #define rbx_dlinit()            lt_dlinit()
  #define rbx_dlhandle            lt_dlhandle
  #define rbx_dldefault()         lt_dlopen(NULL)
  #define rbx_dlopen(name)        lt_dlopen(name)
  #define rbx_dlsym               lt_dlsym
  #define rbx_dlsym_default(name) lt_dlsym(NULL, name)
  #define rbx_dlerror             lt_dlerror

#else

  #include <dlfcn.h>

  #define rbx_dlinit()            /* No expansion */
  #define rbx_dlhandle            void*
  #define rbx_dldefault()         RTLD_DEFAULT
  #define rbx_dlopen(name)        dlopen(name, RTLD_NOW | RTLD_GLOBAL)
  #define rbx_dlsym               dlsym
  #define rbx_dlsym_default(name) dlsym(RTLD_DEFAULT, name)
  #define rbx_dlerror             dlerror

#endif


namespace rubinius {

  typedef void (*GenericFunctionPtr)(void);

  /* TODO: Add ::close(), check use cases. */
  class NativeLibrary {
  public:   /* Interface */

    /** Obtain function pointer to given symbol in given lib. */
    static void*        find_symbol(STATE, String* name, Object* library_name);
    /** Loader error message, if any. */
    static String*      last_error_message();
    /** Load and open a native library (or this process if nil.) */
    static rbx_dlhandle open(STATE, Object* library_name);

  };

}

#endif

