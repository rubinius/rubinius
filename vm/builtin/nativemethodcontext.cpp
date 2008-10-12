#include <cstdlib>

#include "objectmemory.hpp"
#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"

#include "builtin/nativemethodcontext.hpp"


namespace rubinius {


/* NativeMethodContext */


  /**
   *  Currently active NativeMethodContext access.
   *
   *  TODO: use thread-local.
   */
  static NativeMethodContext* hidden_current_native_context = NULL;


  /* Class methods */

  void NativeMethodContext::register_class_with(VM* state)
  {
    state->globals.nativectx.set(state->new_class("NativeMethodContext", state->globals.methctx.get()));
    state->globals.nativectx.get()->set_object_type(state, NativeMethodContextType);
  }

  NativeMethodContext* NativeMethodContext::create(VM* state,
                                                   Message* msg,
                                                   Task* task,
                                                   NativeMethod* method)
  {
    static std::size_t context_size = sizeof(NativeMethodContext) + DEFAULT_STACK_SIZE;

    NativeMethodContext* nmc = static_cast<NativeMethodContext*>(state->new_struct(G(nativectx),
                                                                 context_size));

    /* MethodContext stuff. */
    nmc->sender(state, task->active());
    nmc->home(state, nmc);
    nmc->self(state, msg->recv);
    nmc->module(state, msg->module);
    nmc->name(state, as<Object>(msg->name));
    nmc->block(state, msg->block);

    /* Fake that which is not needed. */
    nmc->cm(state, reinterpret_cast<CompiledMethod*>(Qnil));
    nmc->vmm = NULL;

    /* Instead of storing the memory within as MethodContexts, we heap-allocate. */
    nmc->stack_size = DEFAULT_STACK_SIZE;

    nmc->action_          = ORIGINAL_CALL;
    nmc->handles_         = new HandleStorage();
    nmc->message_         = msg;
    nmc->message_from_c_  = new Message(state);
    nmc->method_          = method;
    nmc->return_value_    = as<Object>(Qnil);
    nmc->stack_           = static_cast<void*>(new char[nmc->stack_size]);
    nmc->state_           = state;
    nmc->task_            = task;

    /* Add the basic Handles. Always crossref with ruby.h when changing. */
    nmc->handles_->push_back(Qfalse);
    nmc->handles_->push_back(Qtrue);
    nmc->handles_->push_back(Qnil);
    nmc->handles_->push_back(Qundef);

    return nmc;
  }

  NativeMethodContext* NativeMethodContext::allocate(VM* state) {
    return create(state);
  }

  void NativeMethodContext::current_context_is(NativeMethodContext* context)
  {
    hidden_current_native_context = context;
  }

  NativeMethodContext* NativeMethodContext::current()
  {
     return hidden_current_native_context;
  }

  HandleStorage& NativeMethodContext::global_handles() {
    static HandleStorage our_global_handles;
    return our_global_handles;
  }

/* Instance methods */

//  Object* NativeMethodContext::clone(STATE) {
//    Object* other = dup(state);
//
//    other->copy_internal_state_from(state, this);
//
//    return other;
//  }
//
//  Object* NativeMethodContext::dup(STATE) {
//    Object* other = state->om->allocate_object(this->num_fields());
//
//    other->initialize_copy(this, age);
//
//    /* Use lookup_begin to preserve any IncludedModules. */
//    other->klass(state, this->lookup_begin(state));
//
//    /* Redo all instance vars. */
//
//    if(other->zone == MatureObjectZone) {
//      state->om->remember_object(other);
//    }
//
//    return other;
//  }

  /**
   *  NOTE: Unlike object_for(), this is _always_ a local Handle.
   *        @see handle_for_global() instead.
   *
   *  TODO: Currently, a Handle may have an entry _both_ in local
   *        handles as well as globals. This should hopefully not
   *        cause problems since the main concern is only that the
   *        global object must always be available. The extras are
   *        unlikely to impede here, since the object itself is
   *        guarded against collection.
   *
   *  TODO: Concurrency.
   */
  Handle NativeMethodContext::handle_for(Object* object) {
    handles_->push_back(object);
    return (handles_->size() - 1);
  }

  /**
   *  Global handles are negative starting at -1.
   *
   *  @see  discussion under handle_for().
   *
   *  TODO: Should the objects be remembered or set
   *        mature here? Unlikely, but needs verification.
   *
   *  TODO: Concurrency.
   */
  Handle NativeMethodContext::handle_for_global(Object* object) {
    HandleStorage& globals = NativeMethodContext::global_handles();

    /* No duplicates. Return existing entry if there is one. */
    for(HandleStorage::iterator it = globals.begin(); it != globals.end(); ++it) {
      if(*it == object) {
        return (-1 - (it - globals.begin()));  /* Grabs the index. */
      }
    }

    globals.push_back(object);

    return (0 - globals.size());
  }

  void NativeMethodContext::mark_handles(ObjectMark& mark) {
    for (HandleStorage::iterator it = handles_->begin(); it != handles_->end(); ++it) {
      Object* marked = mark.call(*it);

      if (marked) {
        *it = marked;
        mark.just_set(this, marked);
      }
    }

    HandleStorage& globals = NativeMethodContext::global_handles();

    for (HandleStorage::iterator it = globals.begin(); it != globals.end(); ++it) {
      Object* marked = mark.call(*it);

      if (marked) {
        *it = marked;
        mark.just_set(this, marked);
      }
    }
  }

  /**
   *  TODO: Error conditions should assert?
   */
  Object* NativeMethodContext::object_from(Handle handle) {
    if(handle < 0) {
      return object_from_global(handle);
    }

    return (*handles_)[handle];
  }

  /**
   *  Global handles are negative starting at -1.
   *
   *  TODO: Error conditions should assert?
   */
  Object* NativeMethodContext::object_from_global(Handle handle) {
    return NativeMethodContext::global_handles()[(-1 - handle)];
  }



/* Info stuff */

  void NativeMethodContext::Info::cleanup(Object* object) {
    NativeMethodContext* context = as<NativeMethodContext>(object);

    delete [] static_cast<char*>(context->stack_);
    context->stack_ = NULL;
  }

  void NativeMethodContext::Info::mark(Object* self, ObjectMark& mark) {
    auto_mark(self, mark);
    as<NativeMethodContext>(self)->mark_handles(mark);
  }

  void NativeMethodContext::Info::show(STATE, Object* self, int level) {
//    MethodContext* ctx = as<MethodContext>(self);
//
//    class_header(state, self);
//    indent_attribute(++level, "name"); ctx->name()->show(state, level);
//    indent_attribute(level, "sender");
//    if(ctx->sender()->nil_p()) {
//      ctx->sender()->show(state, level);
//    } else {
//      class_info(state, ctx->sender(), true);
//    }
//    indent_attribute(level, "home");
//    if(ctx->home()->nil_p()) {
//      ctx->home()->show(state, level);
//    } else {
//      class_info(state, ctx->home(), true);
//    }
//    indent_attribute(level, "self"); ctx->self()->show(state, level);
//    indent_attribute(level, "cm"); ctx->cm()->show(state, level);
//    indent_attribute(level, "module"); ctx->module()->show(state, level);
//    indent_attribute(level, "block"); ctx->block()->show(state, level);
//    close_body(level);
  }

}

