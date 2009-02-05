/* The GC superclass methods, used by both GCs. */
#include "object_utils.hpp"
#include "gc.hpp"
#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/tuple.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/compiledmethod.hpp"
#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"

namespace rubinius {

  ObjectMark::ObjectMark(GarbageCollector* gc) : gc(gc) { }

  Object* ObjectMark::call(Object* obj) {
    if(obj->reference_p()) {
      if(obj->zone == UnspecifiedZone) {
        std::cout << "USZ!\n";
        char* bad = (char*)0;
        if(*bad) exit(11);
      }
      sassert(obj->zone != UnspecifiedZone);
      return gc->saw_object(obj);
    }

    return NULL;
  }

  void ObjectMark::set(Object* target, Object** pos, Object* val) {
    *pos = val;
    if(val->reference_p()) {
      gc->object_memory->write_barrier(target, val);
    }
  }

  void ObjectMark::just_set(Object* target, Object* val) {
    if(val->reference_p()) {
      gc->object_memory->write_barrier(target, val);
    }
  }

  GarbageCollector::GarbageCollector(ObjectMemory *om)
                   :object_memory(om), weak_refs(NULL) { }

  /* Understands how to read the inside of an object and find all references
   * located within. It copies the objects pointed to, but does not follow into
   * those further (ie, not recursive) */
  void GarbageCollector::scan_object(Object* obj) {
    Object* slot;

    // If this object's refs are weak, then add it to the weak_refs
    // vector and don't look at it otherwise.
    if(obj->RefsAreWeak) {
      if(!weak_refs) {
        weak_refs = new ObjectArray(0);
      }

      weak_refs->push_back(obj);
      return;
    }

    if(obj->klass() && obj->klass()->reference_p()) {
      slot = saw_object(obj->klass());
      if(slot) object_memory->set_class(obj, slot);
    }

    if(obj->ivars() && obj->ivars()->reference_p()) {
      slot = saw_object(obj->ivars());
      if(slot) obj->ivars(object_memory->state, slot);
    }

    TypeInfo* ti = object_memory->type_info[obj->obj_type];
    assert(ti);

    ObjectMark mark(this);
    ti->mark(obj, mark);
  }

  void GarbageCollector::delete_object(Object* obj) {
    if (obj->RequiresCleanup) {
      object_memory->find_type_info(obj)->cleanup(obj);
    }

    if(obj->Remember) {
      object_memory->unremember_object(obj);
    }
  }

  void GarbageCollector::saw_variable_scope(VariableScope* scope) {
    scope->update(saw_object(scope->self()),
                  saw_object(scope->module()),
                  saw_object(scope->block()));

    for(int i = 0; i < scope->number_of_locals(); i++) {
      scope->set_local(i, saw_object(scope->get_local(i)));
    }

    VariableScope* parent = scope->parent();
    if(parent && parent->reference_p()) {
      if(parent->stack_allocated_p()) {
        saw_variable_scope(parent);
      } else {
        scope->update_parent((VariableScope*)saw_object(parent));
      }
    }
  }

  void GarbageCollector::walk_call_frame(CallFrame* top_call_frame) {
    CallFrame* call_frame = top_call_frame;
    while(call_frame) {
      if(call_frame->name && call_frame->name->reference_p()) {
        call_frame->name = (Symbol*)saw_object(call_frame->name);
      }

      if(call_frame->cm && call_frame->cm->reference_p()) {
        call_frame->cm = (CompiledMethod*)saw_object(call_frame->cm);
      }

      for(int i = 0; i < call_frame->stack_size; i++) {
        Object* obj = call_frame->stk[i];
        if(obj && obj->reference_p()) {
          call_frame->stk[i] = saw_object(obj);
        }
      }

      if(call_frame->top_scope) {
        if(call_frame->top_scope->stack_allocated_p()) {
          saw_variable_scope(call_frame->top_scope);
        } else {
          call_frame->top_scope = (VariableScope*)saw_object(call_frame->top_scope);
        }
      }

      if(call_frame->scope) {
        if(call_frame->scope->stack_allocated_p()) {
          saw_variable_scope(call_frame->scope);
        } else {
          call_frame->scope = (VariableScope*)saw_object(call_frame->scope);
        }
      }

      call_frame = call_frame->previous;
    }

  }
}
