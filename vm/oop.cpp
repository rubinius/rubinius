#include "oop.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"

#include "objectmemory.hpp"

#include <cassert>

namespace rubinius {
  void ObjectHeader::set_inflated_header(InflatedHeader* ih) {
    HeaderWord orig = header;

    ih->reset_object(this);
    ih->update(header);

    HeaderWord new_val = header;
    new_val.all_flags = ih;
    new_val.f.meaning = eAuxWordInflated;

    // Do a spin update so if someone else is trying to update it at the same time
    // we catch that and keep trying until we get our version in.
    while(!__sync_bool_compare_and_swap(&header.flags64, orig.flags64, new_val.flags64)) {
      orig = header;
      ih->update(header);
    }
  }

  InflatedHeader* ObjectHeader::deflate_header() {
    // Probably needs to CAS, but this only used by immix and in a place
    // we don't hit currently, so don't worry about it for now.
    InflatedHeader* ih = inflated_header();
    header.f = ih->flags();
    header.f.meaning = eAuxWordEmpty;
    header.f.aux_word = 0;

    return ih;
  }

  void InflatedHeader::update(HeaderWord header) {
    flags_ = header.f;

    switch(flags_.meaning) {
    case eAuxWordObjID:
      object_id_ = flags_.aux_word;
      flags_.meaning = eAuxWordEmpty;
      flags_.aux_word = 0;
      break;
    case eAuxWordLock:
      // fine.
    case eAuxWordInflated:
      abort();
    }
  }

  void ObjectHeader::set_object_id(ObjectMemory* om, uint32_t id) {
    // Just ignore trying to reset it to 0 for now.
    if(id == 0) return;

    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = header;

    orig.f.meaning = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning = eAuxWordObjID;
    new_val.f.aux_word = id;

    if(__sync_bool_compare_and_swap(&header.flags64, orig.flags64, new_val.flags64)) {
      return;
    }

    // not inflated, and the aux_word is being used for locking.
    // Inflate!

    om->inflate_for_id(this, id);
  }

  void ObjectHeader::lock(STATE) {
    // #1 Attempt to lock an unlocked object using CAS.

step1:
    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = header;

    orig.f.meaning = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning = eAuxWordLock;
    new_val.f.aux_word = state->thread_id() << cAuxLockTIDShift;

    if(__sync_bool_compare_and_swap(&header.flags64, orig.flags64, new_val.flags64)) {
      // wonderful! Locked! weeeee!
      return;
    }

    // Ok, something went wrong.
    //
    // #2 See if we're locking the object recursively.

step2:
    orig = header;
    switch(orig.f.meaning) {
    case eAuxWordEmpty:
      // O_o why is it empty? must be some weird concurrency stuff going
      // on. Ok, well, start over then.
      goto step1;

    case eAuxWordLock:
      if(orig.f.aux_word >> cAuxLockTIDShift == state->thread_id()) {
        // We're going to do this over and over until we get the new
        // header CASd into place.

        // Yep, we've already got this object locked, so increment the count.
        int count = orig.f.aux_word & cAuxLockRecCountMask;

        // We've recursively locked this object more than we can handle.
        // Inflate the lock then.
        if(++count > cAuxLockRecCountMax) {
          // If we can't inflate the lock, try the whole thing over again.
          if(!state->om->inflate_lock_count_overflow(state, this, count)) {
            goto step1;
          }
          // The header is now set to inflated, and the current thread
          // is holding the inflated lock.
        } else {
          new_val = orig;
          new_val.f.aux_word = (state->thread_id() << cAuxLockTIDShift) | count;

          // Because we've got the object already locked to use, no other
          // thread is going to be trying to lock this thread, but another
          // thread might ask for an object_id and the header will
          // be inflated. So if we can't swap in the new header, we'll start
          // this step over.
          if(!__sync_bool_compare_and_swap(&header.flags64,
                                           orig.flags64,
                                           new_val.flags64)) {
            goto step2;
          }

          // wonderful! Locked! weeeee!
        }

      // Our thread id isn't in the field, so we need to inflated the lock
      // because another thread has it locked.
      } else {
        // We weren't able to contend for it, probably because the header changed.
        // Do it all over again.
        if(!state->om->contend_for_lock(state, this)) goto step1;
      }

      return;

    // The header is inflated, use the full lock.
    case eAuxWordInflated: {
      InflatedHeader* ih = ObjectHeader::header_to_inflated_header(orig);
      ih->lock_mutex(state);
      break;
    }

    // The header is being used for something other than locking, so we need to
    // inflate it.
    case eAuxWordObjID:
      // If we couldn't inflate the lock, that means the header was in some
      // weird state that we didn't detect and handle properly. So redo
      // the whole locking procedure again.
      if(!state->om->inflate_and_lock(state, this)) goto step1;
    }

    return;
  }

  void ObjectHeader::unlock(STATE) {
    // This case is slightly easier than locking.

    for(;;) {
      HeaderWord orig = header;

      switch(orig.f.meaning) {
      case eAuxWordEmpty:
      case eAuxWordObjID:
        // Um. well geez. We don't have this object locked.
        std::cerr << "[LOCK attempted to unlock an object that is not locked.]\n";
        return;

      case eAuxWordInflated: {
        InflatedHeader* ih = ObjectHeader::header_to_inflated_header(orig);
        ih->unlock_mutex(state);
        return;
      }

      case eAuxWordLock: {
        if(orig.f.aux_word >> cAuxLockTIDShift != state->thread_id()) {
          std::cerr << "[LOCK attempted to unlock an object owned by another thread.]\n";
          return;
        }

        uint32_t count = orig.f.aux_word & cAuxLockRecCountMask;

        HeaderWord new_val = orig;

        if(count == 0) {
          new_val.f.meaning = eAuxWordEmpty;
          new_val.f.aux_word = 0;
        } else {
          new_val.f.aux_word = (state->thread_id() << cAuxLockTIDShift) | (count - 1);
        }

        if(!__sync_bool_compare_and_swap(&header.flags64,
              orig.flags64,
              new_val.flags64)) {
          // Try it all over again.
          continue;
        }

        if(new_val.f.meaning == eAuxWordEmpty) {
          if(new_val.f.LockContended == 1) {
            // If we couldn't inflate for contention, redo.
            if(!state->om->inflate_for_contention(state, this)) continue;
            state->om->release_contention();
          }
        }

        return;
      }
      }
    }
  }

  size_t ObjectHeader::slow_size_in_bytes(STATE) const {
    return state->om->type_info[type_id()]->object_size(this);
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

    clear_forwarded();

    if(other->is_tainted_p()) set_tainted();
  }

  void ObjectHeader::initialize_full_state(STATE, Object* other, unsigned int age) {
    assert(type_id() == other->type_id());
    set_age(age);
    klass_ = other->klass_;
    ivars_ = other->ivars_;

    if(other->object_id() > 0) {
      set_object_id(state->om, other->object_id());
    }

    clear_forwarded();

    if(other->is_tainted_p()) set_tainted();

    copy_body(state, other);

    state->om->write_barrier((Object*)this, ivars_);
    state->om->write_barrier((Object*)this, klass_);

    // This method is only used by the GC to move an object, so must retain
    // the settings flags.
    flags().Frozen =  other->flags().Frozen;
    flags().Tainted = other->flags().Tainted;
  }

  void ObjectHeader::copy_body(STATE, Object* other) {
    void* src = other->__body__;
    void* dst = this->__body__;

    memcpy(dst, src, other->body_in_bytes(state));
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void ObjectHeader::clear_fields(size_t bytes) {
    ivars_ = Qnil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(register size_t counter = 0; counter < field_count; ++counter) {
      dst[counter] = Qnil;
    }
  }

  void ObjectHeader::clear_body_to_null(size_t bytes) {
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = 0;
    }

  }

  void InflatedHeader::set_object(ObjectHeader* obj) {
    flags_ = obj->flags();
    object_ = obj;
  }

  void InflatedHeader::initialize_mutex(int thread_id, int count) {
    owner_id_ = thread_id;
    rec_lock_count_ = count;

    // Not sure how this could, fail but if it does, it's better
    // to fail in the assert() now rather than having ->lock()
    // block.
    assert(mutex_.try_lock() == thread::cLocked);
  }

  void InflatedHeader::lock_mutex(STATE) {
    // Gain exclusive access to the insides of the InflatedHeader.
    //
    // Yes, this spins. Yes, we want that.
    while(!__sync_bool_compare_and_swap(&private_lock_, 0, 1));

    // We've got exclusive access to the lock parts of the InflatedHeader now.
    //
    // If the thread id is in owner, we've got it locked.
    //
    // If we're not the owner, then block on mutex_.lock in a loop until the
    // owner_id_ is 0 and we can therefore be the owner.

    if(owner_id_ == state->thread_id()) {
      // We're already the owner, easy.
      rec_lock_count_++;

      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->thread_id()
                  << " recursively locked ih: " << rec_lock_count_ << "\n";
      }

      private_lock_ = 0;
      return;
    }

    // Otherwise lock and take ownership

    private_lock_ = 0;
    state->shared.gc_independent();

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->thread_id() << " locking native mutex]\n";
    }

    mutex_.lock();
    state->shared.gc_dependent();

    // Spin again to get the private spinlock back
    while(!__sync_bool_compare_and_swap(&private_lock_, 0, 1));

    owner_id_ = state->thread_id();

    // OWNED.

    if(cDebugThreading) {
      std::cerr << "[LOCK " << state->thread_id() << " locked inflated header]\n";
    }

    // Unlock the spin lock.
    private_lock_ = 0;

    return;
  }

  void InflatedHeader::unlock_mutex(STATE) {
    // Gain exclusive access to the insides of the InflatedHeader.
    //
    // Yes, this spins. Yes, we want that.
    while(!__sync_bool_compare_and_swap(&private_lock_, 0, 1));

    // We've got exclusive access to the lock parts of the InflatedHeader now.

    // Sanity check.
    if(owner_id_ != state->thread_id()) {
      std::cerr << "[LOCK Inflated unlock consistence error, not the owner]\n";
      private_lock_ = 0;
      return;
    }

    // If the count has dropped to 0, we're truely done, so tell anyone
    // blocking on mutex_.
    if(rec_lock_count_ == 0) {
      owner_id_ = 0;
      mutex_.unlock();
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->thread_id() << " unlocked native]\n";
      }
    } else {
      rec_lock_count_--;
      if(cDebugThreading) {
        std::cerr << "[LOCK " << state->thread_id()
                  << " unlocked via dec rec_lock_count: " << rec_lock_count_ << "]\n";
      }
    }

    // Unlock the spin lock.
    private_lock_ = 0;
  }

}
