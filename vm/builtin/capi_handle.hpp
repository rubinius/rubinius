#ifndef RBX_BUILTIN_CAPI_HANDLE_HPP
#define RBX_BUILTIN_CAPI_HANDLE_HPP

#include "vm.hpp"
#include "builtin/object.hpp"

namespace rubinius {
  namespace capi {
    class Handle;
  }

  class CApiHandle : public Object {
  public:
    const static object_type type = CApiHandleType;

    capi::Handle* handle;

    static CApiHandle* create(STATE, capi::Handle* handle);
  };
}

#endif
