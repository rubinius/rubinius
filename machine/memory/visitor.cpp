#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/address.hpp"
#include "memory/collector.hpp"
#include "memory/visitor.hpp"

namespace rubinius {
  namespace memory {
    void MemoryVisitor::visit_heap(STATE, std::function<void (STATE, Object*)> f) {
      for(Roots::Iterator i(state->globals().roots); i.more(); i.advance()) {
        visit_object(state, i->get(), f);
      }

      {
        std::lock_guard<std::mutex> guard(state->vm()->thread_nexus()->threads_mutex());

        for(ThreadList::iterator i = state->vm()->thread_nexus()->threads()->begin();
            i != state->vm()->thread_nexus()->threads()->end();
            ++i)
        {
          ManagedThread* thr = (*i);

          for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
            visit_object(state, ri->get(), f);
          }

          VariableRootBuffer* vrb = thr->variable_root_buffers().front();

          while(vrb) {
            Object*** buffer = vrb->buffer();
            for(int idx = 0; idx < vrb->size(); idx++) {
              Object** var = buffer[idx];
              Object* cur = *var;

              if(!cur) continue;

              if(cur->reference_p()) {
                visit_object(state, cur, f);
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
            }

            // vrb = displace((VariableRootBuffer*)vrb->next(), offset);
            vrb = (VariableRootBuffer*)vrb->next();
          }

          for(RootBuffers::Iterator i(thr->root_buffers());
              i.more();
              i.advance())
          {
            Object** buffer = i->buffer();
            for(int idx = 0; idx < i->size(); idx++) {
              Object* cur = buffer[idx];

              if(cur->reference_p()) {
                visit_object(state, cur, f);
              } else if(cur->handle_p()) {
                // TODO: MemoryHeader mark MemoryHandle objects
              }
            }
          }

          if(VM* vm = thr->as_vm()) {
            vm->visit_objects(state, f);
          }
        }
      }

      for(auto i = state->collector()->references().begin();
          i != state->collector()->references().end();
          ++i)
      {
        MemoryHeader* header = reinterpret_cast<MemoryHeader*>(*i);

        if(header->object_p()) {
          Object* obj = reinterpret_cast<Object*>(header);

          visit_object(state, obj, f);
        } else if(header->data_p()) {
          // DataHeader* data = reinterpret_cast<DataHeader*>(header);
          // TODO: process data (not C-API Data) instances
        }
      }

      visit_mark_stack(state, f);

      state->memory()->rotate_visit_mark();
    }

    void MemoryVisitor::visit_mark_stack(STATE, std::function<void (STATE, Object*)> f) {
      while(!mark_stack_.empty()) {
#ifdef RBX_GC_STACK_CHECK
        MarkStackEntry& entry = mark_stack_.get();
        Address addr = entry.child();
#else
        Address addr = mark_stack_.get();
#endif
        scan_object(state, addr.as<Object>(), f);
      }
    }

    void MemoryVisitor::scan_object(STATE, Object* obj, std::function<void (STATE, Object*)> f) {
      if(obj->scanned_p()) return;

      obj->set_scanned();

      f(state, obj);

      visit_object(state, obj->klass(), f);

      if(obj->ivars()->reference_p()) {
        visit_object(state, obj->ivars(), f);
      }

      TypeInfo* ti = state->memory()->type_info[obj->type_id()];

      ti->visit_object(state, obj, [this, f](STATE, Object* obj){
          return visit_object(state, obj, f);
        });
    }

    void MemoryVisitor::visit_object(STATE,
        Object* obj, std::function<void (STATE, Object*)> f)
    {
      if(!obj || !obj->reference_p()) return;

      if(obj->visited_p(state->memory()->visit_mark())) return;

      // Set the mark bits in the managed memory instance
      obj->unset_scanned();
      obj->set_visited(state->memory()->visit_mark());

      f(state, obj);

      mark_stack_.add(0, obj);
    }
  }
}
