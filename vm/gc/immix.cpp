#include "objectmemory.hpp"
#include "gc/immix.hpp"
#include "gc/immix_marker.hpp"

#include "capi/handles.hpp"
#include "capi/tag.hpp"
#include "object_watch.hpp"

#include "configuration.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {
  void ImmixGC::ObjectDescriber::added_chunk(int count) {
#ifdef IMMIX_DEBUG
    std::cout << "[GC IMMIX: Added a chunk: " << count << "]\n";
#endif

    if(object_memory_) {
      if(gc_->dec_chunks_left() <= 0) {
        // object_memory_->collect_mature_now = true;
        gc_->reset_chunks_left();
      }
    }
  }


  /**
   * This means we're getting low on memory! Time to schedule a garbage
   * collection.
   */
  void ImmixGC::ObjectDescriber::last_block() {
    if(object_memory_) {
      object_memory_->collect_mature_now = true;
    }
  }

  void ImmixGC::ObjectDescriber::set_forwarding_pointer(memory::Address from, memory::Address to) {
    from.as<Object>()->set_forward(to.as<Object>());
  }

  ImmixGC::ImmixGC(ObjectMemory* om)
    : GarbageCollector(om)
    , allocator_(gc_.block_allocator())
    , marker_(NULL)
    , marked_objects_(0)
    , chunks_left_(0)
    , chunks_before_collection_(10)
  {
    gc_.describer().set_object_memory(om, this);
    reset_chunks_left();
  }

  ImmixGC::~ImmixGC() {
    if(marker_) {
      delete marker_;
    }
  }

  memory::Address ImmixGC::ObjectDescriber::copy(memory::Address original,
      immix::Allocator& alloc) {
    Object* orig = original.as<Object>();

    memory::Address copy_addr = alloc.allocate(
        orig->size_in_bytes(object_memory_->state()));

    Object* copy = copy_addr.as<Object>();

    copy->initialize_full_state(object_memory_->state(), orig, 0);

    copy->set_zone(MatureObjectZone);
    copy->set_in_immix();

    return copy_addr;
  }

  int ImmixGC::ObjectDescriber::size(memory::Address addr) {
    return addr.as<Object>()->size_in_bytes(object_memory_->state());
  }

  Object* ImmixGC::allocate(uint32_t bytes) {
    if(bytes > immix::cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes).as<Object>();
    obj->init_header(MatureObjectZone, InvalidType);
    obj->set_in_immix();

    return obj;
  }

  Object* ImmixGC::move_object(Object* orig, uint32_t bytes) {
    if(bytes > immix::cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes).as<Object>();

    memcpy(obj, orig, bytes);

    obj->set_zone(MatureObjectZone);
    obj->set_in_immix();

    orig->set_forward(obj);

    return obj;
  }

  Object* ImmixGC::saw_object(Object* obj) {
#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during immix scanning.\n";
    }
#endif

    if(!obj->reference_p()) return obj;

    memory::Address fwd = gc_.mark_address(memory::Address(obj), allocator_);
    Object* copy = fwd.as<Object>();

    // Check and update an inflated header
    if(copy && copy != obj) {
      obj->set_forward(copy);
    }

    return copy;
  }

  void ImmixGC::scanned_object(Object* obj) {
    obj->scanned();
  }

  ObjectPosition ImmixGC::validate_object(Object* obj) {
    if(gc_.allocated_address(memory::Address(obj))) {
      if(obj->in_immix_p()) {
        return cInImmix;
      } else {
        return cInImmixCorruptHeader;
      }
    }

    return cUnknown;
  }

  /**
   * Performs a garbage collection of the immix space.
   */
  void ImmixGC::collect(GCData* data) {
    gc_.clear_marks();
    collect_scan(data);
    process_mark_stack();
    collect_finish(data);
  }

  void ImmixGC::collect_start(GCData* data) {
    gc_.clear_marks();
    collect_scan(data);
    marker_->concurrent_mark(data);
  }

  void ImmixGC::collect_scan(GCData* data) {
    for(Roots::Iterator i(data->roots()); i.more(); i.advance()) {
      Object* tmp = i->get();
      if(tmp->reference_p()) saw_object(tmp);
    }

    if(data->threads()) {
      for(std::list<ManagedThread*>::iterator i = data->threads()->begin();
          i != data->threads()->end();
          ++i) {
        scan(*i, false);
      }
    }

    for(Allocator<capi::Handle>::Iterator i(data->handles()->allocator()); i.more(); i.advance()) {
      if(i->in_use_p() && !i->weak_p()) {
        saw_object(i->object());
      }
    }

    std::list<capi::GlobalHandle*>* gh = data->global_handle_locations();

    if(gh) {
      for(std::list<capi::GlobalHandle*>::iterator i = gh->begin();
          i != gh->end();
          ++i) {
        capi::Handle** loc = (*i)->handle();
        if(capi::Handle* hdl = *loc) {
          if(!REFERENCE_P(hdl)) continue;
          if(hdl->valid_p()) {
            Object* obj = hdl->object();
            if(obj && obj->reference_p()) {
              saw_object(obj);
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
  }

  void ImmixGC::collect_finish(GCData* data) {
    collect_scan(data);

    ObjectArray* marked_set = object_memory_->swap_marked_set();
    for(ObjectArray::iterator oi = marked_set->begin();
        oi != marked_set->end();
        ++oi) {
      Object* obj = *oi;
      if(obj) saw_object(obj);
    }
    delete marked_set;

    // Users manipulate values accessible from the data* within an
    // RData without running a write barrier. Thusly if we see any rdata
    // we must always scan it again here because it could contain new pointers.
    //
    // We do this in a loop because the scanning might generate new entries
    // on the mark stack.
    do {
      for(Allocator<capi::Handle>::Iterator i(data->handles()->allocator()); i.more(); i.advance()) {
        capi::Handle* hdl = i.current();
        if(!hdl->in_use_p()) continue;
        if(hdl->is_rdata()) {
          Object* obj = hdl->object();
          if(obj->marked_p(object_memory_->mark())) {
            scan_object(obj);
          }
        }
      }
    } while(process_mark_stack());

    // We've now finished marking the entire object graph.
    // Clean weakrefs before keeping additional objects alive
    // for finalization, so people don't get a hold of finalized
    // objects through weakrefs.
    clean_weakrefs();

    // Marking objects to be Finalized can cause more things to continue to
    // live, so we must check the mark_stack again.
    do {
      walk_finalizers();
      scan_fibers(data, true);
    } while(process_mark_stack());

    // Remove unreachable locked objects still in the list
    if(data->threads()) {
      for(std::list<ManagedThread*>::iterator i = data->threads()->begin();
          i != data->threads()->end();
          ++i) {
        clean_locked_objects(*i, false);
      }
    }

    // Clear unreachable objects from the various remember sets
    unsigned int mark = object_memory_->mark();
    object_memory_->unremember_objects(mark);
  }

  void ImmixGC::sweep() {
    // Copy marks for use in new allocations
    gc_.copy_marks();

    // Sweep up the garbage
    gc_.sweep_blocks();

    // This resets the allocator state to sync it up with the BlockAllocator
    // properly.
    allocator_.get_new_block();

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

    if(object_memory_->state()->shared.config.gc_immix_debug) {
      std::cerr << "[GC IMMIX: " << clear_marked_objects() << " marked"
                << ", "
                << (int)(percentage_live * 100) << "% live"
                << ", " << live_bytes << "/" << total_bytes
                << "]\n";
    }

    if(percentage_live >= 0.90) {
      if(object_memory_->state()->shared.config.gc_immix_debug) {
        std::cerr << "[GC IMMIX: expanding. "
                   << (int)(percentage_live * 100)
                   << "%]\n";
      }
      gc_.block_allocator().add_chunk();
    }

#ifdef IMMIX_DEBUG
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

    delete[] holes;
    holes = NULL;
#endif
  }

  void ImmixGC::start_marker(STATE) {
    if(!marker_) {
      marker_ = new ImmixMarker(state, this);
    }
  }

  void ImmixGC::wait_for_marker(STATE) {
    if(marker_) {
      marker_->wait_for_marker(state);
    }
  }

  bool ImmixGC::process_mark_stack(int count) {
    return gc_.process_mark_stack(allocator_, count);
  }

  immix::MarkStack& ImmixGC::mark_stack() {
    return gc_.mark_stack();
  }

  void ImmixGC::walk_finalizers() {
    FinalizerHandler* fh = object_memory_->finalizer_handler();
    if(!fh) return;

    for(FinalizerHandler::iterator i = fh->begin();
        !i.end();
        /* advance is handled in the loop */)
    {
      FinalizeObject& fi = i.current();

      bool live = fi.object->marked_p(object_memory_->mark());

      if(fi.ruby_finalizer) {
        fi.ruby_finalizer = saw_object(fi.ruby_finalizer);
      }

      fi.object = saw_object(fi.object);

      i.next(live);
    }
  }
}
