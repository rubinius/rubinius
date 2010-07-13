#include "objectmemory.hpp"
#include "gc/immix.hpp"

#include "instruments/stats.hpp"

#include "capi/handle.hpp"
#include "object_watch.hpp"

#include "configuration.hpp"

namespace rubinius {
  void ImmixGC::ObjectDescriber::added_chunk(int count) {
#ifdef IMMIX_DEBUG
    std::cout << "[GC IMMIX: Added a chunk: " << count << "]\n";
#endif

#ifdef RBX_GC_STATS
    stats::GCStats::get()->chunks_added++;
#endif

    if(object_memory_) {
      if(gc_->dec_chunks_left() <= 0) {
        // object_memory_->collect_mature_now = true;
        gc_->reset_chunks_left();
      }
    }
  }

  // This means we're getting low on memory!
  void ImmixGC::ObjectDescriber::last_block() {
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
    , chunks_before_collection_(10)
  {
    gc_.describer().set_object_memory(om, this);
    reset_chunks_left();
  }

  immix::Address ImmixGC::ObjectDescriber::copy(immix::Address original,
      immix::Allocator& alloc) {
    Object* orig = original.as<Object>();

    immix::Address copy_addr = alloc.allocate(
        orig->size_in_bytes(object_memory_->state));

    Object* copy = copy_addr.as<Object>();

    copy->initialize_full_state(object_memory_->state, orig, 0);

    copy->set_zone(MatureObjectZone);
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

    if(!obj->reference_p()) return obj;

    immix::Address fwd = gc_.mark_address(immix::Address(obj), allocator_);
    Object* copy = fwd.as<Object>();

    // Check and update an inflated header
    if(copy && copy != obj && obj->inflated_header_p()) {
      InflatedHeader* ih = obj->deflate_header();
      ih->reset_object(copy);
      copy->set_inflated_header(ih);
    }

    return copy;
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

    int via_handles_ = 0;
    int via_roots = 0;
    int via_stack = 0;
    int callframes = 0;

    for(Roots::Iterator i(data.roots()); i.more(); i.advance()) {
      tmp = i->get();
      if(tmp->reference_p()) saw_object(tmp);
      via_roots++;
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
      if(i->in_use_p() && !i->weak_p()) {
        saw_object(i->object());
        via_handles_++;
      }
    }

    for(capi::Handles::Iterator i(*data.cached_handles()); i.more(); i.advance()) {
      if(i->in_use_p() && !i->weak_p()) {
        saw_object(i->object());
        via_handles_++;
      }
    }

    for(VariableRootBuffers::Iterator i(data.variable_buffers());
        i.more(); i.advance()) {
      Object*** buffer = i->buffer();
      for(int idx = 0; idx < i->size(); idx++) {
        Object** var = buffer[idx];
        Object* tmp = *var;

        via_stack++;
        if(tmp->reference_p() && tmp->young_object_p()) {
          saw_object(tmp);
        }
      }
    }

    // Walk all the call frames
    for(CallFrameLocationList::const_iterator i = data.call_frames().begin();
        i != data.call_frames().end();
        i++) {
      callframes++;
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }

    gc_.process_mark_stack(allocator_);

    // We've now finished marking the entire object graph.

    check_finalize();

    // Finalize can cause more things to continue to live, so we must
    // check the mark_stack again.
    gc_.process_mark_stack(allocator_);

    // Sweep up the garbage
    gc_.sweep_blocks();

    // This resets the allocator state to sync it up with the BlockAllocator
    // properly.
    allocator_.get_new_block();

    ObjectArray *current_rs = object_memory_->remember_set();

    int cleared = 0;

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        oi++) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) {
        assert(tmp->zone() == MatureObjectZone);
        assert(!tmp->forwarded_p());

        if(!tmp->marked_p(object_memory_->mark())) {
          cleared++;
          *oi = NULL;
        }
      }
    }

    for(std::list<gc::WriteBarrier*>::iterator wbi = object_memory_->aux_barriers().begin();
        wbi != object_memory_->aux_barriers().end();
        wbi++) {
      gc::WriteBarrier* wb = *wbi;
      ObjectArray* rs = wb->remember_set();
      for(ObjectArray::iterator oi = rs->begin();
          oi != rs->end();
          oi++) {
        tmp = *oi;

        if(tmp) {
          assert(tmp->zone() == MatureObjectZone);
          assert(!tmp->forwarded_p());

          if(!tmp->marked_p(object_memory_->mark())) {
            cleared++;
            *oi = NULL;
          }
        }
      }
    }


    // Now, calculate how much space we're still using.
    immix::Chunks& chunks = gc_.block_allocator().chunks();
    immix::AllBlockIterator iter(chunks);

    int live_bytes = 0;
    int total_bytes = 0;

    while(immix::Block* block = iter.next()) {
      total_bytes += immix::cBlockSize;
      live_bytes += block->bytes_from_lines();
    }

    double percentage_live = (double)live_bytes / (double)total_bytes;

    if(object_memory_->state->shared.config.gc_immix_debug) {
      std::cerr << "[GC IMMIX: " << clear_marked_objects() << " marked"
                << ", "
                << via_roots << " roots "
                << via_handles_ << " handles "
                << (int)(percentage_live * 100) << "% live"
                << ", " << live_bytes << "/" << total_bytes
                << "]\n";
    }

    if(percentage_live >= 0.90) {
      if(object_memory_->state->shared.config.gc_immix_debug) {
        std::cerr << "[GC IMMIX: expanding. "
                   << (int)(percentage_live * 100)
                   << "%]\n";
      }
      gc_.block_allocator().add_chunk();
    }

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

  void ImmixGC::check_finalize() {
    // If finalizers are running right now, just fixup any finalizer references
    if(object_memory_->running_finalizers()) {
      for(std::list<FinalizeObject>::iterator i = object_memory_->finalize().begin();
          i != object_memory_->finalize().end();
          i++) {
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

      switch(i->status) {
      case FinalizeObject::eLive:
        if(!i->object->marked_p(object_memory_->mark())) {
          // Run C finalizers now rather that queue them.
          if(i->finalizer) {
            (*i->finalizer)(state(), i->object);
            i->status = FinalizeObject::eFinalized;
            remove = true;
          } else {
            i->queued();
            object_memory_->to_finalize().push_back(&fi);

            // We have to still keep it alive though until we finish with it.
            i->object = saw_object(i->object);
          }
        } else {
          // Update the reference
          i->object = saw_object(i->object);
        }

        break;
      case FinalizeObject::eQueued:
        // Nothing, we haven't gotten to it yet.
        // Keep waiting and keep i->object updated.
        i->object = saw_object(i->object);
        i->queue_count++;
        break;
      case FinalizeObject::eFinalized:
        if(!i->object->marked_p(object_memory_->mark())) {
          // finalized and done with.
          remove = true;
        } else {
          // RESURRECTION!
          i->queued();
          i->object = saw_object(i->object);
        }
        break;
      }

      if(remove) {
        i = object_memory_->finalize().erase(i);
      } else {
        i++;
      }
    }
  }

}
