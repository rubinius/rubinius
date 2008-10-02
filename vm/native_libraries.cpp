#include <string>

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

  void* NativeLibrary::find_symbol(String* name, Object* library_name)
  {
    rbx_dlhandle library = open(library_name);

    /* TODO: Check dlerror() */
    if (library == NULL) {
      return NULL;
    }

    void* symbol = rbx_dlsym(library, name->c_str());

    if (symbol == NULL) {
      /* TODO: Why are we using this at all? dlopen should handle. */
      symbol = rbx_dlsym_default(name->c_str());
    }

    return symbol;
  }

  /* TODO: Should be caching the default lib . */
  rbx_dlhandle NativeLibrary::open(Object* name)
  {
    if (name->nil_p()) {
      return rbx_dlopen(NULL);
    }

    /* TODO: Should check dlerror() when loading fails hereafter. */

    /* We should always get path without file extension. */
    std::string path(as<String>(name)->c_str());

    rbx_dlhandle library = rbx_dlopen((path + RBX_LIBSUFFIX).c_str());

    #ifdef RBX_LIBSUFFIX2
    if (library == NULL) {
      library = rbx_dlopen((path + RBX_LIBSUFFIX2).c_str());
    }
    #endif

    return library;
  }

}

