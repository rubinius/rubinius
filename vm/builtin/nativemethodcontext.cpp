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
   *  TODO: Error conditions should assert?
   */
  Handle NativeMethodContext::handle_for(Object* object) {
    handles_->push_back(object);
    return (handles_->size() - 1);
  }

  /**
   *  TODO: Concurrency.
   */
  Handle NativeMethodContext::handle_for(Object* object)
  {
    my_handles->push_back(object);
    return (my_handles->size() - 1);
  }


/* Info stuff */

  void NativeMethodContext::Info::cleanup(Object* object) {
    NativeMethodContext* context = as<NativeMethodContext>(object);

    delete [] static_cast<char*>(context->stack_);
    context->stack_ = NULL;
  }
}

