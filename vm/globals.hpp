#ifndef RBX_GLOBALS_HPP
#define RBX_GLOBALS_HPP

/** Rubinius Root objects
 *
 * This is the set of root objects that the system tracks.
 * When performing garbage collection, it is these objects
 * that are considered the root of the tree and walked
 * to find all live objects.
 *
 * Typically, these are builtin class that the VM needs to
 * reference directly. This is usually the case for things like
 * String::create that need to have a reference to the ruby String
 * class object to properly create a String instance.
 *
 * The rule of thumb for adding classes/modules to this list
 * is whether or not the VM needs to directly reference the
 * class/module. Again, this is typically only because the VM
 * itself needs to create an instance of that class.
 *
 * There are a few non-class roots in here, such as the current
 * thread, current task, and a list of all scheduled threads.
 * These exist so that we walk all the runtime data, such as
 * MethodContexts.
 */


#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

#include "gc_root.hpp"

namespace rubinius {

  class LookupTable;
  class Tuple;
  class Module;
  class Thread;
  class Task;
  class Class;
  class Symbol;


  struct Globals {

    Roots roots;

    /* This requires some manual work when adding a root, but thats ok.
     * First, add the root to the END of this list. Then add it to
     * the constructor for Globals, again, at the END of the list. */

    /* classes for the core 'types' */
    TypedRoot<Class*> blokctx, cmethod, tuple, module, object, array;
    TypedRoot<Class*> klass, methtbl, bytearray, methctx, blank;
    TypedRoot<Class*> blokenv, bignum, regexp, matchdata;
    TypedRoot<Class*> string, symbol, io, metaclass;
    TypedRoot<Class*> nil_class, true_class, false_class, fixnum_class, undef_class;
    TypedRoot<Class*> floatpoint, fastctx, nmc, task, list, list_node;
    TypedRoot<Class*> channel, thread, staticscope, send_site, selector, lookuptable;
    TypedRoot<Class*> iseq, executable, native_function, iobuffer;
    TypedRoot<Class*> cmethod_vis, included_module;

    /* the primary symbol table */
    TypedRoot<Symbol*> sym_method_missing;
    TypedRoot<Symbol*> sym_inherited, sym_opened_class;
    TypedRoot<Symbol*> sym_from_literal, sym_method_added, sym_s_method_added, sym_init_copy;
    TypedRoot<Symbol*> sym_plus, sym_minus, sym_equal, sym_nequal, sym_tequal, sym_lt, sym_gt;
    TypedRoot<Symbol*> sym_initialize;

    TypedRoot<Class*> exception;
    TypedRoot<Class*> exc_arg, exc_segfault;
    TypedRoot<Class*> exc_loe, exc_type, exc_rex;
    TypedRoot<Class*> exc_stack_explosion;
    TypedRoot<Class*> exc_primitive_failure;

    TypedRoot<LookupTable*> external_ivars;
    TypedRoot<Tuple*> scheduled_threads;
    TypedRoot<LookupTable*> errno_mapping;
    TypedRoot<LookupTable*> selectors;
    TypedRoot<Object*> config;
    TypedRoot<Symbol*> sym_send;
    TypedRoot<Symbol*> sym_public, sym_private, sym_protected, sym_const_missing;
    TypedRoot<Symbol*> sym_object_id, sym_call;
    TypedRoot<Object*> top_scope, on_gc_channel;

    TypedRoot<Module*> vm;
    TypedRoot<Thread*> current_thread;
    TypedRoot<Task*> current_task;
    TypedRoot<Object*> main;
    TypedRoot<Class*> dir;
    TypedRoot<Class*> compactlookuptable;
    TypedRoot<Class*> access_variable;
    TypedRoot<Module*> rubinius;
    TypedRoot<Class*> time_class;
    TypedRoot<Class*> integer;
    TypedRoot<Class*> numeric;
    TypedRoot<Class*> memory_pointer;
    TypedRoot<Class*> taskprobe;

    TypedRoot<Class*> nmethod;        /**< NativeMethod */
    TypedRoot<Class*> nativectx;      /**< NativeMethodContext */

    TypedRoot<Class*> data;

    /* Add new globals above this line. */

    /* Leave this as the last data member always */
    TypedRoot<Class*> special_classes[SPECIAL_CLASS_SIZE];

    Globals() :
      blokctx(&roots),
      cmethod(&roots),
      tuple(&roots),
      module(&roots),
      object(&roots),
      array(&roots),
      klass(&roots),
      methtbl(&roots),
      bytearray(&roots),
      methctx(&roots),
      blank(&roots),
      blokenv(&roots),
      bignum(&roots),
      regexp(&roots),
      matchdata(&roots),
      string(&roots),
      symbol(&roots),
      io(&roots),
      metaclass(&roots),
      nil_class(&roots),
      true_class(&roots),
      false_class(&roots),
      fixnum_class(&roots),
      undef_class(&roots),
      floatpoint(&roots),
      fastctx(&roots),
      nmc(&roots),
      task(&roots),
      list(&roots),
      list_node(&roots),
      channel(&roots),
      thread(&roots),
      staticscope(&roots),
      send_site(&roots),
      selector(&roots),
      lookuptable(&roots),
      iseq(&roots),
      executable(&roots),
      native_function(&roots),
      iobuffer(&roots),
      cmethod_vis(&roots),
      included_module(&roots),
      sym_method_missing(&roots),
      sym_inherited(&roots),
      sym_opened_class(&roots),
      sym_from_literal(&roots),
      sym_method_added(&roots),
      sym_s_method_added(&roots),
      sym_init_copy(&roots),
      sym_plus(&roots),
      sym_minus(&roots),
      sym_equal(&roots),
      sym_nequal(&roots),
      sym_tequal(&roots),
      sym_lt(&roots),
      sym_gt(&roots),
      sym_initialize(&roots),
      exception(&roots),
      exc_arg(&roots),
      exc_segfault(&roots),
      exc_loe(&roots),
      exc_type(&roots),
      exc_rex(&roots),
      exc_stack_explosion(&roots),
      exc_primitive_failure(&roots),
      external_ivars(&roots),
      scheduled_threads(&roots),
      errno_mapping(&roots),
      selectors(&roots),
      config(&roots),
      sym_send(&roots),
      sym_public(&roots),
      sym_private(&roots),
      sym_protected(&roots),
      sym_const_missing(&roots),
      sym_object_id(&roots),
      sym_call(&roots),
      top_scope(&roots),
      on_gc_channel(&roots),
      vm(&roots),
      current_thread(&roots),
      current_task(&roots),
      main(&roots),
      dir(&roots),
      compactlookuptable(&roots),
      access_variable(&roots),
      rubinius(&roots),
      time_class(&roots),
      integer(&roots),
      numeric(&roots),
      memory_pointer(&roots),
      taskprobe(&roots),

      nmethod(&roots),
      nativectx(&roots),     /**< NativeMethodContext */

      data(&roots)

      /* Add initialize of globals above this line. */
    { }
  };
};

#endif
