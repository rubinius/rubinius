#include <cstdlib>
#include <iostream>

#include "gc_baker.hpp"
#include "objectmemory.hpp"
#include "builtin/tuple.hpp"
#include "builtin/contexts.hpp"

namespace rubinius {

  /* BakerGC::Heap methods */
  BakerGC::Heap::Heap(size_t bytes) {
    size = bytes;
    start = (address)std::calloc(1, size);
    scan = start;
    last = (void*)((uintptr_t)start + bytes - 1);
    reset();
  }

  BakerGC::Heap::~Heap() {
    std::free(start);
  }

  void BakerGC::Heap::reset() {
    current = start;
    scan = start;
  }

  size_t BakerGC::Heap::remaining() {
    size_t bytes = (uintptr_t)last - (uintptr_t)current;
    return bytes;
  }

  size_t BakerGC::Heap::used() {
    size_t bytes = (uintptr_t)current - (uintptr_t)start;
    return bytes;
  }

  OBJECT BakerGC::Heap::copy_object(OBJECT orig) {
    OBJECT tmp = (OBJECT)allocate(orig->size_in_bytes());
    tmp->all_flags   = orig->all_flags;
    tmp->field_count = orig->field_count;
    tmp->klass       = orig->klass;
    tmp->ivars       = orig->ivars;
    tmp->Forwarded   = 0;

    for(size_t i = 0; i < orig->field_count; i++) {
      tmp->field[i] = orig->field[i];
    }

    return tmp;
  }

  BakerGC::BakerGC(ObjectMemory *om, size_t bytes) :
    GarbageCollector(om), heap_a(bytes), heap_b(bytes), total_objects(0)
  {
    current = &heap_a;
    next = &heap_b;
  }

  BakerGC::~BakerGC() { }

  OBJECT BakerGC::saw_object(OBJECT obj) {
    OBJECT copy;

    if(obj->zone != YoungObjectZone) return obj;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    // TODO test this!
    if(next->contains_p(obj)) return obj;

    if(obj->age++ >= lifetime) {
      copy = object_memory->promote_object(obj);

      // We promoted it, so our scan pointer loop wont see it to walk
      // it's references. Instead we scan in manually here.
      // HACK TODO this can cause a cascading C stack as a lot of
      // objects are promoted at once and cause a stack overflow.
      //
      // We should keep track of all the promoted objects and walk
      // them at the end.
      scan_object(copy);
    } else if(next->enough_space_p(obj->size_in_bytes())) {
      copy = next->copy_object(obj);
      total_objects++;
    } else {
      copy = object_memory->promote_object(obj);
      // See note above concerning promoted objects.
      scan_object(copy);
    }

    if(MethodContext* ctx = try_as<MethodContext>(copy)) {
      ctx->post_copy(as<MethodContext>(obj));
    }

    obj->set_forward(copy);
    return copy;
  }

  void BakerGC::copy_unscanned() {
    OBJECT iobj = next->next_unscanned();

    while(iobj) {
      assert(iobj->zone == YoungObjectZone);
      if(!iobj->forwarded_p()) scan_object(iobj);
      iobj = next->next_unscanned();
    }
  }

  /* Perform garbage collection on the young objects. */
  void BakerGC::collect(Roots &roots) {
    OBJECT tmp;
    ObjectArray *current_rs = object_memory->remember_set;

    object_memory->remember_set = new ObjectArray(0);
    total_objects = 0;

    ObjectArray::iterator oi;
    for(oi = current_rs->begin(); oi != current_rs->end(); oi++) {
      tmp = *oi;
      assert(tmp->zone == MatureObjectZone);
      assert(!tmp->forwarded_p());

      /* Remove the Remember bit, since we're clearing the set. */
      tmp->Remember = 0;
      scan_object(tmp);
    }

    delete current_rs;

    Roots::iterator i;
    for(i = roots.begin(); i != roots.end(); i++) {
      tmp = (*i)->get();
      if(tmp->reference_p() && tmp->young_object_p()) {
        (*i)->set(saw_object(tmp));
      }
    }

    /* We've seeded next with all the roots, we now just move down next
     * against the scan pointer until there are no more objects. */
    copy_unscanned();

    /* Another than is going to be found is found now, so we go back and
     * look at everything in current and call delete_object() on anything
     * thats not been forwarded. */
    find_lost_souls();

    /* Check any weakrefs and replace dead objects with nil*/
    clean_weakrefs();

    /* Swap the 2 halves */
    BakerGC::Heap *x = next;
    next = current;
    current = x;
    next->reset();
  }

  OBJECT BakerGC::next_object(OBJECT obj) {
    return (OBJECT)((uintptr_t)obj + obj->size_in_bytes());
  }

  void BakerGC::clear_marks() {
    OBJECT obj = current->first_object();
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
    OBJECT obj = current->first_object();
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
    OBJECT obj = current->first_object();
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
      for(size_t ti = 0; ti < tup->field_count; ti++) {
        OBJECT obj = tup->at(ti);
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
  }

  ObjectPosition BakerGC::validate_object(OBJECT obj) {
    if(current->contains_p(obj)) {
      return cValid;
    } else if(next->contains_p(obj)) {
      return cInWrongYoungHalf;
    } else {
      return cUnknown;
    }
  }
}
