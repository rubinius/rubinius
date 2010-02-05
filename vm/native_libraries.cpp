#include <string>

#include "builtin/exception.hpp"
#include "builtin/module.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"
#include "native_libraries.hpp"

#include "vm/object_utils.hpp"


namespace rubinius {

  void VM::init_native_libraries() {
    globals().rubinius.get()->set_const(this, "LIBSUFFIX",
                                        String::create(this, RBX_LIBSUFFIX));

    rbx_dlinit();
  }

  // @todo Fix this. There should be a single interface to getting a symbol.
  // FFI expects to be able to search several libraries and fail to bind a
  // symbol, handling the ultimate failure itself. NativeMethod (subtend)
  // expects to fail if unable to bind and searches only the current process.
  // When is there ever a need to open a library without binding a symbol?
  // Also, rbx_dldefault returns different values on different systems.
  void* NativeLibrary::find_symbol(STATE, String* name, Object* library_name, bool raise) {
    rbx_dlhandle library = NativeLibrary::open(state, library_name, raise);
    void* symbol = rbx_dlsym(library, name->c_str());

    if(rbx_dlnosuch(symbol)) {
      if(raise) {
        std::string message("NativeLibrary::load_symbol(): ");
        Exception::system_call_error(state, message + rbx_dlerror());
      } else {
        return NULL;
      }
    }

    return symbol;
  }

  rbx_dlhandle NativeLibrary::open(STATE, Object* name, bool raise) {
    if (name->nil_p()) {
      return NativeLibrary::use_this_process();
    }

    /* We should always get path without file extension. */
    std::string path(as<String>(name)->c_str());
    std::ostringstream error_message("NativeLibrary::open(): ");

    rbx_dlhandle library = rbx_dlopen((path + RBX_LIBSUFFIX).c_str());

    #ifdef RBX_LIBSUFFIX2
      if(rbx_dlnosuch(library)) {
        error_message << rbx_dlerror() << "; ";

        library = rbx_dlopen((path + RBX_LIBSUFFIX2).c_str());
      }
    #endif

    if(rbx_dlnosuch(library)) {
      if(raise) {
        error_message << rbx_dlerror();
        Exception::system_call_error(state, error_message.str());
      } else {
        return NULL;
      }
    }

    return library;
  }

  rbx_dlhandle NativeLibrary::use_this_process() {
    static rbx_dlhandle self = rbx_dldefault();
    return self;
  }

}

