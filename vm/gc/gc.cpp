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
#include "builtin/compiledmethod.hpp"
#include "call_frame.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/block_environment.hpp"
#include "capi/handle.hpp"

#include "arguments.hpp"

#include "object_watch.hpp"

namespace rubinius {

  GCData::GCData(STATE)
    : roots_(state->globals().roots)
    , call_frames_(state->shared.call_frame_locations())
    , variable_buffers_(*state->variable_buffers())
    , root_buffers_(state->root_buffers())
    , handles_(state->shared.global_handles())
    , cached_handles_(state->shared.cached_handles())
    , global_cache_(state->shared.global_cache)
    , threads_(state->shared.threads())
    , global_handle_locations_(state->shared.global_handle_locations())
  {}

  GarbageCollector::GarbageCollector(ObjectMemory *om)
                   :object_memory_(om), weak_refs_(NULL) { }

  VM* GarbageCollector::state() {
    return object_memory_->state;
  }

  /* Understands how to read the inside of an object and find all references
   * located within. It copies the objects pointed to, but does not follow into
   * those further (ie, not recursive) */
  void GarbageCollector::scan_object(Object* obj) {
    Object* slot;

    if(watched_p(obj)) {
      std::cout << "detected " << obj << " during scan_object.\n";
    }

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
      int size = tup->num_fields();

      for(int i = 0; i < size; i++) {
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

    int locals = call_frame->cm->backend_method()->number_of_locals;
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

  void GarbageCollector::walk_call_frame(CallFrame* top_call_frame) {
    CallFrame* call_frame = top_call_frame;
    while(call_frame) {
      if(call_frame->custom_static_scope_p() &&
          call_frame->static_scope_ &&
          call_frame->static_scope_->reference_p()) {
        call_frame->static_scope_ =
          (StaticScope*)mark_object(call_frame->static_scope_);
      }

      if(call_frame->cm && call_frame->cm->reference_p()) {
        call_frame->cm = (CompiledMethod*)mark_object(call_frame->cm);
      }

      if(call_frame->cm && call_frame->stk) {
        native_int stack_size = call_frame->cm->stack_size()->to_native();
        for(native_int i = 0; i < stack_size; i++) {
          Object* obj = call_frame->stk[i];
          if(obj && obj->reference_p()) {
            call_frame->stk[i] = mark_object(obj);
          }
        }
      }

      if(call_frame->multiple_scopes_p() &&
          call_frame->top_scope_) {
        call_frame->top_scope_ = (VariableScope*)mark_object(call_frame->top_scope_);
      }

      if(Dispatch* msg = call_frame->dispatch()) {
        msg->module = (Module*)mark_object(msg->module);
        msg->method = (Executable*)mark_object(msg->method);
      }

      if(BlockEnvironment* env = call_frame->block_env()) {
        call_frame->set_block_env((BlockEnvironment*)mark_object(env));
      }

      Arguments* args = call_frame->arguments;
      if(!call_frame->inline_method_p() && args) {
        args->set_recv(mark_object(args->recv()));
        args->set_block(mark_object(args->block()));

        if(Tuple* tup = args->argument_container()) {
          args->update_argument_container((Tuple*)mark_object(tup));
        } else {
          Object** ary = args->arguments();
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
        rd->method_ = (CompiledMethod*)mark_object(rd->method());
        rd->name_ = (Symbol*)mark_object(rd->name());
        rd->module_ = (Module*)mark_object(rd->module());
      }
#endif

      saw_variable_scope(call_frame, call_frame->scope);

      call_frame = static_cast<CallFrame*>(call_frame->previous);
    }
  }

  class UnmarkVisitor : public ObjectVisitor {
    std::vector<Object*> stack_;
    ObjectMemory* object_memory_;

  public:

    UnmarkVisitor(ObjectMemory* om)
      : object_memory_(om)
    {}

    Object* call(Object* obj) {
      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during unmarking.\n";
      }

      if(obj->reference_p() && obj->marked_p(object_memory_->mark())) {
        obj->clear_mark();
        stack_.push_back(obj);
      }

      return obj;
    }

  };

  void GarbageCollector::clean_weakrefs(bool check_forwards) {
    if(!weak_refs_) return;

    for(ObjectArray::iterator i = weak_refs_->begin();
        i != weak_refs_->end();
        i++) {
      WeakRef* ref = try_as<WeakRef>(*i);
      if(!ref) continue; // WTF.

      Object* obj = ref->object();
      if(!obj->reference_p()) continue;

      if(check_forwards) {
        if(obj->young_object_p()) {
          if(!obj->forwarded_p()) {
            ref->set_object(object_memory_, Qnil);
          } else {
            ref->set_object(object_memory_, obj->forward());
          }
        }
      } else if(!obj->marked_p(object_memory_->mark())) {
        ref->set_object(object_memory_, Qnil);
      }
    }

    delete weak_refs_;
    weak_refs_ = NULL;
  }
}
