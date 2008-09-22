#include "vm.hpp"

#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"

#include "builtin/nativemethodcontext.hpp"


namespace rubinius {

  /**
   *  Currently active NativeMethodContext access.
   *
   *  TODO: use thread-local.
   */
  static NativeMethodContext* hidden_current_native_context = NULL;

  /**
   *  Initialise a new context.
   *
   *  This constructor is always hidden behind NativeMethodContext.create().
   */
  NativeMethodContext::NativeMethodContext(VM* state, Message* message, Task* task, NativeMethod* method)
    : MethodContext()
    , my_action(ORIGINAL_CALL)
    , my_c_call_point()
    , my_dispatch_point()
    , my_message(message)
    , my_method(method)
    , my_handles()
    , my_return_value(Qnil)
    , my_state(state)
    , my_task(task)
  {
    /* Setting up the inherited stuff that we will actually use and others will need. */
    this->sender(my_state, my_task->active());
    this->home(my_state, const_cast<NativeMethodContext*>(this));

    this->self(my_state, my_message->recv);

    this->module(my_state, my_message->module);

    this->name(my_state, my_message->name);

//    my_stack_size = DEFAULT_STACK_SIZE;
//    my_stack = new char[my_stack_size];
    stack_size = DEFAULT_STACK_SIZE;
  }

  /**
   *  Brand new context for a brand new call.
   *
   *  TODO:   Cache these.
   */
  NativeMethodContext* NativeMethodContext::create(VM* state, Message* msg, Task* task, NativeMethod* method)
  {
    /* Using placement new here as an experiment.
     *
     * TODO: OM should be changed to call destructor
     * when using placement new.
     *
     * TODO: Why new_struct? What struct?
     */
    void* object = state->new_struct( G(nativectx), static_cast<std::size_t>(sizeof(NativeMethodContext) + DEFAULT_STACK_SIZE) );
    NativeMethodContext* nmc = new (object) NativeMethodContext(state, msg, task, method);

    /* TODO: Make active here. */

    current_context_is(nmc);
    return nmc;
  }

  /**
   *  Set the currently active NativeMethodContext.
   */
  void NativeMethodContext::current_context_is(NativeMethodContext* context)
  {
    hidden_current_native_context = context;
  }

  /**
   *  Access the currently active NativeMethodContext.
   *
   *  TODO:   Exception handling.
   *  TODO:   Use reference.
   */
  NativeMethodContext* NativeMethodContext::current()
  {
     return hidden_current_native_context;
  }

  /**
   *  Register this class.
   *
   *  Only ever called in bootstrapping.
   */
  void NativeMethodContext::register_class_with(VM* state) {
    state->globals.nativectx.set(state->new_class("NativeMethodContext", state->globals.methctx.get()));
    state->globals.nativectx.get()->set_object_type(state, NativeMethodContextType);
  }
}

