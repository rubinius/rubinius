#include <cstdlib>
#include <iostream>

#include "gc_baker.hpp"
#include "objectmemory.hpp"

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

    for(size_t i = 0; i < orig->field_count; i++) {
      tmp->field[i] = orig->field[i];
    }

    return tmp;
  }

  BakerGC::BakerGC(ObjectMemory *om, size_t bytes)
          :GarbageCollector(om), heap_a(bytes), heap_b(bytes) {
    current = &heap_a;
    next = &heap_b;
  }

  BakerGC::~BakerGC() { }

  OBJECT BakerGC::saw_object(OBJECT obj) {
    OBJECT copy;

    if(obj->forwarded_p()) return obj->forward();

    if(obj->age++ >= lifetime) {
      copy = object_memory->promote_object(obj);
    } else {
      copy = next->copy_object(obj);
    }

    obj->set_forward(copy);
    return copy;
  }

  void BakerGC::copy_unscanned() {
    OBJECT iobj = next->next_unscanned();

    while(iobj) {
      scan_object(iobj);
      iobj = next->next_unscanned();
    }
  }

  /* Perform garbage collection on the young objects. */
  void BakerGC::collect(ObjectArray &roots) {
    OBJECT tmp;
    ObjectArray *current_rs = object_memory->remember_set;

    object_memory->remember_set = new ObjectArray(0);
    for(size_t i = 0; i < current_rs->size(); i++) {
      tmp = current_rs->operator[](i);
      scan_object(tmp);
    }

    delete current_rs;

    for(size_t i = 0; i < roots.size(); i++) {
      tmp = roots[i];
      if(tmp->reference_p() && tmp->young_object_p()) {
        roots[i] = saw_object(tmp);
      }
    }

    /* We've seeded next with all the roots, we now just move down next
     * against the scan pointer until there are no more objects. */
    copy_unscanned();

    /* Another than is going to be found is found now, so we go back and
     * look at everything in current and call delete_object() on anything
     * thats not been forwarded. */
    find_lost_souls();

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
}
