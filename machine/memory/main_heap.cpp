#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/immix_collector.hpp"
#include "memory/main_heap.hpp"
#include "memory/mark_sweep.hpp"

#include "diagnostics/gc.hpp"

namespace rubinius {
  namespace memory {
    void MainHeap::collect_start(STATE, GCData* data) {
      state->memory()->collect_cycle();

      code_manager_.clear_marks();
      immix_->reset_stats();

      immix_->collect(data);
    }

    void MainHeap::collect_references(STATE, std::function<Object* (STATE, void*, Object*)> f) {
      for(Roots::Iterator i(state->globals().roots); i.more(); i.advance()) {
        if(Object* fwd = f(state, 0, i->get())) {
          i->set(fwd);
        }
      }

      {
        std::lock_guard<std::mutex> guard(state->vm()->thread_nexus()->threads_mutex());

        for(ThreadList::iterator i = state->vm()->thread_nexus()->threads()->begin();
            i != state->vm()->thread_nexus()->threads()->end();
            ++i)
        {
          ManagedThread* thr = (*i);

          for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
            if(Object* fwd = f(state, 0, ri->get())) {
              ri->set(fwd);
            }
          }

          //scan(thr->variable_root_buffers(), young_only);
          // VariableRootBuffer* vrb = displace(thr->variable_root_buffers().front(), offset);
          VariableRootBuffer* vrb = thr->variable_root_buffers().front();

          while(vrb) {
            Object*** buffer = vrb->buffer();
            for(int idx = 0; idx < vrb->size(); idx++) {
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
              Object* cur = buffer[idx];

              if(cur->reference_p()) {
                if(Object* tmp = f(state, 0, cur)) {
                  buffer[idx] = tmp;
                }
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
            }
          }

          if(VM* vm = thr->as_vm()) {
            vm->gc_scan(state, f);
          }

          // scan(*i, false);
        }
      }

      // TODO: GC: maybe move this into tracer
      for(auto i = state->collector()->references().begin();
          i != state->collector()->references().end();
          ++i)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(header->referenced() > 0) {
          if(header->memory_handle_p()) {
            MemoryHandle* handle = header->extended_header()->get_handle();

            if(!header->marked_p(state->memory()->mark())) {
              if(handle->cycles() < 3) {
                if(header->object_p()) {
                  Object* obj = reinterpret_cast<Object*>(header);

                  if(Object* fwd = f(state, 0, obj)) {
                    // TODO: MemoryHeader set new address
                  }
                } else if(header->data_p()) {
                  // DataHeader* data = reinterpret_cast<DataHeader*>(header);
                  // TODO: process data (not C-API Data) instances
                }

                handle->cycle();
              } else if(handle->rdata_p()) {
                Object* obj = reinterpret_cast<Object*>(header);

                if(Object* fwd = f(state, 0, obj)) {
                  // TODO: MemoryHeader set new address
                }
              }
            }
          } else if(header->object_p()) {
            Object* obj = reinterpret_cast<Object*>(header);

            if(Object* fwd = f(state, 0, obj)) {
              // TODO: MemoryHeader set new address
            }
          } else if(header->data_p()) {
            // DataHeader* data = reinterpret_cast<DataHeader*>(header);
            // TODO: process data (not C-API Data) instances
          }
        }
      }
    }

    void MainHeap::collect_finish(STATE, GCData* data) {
      immix_->collect_finish(data);

      code_manager_.sweep();
      immix_->sweep(state, data);
      mark_sweep_->sweep();

      state->shared().symbols.sweep(state);

      state->memory()->rotate_mark();

      diagnostics::GCMetrics* metrics = state->shared().gc_metrics();
      metrics->immix_count++;
      metrics->large_count++;
    }
  }
}
