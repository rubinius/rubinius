#include "memory.hpp"
#include "memory/immix_collector.hpp"
#include "memory/immix_marker.hpp"

#include "capi/handles.hpp"
#include "capi/tag.hpp"
#include "object_watch.hpp"

#include "configuration.hpp"

#include "instruments/timing.hpp"

#include "logger.hpp"

namespace rubinius {
namespace memory {
  ImmixGC::Diagnostics::Diagnostics()
    : diagnostics::MemoryDiagnostics()
    , collections_(0)
    , total_bytes_(0)
    , chunks_(0)
    , holes_(0)
    , percentage_(0.0)
  {
    set_type("ImmixCollector");

    document.AddMember("collections", collections_, document.GetAllocator());
    document.AddMember("total_bytes", total_bytes_, document.GetAllocator());
    document.AddMember("chunks", chunks_, document.GetAllocator());
    document.AddMember("holes", holes_, document.GetAllocator());
    document.AddMember("percentage", percentage_, document.GetAllocator());
  }


  void ImmixGC::Diagnostics::update() {
    document["objects"] = objects_;
    document["bytes"] = bytes_;
    document["collections"] = collections_;
    document["total_bytes"] = total_bytes_;
    document["chunks"] = chunks_;
    document["holes"] = holes_;
    document["percentage"] = percentage_;
  }

  void ImmixGC::ObjectDescriber::added_chunk(int count) {
    if(memory_) {
      // memory_->schedule_full_collection("Immix region chunk added");
      memory_->vm()->metrics().memory.immix_chunks++;

      if(gc_->dec_chunks_left() <= 0) {
        gc_->reset_chunks_left();
      }
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
    , diagnostics_(new Diagnostics())
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
        orig->size_in_bytes(memory_->vm()),
        collect_flag);

    if(collect_flag) {
      memory_->schedule_full_collection("Immix region copy object");
    }

    Object* copy = copy_addr.as<Object>();

    copy->initialize_full_state(memory_->vm(), orig, 0);

    copy->set_zone(MatureObjectZone);
    copy->set_in_immix();

    return copy_addr;
  }

  int ImmixGC::ObjectDescriber::size(Address addr) {
    return addr.as<Object>()->size_in_bytes(memory_->vm());
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
      memory_->remember_object(obj);
    }
    return addr;
  }

  bool ImmixGC::ObjectDescriber::mark_address(Address addr, MarkStack& ms, bool push) {
    Object* obj = addr.as<Object>();

    if(obj->marked_p(memory_->mark())) return false;
    obj->mark(memory_, memory_->mark());

    if(push) ms.push_back(addr);
    // If this is a young object, let the GC know not to try and mark
    // the block it's in.
    return obj->in_immix_p();
  }

  Object* ImmixGC::allocate(size_t bytes, bool& collect_now) {
    if(bytes > cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes, collect_now).as<Object>();
    if(likely(obj)) {
      obj->init_header(MatureObjectZone, InvalidType);
      obj->set_in_immix();
    }

    return obj;
  }

  Object* ImmixGC::move_object(Object* orig, size_t bytes, bool& collect_now) {
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
    return memory_->mature_gc_in_progress();
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
  }

  void ImmixGC::collect_scan(GCData* data) {
    for(Roots::Iterator i(data->roots()); i.more(); i.advance()) {
      if(Object* fwd = saw_object(i->get())) {
        i->set(fwd);
      }
    }

    {
      std::lock_guard<std::mutex> guard(data->thread_nexus()->threads_mutex());

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
  }

  void ImmixGC::collect_finish(GCData* data) {
    collect_scan(data);
    process_mark_stack();

    ObjectArray* marked_set = memory_->swap_marked_set();
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
          if(obj->marked_p(memory_->mark())) {
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

    if(FinalizerThread* finalizer = memory_->finalizer()) {
      std::lock_guard<std::mutex> guard(finalizer->list_mutex());

      finalizer->gc_scan(this, memory_);
      process_mark_stack();

      finalizer->list_condition().notify_one();
    }

    // Remove unreachable locked objects still in the list
    {
      std::lock_guard<std::mutex> guard(data->thread_nexus()->threads_mutex());

      for(ThreadList::iterator i = data->thread_nexus()->threads()->begin();
          i != data->thread_nexus()->threads()->end();
          ++i)
      {
        clean_locked_objects(*i, false);
      }
    }

    // Clear unreachable objects from the various remember sets
    unsigned int mark = memory_->mark();
    memory_->unremember_objects(mark);
  }

  void ImmixGC::sweep() {
    // Copy marks for use in new allocations
    gc_.copy_marks();

    // Sweep up the garbage
    gc_.sweep_blocks();

    {
      timer::StopWatch<timer::microseconds> timer(
          vm()->metrics().gc.immix_diagnostics_us);

      // Now, calculate how much space we're still using.
      Chunks& chunks = gc_.block_allocator().chunks();
      AllBlockIterator iter(chunks);

      diagnostics()->chunks_ = chunks.size();

      while(Block* block = iter.next()) {
        diagnostics()->holes_ += block->holes();
        diagnostics()->objects_ += block->objects();
        diagnostics()->bytes_ += block->object_bytes();
        diagnostics()->total_bytes_ += cBlockSize;
      }

      diagnostics()->percentage_ =
        (double)diagnostics()->bytes_ / (double)diagnostics()->total_bytes_;

      diagnostics()->collections_++;

      memory_->shared().report_diagnostics(diagnostics_);
    }

    allocator_.restart(diagnostics()->percentage_,
        diagnostics()->total_bytes_ - diagnostics()->bytes_);
  }

  void ImmixGC::start_marker(STATE, GCData* data) {
    if(!marker_) {
      marker_ = new ImmixMarker(state, this, data);
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
}
}
