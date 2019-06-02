#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/visitor.hpp"

namespace rubinius {
  namespace memory {
    size_t MemoryVisitor::invocation_frame_size = 200 * sizeof(uintptr_t);
    size_t MemoryVisitor::max_recursion_limit = 2000;

    void MemoryVisitor::set_recursion_limit(STATE) {
      uintptr_t stack_var = 1;

      recursion_limit_ = state->vm()->stack_remaining(state, &stack_var)
        / invocation_frame_size;

      if(recursion_limit_ > max_recursion_limit) {
        recursion_limit_ = max_recursion_limit;
      }
    }

    void MemoryVisitor::visit_heap(STATE, std::function<void (STATE, Object**)> f) {
      set_recursion_limit(state);

      for(Roots::Iterator i(state->globals().roots); i.more(); i.advance()) {
        Object* obj = i->get();
        visit_object(state, &obj, f);
        i->set(obj);
      }

      {
        std::lock_guard<std::mutex> guard(state->vm()->thread_nexus()->threads_mutex());

        for(ThreadList::iterator i = state->vm()->thread_nexus()->threads()->begin();
            i != state->vm()->thread_nexus()->threads()->end();
            ++i)
        {
          ManagedThread* thr = (*i);

          for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
            Object* obj = ri->get();
            visit_object(state, &obj, f);
            ri->set(obj);
          }

          VariableRootBuffer* vrb = thr->variable_root_buffers().front();

          while(vrb) {
            Object*** buffer = vrb->buffer();
            for(int idx = 0; idx < vrb->size(); idx++) {
              Object** var = buffer[idx];
              Object* cur = *var;

              if(cur->reference_p()) {
                visit_object(state, var, f);
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
                visit_object(state, &buffer[idx], f);
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

          visit_object(state, &obj, f);
        } else if(header->data_p()) {
          // DataHeader* data = reinterpret_cast<DataHeader*>(header);
          // TODO: process data (not C-API Data) instances
        }
      }

      visit_mark_stack(state, f);

      state->memory()->rotate_visit_mark();
    }

    void MemoryVisitor::visit_mark_stack(STATE, std::function<void (STATE, Object**)> f) {
      while(!mark_stack_.empty()) {
        Object* obj = mark_stack_.get();

        visit_object(state, &obj, f);
      }
    }

    void MemoryVisitor::visit_object(STATE, Object** obj,
        std::function<void (STATE, Object**)> f, size_t recursion_count)
    {
      Object* object = *obj;

      if(!object || !object->reference_p()) return;

      if(object->visited_p(state->memory()->visit_mark())) {
        if(object->scanned_p()) return;
      } else {
        object->set_visited(state->memory()->visit_mark());
        object->unset_scanned();

        f(state, obj);
      }

      if(!recurse_p(state, recursion_count)) {
        mark_stack_.add(0, object);
        return;
      }

      object->set_scanned();

      recursion_count++;

      visit_object(state, object->p_klass(), f, recursion_count);
      visit_object(state, object->p_ivars(), f, recursion_count);

      TypeInfo* ti = state->memory()->type_info[object->type_id()];

      ti->visit(state, object, [&](STATE, Object** obj){
          visit_object(state, obj, f, recursion_count);
        });
    }
  }
}
