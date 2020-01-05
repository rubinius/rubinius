#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/immix.hpp"
#include "memory/third_region.hpp"
#include "memory/main_heap.hpp"

#include "diagnostics/collector.hpp"

namespace rubinius {
  namespace memory {
    MainHeap::MainHeap(STATE, CodeManager& cm)
      : Heap()
      , first_region_(new Immix(state))
      , third_region_(new LargeRegion(state))
      , code_manager_(cm)
    {
    }

    MainHeap::~MainHeap() {
      delete third_region_;
      third_region_ = nullptr;

      delete first_region_;
      first_region_ = nullptr;
    }

    Object* MainHeap::allocate(STATE, intptr_t bytes, object_type type) {
      utilities::thread::SpinLock::LockGuard guard(state->memory()->allocation_lock());

      if(Object* obj = first_region()->allocate(state, bytes)) {
        state->shared().memory_metrics()->first_region_objects++;
        state->shared().memory_metrics()->first_region_bytes += bytes;

        MemoryHeader::initialize(
            obj, state->vm()->thread_id(), eFirstRegion, type, false);

        return obj;
      }

      if(Object* obj = third_region()->allocate(state, bytes)) {
        state->shared().memory_metrics()->large_objects++;
        state->shared().memory_metrics()->large_bytes += bytes;

        MemoryHeader::initialize(
            obj, state->vm()->thread_id(), eThirdRegion, type, false);

        return obj;
      }

      Memory::memory_error(state);
      return nullptr;
    }

    void MainHeap::collect_start(STATE) {
      state->memory()->collect_cycle();

      code_manager_.clear_marks();

      first_region_->clear_marks();
    }

    void MainHeap::collect_references(STATE, std::function<void (STATE, Object**)> f) {
      for(Roots::Iterator i(state->globals().roots); i.more(); i.advance()) {
        Object* fwd = i->get();
        f(state, &fwd);
        i->set(fwd);
      }

      {
        std::lock_guard<std::mutex> guard(state->vm()->thread_nexus()->threads_mutex());

        for(ThreadList::iterator i = state->vm()->thread_nexus()->threads()->begin();
            i != state->vm()->thread_nexus()->threads()->end();
            ++i)
        {
          ManagedThread* thr = (*i);

          for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
            Object* fwd = ri->get();
            f(state, &fwd);
            ri->set(fwd);
          }

          //scan(thr->variable_root_buffers(), young_only);
          // VariableRootBuffer* vrb = displace(thr->variable_root_buffers().front(), offset);
          VariableRootBuffer* vrb = thr->variable_root_buffers().front();

          while(vrb) {
            Object*** buffer = vrb->buffer();
            for(int idx = 0; idx < vrb->size(); idx++) {
              f(state, buffer[idx]);

              /* TODO: GC
              Object** var = buffer[idx];
              Object* cur = *var;

              if(!cur) continue;

              if(cur->reference_p()) {
                if(Object* tmp = f(state, 0, cur)) {
                  *var = tmp;
                }
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
              */
            }

            // vrb = displace((VariableRootBuffer*)vrb->next(), offset);
            vrb = (VariableRootBuffer*)vrb->next();
          }

          // scan(thr->root_buffers(), young_only);

          for(RootBuffers::Iterator i(thr->root_buffers());
              i.more();
              i.advance())
          {
            Object** buffer = i->buffer();
            for(int idx = 0; idx < i->size(); idx++) {
              f(state, &buffer[idx]);

              /* TODO: GC
              Object* cur = buffer[idx];

              if(cur->reference_p()) {
                if(Object* tmp = f(state, 0, cur)) {
                  buffer[idx] = tmp;
                }
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
              */
            }
          }

          if(VM* vm = thr->as_vm()) {
            vm->gc_scan(state, f);
          }
        }
      }

      // TODO: GC: maybe move this into tracer
      for(auto i = state->collector()->references().begin();
          i != state->collector()->references().end();
          ++i)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(header->referenced() > 0) {
          if(header->object_p()) {
            Object* obj = reinterpret_cast<Object*>(header);

            // TODO: GC update the data structure
            f(state, &obj);
          } else if(header->data_p()) {
            // DataHeader* data = reinterpret_cast<DataHeader*>(header);
            // TODO: process data (not C-API Data) instances
          }
        }
      }
    }

    void MainHeap::collect_finish(STATE) {
      code_manager_.sweep();

      first_region_->sweep(state);
      third_region_->sweep(state);

      state->shared().symbols.sweep(state);

      state->memory()->rotate_mark();

      diagnostics::CollectorMetrics* metrics = state->shared().collector_metrics();
      metrics->first_region_count++;
      metrics->large_count++;
    }
  }
}
