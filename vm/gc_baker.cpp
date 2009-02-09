#include <cstdlib>
#include <iostream>

#include "gc_baker.hpp"
#include "objectmemory.hpp"
#include "vm/object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"

#include "call_frame.hpp"

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

    if(!obj->reference_p()) return obj;

    if(obj->zone != YoungObjectZone) return obj;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    // TODO test this!
    if(next->contains_p(obj)) return obj;

    if(obj->age++ >= lifetime) {
      copy = object_memory->promote_object(obj);

      promoted_->push_back(copy);
    } else if(next->enough_space_p(obj->size_in_bytes())) {
      copy = next->copy_object(obj);
      total_objects++;
    } else {
      copy = object_memory->promote_object(obj);
      promoted_->push_back(copy);
    }

    if(MethodContext* ctx = try_as<MethodContext>(copy)) {
      ctx->post_copy(as<MethodContext>(obj));
    }

    obj->set_forward(object_memory->state, copy);
    return copy;
  }

  void BakerGC::copy_unscanned() {
    Object* iobj = next->next_unscanned();

    while(iobj) {
      assert(iobj->zone == YoungObjectZone);
      if(!iobj->forwarded_p()) scan_object(iobj);
      iobj = next->next_unscanned();
    }
  }

  bool BakerGC::fully_scanned_p() {
    return next->fully_scanned_p();
  }

  /* Perform garbage collection on the young objects. */
  void BakerGC::collect(Roots &roots, CallFrameList &call_frames) {
    Object* tmp;
    ObjectArray *current_rs = object_memory->remember_set;

    object_memory->remember_set = new ObjectArray(0);
    total_objects = 0;

    // Tracks all objects that we promoted during this run, so
    // we can scan them at the end.
    promoted_ = new ObjectArray(0);

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        oi++) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        assert(tmp->zone == MatureObjectZone);
        assert(!tmp->forwarded_p());

        /* Remove the Remember bit, since we're clearing the set. */
        tmp->Remember = 0;
        scan_object(tmp);
      }
    }

    delete current_rs;

    Root* root = static_cast<Root*>(roots.head());
    while(root) {
      tmp = root->get();
      if(tmp->reference_p() && tmp->young_object_p()) {
        root->set(saw_object(tmp));
      }

      root = static_cast<Root*>(root->next());
    }

    // Walk all the call frames
    for(CallFrameList::const_iterator i = call_frames.begin();
        i != call_frames.end();
        i++) {
      walk_call_frame(*i);
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

    while(promoted_->size() > 0 || !fully_scanned_p()) {
      ObjectArray* cur = promoted_;

      if(promoted_->size() > 0) {
        promoted_ = new ObjectArray(0);

        for(ObjectArray::iterator oi = cur->begin();
            oi != cur->end();
            oi++) {
          tmp = *oi;
          assert(tmp->zone == MatureObjectZone);
          scan_object(tmp);
        }

        delete cur;

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
    clean_weakrefs();

    /* Swap the 2 halves */
    Heap *x = next;
    next = current;
    current = x;
    next->reset();
  }

  Object* BakerGC::next_object(Object* obj) {
    return (Object*)((uintptr_t)obj + obj->size_in_bytes());
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
      if(!obj->forwarded_p()) delete_object(obj);
      obj = next_object(obj);
    }
  }

  // HACK todo test this!
  void BakerGC::clean_weakrefs() {
    if(!weak_refs) return;

    for(ObjectArray::iterator i = weak_refs->begin();
        i != weak_refs->end();
        i++) {
      // ATM, only a Tuple can be marked weak.
      Tuple* tup = as<Tuple>(*i);
      for(size_t ti = 0; ti < tup->num_fields(); ti++) {
        Object* obj = tup->at(object_memory->state, ti);

        if(!obj->reference_p()) continue;

        if(obj->young_object_p()) {
          if(!obj->forwarded_p()) {
            // HACK you shouldn't really set these directly, but since
            // we know that the write barrier has already run for this
            // it should be fine.
            tup->field[ti] = Qnil;
          } else {
            tup->field[ti] = obj->forward();
          }
        }
      }
    }

    delete weak_refs;
    weak_refs = NULL;
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
