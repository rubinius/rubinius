#include <cstdlib>
#include <iostream>

#include "gc/baker.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "builtin/io.hpp"

#include "instruments/stats.hpp"

#include "call_frame.hpp"

#include "gc/gc.hpp"

#include "capi/handle.hpp"

namespace rubinius {
  BakerGC::BakerGC(ObjectMemory *om, size_t bytes)
    : GarbageCollector(om)
    , eden(bytes)
    , heap_a(bytes / 2)
    , heap_b(bytes / 2)
    , total_objects(0)
    , copy_spills_(0)
    , autotune_(false)
    , tune_threshold_(0)
    , original_lifetime_(1)
    , lifetime_(1)
  {
    current = &heap_a;
    next = &heap_b;
  }

  BakerGC::~BakerGC() { }

  Object* BakerGC::saw_object(Object* obj) {
    Object* copy;

    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during baker collection\n";
    }

    if(!obj->reference_p()) return obj;

    if(obj->zone() != YoungObjectZone) return obj;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    // TODO test this!
    if(next->contains_p(obj)) return obj;

    if(unlikely(obj->inc_age() >= lifetime_)) {
      copy = object_memory_->promote_object(obj);

      promoted_push(copy);
    } else if(likely(next->enough_space_p(obj->size_in_bytes(object_memory_->state)))) {
      copy = next->copy_object(object_memory_->state, obj);
      total_objects++;
    } else {
      copy_spills_++;
      copy = object_memory_->promote_object(obj);
      promoted_push(copy);
    }

    if(watched_p(copy)) {
      std::cout << "detected " << copy << " during baker collection (2)\n";
    }

    obj->set_forward(copy);
    return copy;
  }

  void BakerGC::copy_unscanned() {
    Object* iobj = next->next_unscanned(object_memory_->state);

    while(iobj) {
      assert(iobj->zone() == YoungObjectZone);
      if(!iobj->forwarded_p()) scan_object(iobj);
      iobj = next->next_unscanned(object_memory_->state);
    }
  }

  bool BakerGC::fully_scanned_p() {
    return next->fully_scanned_p();
  }

  const static double cOverFullThreshold = 95.0;
  const static int cOverFullTimes = 3;
  const static size_t cMinimumLifetime = 1;

  const static double cUnderFullThreshold = 20.0;
  const static int cUnderFullTimes = -3;
  const static size_t cMaximumLifetime = 6;

  /* Perform garbage collection on the young objects. */
  void BakerGC::collect(GCData& data, YoungCollectStats* stats) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->bytes_copied.start();
    stats::GCStats::get()->objects_copied.start();
    stats::GCStats::get()->objects_promoted.start();
    stats::GCStats::get()->collect_young.start();
#endif

    Object* tmp;
    ObjectArray *current_rs = object_memory_->swap_remember_set();

    total_objects = 0;

    copy_spills_ = 0;
    reset_promoted();

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        ++oi) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        // assert(tmp->zone == MatureObjectZone);
        // assert(!tmp->forwarded_p());

        // Remove the Remember bit, since we're clearing the set.
        tmp->clear_remember();
        scan_object(tmp);
      }
    }

    delete current_rs;

    for(std::list<gc::WriteBarrier*>::iterator wbi = object_memory_->aux_barriers().begin();
        wbi != object_memory_->aux_barriers().end();
        wbi++) {
      gc::WriteBarrier* wb = *wbi;
      ObjectArray* rs = wb->swap_remember_set();
      for(ObjectArray::iterator oi = rs->begin();
          oi != rs->end();
          oi++) {
        tmp = *oi;

        if(tmp) {
          tmp->clear_remember();
          scan_object(tmp);
        }
      }

      delete rs;
    }

    for(Roots::Iterator i(data.roots()); i.more(); i.advance()) {
      i->set(saw_object(i->get()));
    }

    if(data.threads()) {
      for(std::list<ManagedThread*>::iterator i = data.threads()->begin();
          i != data.threads()->end();
          i++) {
        for(Roots::Iterator ri((*i)->roots()); ri.more(); ri.advance()) {
          ri->set(saw_object(ri->get()));
        }
      }
    }

    for(capi::Handles::Iterator i(*data.handles()); i.more(); i.advance()) {
      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
        assert(i->object()->inflated_header_p());
      }

      assert(i->object()->type_id() != InvalidType);
    }

    for(capi::Handles::Iterator i(*data.cached_handles()); i.more(); i.advance()) {
      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
        assert(i->object()->inflated_header_p());
      }

      assert(i->object()->type_id() != InvalidType);
    }

    for(VariableRootBuffers::Iterator i(data.variable_buffers());
        i.more(); i.advance()) {
      Object*** buffer = i->buffer();
      for(int idx = 0; idx < i->size(); idx++) {
        Object** var = buffer[idx];
        Object* tmp = *var;

        if(tmp->reference_p() && tmp->young_object_p()) {
          *var = saw_object(tmp);
        }
      }
    }

    // Walk all the call frames
    for(CallFrameLocationList::iterator i = data.call_frames().begin();
        i != data.call_frames().end();
        i++) {
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }

    // Handle all promotions to non-young space that occured.
    handle_promotions();

    assert(fully_scanned_p());
    // We're now done seeing the entire object graph of normal, live references.
    // Now we get to handle the unusual references, like finalizers and such.
    //

    /* Update finalizers. Doing so can cause objects that would have just died
     * to continue life until we can get around to running the finalizer. That
     * more promoted objects, etc. */
    check_finalize();

    // Run promotions again, because checking finalizers can keep more objects
    // alive (and thus promoted).
    handle_promotions();

    assert(fully_scanned_p());

#ifdef RBX_GC_STATS
    // Lost souls just tracks the ages of objects, so we know how old they
    // were when they died.
    // find_lost_souls();
#endif

    /* Check any weakrefs and replace dead objects with nil*/
    clean_weakrefs(true);

    /* Swap the 2 halves */
    Heap *x = next;
    next = current;
    current = x;
    next->reset();

    // Reset eden to empty
    eden.reset();

    if(stats) {
      stats->lifetime = lifetime_;
      stats->percentage_used = current->percentage_used();
      stats->promoted_objects = promoted_objects_;
      stats->excess_objects = copy_spills_;
    }

    if(autotune_) {
      double used = current->percentage_used();
      if(used > cOverFullThreshold) {
        if(tune_threshold_ >= cOverFullTimes) {
          if(lifetime_ > cMinimumLifetime) lifetime_--;
        } else {
          tune_threshold_++;
        }
      } else if(used < cUnderFullThreshold) {
        if(tune_threshold_ <= cUnderFullTimes) {
          if(lifetime_ < cMaximumLifetime) lifetime_++;
        } else {
          tune_threshold_--;
        }
      } else if(tune_threshold_ > 0) {
        tune_threshold_--;
      } else if(tune_threshold_ < 0) {
        tune_threshold_++;
      } else if(tune_threshold_ == 0) {
        if(lifetime_ < original_lifetime_) {
          lifetime_++;
        } else if(lifetime_ > original_lifetime_) {
          lifetime_--;
        }
      }
    }

#ifdef RBX_GC_STATS
    stats::GCStats::get()->collect_young.stop();
    stats::GCStats::get()->objects_copied.stop();
    stats::GCStats::get()->objects_promoted.stop();
    stats::GCStats::get()->bytes_copied.stop();
#endif
  }

  void BakerGC::handle_promotions() {
    while(promoted_stack_.size() > 0 || !fully_scanned_p()) {
      while(promoted_stack_.size() > 0) {
        Object* obj = promoted_stack_.back();
        promoted_stack_.pop_back();

        scan_object(obj);
      }

      copy_unscanned();
    }
  }

  inline Object *BakerGC::next_object(Object * obj) {
    return reinterpret_cast<Object*>(reinterpret_cast<uintptr_t>(obj) +
      obj->size_in_bytes(object_memory_->state));
  }

  void BakerGC::clear_marks() {
    Object* obj = current->first_object();
    while(obj < current->current()) {
      obj->clear_mark();
      obj = next_object(obj);
    }

    obj = next->first_object();
    while(obj < next->current()) {
      obj->clear_mark();
      obj = next_object(obj);
    }

    obj = eden.first_object();
    while(obj < eden.current()) {
      obj->clear_mark();
      obj = next_object(obj);
    }
  }

  void BakerGC::find_lost_souls() {
    Object* obj = current->first_object();
    while(obj < current->current()) {
      if(!obj->forwarded_p()) {
#ifdef RBX_GC_STATS
        stats::GCStats::get()->lifetimes[obj->age()]++;
#endif
      }
      obj = next_object(obj);
    }

    obj = eden.first_object();
    while(obj < eden.current()) {
      if(!obj->forwarded_p()) {
#ifdef RBX_GC_STATS
        stats::GCStats::get()->lifetimes[obj->age()]++;
#endif
      }
      obj = next_object(obj);
    }
  }

  void BakerGC::check_finalize() {
    for(std::list<FinalizeObject>::iterator i = object_memory_->finalize().begin();
        i != object_memory_->finalize().end(); ) {
      FinalizeObject& fi = *i;

      bool remove = false;

      if(i->object->young_object_p()) {
        Object* orig = i->object;
        switch(i->status) {
        case FinalizeObject::eLive:
          if(!i->object->forwarded_p()) {
            i->queued();
            object_memory_->to_finalize().push_back(&fi);
          }

          // We have to still keep it alive though until we finish with it.
          i->object = saw_object(orig);
          break;
        case FinalizeObject::eQueued:
          // Nothing, we haven't gotten to it yet.
          // Keep waiting and keep i->object updated.
          i->object = saw_object(i->object);

          i->queue_count++;
          break;
        case FinalizeObject::eFinalized:
          if(!i->object->forwarded_p()) {
            // finalized and done with.
            remove = true;
          } else {
            // RESURECTION!
            i->queued();
          }
          break;
        }
      }

      if(remove) {
        i = object_memory_->finalize().erase(i);
      } else {
        i++;
      }
    }
  }

  bool BakerGC::in_current_p(Object* obj) {
    return current->contains_p(obj);
  }

  ObjectPosition BakerGC::validate_object(Object* obj) {
    if(current->in_current_p(obj) || eden.in_current_p(obj)) {
      return cValid;
    } else if(next->contains_p(obj)) {
      return cInWrongYoungHalf;
    } else {
      return cUnknown;
    }
  }
}
