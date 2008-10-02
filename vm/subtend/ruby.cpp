#include <cstdarg>

#include "builtin/array.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/module.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/string.hpp"
#include "builtin/task.hpp"

#include "vm/global_cache.hpp"
#include "vm/message.hpp"
#include "vm/object.hpp"
#include "vm/quantum_stack_leap.hpp"
#include "vm/vm.hpp"

#include "subtend/ruby.h"


namespace rubinius {

  /**
   *  Common implementation for rb_funcall*
   *
   *  TODO:   Set up permanent SendSites through macroing?
   *  TODO:   Stricter action check?
   */
  static VALUE hidden_rb_funcall_backend(VALUE receiver,
                                         ID method_name,
                                         std::size_t arg_count,
                                         VALUE* arg_array)
  {
    NativeMethodContext* context = NativeMethodContext::current();

    // Create Message.. bit more involved than usual because we bypass some conveniences
    Message& msg = context->message_from_c();

    msg.send_site   = static_cast<SendSite*>(RBX_Qnil);
    msg.name        = reinterpret_cast<Symbol*>(method_name);
    msg.recv        = context->object_from(receiver);
    msg.block       = RBX_Qnil;
    msg.splat       = RBX_Qnil;
    msg.total_args  = arg_count;
    msg.stack       = arg_count + 1;
    msg.priv        = true;           /* TODO: Double-check this */

    msg.lookup_from = (msg.recv->lookup_begin(context->state()));

    msg.task        = context->task();
    msg.task->call_flags = 0;

    Array* args = Array::create(context->state(), arg_count);

    for (std::size_t i = 0; i < arg_count; ++i) {
      args->set(context->state(), i, context->object_from(arg_array[i]));
    }

    msg.set_arguments(context->state(), args);

    context->action(NativeMethodContext::CALL_FROM_C);
    store_current_execution_point_in(context->inside_c_method_point());
    /* Execution resumes here when returning */

    context = NativeMethodContext::current();

    if (context->action() != NativeMethodContext::RETURNED_BACK_TO_C) {
      jump_to_execution_point_in(context->dispatch_point());
    }

    context->action(NativeMethodContext::ORIGINAL_CALL);

    VALUE ret = context->value_returned_to_c();

    context->value_returned_to_c(RBX_Qnil);
    context->message_from_c().reset();

    return ret;
  }

}


/** API */
extern "C" {

  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...)
  {
    va_list varargs;
    va_start(varargs, arg_count);

    VALUE* args = new VALUE[arg_count];

    for (int i = 0; i < arg_count; ++i) {
      args[i] = va_arg(varargs, VALUE);
    }

    va_end(varargs);

    VALUE ret = rubinius::hidden_rb_funcall_backend(receiver, method_name, arg_count, args);

    delete args;
    return ret;
  }

  VALUE rb_funcall2(VALUE receiver, ID method_name, int arg_count, VALUE* args)
  {
    return rubinius::hidden_rb_funcall_backend(receiver, method_name, arg_count, args);
  }

  /**
   *  Kind:
   *    0:  Public
   *    1:  Private
   *    2:  Protected
   *    3:  Singleton
   */
  void rb_define_method_impl(const char* file,
                             VALUE module_handle,
                             const char* name,
                             SubtendGenericFunction fptr,
                             int arity,
                             int kind)
  {
    rubinius::NativeMethodContext* context = rubinius::NativeMethodContext::current();

    rubinius::VM* state = context->state();
    rubinius::Symbol* method_name = state->symbol(name);
    rubinius::Module* module = rubinius::as<rubinius::Module>(context->object_from(module_handle));

    rubinius::NativeMethod* method = NULL;         // Fortran FTW.
    method = rubinius::NativeMethod::create(state,
                                            rubinius::String::create(state, file),
                                            module,
                                            method_name,
                                            fptr,
                                            rubinius::Fixnum::from(arity));

    /* HACK: Should refactor Task::add/attach to use Executable? */
    module->method_table()->store(state, method_name, method);
    state->global_cache->clear(module, method_name);
  }

}
