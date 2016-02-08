#include "memory.hpp"
#include "memory/immix_collector.hpp"
#include "memory/immix_marker.hpp"

#include "capi/handles.hpp"
#include "capi/tag.hpp"
#include "object_watch.hpp"

#include "configuration.hpp"

#include "instruments/timing.hpp"

#include "util/logger.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {
namespace memory {
  void ImmixGC::Diagnostics::log() {
    if(!modified_p()) return;

    diagnostics::Diagnostics::log();

    utilities::logger::write("immix: diagnostics: " \
        "collections: %ld, " \
        "objects: %ld, " \
        "bytes: %ld, " \
        "total_bytes: %ld, " \
        "chunks: %ld, " \
        "holes: %ld, " \
        "percentage: %f",
        collections_, objects_, bytes_, total_bytes_,
        chunks_, holes_, percentage_);
  }

  void ImmixGC::ObjectDescriber::added_chunk(int count) {
    if(object_memory_) {
      object_memory_->schedule_full_collection();
      object_memory_->vm()->metrics().memory.immix_chunks++;

      if(gc_->dec_chunks_left() <= 0) {
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
      object_memory_->schedule_full_collection();
    }
  }

  void ImmixGC::ObjectDescriber::set_forwarding_pointer(Address from, Address to) {
    from.as<Object>()->set_forward(to.as<Object>());
  }

  ImmixGC::ImmixGC(Memory* om)
    : GarbageCollector(om)
    , allocator_(gc_.block_allocator())
    , memory_(om)
    , marker_(NULL)
    , chunks_left_(0)
    , chunks_before_collection_(10)
    , diagnostics_(Diagnostics())
  {
    gc_.describer().set_object_memory(om, this);
    reset_chunks_left();
  }

  ImmixGC::~ImmixGC() {
    if(marker_) {
      delete marker_;
    }
  }

  Address ImmixGC::ObjectDescriber::copy(Address original,
      ImmixAllocator& alloc) {
    Object* orig = original.as<Object>();

    bool collect_flag = false;

    Address copy_addr = alloc.allocate(
        orig->size_in_bytes(object_memory_->vm()),
        collect_flag);

    if(collect_flag) {
      object_memory_->schedule_full_collection();
    }

    Object* copy = copy_addr.as<Object>();

    copy->initialize_full_state(object_memory_->vm(), orig, 0);

    copy->set_zone(MatureObjectZone);
    copy->set_in_immix();

    return copy_addr;
  }

  int ImmixGC::ObjectDescriber::size(Address addr) {
    return addr.as<Object>()->size_in_bytes(object_memory_->vm());
  }

  Address ImmixGC::ObjectDescriber::update_pointer(Address addr) {
    Object* obj = addr.as<Object>();
    if(!obj) return Address::null();
    if(obj->young_object_p()) {
      if(obj->forwarded_p()) return obj->forward();
      return Address::null();
    } else {
      // we must remember this because it might
      // contain references to young gen objects
      object_memory_->remember_object(obj);
    }
    return addr;
  }

  bool ImmixGC::ObjectDescriber::mark_address(Address addr, MarkStack& ms, bool push) {
    Object* obj = addr.as<Object>();

    if(!object_memory_->valid_object_p(obj)) {
      std::cerr << "mark_address: invalid object" << std::endl;
      ::abort();
    }

    if(obj->marked_p(object_memory_->mark())) return false;
    obj->mark(object_memory_, object_memory_->mark());

    if(push) ms.push_back(addr);
    // If this is a young object, let the GC know not to try and mark
    // the block it's in.
    return obj->in_immix_p();
  }

  Object* ImmixGC::allocate(uint32_t bytes, bool& collect_now) {
    if(bytes > cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes, collect_now).as<Object>();
    obj->init_header(MatureObjectZone, InvalidType);
    obj->set_in_immix();

    return obj;
  }

  Object* ImmixGC::move_object(Object* orig, uint32_t bytes, bool& collect_now) {
    if(bytes > cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes, collect_now).as<Object>();

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

    if(!obj->reference_p()) return NULL;

    Address fwd = gc_.mark_address(Address(obj), allocator_);
    Object* copy = fwd.as<Object>();

    // Check and update an inflated header
    if(copy && copy != obj) {
      obj->set_forward(copy);
      return copy;
    }

    // Always return NULL for non moved objects
    return NULL;
  }

  void ImmixGC::scanned_object(Object* obj) {
    obj->scanned();
  }

  bool ImmixGC::mature_gc_in_progress() {
    return object_memory_->mature_gc_in_progress();
  }

  ObjectPosition ImmixGC::validate_object(Object* obj) {
    if(gc_.allocated_address(Address(obj))) {
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
  }

  void ImmixGC::collect_start(GCData* data) {
    gc_.clear_marks();
    collect_scan(data);
    marker_->concurrent_mark(data);
  }

  void ImmixGC::collect_scan(GCData* data) {
    for(Roots::Iterator i(data->roots()); i.more(); i.advance()) {
      if(Object* fwd = saw_object(i->get())) {
        i->set(fwd);
      }
    }

    {
      utilities::thread::SpinLock::LockGuard guard(data->thread_nexus()->threads_lock());

      for(ThreadList::iterator i = data->thread_nexus()->threads()->begin();
          i != data->thread_nexus()->threads()->end();
          ++i)
      {
        scan(*i, false);
      }
    }

    for(Allocator<capi::Handle>::Iterator i(data->handles()->allocator()); i.more(); i.advance()) {
      if(i->in_use_p() && !i->weak_p()) {
        if(Object* fwd = saw_object(i->object())) {
          i->set_object(fwd);
        }
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
  }

  void ImmixGC::collect_finish(GCData* data) {
    collect_scan(data);
    process_mark_stack();

    ObjectArray* marked_set = object_memory_->swap_marked_set();
    for(ObjectArray::iterator oi = marked_set->begin();
        oi != marked_set->end();
        ++oi) {
      Object* obj = *oi;
      if(obj) {
        if(Object* fwd = saw_object(obj)) {
          *oi = fwd;
        }
      }
    }
    delete marked_set;

    // Users manipulate values accessible from the data* within an
    // RData without running a write barrier. Thusly if we see any rdata
    // we must always scan it again here because it could contain new pointers.
    //
    // We do this in a loop because the scanning might generate new entries
    // on the mark stack.
    do {
      for(Allocator<capi::Handle>::Iterator i(data->handles()->allocator());
          i.more();
          i.advance())
      {
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
    {
      utilities::thread::SpinLock::LockGuard guard(data->thread_nexus()->threads_lock());

      for(ThreadList::iterator i = data->thread_nexus()->threads()->begin();
          i != data->thread_nexus()->threads()->end();
          ++i)
      {
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

    {
      timer::StopWatch<timer::microseconds> timer(
          vm()->metrics().gc.immix_diagnostics_us);

      diagnostics_ = Diagnostics(diagnostics_.collections_);

      // Now, calculate how much space we're still using.
      Chunks& chunks = gc_.block_allocator().chunks();
      AllBlockIterator iter(chunks);

      diagnostics_.chunks_ = chunks.size();

      while(Block* block = iter.next()) {
        diagnostics_.holes_ += block->holes();
        diagnostics_.objects_ += block->objects();
        diagnostics_.bytes_ += block->object_bytes();
        diagnostics_.total_bytes_ += cBlockSize;
      }

      diagnostics_.percentage_ =
        (double)diagnostics_.bytes_ / (double)diagnostics_.total_bytes_;

      diagnostics_.collections_++;
      diagnostics_.modify();
    }

    if(diagnostics_.percentage_ >= 0.90) {
      gc_.block_allocator().add_chunk();
    }
  }

  void ImmixGC::start_marker(STATE) {
    if(!marker_) {
      marker_ = new ImmixMarker(state, this);
    }
  }

  bool ImmixGC::process_mark_stack() {
    bool exit = false;
    return gc_.process_mark_stack(allocator_, exit);
  }

  bool ImmixGC::process_mark_stack(bool& exit) {
    return gc_.process_mark_stack(allocator_, exit);
  }

  MarkStack& ImmixGC::mark_stack() {
    return gc_.mark_stack();
  }

  void ImmixGC::walk_finalizers() {
    FinalizerThread* fh = object_memory_->finalizer_handler();
    if(!fh) return;

    for(FinalizerThread::iterator i = fh->begin();
        !i.end();
        /* advance is handled in the loop */)
    {
      FinalizeObject& fi = i.current();

      bool live = fi.object->marked_p(object_memory_->mark());

      if(fi.ruby_finalizer) {
        if(Object* fwd = saw_object(fi.ruby_finalizer)) {
          fi.ruby_finalizer = fwd;
        }
      }

      if(Object* fwd = saw_object(fi.object)) {
        fi.object = fwd;
      }

      i.next(live);
    }
  }
}
}
