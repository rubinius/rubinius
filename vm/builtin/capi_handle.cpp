#include "builtin/capi_handle.hpp"

namespace rubinius {
  CApiHandle* CApiHandle::create(STATE, capi::Handle* handle) {
    CApiHandle* wrapper = state->new_object<CApiHandle>(G(object));
    wrapper->handle = handle;
    return wrapper;
  }
}
