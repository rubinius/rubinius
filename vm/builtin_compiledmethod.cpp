#include "objects.hpp"
#include "vmmethod.hpp"
#include "builtin_task.hpp"
#include "builtin_channel.hpp"
#include "ffi.hpp"

namespace rubinius {
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
