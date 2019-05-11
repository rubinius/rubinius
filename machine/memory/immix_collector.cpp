#include "memory.hpp"
#include "object_watch.hpp"
#include "configuration.hpp"
#include "logger.hpp"

#include "memory/immix_collector.hpp"

#include "capi/tag.hpp"

#include "diagnostics/gc.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/timing.hpp"

namespace rubinius {
namespace memory {
  void ImmixGC::ObjectDescriber::added_chunk(int count) {
    if(memory_) {
      // memory_->schedule_full_collection("Immix region chunk added");
      memory_->shared().memory_metrics()->immix_chunks++;

      if(gc_->dec_chunks_left() <= 0) {
        gc_->reset_chunks_left();
      }
    }
  }

  void ImmixGC::ObjectDescriber::set_forwarding_pointer(Address from, Address to) {
    from.as<Object>()->set_forwarded(to.as<Object>());
  }

  ImmixGC::ImmixGC(Memory* om)
    : GarbageCollector(om)
    , allocator_(gc_.block_allocator())
    , memory_(om)
    , chunks_left_(0)
    , chunks_before_collection_(10)
    , diagnostic_(new diagnostics::Immix())
  {
    gc_.describer().set_object_memory(om, this);
    reset_chunks_left();
  }

  ImmixGC::~ImmixGC() {
    if(diagnostic_) {
      delete diagnostic_;
      diagnostic_ = nullptr;
    }
  }

  Address ImmixGC::ObjectDescriber::copy(Address original, ImmixAllocator& alloc) {
    Object* orig = original.as<Object>();

    bool collect_flag = false;

    Address copy_addr = alloc.allocate(
        orig->size_in_bytes(memory_->vm()),
        collect_flag);

    if(collect_flag) {
      memory_->schedule_full_collection("Immix region copy object");
    }

    // TODO: MemoryHeader
    // Object* copy = copy_addr.as<Object>();
    // copy->initialize_full_state(memory_->vm(), orig, 0);

    return copy_addr;
  }

  int ImmixGC::ObjectDescriber::size(Address addr) {
    return addr.as<Object>()->size_in_bytes(memory_->vm());
  }

  Address ImmixGC::ObjectDescriber::update_pointer(Address addr) {
    Object* obj = addr.as<Object>();
    if(!obj) return Address::null();

    if(obj->forwarded_p()) return obj->forward();

    return addr;
  }

  bool ImmixGC::ObjectDescriber::mark_address(
      Address parent, Address child, MarkStack& ms, bool push)
  {
    Object* obj = child.as<Object>();

    if(obj->marked_p(memory_->mark())) return false;
    obj->set_marked(memory_->mark());

    if(push) ms.add(parent.as<Object>(), obj);

    return obj->region() != eLargeRegion;
  }

  Object* ImmixGC::allocate(size_t bytes, bool& collect_now) {
    if(bytes > cMaxObjectSize) return 0;

    return allocator_.allocate(bytes, collect_now).as<Object>();
  }

  Object* ImmixGC::move_object(Object* orig, size_t bytes, bool& collect_now) {
    if(bytes > cMaxObjectSize) return 0;

    Object* obj = allocator_.allocate(bytes, collect_now).as<Object>();

    memcpy(obj, orig, bytes);

    orig->set_forwarded(obj);

    return obj;
  }

  Object* ImmixGC::saw_object(void* parent, Object* child) {
#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(child)) {
      std::cout << "detected " << child << " during immix scanning.\n";
    }
#endif

    if(!child->reference_p()) return NULL;

    Address fwd = gc_.mark_address(parent, Address(child), allocator_);
    Object* copy = fwd.as<Object>();

    // Check and update an inflated header
    if(copy && copy != child) {
      child->set_forwarded(copy);
      return copy;
    }

    // Always return NULL for non moved objects
    return NULL;
  }

  void ImmixGC::scanned_object(Object* obj) {
    obj->set_scanned();
  }

  bool ImmixGC::mature_gc_in_progress() {
    return memory_->mature_gc_in_progress();
  }

  ObjectPosition ImmixGC::validate_object(Object* obj) {
    if(gc_.allocated_address(Address(obj))) {
      if(obj->region() != eLargeRegion) {
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
    for(auto i = data->references().begin();
        i != data->references().end();
        ++i)
    {
      if((*i)->reference_p()) {
        if((*i)->object_p()) {
          Object* obj = reinterpret_cast<Object*>(*i);

          if(obj->referenced() > 0) {
            if(Object* fwd = saw_object(0, obj)) {
              // TODO: MemoryHeader set new address
            }
          } else if(obj->memory_handle_p()) {
            MemoryHandle* handle = obj->extended_header()->get_handle();
            if(handle->accesses() > 0) {
              if(Object* fwd = saw_object(0, obj)) {
                // TODO: MemoryHeader set new address
              }

              handle->unset_accesses();
            }
          } else if(!obj->finalizer_p() && !obj->weakref_p()) {
            if(Object* fwd = saw_object(0, obj)) {
              // TODO: MemoryHeader set new address
            }
          }
        } else if((*i)->data_p()) {
          // TODO: MemoryHeader process data object
        }
      } else {
        // TODO: MemoryHeader if not reference, remove or log?
        // is it an error to add a non-reference?
      }
    }

    for(Roots::Iterator i(data->roots()); i.more(); i.advance()) {
      if(Object* fwd = saw_object(0, i->get())) {
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
        if(Object* fwd = saw_object(0, obj)) {
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
      for(auto i = data->references().begin();
          i != data->references().end();)
      {
        if((*i)->reference_p()) {
          if((*i)->object_p()) {
            Object* obj = reinterpret_cast<Object*>(*i);

            if(obj->memory_handle_p()) {
              MemoryHandle* handle = obj->extended_header()->get_handle();

              if(!obj->marked_p(data->mark())) {
                if(handle->cycles() < 3) {
                  if(Object* fwd = saw_object(0, obj)) {
                    // TODO: MemoryHeader set new address
                  }
                  handle->cycle();
                } else {
                  i = data->references().erase(i);
                  continue;
                }
              } else if(handle->rdata_p()) {
                if(obj->marked_p(data->mark())) {
                  scan_object(obj);
                }
              }
            }
          }
        }

        ++i;
      }

      /* TODO: MemoryHandle
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
      */
    } while(process_mark_stack());

    if(Collector* collector = memory_->collector()) {
      std::lock_guard<std::mutex> guard(collector->list_mutex());

      collector->gc_scan(this, memory_);
      process_mark_stack();

      collector->list_condition().notify_one();
    }

    // Clear unreachable objects from the various remember sets
    unsigned int mark = memory_->mark();
    memory_->unremember_objects(mark);
  }

  void ImmixGC::sweep(GCData* data) {
    // Copy marks for use in new allocations
    gc_.copy_marks();

    // Sweep up the garbage
    gc_.sweep_blocks();

    /* TODO: MemoryHeader add to diagnostics
    unsigned int references = 0;
    unsigned int handles = 0;
    unsigned int refd = 0;
    unsigned int weakref = 0;
    unsigned int finalizer = 0;
    unsigned int remembered = 0;
    */

    for(auto i = data->references().begin();
        i != data->references().end();)
    {
      if((*i)->reference_p()) {
        // references++;

        if((*i)->object_p()) {
          Object* obj = reinterpret_cast<Object*>(*i);

          if(obj->referenced() > 0) {
            if(!obj->marked_p(data->mark())) {
              std::cerr << "immix gc: sweep: ref'd object not marked" << std::endl;
              ::abort();
            }

            // refd++;
          } else if(obj->memory_handle_p()) {
            // handles++;
            MemoryHandle* handle = obj->extended_header()->get_handle();

            if(!obj->marked_p(data->mark())) {
              if(handle->accesses() > 0) {
                std::cerr << "immix gc: sweep: handle with refs not marked" << std::endl;
                ::abort();
              } else {
                i = data->references().erase(i);
                continue;
              }
            }
          } else if(obj->weakref_p()) {
            // weakref++;
            if(obj->marked_p(data->mark())) {
              TypeInfo* ti = memory_->type_info[obj->type_id()];

              // TODO: MemoryHeader get rid of this construct
              ObjectMark mark(this);
              ti->mark_weakref(obj, mark);
            } else {
              i = data->references().erase(i);
              continue;
            }
          } else if(obj->finalizer_p()) {
            // finalizer++;
            if(!obj->marked_p(data->mark())) {
              // TODO: MemoryHeader process finalizer
            }
          } else if(obj->remembered_p()) {
            // remembered++;
            i = data->references().erase(i);
            continue;
          }
        }
      } else {
        // TODO: MemoryHeader non-reference object in refs list
      }

      if(!(*i)->marked_p(data->mark())) {
        // TODO: MemoryHeader
        const MemoryHeader* h = (*i);
        std::cerr << "ref'd object not marked after sweep: bits: " << (h->extended_header_p() ? h->extended_header()->header : h->header.load()) << ", region: " << h->region() << std::endl;
      }

      ++i;
    }

    gc_.mark_stack().finish();

    /* TODO: MemoryHeader
    std::cerr <<
      "references: " << references << ", " <<
      "handles: " << handles << ", " <<
      "refd: " << refd << ", " <<
      "weakref: " << weakref << ", " <<
      "finalizer: " << finalizer << ", " <<
      "remembered: " << remembered << std::endl;
      */

    {
      timer::StopWatch<timer::microseconds> timer(
          vm()->shared.gc_metrics()->immix_diagnostics_us);

      // Now, calculate how much space we're still using.
      Chunks& chunks = gc_.block_allocator().chunks();
      AllBlockIterator iter(chunks);

      diagnostic()->chunks = chunks.size();

      while(Block* block = iter.next()) {
        diagnostic()->holes += block->holes();
        diagnostic()->objects += block->objects();
        diagnostic()->bytes += block->object_bytes();
        diagnostic()->total_bytes += cBlockSize;
      }

      diagnostic()->percentage =
        (double)diagnostic()->bytes / (double)diagnostic()->total_bytes;

      diagnostic()->collections++;

      if(memory_->shared().config.diagnostics_memory_enabled) {
        diagnostic()->update();
        memory_->shared().report_diagnostics(diagnostic());
      }
    }

    allocator_.restart(diagnostic()->percentage,
        diagnostic()->total_bytes - diagnostic()->bytes);
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
