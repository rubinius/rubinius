#include "gc/walker.hpp"
#include "objectmemory.hpp"

#include "capi/handle.hpp"

namespace rubinius {
  ObjectWalker::~ObjectWalker() {
  }

  Object* ObjectWalker::saw_object(Object* obj) {
    if(obj->reference_p()) {
      std::map<Object*,bool>::iterator i = mark_bits_.find(obj);
      if(i == mark_bits_.end()) {
        stack_.push_back(obj);
        mark_bits_[obj] = true;
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

    for(std::list<gc::WriteBarrier*>::iterator wbi = object_memory_->aux_barriers().begin();
        wbi != object_memory_->aux_barriers().end();
        wbi++) {
      gc::WriteBarrier* wb = *wbi;
      ObjectArray* rs = wb->remember_set();
      for(ObjectArray::iterator oi = rs->begin();
          oi != rs->end();
          oi++) {
        tmp = *oi;

        if(tmp) saw_object(tmp);
      }
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
