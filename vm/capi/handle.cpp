#include "builtin/nativemethod.hpp"
#include "capi/handle.hpp"

#include "capi/18/include/ruby.h"

namespace rubinius {
  namespace capi {

    bool Handle::valid_handle_p(STATE, Handle* handle) {
      Handles* global_handles = state->shared().global_handles();
      Handles* cached_handles = state->shared().cached_handles();

      for(Handles::Iterator i(*global_handles); i.more(); i.advance()) {
        if(i.current() == handle) return true;
      }

      for(Handles::Iterator i(*cached_handles); i.more(); i.advance()) {
        if(i.current() == handle) return true;
      }

      return false;
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

    Handle::~Handle() {
      free_data();
      invalidate();
    }

    Handles::~Handles() {
      capi::Handle* handle = front();

      while(handle) {
        capi::Handle* next = static_cast<capi::Handle*>(handle->next());

        remove(handle);
        delete handle;

        handle = next;
      }
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
          capi::Handle* handle = *i;
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
          capi::Handle* handle = *i;
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
          capi::Handle* handle = *i;
          handle->update(env);
        }
      }
    }

    bool HandleSet::slow_add_if_absent(Handle* handle) {
      for(int i = 0; i < cFastHashSize; i++) {
        if(table_[i] == handle) return false;
      }

      SlowHandleSet::iterator pos = slow_->find(handle);
      if(pos != slow_->end()) return false;

      slow_->insert(handle);
      handle->ref();

      return true;
    }

    void HandleSet::make_slow_and_add(Handle* handle) {
      // Inflate it to the slow set.
      slow_ = new SlowHandleSet;
      slow_->insert(handle);
      handle->ref();
    }
  }
}
