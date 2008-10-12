#include <string>

#include "builtin/exception.hpp"
#include "builtin/module.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"
#include "native_libraries.hpp"

#include "vm/object.hpp"


namespace rubinius {

  void VM::init_native_libraries()
  {
    globals.rubinius.get()->set_const(this, "LIBSUFFIX", String::create(this, RBX_LIBSUFFIX));
  }

  void* NativeLibrary::find_symbol(STATE, String* name, Object* library_name) {
    rbx_dlhandle library = open(state, library_name);

    void* symbol = rbx_dlsym(library, name->c_str());

    if (symbol == NULL) {
      /* TODO: Why are we using this at all? dlopen should handle. */
      symbol = rbx_dlsym_default(name->c_str());

      if (symbol == NULL) {
        std::string message("NativeLibrary::find_symbol(): ");
        message += rbx_dlerror();

        Exception::system_call_error(state, message);
      }
    }

    return symbol;
  }

  /* TODO: Should be caching the default lib . */
  rbx_dlhandle NativeLibrary::open(STATE, Object* name) {
    std::ostringstream error_message("NativeLibrary::open(): ");

    /* TODO: fix */
    if (name->nil_p()) {
      return rbx_dlopen(NULL);
    }

    /* We should always get path without file extension. */
    std::string path(as<String>(name)->c_str());

    rbx_dlhandle library = rbx_dlopen((path + RBX_LIBSUFFIX).c_str());

    #ifdef RBX_LIBSUFFIX2
    if (library == NULL) {
      error_message << rbx_dlerror() << "; ";

      library = rbx_dlopen((path + RBX_LIBSUFFIX2).c_str());
    }
    #endif

    if (library == NULL) {
      error_message << rbx_dlerror();
      Exception::system_call_error(state, error_message.str());
    }

    return library;
  }

}

