/* The GC superclass methods, used by both GCs. */
#include "object_utils.hpp"
#include "memory/gc.hpp"

#include "memory.hpp"
#include "machine_code.hpp"

#include "memory/object_mark.hpp"

#include "class/class.hpp"
#include "class/tuple.hpp"
#include "class/packed_object.hpp"
#include "class/module.hpp"
#include "class/symbol.hpp"
#include "class/weakref.hpp"
#include "class/compiled_code.hpp"
#include "class/native_method.hpp"
#include "call_frame.hpp"
#include "class/variable_scope.hpp"
#include "class/lexical_scope.hpp"
#include "class/block_environment.hpp"
#include "capi/handle.hpp"

#include "arguments.hpp"
#include "object_watch.hpp"
#include "thread_nexus.hpp"

#include <mutex>

namespace rubinius {
namespace memory {

  GCData::GCData(VM* state)
    : roots_(state->globals().roots)
    , handles_(state->memory()->capi_handles())
    , cached_handles_(state->memory()->cached_capi_handles())
    , global_cache_(state->shared.global_cache)
    , thread_nexus_(state->shared.thread_nexus())
    , global_handle_locations_(state->memory()->global_capi_handle_locations())
  { }

  GarbageCollector::GarbageCollector(Memory *om)
    : memory_(om)
    , weak_refs_(NULL)
  { }

  VM* GarbageCollector::vm() {
    return memory_->vm();
  }

  /**
   * Scans the specified Object +obj+ for references to other Objects, and
   * marks those Objects as reachable. Understands how to read the inside of
   * an Object and find all references located within. For each reference
   * found, it marks the object pointed to as live (which may trigger
   * movement of the object in a copying garbage collector), but does not
   * recursively scan into the referenced object (since such recursion could
   * be arbitrarily deep, depending on the object graph, and this could cause
   * the stack to blow up).
   * /param obj The Object to be scanned for references to other Objects.
   */
  void GarbageCollector::scan_object(Object* obj) {
#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during scan_object.\n";
    }
#endif
    // We set scanned here before we finish scanning the object.
    // This is done so we don't have a race condition while we're
    // scanning the object and another thread updates a field during
    // the phase where the object is partially scanned.
    scanned_object(obj);

    if(Object* klass = saw_object(obj->klass())) {
      obj->klass(memory_, force_as<Class>(klass));
    }

    if(obj->ivars()->reference_p()) {
      if(Object* ivars = saw_object(obj->ivars())) {
        obj->ivars(memory_, ivars);
      }
    }

    // Handle Tuple directly, because it's so common
    if(Tuple* tup = try_as<Tuple>(obj)) {
      native_int size = tup->num_fields();

      for(native_int i = 0; i < size; i++) {
        Object* slot = tup->field[i];
        if(slot->reference_p()) {
          if(Object* moved = saw_object(slot)) {
            tup->field[i] = moved;
            memory_->write_barrier(tup, moved);
          }
        }
      }
    } else {
      TypeInfo* ti = memory_->type_info[obj->type_id()];

      ObjectMark mark(this);
      ti->mark(obj, mark);
    }
  }

  /**
   * Removes a mature object from the remembered set, ensuring it will not keep
   * alive any young objects if no other live references to those objects exist.
   */
  void GarbageCollector::delete_object(Object* obj) {
    if(obj->remembered_p()) {
      memory_->unremember_object(obj);
    }
  }

  void GarbageCollector::saw_variable_scope(CallFrame* call_frame,
      StackVariables* scope)
  {
    scope->self_ = mark_object(scope->self());
    scope->block_ = mark_object(scope->block());
    scope->module_ = (Module*)mark_object(scope->module());

    int locals = call_frame->compiled_code->machine_code()->number_of_locals;
    for(int i = 0; i < locals; i++) {
      Object* local = scope->get_local(i);
      if(local->reference_p()) {
        scope->set_local(i, mark_object(local));
      }
    }

    if(scope->last_match_ && scope->last_match_->reference_p()) {
      scope->last_match_ = mark_object(scope->last_match_);
    }

    VariableScope* parent = scope->parent();
    if(parent) {
      scope->parent_ = (VariableScope*)mark_object(parent);
    }

    VariableScope* heap = scope->on_heap();
    if(heap) {
      scope->on_heap_ = (VariableScope*)mark_object(heap);
    }
  }


  void GarbageCollector::verify_variable_scope(CallFrame* call_frame,
      StackVariables* scope)
  {
    scope->self_->validate();
    scope->block_->validate();
    scope->module_->validate();

    int locals = call_frame->compiled_code->machine_code()->number_of_locals;
    for(int i = 0; i < locals; i++) {
      Object* local = scope->get_local(i);
      local->validate();
    }

    if(scope->last_match_ && scope->last_match_->reference_p()) {
      scope->last_match_->validate();
    }

    VariableScope* parent = scope->parent();
    if(parent) {
      scope->parent_->validate();
    }

    VariableScope* heap = scope->on_heap();
    if(heap) {
      scope->on_heap_->validate();
    }
  }

  template <typename T>
    T displace(T ptr, AddressDisplacement* dis) {
      if(!dis) return ptr;
      return dis->displace(ptr);
    }

  /**
   * Walks the chain of objects accessible from the specified CallFrame.
   */
  void GarbageCollector::walk_call_frame(CallFrame* call_frame,
                                         AddressDisplacement* offset)
  {
    CallFrame* frame = call_frame;

    while(frame) {
      frame = displace(frame, offset);

      if(frame->lexical_scope_ &&
          frame->lexical_scope_->reference_p()) {
        frame->lexical_scope_ =
          (LexicalScope*)mark_object(frame->lexical_scope_);
      }

      if(frame->compiled_code && frame->compiled_code->reference_p()) {
        frame->compiled_code = (CompiledCode*)mark_object(frame->compiled_code);
      }

      if(frame->compiled_code) {
        native_int stack_size = frame->compiled_code->stack_size()->to_native();
        for(native_int i = 0; i < stack_size; i++) {
          Object* obj = frame->stk[i];
          if(obj && obj->reference_p()) {
            frame->stk[i] = mark_object(obj);
          }
        }
      }

      if(frame->multiple_scopes_p() && frame->top_scope_) {
        frame->top_scope_ = (VariableScope*)mark_object(frame->top_scope_);
      }

      if(BlockEnvironment* env = frame->block_env()) {
        frame->set_block_env((BlockEnvironment*)mark_object(env));
      }

      Arguments* args = displace(frame->arguments, offset);

      if(!frame->inline_method_p() && args) {
        args->set_recv(mark_object(args->recv()));
        args->set_block(mark_object(args->block()));

        if(Tuple* tup = args->argument_container()) {
          args->update_argument_container((Tuple*)mark_object(tup));
        } else {
          Object** ary = displace(args->arguments(), offset);
          for(uint32_t i = 0; i < args->total(); i++) {
            ary[i] = mark_object(ary[i]);
          }
        }
      }

      if(NativeMethodFrame* nmf = frame->native_method_frame()) {
        nmf->handles().gc_scan(this);
      }

      if(frame->scope && frame->compiled_code) {
        saw_variable_scope(frame, displace(frame->scope, offset));
      }

      frame = frame->previous;
    }
  }

  /**
   * Walks the chain of objects accessible from the specified CallFrame.
   */
  void GarbageCollector::verify_call_frame(CallFrame* call_frame,
                                           AddressDisplacement* offset)
  {
    CallFrame* frame = call_frame;

    while(frame) {
      frame = displace(frame, offset);

      if(frame->lexical_scope_) {
        frame->lexical_scope_->validate();
      }

      if(frame->compiled_code) {
        frame->compiled_code->validate();
      }

      if(frame->compiled_code) {
        native_int stack_size = frame->compiled_code->stack_size()->to_native();
        for(native_int i = 0; i < stack_size; i++) {
          Object* obj = frame->stk[i];
          obj->validate();
        }
      }

      VariableScope* scope = frame->top_scope_;
      if(frame->multiple_scopes_p() && scope && !scope->nil_p()) {
        scope->validate();
      }

      if(BlockEnvironment* env = frame->block_env()) {
        env->validate();
      }

      Arguments* args = displace(frame->arguments, offset);

      if(!frame->inline_method_p() && args) {
        args->recv()->validate();
        args->block()->validate();

        Object** ary = displace(args->arguments(), offset);
        for(uint32_t i = 0; i < args->total(); i++) {
          ary[i]->validate();
        }
      }

      if(frame->scope && frame->compiled_code) {
        verify_variable_scope(frame, displace(frame->scope, offset));
      }

      frame = frame->previous;
    }
  }


  void GarbageCollector::scan(ManagedThread* thr, bool young_only) {
    for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
      if(Object* fwd = saw_object(ri->get())) {
        ri->set(fwd);
      }
    }

    scan(thr->variable_root_buffers(), young_only);
    scan(thr->root_buffers(), young_only);

    if(VM* vm = thr->as_vm()) {
      vm->gc_scan(this);
    }
  }

  void GarbageCollector::verify(GCData* data) {
    std::lock_guard<std::mutex> guard(data->thread_nexus()->threads_mutex());

    for(ThreadList::iterator i = data->thread_nexus()->threads()->begin();
        i != data->thread_nexus()->threads()->end();
        ++i)
    {
      ManagedThread* thr = *i;
      for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
        ri->get()->validate();
      }

      if(VM* vm = thr->as_vm()) {
        vm->gc_verify(this);
      }
    }
  }

  void GarbageCollector::scan(VariableRootBuffers& buffers,
                              bool young_only, AddressDisplacement* offset)
  {
    VariableRootBuffer* vrb = displace(buffers.front(), offset);

    while(vrb) {
      Object*** buffer = displace(vrb->buffer(), offset);
      for(int idx = 0; idx < vrb->size(); idx++) {
        Object** var = displace(buffer[idx], offset);
        Object* cur = *var;

        if(cur && cur->reference_p() && (!young_only || cur->young_object_p())) {
          if(Object* tmp = saw_object(cur)) {
            *var = tmp;
          }
        }
      }

      vrb = displace((VariableRootBuffer*)vrb->next(), offset);
    }
  }

  void GarbageCollector::scan(RootBuffers& buffers, bool young_only) {
    for(RootBuffers::Iterator i(buffers);
        i.more();
        i.advance())
    {
      Object** buffer = i->buffer();
      for(int idx = 0; idx < i->size(); idx++) {
        Object* cur = buffer[idx];

        if(cur->reference_p() && (!young_only || cur->young_object_p())) {
          if(Object* tmp = saw_object(cur)) {
            buffer[idx] = tmp;
          }
        }
      }
    }
  }

  void GarbageCollector::clean_weakrefs(bool check_forwards) {
    if(!weak_refs_) return;

    for(ObjectArray::iterator i = weak_refs_->begin();
        i != weak_refs_->end();
        ++i) {
      if(!*i) continue; // Object was removed during young gc.
      WeakRef* ref = try_as<WeakRef>(*i);
      if(!ref) continue; // Other type for some reason?

      Object* obj = ref->object();
      if(!obj->reference_p()) continue;

      if(check_forwards) {
        if(obj->young_object_p()) {
          if(!obj->forwarded_p()) {
            ref->set_object(memory_, cNil);
          } else {
            ref->set_object(memory_, obj->forward());
          }
        }
      } else if(!obj->marked_p(memory_->mark())) {
        ref->set_object(memory_, cNil);
      }
    }

    delete weak_refs_;
    weak_refs_ = NULL;
  }

  void GarbageCollector::clean_locked_objects(ManagedThread* thr, bool young_only) {
    LockedObjects& los = thr->locked_objects();
    for(LockedObjects::iterator i = los.begin();
        i != los.end();) {
      Object* obj = static_cast<Object*>(*i);
      if(young_only) {
        if(obj->young_object_p()) {
          if(obj->forwarded_p()) {
            *i = obj->forward();
            ++i;
          } else {
            i = los.erase(i);
          }
        } else {
          ++i;
        }
      } else {
        if(!obj->marked_p(memory_->mark())) {
          i = los.erase(i);
        } else {
          ++i;
        }
      }
    }
  }
}
}
