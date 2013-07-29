#include "gc/walker.hpp"
#include "objectmemory.hpp"

#include "capi/handles.hpp"

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
    ObjectArray *current_rs = object_memory_->remember_set();

    for(ObjectArray::iterator oi = current_rs->begin();
        oi != current_rs->end();
        ++oi) {
      Object* tmp = *oi;
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
          ++i) {
        scan(*i, false);
      }
    }

    for(Allocator<capi::Handle>::Iterator i(data.handles()->allocator()); i.more(); i.advance()) {
      saw_object(i->object());
    }
  }

  Object* ObjectWalker::next() {
    if(stack_.empty()) return 0;

    Object* obj = stack_.back();
    stack_.pop_back();

    scan_object(obj);

    return obj;
  }

  size_t ObjectWalker::stack_size() {
    return stack_.size();
  }

  Object** ObjectWalker::stack_buf() {
    return stack_.data();
  }
}
