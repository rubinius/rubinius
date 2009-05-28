#include "gc/immix.hpp"
#include "objectmemory.hpp"

#include "instruments/stats.hpp"

#include "capi/handle.hpp"

namespace rubinius {
  void ImmixGC::ObjectDescriber::added_chunk(int count) {
#ifdef IMMIX_DEBUG
    std::cout << "Added a chunk: " << count << "\n";
#endif

#ifdef RBX_GC_STATS
    stats::GCStats::get()->chunks_added++;
#endif

    if(object_memory_) {
      object_memory_->collect_mature_now = true;
    }
  }

  void ImmixGC::ObjectDescriber::set_forwarding_pointer(immix::Address from, immix::Address to) {
    from.as<Object>()->set_forward(to.as<Object>());
  }

  ImmixGC::ImmixGC(ObjectMemory* om)
    : GarbageCollector(om)
    , allocator_(gc_.block_allocator())
    , which_mark_(1)
  {
    gc_.describer().set_object_memory(om, this);
  }

  immix::Address ImmixGC::ObjectDescriber::copy(immix::Address original,
      immix::Allocator& alloc) {
    Object* orig = original.as<Object>();

    immix::Address copy_addr = alloc.allocate(
        orig->size_in_bytes(object_memory_->state));

    Object* copy = copy_addr.as<Object>();

    copy->initialize_copy(orig, 0);
    copy->copy_body(object_memory_->state, orig);

    copy->zone = MatureObjectZone;
    copy->set_in_immix();

    return copy_addr;
  }

  int ImmixGC::ObjectDescriber::size(immix::Address addr) {
    return addr.as<Object>()->size_in_bytes(object_memory_->state);
  }

  ImmixGC::~ImmixGC() {
    // TODO free data
  }

  Object* ImmixGC::allocate(int bytes) {
#ifdef RBX_GC_STATS
    stats::GCStats::get()->mature_bytes_allocated += bytes;
    stats::GCStats::get()->allocate_mature.start();
#endif

    Object* obj = allocator_.allocate(bytes).as<Object>();
    obj->init_header(MatureObjectZone, InvalidType);
    obj->set_in_immix();

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

  ObjectPosition ImmixGC::validate_object(Object* obj) {
    if(gc_.allocated_address(immix::Address(obj))) {
      if(obj->in_immix_p()) {
        return cInImmix;
      } else {
        return cInImmixCorruptHeader;
      }
    }

    return cUnknown;
  }

  void ImmixGC::collect(GCData& data) {
    Object* tmp;

    gc_.clear_lines();

    for(Roots::Iterator i(data.roots()); i.more(); i.advance()) {
      tmp = i->get();
      if(tmp->reference_p()) {
        saw_object(tmp);
      }
    }

    for(capi::Handles::Iterator i(*data.handles()); i.more(); i.advance()) {
      if(!i->weak_p()) saw_object(i->object());
    }

    for(VariableRootBuffers::Iterator i(data.variable_buffers());
        i.more(); i.advance()) {
      Object*** buffer = i->buffer();
      for(int idx = 0; idx < i->size(); idx++) {
        Object** var = buffer[idx];
        Object* tmp = *var;

        if(tmp->reference_p() && tmp->young_object_p()) {
          saw_object(tmp);
        }
      }
    }

    // Walk all the call frames
    for(CallFrameLocationList::const_iterator i = data.call_frames().begin();
        i != data.call_frames().end();
        i++) {
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }

    gc_.process_mark_stack(allocator_);

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

        if(!tmp->marked_p()) {
          cleared++;
          *oi = NULL;
        }
      }
    }

    // Switch the which_mark_ for next time.
    which_mark_ = (which_mark_ == 1 ? 2 : 1);

#ifdef IMMIX_DEBUG
    std::cout << "Immix: RS size cleared: " << cleared << "\n";

    immix::Chunks& chunks = gc_.block_allocator().chunks();
    std::cout << "chunks=" << chunks.size() << "\n";

    immix::AllBlockIterator iter(chunks);

    int blocks_seen = 0;
    int total_objects = 0;
    int total_object_bytes = 0;

    while(immix::Block* block = iter.next()) {
      blocks_seen++;
      std::cout << "block " << block << ", holes=" << block->holes() << " "
                << "objects=" << block->objects() << " "
                << "object_bytes=" << block->object_bytes() << " "
                << "frag=" << block->fragmentation_ratio()
                << "\n";

      total_objects += block->objects();
      total_object_bytes += block->object_bytes();
    }

    std::cout << blocks_seen << " blocks\n";
    std::cout << gc_.bytes_allocated() << " bytes allocated\n";
    std::cout << total_object_bytes << " object bytes / " << total_objects << " objects\n";

    int* holes = new int[10];
    for(int i = 0; i < 10; i++) {
      holes[i] = 0;
    }

    immix::AllBlockIterator iter2(chunks);

    while(immix::Block* block = iter2.next()) {
      int h = block->holes();
      if(h > 9) h = 9;

      holes[h]++;
    }

    std::cout << "== hole stats ==\n";
    for(int i = 0; i < 10; i++) {
      if(holes[i] > 0) {
        std::cout << i << ": " << holes[i] << "\n";
      }
    }
#endif
  }

}
