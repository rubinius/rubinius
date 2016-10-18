#include "call_frame.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/call_site.hpp"
#include "class/exception.hpp"
#include "class/executable.hpp"
#include "class/object.hpp"

#include <stdlib.h>
#include <sstream>

namespace rubinius {
  int CallSite::max_caches = 0;
  int CallSite::max_evictions = 0;
  CallSite::Executor CallSite::default_execute = CallSite::lookup_invoke_cache;

  void CallSite::bootstrap(STATE) {
    GO(call_site).set(state->memory()->new_class<Class, CallSite>(
          state, G(rubinius), "CallSite"));

    if(state->shared().config.machine_call_site_cache.value) {
      default_execute = lookup_invoke_cache;
    } else {
      default_execute = dispatch;
    }

    max_caches = state->shared().config.machine_call_site_limit.value;
    max_evictions = state->shared().config.machine_call_site_evictions.value;
  }

  void CallSite::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);

    CallSite* call_site = as<CallSite>(obj);

    call_site->evict_and_mark(mark);
  }
}
