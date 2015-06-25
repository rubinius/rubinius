#include <stdlib.h>
#include <iostream>

#include "gc/baker.hpp"
#include "object_memory.hpp"
#include "object_utils.hpp"

#include "builtin/tuple.hpp"
#include "builtin/class.hpp"
#include "builtin/io.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

#include "gc/gc.hpp"

#include "capi/handles.hpp"
#include "capi/tag.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

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
  BakerGC::BakerGC(ObjectMemory *om, Configuration& config)
    : GarbageCollector(om)
    , bytes_(config.gc_young_bytes * 2)
    , full(new Heap(bytes_))
    , eden(new Heap(full->allocate(bytes_ / 2), bytes_ / 2))
    , heap_a(new Heap(full->allocate(bytes_ / 4), bytes_ / 4))
    , heap_b(new Heap(full->allocate(bytes_ / 4), bytes_ / 4))
    , current(heap_a)
    , next(heap_b)
    , lifetime_(config.gc_young_lifetime)
  {
    reset();
  }

  BakerGC::~BakerGC() {
    delete heap_a;
    delete heap_b;
    delete eden;
    delete full;
  }

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

    if(!obj->reference_p()) return NULL;

    if(!obj->young_object_p()) return NULL;

    if(obj->forwarded_p()) return obj->forward();

    // This object is already in the next space, we don't want to
    // copy it again!
    if(next->contains_p(obj)) return NULL;

    if(unlikely(obj->inc_age() >= lifetime_)) {
      copy = object_memory_->promote_object(obj);

      promoted_push(copy);
    } else if(likely(next->enough_space_p(
                obj->size_in_bytes(object_memory_->state())))) {
      copy = next->move_object(object_memory_->state(), obj);
    } else {
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

  /**
   * Perform garbage collection on the young objects.
   */
  void BakerGC::collect(GCData* data) {

#ifdef HAVE_VALGRIND_H
    (void)VALGRIND_MAKE_MEM_DEFINED(next->start().as_int(), next->size());
    (void)VALGRIND_MAKE_MEM_DEFINED(current->start().as_int(), current->size());
#endif
    mprotect(next->start(), next->size(), PROT_READ | PROT_WRITE);
    mprotect(current->start(), current->size(), PROT_READ | PROT_WRITE);

    ObjectArray *current_rs = object_memory_->swap_remember_set();

    // Start by copying objects in the remember set
    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        ++oi) {
      Object* tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        // Remove the Remember bit, since we're clearing the set.
        tmp->clear_remember();
        scan_object(tmp);
      }
    }

    delete current_rs;

    scan_mark_set();
    scan_mature_mark_stack();

    for(Roots::Iterator i(data->roots()); i.more(); i.advance()) {
      if(Object* fwd = saw_object(i->get())) {
        i->set(fwd);
      }
    }

    if(data->threads()) {
      for(ThreadList::iterator i = data->threads()->begin();
          i != data->threads()->end();
          ++i) {
        scan(*i, true);
      }
    }

    for(Allocator<capi::Handle>::Iterator i(data->handles()->allocator());
        i.more();
        i.advance())
    {
      if(!i->in_use_p()) continue;

      if(!i->weak_p() && i->object()->young_object_p()) {
        if(Object* fwd = saw_object(i->object())) {
          i->set_object(fwd);
        }

      // Users manipulate values accessible from the data* within an
      // RData without running a write barrier. Thusly if we see a mature
      // rdata, we must always scan it because it could contain
      // young pointers.
      } else if(!i->object()->young_object_p() && i->is_rdata()) {
        scan_object(i->object());
      }
    }

    std::list<capi::GlobalHandle*>* gh = data->global_handle_locations();

    if(gh) {
      for(std::list<capi::GlobalHandle*>::iterator i = gh->begin();
          i != gh->end();
          ++i) {
        capi::GlobalHandle* global_handle = *i;
        capi::Handle** loc = global_handle->handle();
        if(capi::Handle* hdl = *loc) {
          if(!REFERENCE_P(hdl)) continue;
          if(hdl->valid_p()) {
            Object* obj = hdl->object();
            if(obj && obj->reference_p() && obj->young_object_p()) {
              if(Object* fwd = saw_object(obj)) {
                hdl->set_object(fwd);
              }
            }
          } else {
            std::cerr << "Detected bad handle checking global capi handles\n";
          }
        }
      }
    }

#ifdef ENABLE_LLVM
    if(LLVMState* ls = data->llvm_state()) ls->gc_scan(this);
#endif

    // Handle all promotions to non-young space that occurred.
    handle_promotions();

    assert(fully_scanned_p());
    // We're now done seeing the entire object graph of normal, live references.
    // Now we get to handle the unusual references, like finalizers and such.

    // Check any weakrefs and replace dead objects with nil
    // We need to do this before checking finalizers so people can't access
    // objects kept alive for finalization through weakrefs.
    clean_weakrefs(true);

    do {
      // Objects with finalizers must be kept alive until the finalizers have
      // run.
      walk_finalizers();
      // Scan any fibers that aren't running but still active
      scan_fibers(data, false);
      // Scanning finalizers and fibers can result in
      // new objects in the mark set when we do a young GC
      // during a mature mark phase.
      scan_mark_set();

      handle_promotions();
    } while(!promoted_stack_.empty() && !fully_scanned_p());

    // Remove unreachable locked objects still in the list
    if(data->threads()) {
      for(ThreadList::iterator i = data->threads()->begin();
          i != data->threads()->end();
          ++i) {
        clean_locked_objects(*i, true);
      }
    }

    // Update the pending mark set to remove unreachable objects.
    update_mark_set();

    // Update the existing mark stack of the mature gen because young
    // objects might have moved.
    update_mature_mark_stack();

    // Update the weak ref set to remove unreachable weak refs.
    update_weak_refs_set();

    // Swap the 2 halves
    Heap* x = next;
    next = current;
    current = x;
  }

  void BakerGC::reset() {
    next->reset();
    eden->reset();

#ifdef HAVE_VALGRIND_H
    (void)VALGRIND_MAKE_MEM_NOACCESS(next->start().as_int(), next->size());
    (void)VALGRIND_MAKE_MEM_DEFINED(current->start().as_int(), current->size());
#endif
    mprotect(next->start(), next->size(), PROT_NONE);
    mprotect(current->start(), current->size(), PROT_READ | PROT_WRITE);
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

  void BakerGC::scan_mark_set() {
    // Scan any mature objects in the mark set
    // since they might refer to young objects.
    ObjectArray* marked_set = object_memory_->marked_set();

    // This explicitly does not use an iterator. This is because
    // there might be elements appended to this list by scanning
    // which in turn can trigger a vector resize. This would invalidate
    // the iterator and result in invalid memory being read.
    for(ObjectArray::size_type i = 0; i < marked_set->size(); ++i) {
      Object* obj = marked_set->at(i);
      if(obj && obj->mature_object_p()) {
        scan_object(obj);
      }
    }
  }

  void BakerGC::update_mark_set() {
    // Update the marked set and remove young not forwarded objects
    ObjectArray* marked_set = object_memory_->marked_set();

    for(ObjectArray::iterator oi = marked_set->begin();
        oi != marked_set->end(); ++oi) {
      Object* obj = *oi;
      if(!obj) continue; // Already removed during previous cycle
      if(obj->young_object_p()) {
        if(obj->forwarded_p()) {
          *oi = obj->forward();
        } else {
          *oi = NULL;
        }
      }
    }
  }

  void BakerGC::scan_mature_mark_stack() {
    immix::MarkStack& stack = object_memory_->mature_mark_stack();
    for(immix::MarkStack::iterator i = stack.begin(); i != stack.end(); ++i) {
      Object* obj = (*i).as<Object>();
      if(obj && obj->mature_object_p()) {
        scan_object(obj);
      }
    }
  }

  void BakerGC::update_mature_mark_stack() {
    immix::MarkStack& stack = object_memory_->mature_mark_stack();
    for(immix::MarkStack::iterator i = stack.begin(); i != stack.end(); ++i) {
      Object* obj = (*i).as<Object>();
      if(obj && obj->young_object_p()) {
        if(obj->forwarded_p()) {
          *i = obj->forward();
        } else {
          *i = memory::Address::null();
        }
      }
    }
  }

  void BakerGC::update_weak_refs_set() {
    // Update the weakref set for mature GC and remove young not forwarded objects
    ObjectArray* weak_refs_set = object_memory_->weak_refs_set();

    if(weak_refs_set) {
      for(ObjectArray::iterator oi = weak_refs_set->begin();
          oi != weak_refs_set->end(); ++oi) {
        Object* obj = *oi;
        if(!obj) continue; // Already removed during previous cycle
        if(obj->young_object_p()) {
          if(obj->forwarded_p()) {
            *oi = obj->forward();
          } else {
            *oi = NULL;
          }
        }
      }
    }
  }

  void BakerGC::walk_finalizers() {
    FinalizerThread* ft = object_memory_->finalizer_handler();
    if(!ft) return;

    for(FinalizerThread::iterator i = ft->begin();
        !i.end();
        /* advance is handled in the loop */)
    {
      FinalizeObject& fi = i.current();
      bool live = true;

      if(fi.object->young_object_p()) {
        live = fi.object->forwarded_p();
        if(Object* fwd = saw_object(fi.object)) {
          fi.object = fwd;
        }
      } else {
        // If this object is mature, scan it. This
        // means that any young objects it refers to are properly
        // GC'ed and kept alive if necessary
        scan_object(fi.object);
      }

      Object* fin = fi.ruby_finalizer;
      if(fin && fin->reference_p()) {
        if(fin->young_object_p()) {
          if(Object* fwd = saw_object(fin)) {
            fi.ruby_finalizer = fwd;
          }
        } else {
          // If this object is mature, scan it. This
          // means that any young objects it refers to are properly
          // GC'ed and kept alive if necessary
          scan_object(fin);
        }
      }

      i.next(live);
    }
  }

  bool BakerGC::in_current_p(Object* obj) {
    return current->contains_p(obj);
  }

  ObjectPosition BakerGC::validate_object(Object* obj) {
    if(current->contains_p(obj) || eden->contains_p(obj)) {
      return cValid;
    } else if(next->contains_p(obj)) {
      return cInWrongYoungHalf;
    } else {
      return cUnknown;
    }
  }
}
