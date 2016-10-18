#include "memory.hpp"

#include "class/class.hpp"
#include "class/thread_state.hpp"

namespace rubinius {

  void ThreadState::bootstrap(STATE) {
    GO(thread_state).set(state->memory()->new_class<Class, ThreadState>(
          state, G(rubinius), "ThreadState"));
  }

  ThreadState* ThreadState::create(STATE) {
    return state->memory()->new_object<ThreadState>(state, G(thread_state));
  }
}
