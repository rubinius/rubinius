#include "builtin/thread_state.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/class.hpp"

#include "object_utils.hpp"

#include "ontology.hpp"

namespace rubinius {

  void ThreadState::init(STATE) {
    GO(thread_state).set(ontology::new_class(state, "ThreadState", G(object), G(rubinius)));
    G(thread_state)->set_object_type(state, ThreadStateType);
  }

  ThreadState* ThreadState::create(STATE) {
    return state->new_object<ThreadState>(G(thread_state));
  }

} // rubinius
