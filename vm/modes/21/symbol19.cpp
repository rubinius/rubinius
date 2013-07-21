#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "capi/capi.hpp"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
  }
}

extern "C" {

  VALUE rb_id2str(ID sym) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    String* str = reinterpret_cast<Symbol*>(sym)->to_str(env->state());
    return env->get_handle(str);
  }

}
