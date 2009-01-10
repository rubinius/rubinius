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


/* Little dynamic loading API. */

#include <dlfcn.h>

  #define rbx_dlerror         dlerror
  #define rbx_dldefault()     RTLD_DEFAULT
  #define rbx_dlhandle        void*
  #define rbx_dlinit()        /* No expansion */
  #define rbx_dlnosuch(ptr)   ((ptr) == NULL)
  #define rbx_dlopen(name)    dlopen(name, RTLD_NOW | RTLD_GLOBAL)
  #define rbx_dlsym           dlsym


namespace rubinius {

  typedef void (*GenericFunctionPtr)(void);

  /* @todo Add ::close(), check use cases. */
  class NativeLibrary {
  public:   /* Interface */

    /** Obtain function pointer to given symbol in given lib. */
    static void* find_symbol(STATE, String* name, Object* library_name, bool raise = true);

    /** Load and open a native library (or this process if nil.) */
    static rbx_dlhandle open(STATE, Object* library_name, bool raise = true);


  private:  /* Internals */

    /** Pseudo-library handle to this process and all its symbols. */
    static rbx_dlhandle use_this_process();

  };

}

#endif

