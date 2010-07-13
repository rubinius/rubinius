#ifndef RBX_BUILTIN_SYSTEM_HPP
#define RBX_BUILTIN_SYSTEM_HPP

#include "vm/type_info.hpp"

#include "builtin/object.hpp"

namespace rubinius {

  class Array;
  class Fixnum;
  class String;
  class BlockEnvironment;
  class CompiledMethod;
  class StaticScope;
  class IO;

  struct CallFrame;

  /**
   *  VM primitives etc.
   */
  class System : public Object {

  public:   /* Bookkeeping */

    /** No particular type needed. @todo Add VMType? --rue */
    static const object_type type = ObjectType;


  public:   /* Primitives */

    static void attach_primitive(STATE, Module* mod, bool meta, Symbol* name, Symbol* prim);

    /** Load a compiled file. */
    // Ruby.primitive :compiledfile_load
    static Object*  compiledfile_load(STATE, String* path, Integer* version);

    /**
     *  When running under GDB, stop here.
     *
     *  NOT IMPLEMENTED.
     *
     *  This ONLY works when the program is being run under GDB.
     *
     *  Using some rather nasty tricks, this primitive will cause
     *  the program to stop inside this method and return control
     *  to GDB for user input. The user can then return from this
     *  method and continue debugging from the call point.
     *
     *  @todo Actually attach GDB to the process at the time? --rue
     */
    // Ruby.primitive :yield_gdb
    static Object*  yield_gdb(STATE, Object* obj);

    /**
     *  Replace process with given program and args (exec()).
     */
    // Ruby.primitive :vm_exec
    static Object*  vm_exec(STATE, String* path, Array* args);

    // Ruby.primitive :vm_wait_pid
    static Object*  vm_wait_pid(STATE, Fixnum* pid, Object* no_hang, CallFrame* calling_environment);

    // Ruby.primitive :vm_exit
    static Object*  vm_exit(STATE, Fixnum* code);

    /**
     *  System fork()
     */
    // Ruby.primitive :vm_fork
    static Fixnum*  vm_fork(STATE);

    /**
     *  Force garbage collection as soon as possible.
     *
     */
    // Ruby.primitive :vm_gc_start
    static Object*  vm_gc_start(STATE, Object* force);

    /**
     *  Retrieve a value from VM configuration.
     *
     *  The config is essentially a hierarchical set of
     *  values, indexed by strings in the form of "a.b.c",
     *  for example "rbx.ffi.some_fictional_item".
     *
     *  @see  vm_get_config_section().
     */
    // Ruby.primitive :vm_get_config_item
    static Object*  vm_get_config_item(STATE, String* var);

    /**
     *  Retrieve all values from a VM config section.
     *
     *  For a section such as "rbx.ffi", an Array in the form
     *  of [[key1, val1], ...] is returned.
     *
     *  @see  vm_get_config_item().
     */
    // Ruby.primitive :vm_get_config_section
    static Object*  vm_get_config_section(STATE, String* section);

    /**
     *  Clears caches for any methods with given name.
     *
     *  Clears both the global cache and the SendSite caches.
     *  Typically used when e.g. a method is added to a Module's
     *  MethodTable in order to ensure that the correct method
     *  will be picked up from calls.
     */
    // Ruby.primitive :vm_reset_method_cache
    static Object*  vm_reset_method_cache(STATE, Symbol* name);

    /**
     *  Backtrace as an Array.
     */
    // Ruby.primitive :vm_backtrace
    static Array* vm_backtrace(STATE, Fixnum* skip, Object* inc_vars, CallFrame* calling_environment);

    // Ruby.primitive :vm_mri_backtrace
    static Array* vm_mri_backtrace(STATE, Fixnum* skip, CallFrame* calling_environment);

    /** Return true if the profiler is available. */
    // Ruby.primitive :vm_profiler_instrumenter_available_p
    static Object* vm_profiler_instrumenter_available_p(STATE);

    /** Return true if the profiler is running. */
    // Ruby.primitive :vm_profiler_instrumenter_active_p
    static Object* vm_profiler_instrumenter_active_p(STATE);

    /** Starts the instrumenting profiler. */
    // Ruby.primitive :vm_profiler_instrumenter_start
    static Object* vm_profiler_instrumenter_start(STATE);

    /** Stops the instrumenting profiler. */
    // Ruby.primitive :vm_profiler_instrumenter_stop
    static LookupTable* vm_profiler_instrumenter_stop(STATE);

    /**
     *  Writes String to standard error stream.
     */
    // Ruby.primitive :vm_write_error
    static Object*  vm_write_error(STATE, String* str);

    /**
     *  Returns information about how the JIT is working.
     */
    // Ruby.primitive :vm_jit_info
    static Object*  vm_jit_info(STATE);

    /**
     *  Returns information about the GC.
     */
    // Ruby.primitive :vm_stats_gc_clear
    static Object* vm_stats_gc_clear(STATE);

    // Ruby.primitive :vm_stats_gc_info
    static Object* vm_stats_gc_info(STATE);

    // Ruby.primitive :vm_memory_size
    static Fixnum* vm_memory_size(STATE, Object* obj);

    // Ruby.primitive :vm_watch_signal
    static Object*  vm_watch_signal(STATE, Fixnum* sig);

    // Ruby.primitive :vm_time
    static Object*  vm_time(STATE);

    // Ruby.primitive :vm_open_class
    static Class* vm_open_class(STATE, Symbol* name, Object* super, StaticScope* scope);

    // Ruby.primitive :vm_open_class_under
    static Class* vm_open_class_under(STATE, Symbol* name, Object* super, Module* under);

    // Ruby.primitive :vm_open_module
    static Module* vm_open_module(STATE, Symbol* name, StaticScope* scope);

    // Ruby.primitive :vm_open_module_under
    static Module* vm_open_module_under(STATE, Symbol* name, Module* under);

    // Ruby.primitive :vm_find_method
    static Tuple* vm_find_method(STATE, Object* recv, Symbol* name);

    // Ruby.primitive :vm_add_method
    static Object* vm_add_method(STATE, Symbol* name, CompiledMethod* meth, StaticScope* scope, Object* vis);

    // Ruby.primitive :vm_attach_method
    static Object* vm_attach_method(STATE, Symbol* name, CompiledMethod* meth, StaticScope* scope, Object* recv);

    // A robust way to get the class of an object, since Object#class can be redefined.
    // Ruby.primitive :vm_object_class
    static Class* vm_object_class(STATE, Object* obj);

    // A robust way to get the metaclass of an object.
    // Ruby.primitive :vm_object_metaclass
    static Object* vm_object_metaclass(STATE, Object* obj);

    // A robust way to find out if an object responds to a method, since #respond_to?
    // can be redefined.
    // Ruby.primitive :vm_object_respond_to
    static Object* vm_object_respond_to(STATE, Object* obj, Symbol* sym);

    // Increment the internal global serial number, used for caching
    // Ruby.primitive :vm_inc_global_serial
    static Object* vm_inc_global_serial(STATE);

    // Print out the current backtrace of ruby code
    // Ruby.primitive :vm_show_backtrace
    static Object* vm_show_backtrace(STATE, CallFrame* calling_environment);

    // Compile a block with the JIT
    // Ruby.primitive :vm_jit_block
    static Object* vm_jit_block(STATE, BlockEnvironment* env, Object* show);

    // Deoptimze any method that inlined exec
    // Ruby.primitive :vm_deoptimize_inliners
    static Object* vm_deoptimize_inliners(STATE, Executable* exec);

    // Ruby.primitive :vm_raise_exception
    static Object* vm_raise_exception(STATE, Exception* exc);

    // Ruby.primitive :vm_throw
    static Object* vm_throw(STATE, Symbol* dest, Object* value);

    // Ruby.primitive :vm_catch
    static Object* vm_catch(STATE, Symbol* dest, Object* obj, CallFrame* calling_environment);

    // Ruby.primitive :vm_set_class
    static Object* vm_set_class(STATE, Object* obj, Class* cls);

    // Ruby.primitive :vm_method_missing_reason
    static Object* vm_method_missing_reason(STATE);

    // Ruby.primitive :vm_extended_modules
    static Object* vm_extended_modules(STATE, Object* obj);

    // Ruby.primitive :vm_find_object
    static Object* vm_find_object(STATE, Array* arg, Object* callable, CallFrame* calling_environment);

    // Ruby.primitive :vm_set_kcode
    static Object* vm_set_kcode(STATE, String* what);

    // Ruby.primitive :vm_get_kcode
    static Symbol* vm_get_kcode(STATE);

    // Ruby.primitive :vm_const_defined
    static Object* vm_const_defined(STATE, Symbol* sym, CallFrame* calling_environment);

    // Ruby.primitive :vm_const_defined_under
    static Object* vm_const_defined_under(STATE, Module* under, Symbol* sym, Object* send_const_missing, CallFrame* calling_environment);

    // Ruby.primitive :vm_check_callable
    static Object* vm_check_callable(STATE, Object* obj, Symbol* sym, Object* self);

    // Ruby.primitive :vm_check_super_callable
    static Object* vm_check_super_callable(STATE, CallFrame* calling_environment);

    // Ruby.primitive :vm_get_user_home
    static String* vm_get_user_home(STATE, String* name);

    // Ruby.primitive :vm_agent_io
    static IO*     vm_agent_io(STATE);

    // Ruby.primitive :vm_dump_heap
    static Object* vm_dump_heap(STATE, String* path);

    // Ruby.primitive :vm_set_finalizer
    static Object* vm_set_finalizer(STATE, Object* obj, Object* fin);

  public:   /* Type info */

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) {}
      virtual ~Info() {}
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };


  private:  /* Disallowed operations */

    System();
    ~System();

    System(const System& other);
    System& operator=(System& other);

  };

}

#endif
