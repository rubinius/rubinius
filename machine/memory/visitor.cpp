#include "config.h"
#include "vm.hpp"
#include "state.hpp"

#include "memory.hpp"

#include "memory/collector.hpp"
#include "memory/visitor.hpp"

namespace rubinius {
  namespace memory {
    void MemoryVisitor::visit_heap(STATE, std::function<void (STATE, Object**)> f) {
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
        std::function<void (STATE, Object**)> f)
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

      if(state->vm()->stack_limit_p(&object)) {
        mark_stack_.add(0, object);
        return;
      }

      object->set_scanned();

      visit_object(state, object->p_klass(), f);
      visit_object(state, object->p_ivars(), f);

      TypeInfo* ti = state->memory()->type_info[object->type_id()];

      ti->visit(state, object, [&](STATE, Object** obj){
          visit_object(state, obj, f);
        });
    }
  }
}
