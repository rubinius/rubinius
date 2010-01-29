#include "gc/walker.hpp"
#include "objectmemory.hpp"

#include "capi/handle.hpp"

namespace rubinius {
  ObjectWalker::~ObjectWalker() {
    object_memory_->rotate_mark();
  }

  Object* ObjectWalker::saw_object(Object* obj) {
    int mark = object_memory_->mark();

    if(obj->reference_p()) {
      if(!obj->marked_p(mark)) {
        stack_.push_back(obj);
        obj->mark(mark);
      }
    }

    return obj;
  }

  void ObjectWalker::seed(GCData& data) {
    Object* tmp;
    ObjectArray *current_rs = object_memory_->remember_set();

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        ++oi) {
      tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp) saw_object(tmp);
    }

    for(Roots::Iterator i(data.roots()); i.more(); i.advance()) {
      saw_object(i->get());
    }

    if(data.threads()) {
      for(std::list<ManagedThread*>::iterator i = data.threads()->begin();
          i != data.threads()->end();
          i++) {
        for(Roots::Iterator ri((*i)->roots()); ri.more(); ri.advance()) {
          saw_object(ri->get());
        }
      }
    }

    for(capi::Handles::Iterator i(*data.handles()); i.more(); i.advance()) {
      saw_object(i->object());
    }

    for(capi::Handles::Iterator i(*data.cached_handles()); i.more(); i.advance()) {
      saw_object(i->object());
    }

    for(VariableRootBuffers::Iterator i(data.variable_buffers());
        i.more(); i.advance()) {
      Object*** buffer = i->buffer();
      for(int idx = 0; idx < i->size(); idx++) {
        Object** var = buffer[idx];
        Object* tmp = *var;

        saw_object(tmp);
      }
    }

    // Walk all the call frames
    for(CallFrameLocationList::iterator i = data.call_frames().begin();
        i != data.call_frames().end();
        i++) {
      CallFrame** loc = *i;
      walk_call_frame(*loc);
    }
  }

  Object* ObjectWalker::next() {
    if(stack_.size() == 0) return 0;

    Object* obj = stack_.back();
    stack_.pop_back();

    scan_object(obj);

    return obj;
  }
}
