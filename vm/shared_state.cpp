#include "shared_state.hpp"
#include "config_parser.hpp"
#include "config.h"
#include "objectmemory.hpp"
#include "instruments/profiler.hpp"
#include "global_cache.hpp"
#include "capi/handle.hpp"

namespace rubinius {
  SharedState::SharedState(Configuration& config, ConfigParser& cp)
    : initialized_(false)
    , global_handles_(new capi::Handles)
    , profiling_(false)
    , profiler_collection_(0)
    , global_serial_(0)
    , om(0)
    , global_cache(0)
    , config(config)
    , user_variables(cp)
  {
    ref();
  }

  SharedState::~SharedState() {
    if(!initialized_) return;

    delete om;
    delete global_cache;
    delete global_handles_;

#ifdef ENABLE_LLVM
    if(!reuse_llvm) llvm_cleanup();
#endif
  }

  void SharedState::discard(SharedState* ss) {
    if(ss->deref()) delete ss;
  }

  VM* SharedState::new_vm() {
    VM* vm = new VM(*this);
    cf_locations_.push_back(vm->call_frame_location());
    this->ref();
    return vm;
  }

  void SharedState::remove_vm(VM* vm) {
    cf_locations_.remove(vm->call_frame_location());
    this->deref();

    // Don't delete ourself here, it's too problematic.
  }

  void SharedState::enable_profiling(VM* vm) {
    profiler_collection_ = new profiler::ProfilerCollection(vm);
    profiling_ = true;
  }

  LookupTable* SharedState::disable_profiling(VM* vm) {
    if(profiler_collection_) {
      LookupTable* profile = profiler_collection_->results(vm);
      delete profiler_collection_;
      profiler_collection_ = 0;
      profiling_ = false;
      return profile;
    } else {
      return reinterpret_cast<LookupTable*>(Qnil);
    }
  }

  void SharedState::add_profiler(VM* vm, profiler::Profiler* profiler) {
    if(profiler_collection_) {
      profiler_collection_->add_profiler(vm, profiler);
    }
  }

  void SharedState::remove_profiler(VM* vm, profiler::Profiler* profiler) {
    if(profiler_collection_) {
      profiler_collection_->remove_profiler(vm, profiler);
    }
  }
}
