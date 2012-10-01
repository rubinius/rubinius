/* The GC superclass methods, used by both GCs. */
#include "object_utils.hpp"
#include "gc/gc.hpp"

#include "objectmemory.hpp"

#include "gc/object_mark.hpp"

#include "builtin/class.hpp"
#include "builtin/tuple.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/weakref.hpp"
#include "builtin/compiledcode.hpp"
#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/block_environment.hpp"
#include "capi/handle.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

#include "instruments/tooling.hpp"

#include "arguments.hpp"

#include "object_watch.hpp"

namespace rubinius {

  GCData::GCData(VM* state)
    : roots_(state->globals().roots)
    , handles_(state->shared.global_handles())
    , cached_handles_(state->shared.cached_handles())
    , global_cache_(state->shared.global_cache)
    , threads_(state->shared.threads())
    , global_handle_locations_(state->shared.global_handle_locations())
    , gc_token_(0)
#ifdef ENABLE_LLVM
    , llvm_state_(LLVMState::get_if_set(state))
#endif
  {}

  GCData::GCData(VM* state, GCToken gct)
    : roots_(state->globals().roots)
    , handles_(state->shared.global_handles())
    , cached_handles_(state->shared.cached_handles())
    , global_cache_(state->shared.global_cache)
    , threads_(state->shared.threads())
    , global_handle_locations_(state->shared.global_handle_locations())
    , gc_token_(&gct)
#ifdef ENABLE_LLVM
    , llvm_state_(LLVMState::get_if_set(state))
#endif
  {}

  GarbageCollector::GarbageCollector(ObjectMemory *om)
                   :object_memory_(om), weak_refs_(NULL) { }

  VM* GarbageCollector::state() {
    return object_memory_->state();
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
    Object* slot;

#ifdef ENABLE_OBJECT_WATCH
    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during scan_object.\n";
    }
#endif

    // Check and update an inflated header
    if(obj->inflated_header_p()) {
      obj->inflated_header()->reset_object(obj);
    }

    slot = saw_object(obj->klass());
    if(slot) obj->klass(object_memory_, force_as<Class>(slot));

    if(obj->ivars()->reference_p()) {
      slot = saw_object(obj->ivars());
      if(slot) obj->ivars(object_memory_, slot);
    }

    // Handle Tuple directly, because it's so common
    if(Tuple* tup = try_as<Tuple>(obj)) {
      native_int size = tup->num_fields();

      for(native_int i = 0; i < size; i++) {
        slot = tup->field[i];
        if(slot->reference_p()) {
          slot = saw_object(slot);
          if(slot) {
            tup->field[i] = slot;
            object_memory_->write_barrier(tup, slot);
          }
        }
      }
    } else {
      TypeInfo* ti = object_memory_->type_info[obj->type_id()];

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
      object_memory_->unremember_object(obj);
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

  template <typename T>
    T displace(T ptr, AddressDisplacement* dis) {
      if(!dis) return ptr;
      return dis->displace(ptr);
    }

  /**
   * Walks the chain of objects accessible from the specified CallFrame.
   */
  void GarbageCollector::walk_call_frame(CallFrame* top_call_frame,
                                         AddressDisplacement* offset)
  {
    CallFrame* call_frame = top_call_frame;

    while(call_frame) {
      call_frame = displace(call_frame, offset);

      // Skip synthetic, non CompiledCode frames
      if(!call_frame->compiled_code) {
        call_frame = call_frame->previous;
        continue;
      }

      if(call_frame->custom_constant_scope_p() &&
          call_frame->constant_scope_ &&
          call_frame->constant_scope_->reference_p()) {
        call_frame->constant_scope_ =
          (ConstantScope*)mark_object(call_frame->constant_scope_);
      }

      if(call_frame->compiled_code && call_frame->compiled_code->reference_p()) {
        call_frame->compiled_code = (CompiledCode*)mark_object(call_frame->compiled_code);
      }

      if(call_frame->compiled_code && call_frame->stk) {
        native_int stack_size = call_frame->compiled_code->stack_size()->to_native();
        for(native_int i = 0; i < stack_size; i++) {
          Object* obj = call_frame->stk[i];
          if(obj && obj->reference_p()) {
            call_frame->stk[i] = mark_object(obj);
          }
        }
      }

      if(call_frame->multiple_scopes_p() && call_frame->top_scope_) {
        call_frame->top_scope_ = (VariableScope*)mark_object(call_frame->top_scope_);
      }

      if(BlockEnvironment* env = call_frame->block_env()) {
        call_frame->set_block_env((BlockEnvironment*)mark_object(env));
      }

      Arguments* args = displace(call_frame->arguments, offset);

      if(!call_frame->inline_method_p() && args) {
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


#ifdef ENABLE_LLVM
      if(jit::RuntimeDataHolder* jd = call_frame->jit_data()) {
        jd->set_mark();

        ObjectMark mark(this);
        jd->mark_all(0, mark);
      }

      if(jit::RuntimeData* rd = call_frame->runtime_data()) {
        rd->method_ = (CompiledCode*)mark_object(rd->method());
        rd->name_ = (Symbol*)mark_object(rd->name());
        rd->module_ = (Module*)mark_object(rd->module());
      }
#endif

      saw_variable_scope(call_frame, displace(call_frame->scope, offset));

      call_frame = call_frame->previous;
    }
  }

  void GarbageCollector::scan(ManagedThread* thr, bool young_only) {
    for(Roots::Iterator ri(thr->roots()); ri.more(); ri.advance()) {
      ri->set(saw_object(ri->get()));
    }

    scan(thr->variable_root_buffers(), young_only);
    scan(thr->root_buffers(), young_only);

    if(VM* vm = thr->as_vm()) {
      vm->gc_scan(this);
    }

    std::list<ObjectHeader*>& los = thr->locked_objects();
    for(std::list<ObjectHeader*>::iterator i = los.begin();
        i != los.end();
        ++i) {
      *i = saw_object((Object*)*i);
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
        Object* tmp = *var;

        if(tmp && tmp->reference_p() && (!young_only || tmp->young_object_p())) {
          *var = saw_object(tmp);
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
        Object* tmp = buffer[idx];

        if(tmp->reference_p() && (!young_only || tmp->young_object_p())) {
          buffer[idx] = saw_object(tmp);
        }
      }
    }
  }

  void GarbageCollector::clean_weakrefs(bool check_forwards) {
    if(!weak_refs_) return;

    for(ObjectArray::iterator i = weak_refs_->begin();
        i != weak_refs_->end();
        ++i) {
      WeakRef* ref = try_as<WeakRef>(*i);
      if(!ref) continue; // WTF.

      Object* obj = ref->object();
      if(!obj->reference_p()) continue;

      if(check_forwards) {
        if(obj->young_object_p()) {
          if(!obj->forwarded_p()) {
            ref->set_object(object_memory_, cNil);
          } else {
            ref->set_object(object_memory_, obj->forward());
          }
        }
      } else if(!obj->marked_p(object_memory_->mark())) {
        ref->set_object(object_memory_, cNil);
      }
    }

    delete weak_refs_;
    weak_refs_ = NULL;
  }

}
