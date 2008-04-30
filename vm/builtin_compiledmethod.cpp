#include "builtin.hpp"
#include "ffi.hpp"

namespace rubinius {
  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = (CompiledMethod*)state->new_object(G(cmethod));
    return cm;
  }

  CompiledMethod::Visibility* CompiledMethod::Visibility::create(STATE) {
    return (CompiledMethod::Visibility*)state->new_object(G(cmethod_vis));
  }

  void CompiledMethod::post_marshal(STATE) {

  }

  size_t CompiledMethod::number_of_locals() {
    return 0;
  }
  void CompiledMethod::set_scope(StaticScope* scope) {
  }

  VMMethod* CompiledMethod::vmmethod(STATE) {
    if(compiled->nil_p()) {
      VMMethod* vmm = new VMMethod(this);
      compiled = MemoryPointer::create(state, vmm);
      return vmm;
    }

    /* This isn't type safe. */
    return (VMMethod*)compiled->pointer;
  }
}
