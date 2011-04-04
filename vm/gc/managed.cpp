#include "vm/vm.hpp"

#include "gc/managed.hpp"
#include "shared_state.hpp"

namespace rubinius {
  ManagedThread::ManagedThread(SharedState& ss, ManagedThread::Kind kind)
    : shared_(ss)
    , kind_(kind)
    , name_(kind == eRuby ? "<ruby>" : "<system>")
    , thread_id_(ss.new_thread_id())
  {}
}
