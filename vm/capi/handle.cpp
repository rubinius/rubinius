#include "builtin/native_method.hpp"
#include "object_memory.hpp"
#include "gc/baker.hpp"
#include "util/allocator.hpp"
#include "capi/capi.hpp"
#include "capi/handle.hpp"
#include "capi/handles.hpp"
#include "capi/ruby.h"

namespace rubinius {
  namespace capi {

    bool Handle::valid_handle_p(STATE, Handle* handle) {
      Handles* capi_handles = state->memory()->capi_handles();
      return capi_handles->validate(handle);
    }

    void Handle::free_data() {
      if(as_.cache_data) {
        switch(type_) {
        case cRArray:
          delete[] as_.rarray->dmwmb;
          delete as_.rarray;
          break;
        case cRString:
          delete as_.rstring;
          break;
        case cRFloat:
          delete as_.rfloat;
          break;
        case cRIO:
          // When the IO is finalized, the FILE* is closed.
          delete as_.rio;
          break;
        case cRFile:
          delete as_.rfile;
          break;
        case cRData:
          delete as_.rdata;
          break;
        default:
          break;
        }
        as_.cache_data = 0;
      }

      type_ = cUnknown;
    }

    void Handle::debug_print() {
      std::cerr << std::endl << "Invalid handle usage detected!" << std::endl;
      std::cerr << "  handle:     " << this << std::endl;
      std::cerr << "  checksum:   0x" << std::hex << checksum_ << std::endl;
      std::cerr << "  references: " << references_ << std::endl;
      std::cerr << "  type:       " << type_ << std::endl;
      std::cerr << "  object:     " << object_ << std::endl;
    }

    HandleSet::HandleSet()
      : slow_(0)
    {
      for(int i = 0; i < cFastHashSize; i++) {
        table_[i] = 0;
      }
    }

    void HandleSet::deref_all() {
      for(int i = 0; i < cFastHashSize; i++) {
        if(table_[i]) table_[i]->deref();
      }

      if(slow_) {
        for(SlowHandleSet::iterator i = slow_->begin();
            i != slow_->end();
            ++i) {
          Handle* handle = *i;
          handle->deref();
        }
      }
    }

    void HandleSet::flush_all(NativeMethodEnvironment* env) {
      for(int i = 0; i < cFastHashSize; i++) {
        if(table_[i]) table_[i]->flush(env);
      }

      if(slow_) {
        for(SlowHandleSet::iterator i = slow_->begin();
            i != slow_->end();
            ++i) {
          Handle* handle = *i;
          handle->flush(env);
        }
      }
    }

    void HandleSet::update_all(NativeMethodEnvironment* env) {
      for(int i = 0; i < cFastHashSize; i++) {
        if(table_[i]) table_[i]->update(env);
      }

      if(slow_) {
        for(SlowHandleSet::iterator i = slow_->begin();
            i != slow_->end();
            ++i) {
          Handle* handle = *i;
          handle->update(env);
        }
      }
    }

    void HandleSet::gc_scan(GarbageCollector* gc) {
      for(int i = 0; i < cFastHashSize; i++) {
        if(Handle* handle = table_[i]) {
          handle->set_object(gc->mark_object(handle->object()));
        }
      }

      if(slow_) {
        for(SlowHandleSet::iterator i = slow_->begin();
            i != slow_->end();
            ++i) {
          Handle* handle = *i;
          handle->set_object(gc->mark_object(handle->object()));
        }
      }
    }

    bool HandleSet::slow_add_if_absent(Handle* handle) {
      for(int i = 0; i < cFastHashSize; i++) {
        if(table_[i] == handle) return false;
      }

      std::pair<SlowHandleSet::iterator, bool> ret = slow_->insert(handle);

      if(ret.second) {
        handle->ref();
      }
      return ret.second;
    }

    void HandleSet::make_slow_and_add(Handle* handle) {
      // Inflate it to the slow set.
      slow_ = new SlowHandleSet;
      slow_->insert(handle);
      handle->ref();
    }
  }
}
