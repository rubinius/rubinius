#include <cstdlib>
#include <iostream>

#include "gc/baker.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "builtin/tuple.hpp"

#include "instruments/stats.hpp"

#include "call_frame.hpp"

#include "gc/gc.hpp"

#include "capi/handle.hpp"

namespace rubinius {
  BakerGC::BakerGC(ObjectMemory *om, size_t bytes) :
    GarbageCollector(om),
    heap_a(bytes),
    heap_b(bytes),
    total_objects(0),
    promoted_(0)
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

    if(obj->zone != YoungObjectZone) return obj;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    // TODO test this!
    if(next->contains_p(obj)) return obj;

    if(unlikely(obj->age++ >= lifetime)) {
      copy = object_memory->promote_object(obj);

      promoted_push(copy);
    } else if(likely(next->enough_space_p(obj->size_in_bytes(object_memory->state)))) {
      copy = next->copy_object(object_memory->state, obj);
      total_objects++;
    } else {
      copy = object_memory->promote_object(obj);
      promoted_push(copy);
    }

    if(watched_p(copy)) {
      std::cout << "detected " << copy << " during baker collection (2)\n";
    }

    obj->set_forward(copy);
    return copy;
  }

  void BakerGC::copy_unscanned() {
    Object* iobj = next->next_unscanned(object_memory->state);

    while(iobj) {
      assert(iobj->zone == YoungObjectZone);
      if(!iobj->forwarded_p()) scan_object(iobj);
      iobj = next->next_unscanned(object_memory->state);
    }
  }

  bool BakerGC::fully_scanned_p() {
    return next->fully_scanned_p();
  }

  /* Perform garbage collection on the young objects. */
  void BakerGC::collect(GCData& data) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->bytes_copied.start();
    stats::GCStats::get()->objects_copied.start();
    stats::GCStats::get()->objects_promoted.start();
    stats::GCStats::get()->collect_young.start();
#endif

    Object* tmp;
    ObjectArray *current_rs = object_memory->remember_set;

    object_memory->remember_set = new ObjectArray(0);
    total_objects = 0;

    // Tracks all objects that we promoted during this run, so
    // we can scan them at the end.
    promoted_ = new ObjectArray(0);

    promoted_current = promoted_insert = promoted_->begin();

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        ++oi) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        assert(tmp->zone == MatureObjectZone);
        assert(!tmp->forwarded_p());

        // Remove the Remember bit, since we're clearing the set.
        tmp->clear_remember();
        scan_object(tmp);
      }
    }

    delete current_rs;

    for(Roots::Iterator i(data.roots()); i.more(); i.advance()) {
      tmp = i->get();
      if(tmp->reference_p() && tmp->young_object_p()) {
        i->set(saw_object(tmp));
      }
    }

    for(capi::Handles::Iterator i(*data.handles()); i.more(); i.advance()) {
      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
      }

      assert(i->object()->type_id() != InvalidType);
    }

    for(capi::Handles::Iterator i(*data.cached_handles()); i.more(); i.advance()) {
      if(!i->weak_p() && i->object()->young_object_p()) {
        i->set_object(saw_object(i->object()));
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

    /* Ok, now handle all promoted objects. This is setup a little weird
     * so I should explain.
     *
     * We want to scan each promoted object. But this scanning will likely
     * cause more objects to be promoted. Adding to an ObjectArray that your
     * iterating over blows up the iterators, so instead we rotate the
     * current promoted set out as we iterator over it, and stick an
     * empty ObjectArray in.
     *
     * This way, when there are no more objects that are promoted, the last
     * ObjectArray will be empty.
     * */

    promoted_current = promoted_insert = promoted_->begin();

    while(promoted_->size() > 0 || !fully_scanned_p()) {
      if(promoted_->size() > 0) {
        for(;promoted_current != promoted_->end();
            ++promoted_current) {
          tmp = *promoted_current;
          assert(tmp->zone == MatureObjectZone);
          scan_object(tmp);
          if(watched_p(tmp)) {
            std::cout << "detected " << tmp << " during scan of promoted objects.\n";
          }
        }

        promoted_->resize(promoted_insert - promoted_->begin());
        promoted_current = promoted_insert = promoted_->begin();

      }

      /* As we're handling promoted objects, also handle unscanned objects.
       * Scanning these unscanned objects (via the scan pointer) will
       * cause more promotions. */
      copy_unscanned();
    }

    assert(promoted_->size() == 0);

    delete promoted_;
    promoted_ = NULL;

    assert(fully_scanned_p());

    /* Another than is going to be found is found now, so we go back and
     * look at everything in current and call delete_object() on anything
     * thats not been forwarded. */
    find_lost_souls();

    /* Check any weakrefs and replace dead objects with nil*/
    clean_weakrefs(true);

    /* Swap the 2 halves */
    Heap *x = next;
    next = current;
    current = x;
    next->reset();

#ifdef RBX_GC_STATS
    stats::GCStats::get()->collect_young.stop();
    stats::GCStats::get()->objects_copied.stop();
    stats::GCStats::get()->objects_promoted.stop();
    stats::GCStats::get()->bytes_copied.stop();
#endif
  }

  inline Object * BakerGC::next_object(Object * obj) {
    return reinterpret_cast<Object*>(reinterpret_cast<uintptr_t>(obj) +
      obj->size_in_bytes(object_memory->state));
  }

  void BakerGC::clear_marks() {
    Object* obj = current->first_object();
    while(obj < current->current) {
      obj->clear_mark();
      obj = next_object(obj);
    }

    obj = next->first_object();
    while(obj < next->current) {
      obj->clear_mark();
      obj = next_object(obj);
    }
  }

  void BakerGC::free_objects() {
    Object* obj = current->first_object();
    while(obj < current->current) {
      delete_object(obj);
      obj = next_object(obj);
    }

    assert(next->current < next->last);
    obj = next->first_object();
    while(obj < next->current) {
      delete_object(obj);
      obj = next_object(obj);
    }
  }

  void BakerGC::find_lost_souls() {
    Object* obj = current->first_object();
    while(obj < current->current) {
      if(!obj->forwarded_p()) {
        delete_object(obj);

#ifdef RBX_GC_STATS
        stats::GCStats::get()->lifetimes[obj->age]++;
#endif
      }
      obj = next_object(obj);
    }
  }

  ObjectPosition BakerGC::validate_object(Object* obj) {
    if(current->contains_p(obj)) {
      return cValid;
    } else if(next->contains_p(obj)) {
      return cInWrongYoungHalf;
    } else {
      return cUnknown;
    }
  }
}
