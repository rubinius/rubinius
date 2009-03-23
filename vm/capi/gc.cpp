#include "builtin/object.hpp"

#include "capi/capi.hpp"

using namespace capi;

extern "C" {
  void rb_gc_mark(VALUE ptr) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Object* object = env->get_object(ptr);

    if(object->reference_p()) {
      Object* res = VM::current_state()->current_mark.call(object);

      if(res) {
        env->handles()[ptr]->set(res);
      }
    }
  }
}
