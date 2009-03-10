#include "gc_immix.hpp"
#include "objectmemory.hpp"

#include "instruments/stats.hpp"

namespace rubinius {
  void ImmixGC::ObjectDescriber::added_chunk(int count) {
#ifdef IMMIX_DEBUG
    std::cout << "Added a chunk: " << count << "\n";
#endif

    if(object_memory_) {
      object_memory_->collect_mature_now = true;
    }
  }

  void ImmixGC::ObjectDescriber::set_forwarding_pointer(immix::Address from, immix::Address to) {
    from.as<Object>()->set_forward(object_memory_->state, to.as<Object>());
  }

  ImmixGC::ImmixGC(ObjectMemory* om)
    : GarbageCollector(om)
    , allocator_(gc_.block_allocator())
  {
    gc_.describer().set_object_memory(om, this);
  }

  ImmixGC::~ImmixGC() {
    // TODO free data
  }

  Object* ImmixGC::allocate(int bytes) {
#ifdef RBX_GC_STATS
    // duplicating the calulation so it is included in the time below
    stats::GCStats::get()->mature_bytes_allocated += bytes;
    stats::GCStats::get()->allocate_mature.start();
#endif

    Object* obj = allocator_.allocate(bytes).as<Object>();
    obj->init_header(MatureObjectZone, bytes);
    obj->InImmix = 1;

#ifdef RBX_GC_STATS
    stats::GCStats::get()->allocate_mature.stop();
#endif

    return obj;
  }

  Object* ImmixGC::saw_object(Object* obj) {
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during immix scanning.\n";
    }

    immix::Address fwd = gc_.mark_address(immix::Address(obj), allocator_);
    return fwd.as<Object>();
  }

  void ImmixGC::collect(Roots &roots, CallFrameLocationList& call_frames) {
    Object* tmp;

    Root* root = static_cast<Root*>(roots.head());
    while(root) {
      tmp = root->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }

      root = static_cast<Root*>(root->next());
    }

    // Walk all the call frames
    for(CallFrameLocationList::const_iterator i = call_frames.begin();
        i != call_frames.end();
        i++) {
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }

    gc_.process_mark_stack(allocator_);

    // Cleanup all weakrefs seen
    // TODO support weakrefs!

    // Sweep up the garbage
    gc_.sweep_blocks();

    // This resets the allocator state to sync it up with the BlockAllocator
    // properly.
    allocator_.get_new_block();

    ObjectArray *current_rs = object_memory->remember_set;

    int cleared = 0;

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        oi++) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        assert(tmp->zone == MatureObjectZone);
        assert(!tmp->forwarded_p());

        if(!tmp->Marked) {
          cleared++;
          *oi = NULL;
        }
      }
    }

#ifdef IMMIX_DEBUG
    std::cout << "Immix: RS size cleared: " << cleared << "\n";
#endif

  }

}
