#include <stdlib.h>
#include <iostream>

#include "gc/baker.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "builtin/io.hpp"

#include "call_frame.hpp"

#include "gc/gc.hpp"

#include "capi/handle.hpp"
#include "capi/tag.hpp"

namespace rubinius {

  /**
   * Creates a BakerGC of the specified size.
   *
   * The requested size is allocated as a contiguous heap, which is then split
   * into three spaces:
   * - Eden, which gets half of the heap
   * - Heap A and Heap B, which get one quarter of the heap each. Heaps A and B
   *   alternate between being the Current and Next space on each collection.
   */
  BakerGC::BakerGC(ObjectMemory *om, size_t bytes)
    : GarbageCollector(om)
    , full(bytes * 2)
    , eden(full.allocate(bytes), bytes)
    , heap_a(full.allocate(bytes / 2), bytes / 2)
    , heap_b(full.allocate(bytes / 2), bytes / 2)
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

  /**
   * Called for each object in the young generation that is seen during garbage
   * collection. An object is seen by scanning from the root objects to all
   * reachable objects. Therefore, only reachable objects will be seen, and
   * reachable objects may be seen more than once.
   *
   * @returns the new address for the object, so that the source reference can
   * be updated when the object has been moved.
   */
  Object* BakerGC::saw_object(Object* obj) {
    Object* copy;

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during baker collection\n";
    }
#endif

    if(!obj->reference_p()) return obj;

    if(obj->zone() != YoungObjectZone) return obj;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    if(next->contains_p(obj)) return obj;

    if(unlikely(obj->inc_age() >= lifetime_)) {
      copy = object_memory_->promote_object(obj);

      promoted_push(copy);
    } else if(likely(next->enough_space_p(
                obj->size_in_bytes(object_memory_->state())))) {
      copy = next->move_object(object_memory_->state(), obj);
      total_objects++;
    } else {
      copy_spills_++;
      copy = object_memory_->promote_object(obj);
      promoted_push(copy);
    }

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(copy)) {
      std::cout << "detected " << copy << " during baker collection (2)\n";
    }
#endif

    return copy;
  }


  /**
   * Scans the remaining unscanned portion of the Next heap.
   */
  void BakerGC::copy_unscanned() {
    Object* iobj = next->next_unscanned(object_memory_->state());

    while(iobj) {
      assert(iobj->zone() == YoungObjectZone);
      if(!iobj->forwarded_p()) scan_object(iobj);
      iobj = next->next_unscanned(object_memory_->state());
    }
  }


  /**
   * Returns true if the young generation has been fully scanned in the
   * current collection.
   */
  bool BakerGC::fully_scanned_p() {
    // Note: The spaces are swapped at the start of collection, which is why we
    // check the Next heap
    return next->fully_scanned_p();
  }

  const static double cOverFullThreshold = 95.0;
  const static int cOverFullTimes = 3;
  const static size_t cMinimumLifetime = 1;

  const static double cUnderFullThreshold = 20.0;
  const static int cUnderFullTimes = -3;
  const static size_t cMaximumLifetime = 6;

  /**
   * Perform garbage collection on the young objects.
   */
  void BakerGC::collect(GCData& data, YoungCollectStats* stats) {

    Object* tmp;
    ObjectArray *current_rs = object_memory_->swap_remember_set();

    total_objects = 0;

    copy_spills_ = 0;
    reset_promoted();

    // Start by copying objects in the remember set
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
        ++wbi) {
      gc::WriteBarrier* wb = *wbi;
      ObjectArray* rs = wb->swap_remember_set();
      for(ObjectArray::iterator oi = rs->begin();
          oi != rs->end();
          ++oi) {
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
          ++i) {
        scan(*i, true);
      }
    }

    for(capi::Handles::Iterator i(*data.handles()); i.more(); i.advance()) {
      if(!i->in_use_p()) continue;

      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
        assert(i->object()->inflated_header_p());

      // Users manipulate values accessible from the data* within an
      // RData without running a write barrier. Thusly if we see a mature
      // rdata, we must always scan it because it could contain
      // young pointers.
      } else if(!i->object()->young_object_p() && i->is_rdata()) {
        scan_object(i->object());
      }

      assert(i->object()->type_id() != InvalidType);
    }

    for(capi::Handles::Iterator i(*data.cached_handles()); i.more(); i.advance()) {
      if(!i->in_use_p()) continue;

      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
        assert(i->object()->inflated_header_p());

      // Users manipulate values accessible from the data* within an
      // RData without running a write barrier. Thusly if we see a mature
      // rdata, we must always scan it because it could contain
      // young pointers.
      } else if(!i->object()->young_object_p() && i->is_rdata()) {
        scan_object(i->object());
      }

      assert(i->object()->type_id() != InvalidType);
    }

    std::list<capi::Handle**>* gh = data.global_handle_locations();

    if(gh) {
      for(std::list<capi::Handle**>::iterator i = gh->begin();
          i != gh->end();
          ++i) {
        capi::Handle** loc = *i;
        if(capi::Handle* hdl = *loc) {
          if(!CAPI_REFERENCE_P(hdl)) continue;
          if(hdl->valid_p()) {
            Object* obj = hdl->object();
            if(obj && obj->reference_p() && obj->young_object_p()) {
              hdl->set_object(saw_object(obj));
            }
          } else {
            std::cerr << "Detected bad handle checking global capi handles\n";
          }
        }
      }
    }

    // Handle all promotions to non-young space that occurred.
    handle_promotions();

    assert(fully_scanned_p());
    // We're now done seeing the entire object graph of normal, live references.
    // Now we get to handle the unusual references, like finalizers and such.

    // Update finalizers. Doing so can cause objects that would have just died
    // to continue life until we can get around to running the finalizer. That
    // means more promoted objects, etc.
    check_finalize();

    // Run promotions again, because checking finalizers can keep more objects
    // alive (and thus promoted).
    handle_promotions();

    if(!promoted_stack_.empty()) rubinius::bug("promote stack has elements!");
    if(!fully_scanned_p()) rubinius::bug("more young refs");

    // Check any weakrefs and replace dead objects with nil
    clean_weakrefs(true);

    // Swap the 2 halves
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

    // Tune the age at which promotion occurs
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

  }

  bool BakerGC::handle_promotions() {
    if(promoted_stack_.empty() && fully_scanned_p()) return false;

    while(!promoted_stack_.empty() || !fully_scanned_p()) {
      while(!promoted_stack_.empty()) {
        Object* obj = promoted_stack_.back();
        promoted_stack_.pop_back();

        scan_object(obj);
      }

      copy_unscanned();
    }

    return true;
  }

  void BakerGC::check_finalize() {
    // If finalizers are running right now, just fixup any finalizer references
    if(object_memory_->running_finalizers()) {
      for(std::list<FinalizeObject>::iterator i = object_memory_->finalize().begin();
          i != object_memory_->finalize().end();
          ++i) {
        if(i->object) {
          i->object = saw_object(i->object);
        }

        if(i->ruby_finalizer) {
          i->ruby_finalizer = saw_object(i->ruby_finalizer);
        }
      }
      return;
    }

    for(std::list<FinalizeObject>::iterator i = object_memory_->finalize().begin();
        i != object_memory_->finalize().end(); )
    {
      FinalizeObject& fi = *i;

      if(i->ruby_finalizer) {
        i->ruby_finalizer = saw_object(i->ruby_finalizer);
      }

      bool remove = false;

      if(i->object->young_object_p()) {
        Object* orig = i->object;
        switch(i->status) {
        case FinalizeObject::eLive:
          if(!i->object->forwarded_p()) {
            // Run C finalizers now rather that queue them.
            if(i->finalizer) {
              State state_obj(state());
              (*i->finalizer)(&state_obj, i->object);
              i->status = FinalizeObject::eFinalized;
              remove = true;
            } else {
              i->queued();
              object_memory_->add_to_finalize(&fi);

              // We need to keep it alive still.
              i->object = saw_object(orig);
            }
          } else {
            // Still alive, update the reference.
            i->object = saw_object(orig);
          }
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
            // RESURRECTION!
            i->queued();
            i->object = saw_object(i->object);
          }
          break;
        }
      }

      if(remove) {
        i = object_memory_->finalize().erase(i);
      } else {
        ++i;
      }
    }
  }

  bool BakerGC::in_current_p(Object* obj) {
    return current->contains_p(obj);
  }

  ObjectPosition BakerGC::validate_object(Object* obj) {
    if(current->contains_p(obj) || eden.contains_p(obj)) {
      return cValid;
    } else if(next->contains_p(obj)) {
      return cInWrongYoungHalf;
    } else {
      return cUnknown;
    }
  }
}
