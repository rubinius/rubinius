#include "util/atomic.hpp"

#include "oop.hpp"
#include "bug.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"

#include "capi/handles.hpp"
#include "gc/inflated_headers.hpp"

#include "object_memory.hpp"
#include "configuration.hpp"

#include "on_stack.hpp"

#include <assert.h>
#include <sys/time.h>

namespace rubinius {

  bool HeaderWord::atomic_set(HeaderWord& old, HeaderWord& nw) {
    return atomic::compare_and_swap(&flags64,
                                     old.flags64,
                                     nw.flags64);
  }

  bool ObjectHeader::set_inflated_header(STATE, uint32_t ih_index, HeaderWord orig) {

    if(orig.f.meaning == eAuxWordInflated) return false;

    HeaderWord new_val = orig;
    new_val.f.aux_word = ih_index;
    new_val.f.meaning  = eAuxWordInflated;

    // Make sure to include a barrier to the header is all properly initialized
    atomic::memory_barrier();
    return header.atomic_set(orig, new_val);
  }

  InflatedHeader* ObjectHeader::header_to_inflated_header(STATE, HeaderWord header) {
    return state->memory()->inflated_headers()->find_index(header.f.aux_word);
  }

  InflatedHeader* ObjectHeader::header_to_inflated_header(ObjectMemory* om, HeaderWord header) {
    return om->inflated_headers()->find_index(header.f.aux_word);
  }

  bool InflatedHeader::update(STATE, HeaderWord header) {
    // Gain exclusive access to the insides of the InflatedHeader.
    utilities::thread::Mutex::LockGuard lg(mutex_);

    switch(header.f.meaning) {
    case eAuxWordEmpty:
      owner_id_       = 0;
      rec_lock_count_ = 0;
      object_id_      = 0;
      handle_         = NULL;
      return true;

    case eAuxWordObjID:
      owner_id_       = 0;
      rec_lock_count_ = 0;
      handle_         = NULL;

      object_id_      = header.f.aux_word;
      return true;
    case eAuxWordLock:
      {
        object_id_     = 0;
        handle_        = NULL;

        uint32_t tid   = header.f.aux_word >> cAuxLockTIDShift;
        uint32_t count = header.f.aux_word & cAuxLockRecCountMask;
        initialize_mutex(tid, count);
      }

      return true;
    case eAuxWordHandle:
      owner_id_       = 0;
      rec_lock_count_ = 0;
      object_id_      = 0;

      handle_ = state->memory()->capi_handles()->find_index(header.f.aux_word);
      return true;
    // Unsupported state, abort.
    default:
      return false;
    }
  }

  void ObjectHeader::set_object_id(STATE, uint32_t id) {
    // Just ignore trying to reset it to 0 for now.
    if(id == 0) return;

retry:
    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = header;

    orig.f.meaning  = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning = eAuxWordObjID;
    new_val.f.aux_word = id;

    if(header.atomic_set(orig, new_val)) return;

    orig = header;

    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // The header was used for something else but not anymore
      // Therefore we can just retry here.
      goto retry;
    case eAuxWordObjID:
      // Someone beat us to it, ignore it
      break;
    case eAuxWordLock:
    case eAuxWordHandle:
      // not inflated, and the aux_word is being used for locking
      // or a handle.
      // Inflate!

      state->memory()->inflate_for_id(state, this, id);
      break;
    case eAuxWordInflated:
      // Already inflated, just set it in the inflated header
      ObjectHeader::header_to_inflated_header(state, orig)->set_object_id(id);
      break;
    }
  }

  // Run only while om's lock is held.
  void ObjectHeader::set_handle_index(STATE, uintptr_t handle_index) {
    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.

retry:
    HeaderWord orig = header;

    orig.f.meaning  = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning = eAuxWordHandle;
    new_val.f.aux_word = (uint32_t) handle_index;

    if(header.atomic_set(orig, new_val)) return;

    capi::Handle* handle = state->memory()->capi_handles()->find_index(handle_index);

    orig = header;

    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // The header was used for something else but not anymore
      // Therefore we can just retry here.
      goto retry;
    case eAuxWordHandle:
      // Someone else has beaten us to it, clear the allocated
      // header so it can be cleaned up on the next GC
      handle->clear();
      break;
    case eAuxWordLock:
    case eAuxWordObjID:
      // not inflated, and the aux_word is being used for locking
      // or a handle.
      // Inflate!

      state->memory()->inflate_for_handle(state, this, handle);
      break;
    case eAuxWordInflated:
      // Already inflated, set the handle in the inflated header
      ObjectHeader::header_to_inflated_header(state, orig)->set_handle(state, handle);
      break;
    }
  }

  void ObjectHeader::set_age(unsigned int age) {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.age      = age;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::mark(ObjectMemory* om, unsigned int which) {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Marked   = which;

      if(header.atomic_set(orig, new_val)) {
        // Mark the inflated header so the GC can see it's used
        if(new_val.f.meaning == eAuxWordInflated) {
          header_to_inflated_header(om, new_val)->mark(om, which);
        }
        return;
      }
    }
  }

  void ObjectHeader::scanned() {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Marked  |= 1;

      if(header.atomic_set(orig, new_val)) {
        return;
      }
    }
  }

  bool ObjectHeader::pin() {
    // Can't pin young objects!
    if(young_object_p()) return false;

    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Pinned   = 1;

      if(header.atomic_set(orig, new_val)) return true;
    }
    return true;
  }

  void ObjectHeader::unpin() {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Pinned   = 0;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::clear_lock_contended() {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.LockContended = 0;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::set_remember() {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Remember = 1;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::clear_remember() {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Remember = 0;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::set_frozen(int val) {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Frozen   = val;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::set_tainted(int val) {
    for(;;) {
      HeaderWord orig    = header;
      HeaderWord new_val = orig;
      new_val.f.Tainted  = val;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::set_untrusted(int val) {
    for(;;) {
      HeaderWord orig     = header;
      HeaderWord new_val  = orig;
      new_val.f.Untrusted = val;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  void ObjectHeader::clear_handle(STATE) {
    for(;;) {
      HeaderWord orig = header;
      if(orig.f.meaning == eAuxWordInflated) {
        header_to_inflated_header(state, orig)->clear_handle(state);
        return;
      }
      orig.f.meaning  = eAuxWordHandle;

      HeaderWord new_val = orig;
      new_val.f.meaning  = eAuxWordEmpty;
      new_val.f.aux_word = 0;

      if(header.atomic_set(orig, new_val)) return;
    }
  }

  capi::Handle* ObjectHeader::handle(STATE) {
    HeaderWord orig = header;

    switch(orig.f.meaning) {
    case eAuxWordHandle:
      return state->memory()->capi_handles()->find_index(orig.f.aux_word);
    case eAuxWordInflated:
      return header_to_inflated_header(state, orig)->handle(state);
    default:
      return NULL;
    }
  }

  LockStatus ObjectHeader::lock(STATE, GCToken gct, CallFrame* call_frame, size_t us, bool interrupt) {
    // #1 Attempt to lock an unlocked object using CAS.

    ObjectHeader* self = this;

step1:
    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = self->header;

    orig.f.meaning  = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning  = eAuxWordLock;
    new_val.f.aux_word = state->vm()->thread_id() << cAuxLockTIDShift;

    if(self->header.atomic_set(orig, new_val)) {
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " locked with CAS]\n";
      }

      // wonderful! Locked! weeeee!
      state->vm()->add_locked_object(self);
      return eLocked;
    }

    // Ok, something went wrong.
    //
    // #2 See if we're locking the object recursively.
step2:
    orig = self->header;

    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // O_o why is it empty? must be some weird concurrency stuff going
      // on. Ok, well, start over then.
      goto step1;

    case eAuxWordLock:
      if(orig.f.aux_word >> cAuxLockTIDShift == state->vm()->thread_id()) {
        // We're going to do this over and over until we get the new
        // header CASd into place.

        // Yep, we've already got this object locked, so increment the count.
        int count = orig.f.aux_word & cAuxLockRecCountMask;

        // We've recursively locked this object more than we can handle.
        // Inflate the lock then.
        if(++count > cAuxLockRecCountMax) {
          OnStack<1> os(state, self);
          // If we can't inflate the lock, try the whole thing over again.
          if(!state->memory()->inflate_lock_count_overflow(state, self, count)) {
            goto step1;
          }
          // The header is now set to inflated, and the current thread
          // is holding the inflated lock.
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id() << " inflated due to recursion overflow: " << count << " ]\n";
          }
        } else {
          new_val = orig;
          new_val.f.aux_word = (state->vm()->thread_id() << cAuxLockTIDShift) | count;

          // Because we've got the object already locked to use, no other
          // thread is going to be trying to lock this thread, but another
          // thread might ask for an object_id and the header will
          // be inflated. So if we can't swap in the new header, we'll start
          // this step over.
          if(!self->header.atomic_set(orig, new_val)) goto step2;

          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id() << " recursively locked with CAS: " << count << " ]\n";
          }
        }
        return eLocked;

      // Our thread id isn't in the field, so we need to inflated the lock
      // because another thread has it locked.
      } else {
        // We weren't able to contend for it, probably because the header changed.
        // Do it all over again.
        OnStack<1> os(state, self);
        LockStatus ret = state->memory()->contend_for_lock(state, gct, call_frame, self, us, interrupt);
        if(ret == eLockError) goto step1;
        return ret;
      }

    // The header is being used for something other than locking, so we need to
    // inflate it.
    case eAuxWordObjID:
    case eAuxWordHandle: {
      // If we couldn't inflate the lock, that means the header was in some
      // weird state that we didn't detect and handle properly. So redo
      // the whole locking procedure again.
      OnStack<1> os(state, self);
      if(!state->memory()->inflate_and_lock(state, self)) goto step1;
      state->vm()->add_locked_object(self);
      return eLocked;
    }
    case eAuxWordInflated: {
      // The header is inflated, use the full lock.
      OnStack<1> os(state, self);
      InflatedHeader* ih = ObjectHeader::header_to_inflated_header(state, orig);
      return ih->lock_mutex(state, gct, call_frame, self, us, interrupt);
    }
    }

    rubinius::bug("Invalid header meaning");
    return eLockError;
  }

  void ObjectHeader::hard_lock(STATE, GCToken gct, CallFrame* call_frame, size_t us) {
retry:
    LockStatus status = lock(state, gct, call_frame, us);
    if(status == eLockInterrupted) goto retry;
    if(status != eLocked) rubinius::bug("Unable to lock object");
  }

  LockStatus ObjectHeader::try_lock(STATE, GCToken gct, CallFrame* call_frame) {

    ObjectHeader* self = this;
    // #1 Attempt to lock an unlocked object using CAS.

step1:
    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = self->header;

    orig.f.meaning  = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning  = eAuxWordLock;
    new_val.f.aux_word = state->vm()->thread_id() << cAuxLockTIDShift;

    if(self->header.atomic_set(orig, new_val)) {
      // wonderful! Locked! weeeee!
      state->vm()->add_locked_object(self);
      return eLocked;
    }

    // Ok, something went wrong.
    //
    // #2 See if we're locking the object recursively.
step2:
    orig = self->header;

    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // O_o why is it empty? must be some weird concurrency stuff going
      // on. Ok, well, start over then.
      goto step1;

    case eAuxWordLock:
      if(orig.f.aux_word >> cAuxLockTIDShift == state->vm()->thread_id()) {
        // We're going to do this over and over until we get the new
        // header CASd into place.

        // Yep, we've already got this object locked, so increment the count.
        int count = orig.f.aux_word & cAuxLockRecCountMask;

        // We've recursively locked this object more than we can handle.
        // Inflate the lock then.
        if(++count > cAuxLockRecCountMax) {
          OnStack<1> os(state, self);
          // If we can't inflate the lock, try the whole thing over again.
          if(!state->memory()->inflate_lock_count_overflow(state, self, count)) {
            goto step1;
          }
          // The header is now set to inflated, and the current thread
          // is holding the inflated lock.
        } else {
          new_val = orig;
          new_val.f.aux_word = (state->vm()->thread_id() << cAuxLockTIDShift) | count;

          // Because we've got the object already locked to use, no other
          // thread is going to be trying to lock this thread, but another
          // thread might ask for an object_id and the header will
          // be inflated. So if we can't swap in the new header, we'll start
          // this step over.
          if(!self->header.atomic_set(orig, new_val)) goto step2;
        }

        return eLocked;

      // Our thread id isn't in the field, then we can't lock it.
      } else {
        return eLockError;
      }

    // The header is being used for something other than locking, so we need to
    // inflate it.
    case eAuxWordObjID:
    case eAuxWordHandle: {
      // If we couldn't inflate the lock, that means the header was in some
      // weird state that we didn't detect and handle properly. So redo
      // the whole locking procedure again.
      OnStack<1> os(state, self);
      if(!state->memory()->inflate_and_lock(state, self)) goto step1;
      return eLocked;
    }
    case eAuxWordInflated: {
      // The header is inflated, use the full lock.
      OnStack<1> os(state, self);
      InflatedHeader* ih = ObjectHeader::header_to_inflated_header(state, orig);
      return ih->try_lock_mutex(state, gct, call_frame, self);
    }
    }

    rubinius::bug("Invalid header meaning");
    return eLockError;
  }

  bool ObjectHeader::locked_p(STATE, GCToken gct, CallFrame* call_frame) {
    HeaderWord orig = header;

    switch(orig.f.meaning) {
    case eAuxWordLock:
      return true;
    case eAuxWordEmpty:
    case eAuxWordObjID:
    case eAuxWordHandle:
      return false;
    case eAuxWordInflated:
      InflatedHeader* ih = ObjectHeader::header_to_inflated_header(state, orig);
      return ih->locked_mutex_p(state, gct, call_frame, this);
    }

    rubinius::bug("Invalid header meaning");
    return false;
  }

  LockStatus ObjectHeader::unlock(STATE, GCToken gct, CallFrame* call_frame) {
    // This case is slightly easier than locking.

    for(;;) {
      HeaderWord orig = header;

      switch(orig.f.meaning) {
      case eAuxWordEmpty:
      case eAuxWordObjID:
      case eAuxWordHandle:
        if(cDebugThreading) {
          std::cerr << "[LOCK " << state->vm()->thread_id() << " attempted to unlock an unlocked header]\n";
        }
        return eLockError;

      case eAuxWordLock: {
        unsigned int locker_tid = orig.f.aux_word >> cAuxLockTIDShift;
        if(locker_tid != state->vm()->thread_id()) {
          if(cDebugThreading) {
            std::cerr
              << "[LOCK " << state->vm()->thread_id() << " attempted to unlock an object locked by other thread."
              << "locker=" << locker_tid
              << "]\n";
          }

          return eLockError;
        }

        uint32_t count = orig.f.aux_word & cAuxLockRecCountMask;

        HeaderWord new_val = orig;

        if(count == 0) {
          if(orig.f.LockContended == 1) {
            // unlock and inflate as one step to keep things
            // consistent.
            if(!state->memory()->inflate_for_contention(state, this)) continue;

            state->vm()->del_locked_object(this);
            state->memory()->release_contention(state, gct, call_frame);

            return eUnlocked;
          }

          new_val.f.meaning = eAuxWordEmpty;
          new_val.f.aux_word = 0;
        } else {
          new_val.f.aux_word = (state->vm()->thread_id() << cAuxLockTIDShift) | (count - 1);
        }

        // Try it all over again if it fails.
        if(!header.atomic_set(orig, new_val)) continue;

        if(new_val.f.meaning == eAuxWordEmpty) {
          // Since we no longer have any association with this lock,
          // remove it from the current threads lock list
          state->vm()->del_locked_object(this);

          if(cDebugThreading) {
            if(new_val.f.LockContended == 1) {
              std::cerr << "[LOCK " << state->vm()->thread_id() << " invalid state. CAS unlocking with contention, no inflation]\n";
            } else {
              std::cerr << "[LOCK " << state->vm()->thread_id() << " unlocked with CAS]\n";
            }
          }
        } else {
          if(cDebugThreading) {
            std::cerr << "[LOCK " << state->vm()->thread_id() << " unlocked with CAS via count decrement]\n";
          }
        }

        return eUnlocked;
      }
      case eAuxWordInflated:
        InflatedHeader* ih = ObjectHeader::header_to_inflated_header(state, orig);
        return ih->unlock_mutex(state, gct, call_frame, this);
      }
    }

    rubinius::bug("Invalid header meaning");
    return eLockError;
  }

  void ObjectHeader::hard_unlock(STATE, GCToken gct, CallFrame* call_frame) {
    if(unlock(state, gct, call_frame) != eUnlocked) rubinius::bug("Unable to unlock object");
  }

  void ObjectHeader::unlock_for_terminate(STATE, GCToken gct, CallFrame* call_frame) {
    // This case is slightly easier than locking.

    for(;;) {
      HeaderWord orig = header;

      switch(orig.f.meaning) {
      case eAuxWordEmpty:
      case eAuxWordObjID:
      case eAuxWordHandle:
        // Um. well geez. We don't have this object locked.
        return;

      case eAuxWordLock: {
        if(orig.f.aux_word >> cAuxLockTIDShift != state->vm()->thread_id()) {
          return;
        }

        HeaderWord new_val = orig;

        new_val.f.meaning = eAuxWordEmpty;
        new_val.f.aux_word = 0;

        // Try it all over again if it fails.
        if(!header.atomic_set(orig, new_val)) continue;

        // Don't call state->del_locked_object() here. We iterate over
        // that list to call this function, so we don't want to invalidate
        // the iterators. Plus, we don't need to cleanup the list anyway, this
        // is only used when the Thread is exiting.

        if(new_val.f.LockContended == 1) {
          // If we couldn't inflate for contention, redo.
          if(!state->memory()->inflate_for_contention(state, this)) continue;
          state->memory()->release_contention(state, gct, call_frame);
        }
        return;
      }
      case eAuxWordInflated:
        InflatedHeader* ih = ObjectHeader::header_to_inflated_header(state, orig);
        ih->unlock_mutex_for_terminate(state, gct, call_frame, this);
        return;
      }
    }
  }

  size_t ObjectHeader::slow_size_in_bytes(VM* vm) const {
    return vm->om->type_info[type_id()]->object_size(this);
  }

  void ObjectHeader::initialize_copy(ObjectMemory* om, Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    // DO NOT, EVER, CHANGE obj_type_.
    // obj_type_ indicates the shape of the object in memory. There are
    // fucked up cases where this is called where other is another type
    // (that, btw, is likely a bug in itself), but we should never,
    // MUST never change obj_type_ to make them match. This causes the GC
    // to get confused about the memory shape of the object!
    assert(type_id() == other->type_id());
    set_age(new_age);
    klass_ = other->klass_;
    ivars_ = other->ivars_;
  }

  void ObjectHeader::initialize_full_state(VM* state, Object* other, unsigned int age) {
    assert(type_id() == other->type_id());
    set_age(age);
    klass_ = other->klass_;
    ivars_ = other->ivars_;

    HeaderWord hdr = other->header;

    header.f.meaning = hdr.f.meaning;
    header.f.aux_word = hdr.f.aux_word;

    if(other->is_tainted_p()) set_tainted();

    copy_body(state, other);

    reinterpret_cast<Object*>(this)->write_barrier(state, ivars_);
    reinterpret_cast<Object*>(this)->write_barrier(state, klass_);

    // This method is only used by the GC to move an object, so must retain
    // the settings flags.
    set_frozen(other->is_frozen_p());
    set_tainted(other->is_tainted_p());
  }

  void ObjectHeader::copy_body(VM* state, Object* other) {
    void* src = other->__body__;
    void* dst = this->__body__;

    memcpy(dst, src, other->body_in_bytes(state));
  }

  /* Clear the body of the object, by setting each field to cNil */
  void ObjectHeader::clear_fields(size_t bytes) {
    ivars_ = cNil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(register size_t counter = 0; counter < field_count; ++counter) {
      dst[counter] = cNil;
    }
  }

  void InflatedHeader::initialize_mutex(int thread_id, int count) {
    owner_id_ = thread_id;
    rec_lock_count_ = count;
  }

  LockStatus InflatedHeader::lock_mutex(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj, size_t us, bool interrupt) {
    if(us == 0) return lock_mutex_timed(state, gct, call_frame, obj, 0, interrupt);

    struct timeval tv;
    struct timespec ts;

    gettimeofday(&tv, NULL);

    ts.tv_sec = tv.tv_sec + (us / 1000000);
    ts.tv_nsec = (us % 10000000) * 1000;

    return lock_mutex_timed(state, gct, call_frame, obj, &ts, interrupt);
  }

  LockStatus InflatedHeader::lock_mutex_timed(STATE, GCToken gct,
                                              CallFrame* call_frame,
                                              ObjectHeader* obj,
                                              const struct timespec* ts,
                                              bool interrupt)
  {
    OnStack<1> os(state, obj);

    // Gain exclusive access to the insides of the InflatedHeader.
    GCLockGuard lg(state, gct, call_frame, mutex_);

    // We've got exclusive access to the lock parts of the InflatedHeader now.
    //
    // If the thread id is in owner, we've got it locked.
    //
    // If we're not the owner, then block on mutex_.lock in a loop until the
    // owner_id_ is 0 and we can therefore be the owner.

    if(owner_id_ == state->vm()->thread_id()) {
      // We're already the owner, easy.
      rec_lock_count_++;

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id()
                  << " recursively locked inflated: " << rec_lock_count_ << "\n";
      }

      return eLocked;
    }

    // Otherwise lock and take ownership

    bool timeout = false;

    {
      GCIndependent gc_guard(state, call_frame);

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " locking native mutex: " << this << "]\n";
      }

      state->vm()->wait_on_inflated_lock(reinterpret_cast<Object*>(obj));

      // Loop until there is no owner.
      while(owner_id_ != 0) {

        state->vm()->set_sleeping();
        if(ts) {
          timeout = (condition_.wait_until(mutex_, ts) == utilities::thread::cTimedOut);
          if(timeout) break;
        } else {
          condition_.wait(mutex_);
        }

        // Someone is interrupting us trying to lock.
        if(interrupt && state->check_local_interrupts()) {
          state->vm()->clear_check_local_interrupts();

          if(!state->vm()->interrupted_exception()->nil_p()) {
            if(cDebugThreading) {
              std::cerr << "[LOCK " << state->vm()->thread_id() << " detected interrupt]\n";
            }

            state->vm()->clear_sleeping();
            return eLockInterrupted;
          }
        }
      }

      state->vm()->clear_sleeping();
      state->vm()->clear_waiter();
    }

    if(timeout) {
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " locking timed out]\n";
      }

      return eLockTimeout;
    }

    // OWNED.

    owner_id_ = state->vm()->thread_id();
    state->vm()->add_locked_object(obj);

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->vm()->thread_id() << " locked inflated header: " << this << "]\n";
    }

    return eLocked;
  }

  LockStatus InflatedHeader::try_lock_mutex(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj) {
    OnStack<1> os(state, obj);

    // Gain exclusive access to the insides of the InflatedHeader.
    GCLockGuard lg(state, gct, call_frame, mutex_);

    // We've got exclusive access to the lock parts of the InflatedHeader now.
    //
    // If the thread id is in owner, we've got it locked.
    //
    // If we're not the owner, then block on mutex_.lock in a loop until the
    // owner_id_ is 0 and we can therefore be the owner.

    bool locked = false;

    if(owner_id_ == state->vm()->thread_id()) {
      // We're already the owner, easy.
      rec_lock_count_++;

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id()
                  << " recursively locked ih: " << rec_lock_count_ << "\n";
      }

      locked = true;

    // No owner! It's ours!
    } else if(owner_id_ == 0) {
      owner_id_ = state->vm()->thread_id();
      locked = true;
      state->vm()->add_locked_object(obj);

      // OWNED.

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " locked inflated header]\n";
      }
    }

    return locked ? eLocked : eUnlocked;
  }

  bool InflatedHeader::locked_mutex_p(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj) {
    // Gain exclusive access to the insides of the InflatedHeader.
    OnStack<1> os(state, obj);

    GCLockGuard lg(state, gct, call_frame, mutex_);

    return owner_id_ != 0;
  }

  LockStatus InflatedHeader::unlock_mutex(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj) {
    OnStack<1> os(state, obj);

    // Gain exclusive access to the insides of the InflatedHeader.
    GCLockGuard lg(state, gct, call_frame, mutex_);

    // Sanity check.
    if(owner_id_ != state->vm()->thread_id()) {
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " inflated unlock consistence error, not the owner:" << owner_id_ << "]\n";
      }
      return eLockError;
    }

    // If the count has dropped to 0, we're truly done, so tell anyone
    // blocking on mutex_.
    if(rec_lock_count_ == 0) {
      state->vm()->del_locked_object(obj);

      owner_id_ = 0;
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id() << " unlocked native: " << this << "]\n";
      }
    } else {
      rec_lock_count_--;
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id()
                  << " unlocked via dec rec_lock_count: " << rec_lock_count_ << "]\n";
      }
    }

    // Wake up anyone waiting for us.
    condition_.signal();

    return eUnlocked;
  }

  void InflatedHeader::unlock_mutex_for_terminate(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj) {
    OnStack<1> os(state, obj);

    // Gain exclusive access to the insides of the InflatedHeader.
    GCLockGuard lg(state, gct, call_frame, mutex_);

    // We've got exclusive access to the lock parts of the InflatedHeader now.

    // Sanity check.
    if(owner_id_ != state->vm()->thread_id()) {
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->vm()->thread_id()
                  << " (term) inflated unlock consistence error, not the owner: "
                  << owner_id_ << "]\n";
      }
      return;
    }

    // Don't call state->vm()->del_locked_object() here. We iterate over
    // that list to call this function, so we don't want to invalidate
    // the iterators. Plus, we don't need to cleanup the list anyway, this
    // is only used when the Thread is exiting.

    owner_id_ = 0;
    condition_.signal();

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->vm()->thread_id() << " (term) unlocked native]\n";
    }
  }

  void InflatedHeader::wakeup(STATE, GCToken gct, CallFrame* call_frame, ObjectHeader* obj) {
    OnStack<1> os(state, obj);
    GCLockGuard lg(state, gct, call_frame, mutex_);
    condition_.signal();

    if(cDebugThreading) {
      std::cerr << "[LOCK signaled wake up for inflated lock]\n";
    }
  }

}
