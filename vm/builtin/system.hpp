#ifndef RBX_BUILTIN_SYSTEM_HPP
#define RBX_BUILTIN_SYSTEM_HPP

#include "builtin/object.hpp"

namespace rubinius {

  class Array;
  class Fixnum;
  class String;
  class BlockEnvironment;
  class CompiledCode;
  class ConstantScope;
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

    static void bootstrap_methods(STATE);
    static void attach_primitive(STATE, GCToken gct, Module* mod, bool meta, Symbol* name, Symbol* prim);

    /** Load a compiled file. */
    // Rubinius.primitive :compiledfile_load
    static Object*  compiledfile_load(STATE, String* path, Integer* signature, Integer* version);

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
    // Rubinius.primitive :yield_gdb
    static Object*  yield_gdb(STATE, Object* obj);

    /**
     *  Replace process with given program and args (exec()).
     */
    // Rubinius.primitive :vm_exec
    static Object*  vm_exec(STATE, String* path, Array* args, CallFrame* calling_environment);

    // Rubinius.primitive :vm_replace
    static Object*  vm_replace(STATE, String* str, CallFrame* calling_environment);

    // Rubinius.primitive :vm_wait_pid
    static Object*  vm_wait_pid(STATE, Fixnum* pid, Object* no_hang, CallFrame* calling_environment);

    // Rubinius.primitive :vm_exit
    static Object*  vm_exit(STATE, Fixnum* code);

    /**
     *  System fork()
     */
    // Rubinius.primitive :vm_fork
    static Fixnum*  vm_fork(STATE, GCToken gct, CallFrame* calling_environment);

    /**
     *  Force garbage collection as soon as possible.
     *
     */
    // Rubinius.primitive :vm_gc_start
    static Object*  vm_gc_start(STATE, GCToken gct, Object* force, CallFrame* calling_environment);

    /**
     *  Retrieve the number of garbage collections
     *
     */
    // Rubinius.primitive+ :vm_gc_count
    static Integer*  vm_gc_count(STATE);

    /**
     *  Retrieve the amount of memory allocated by the gc
     *
     */
    // Rubinius.primitive+ :vm_gc_size
    static Integer*  vm_gc_size(STATE);

    /**
     *  Retrieve the total time spent garbage collecting
     *
     */
    // Rubinius.primitive+ :vm_gc_time
    static Integer*  vm_gc_time(STATE);

    /**
     *  Retrieve a value from VM configuration.
     *
     *  The config is essentially a hierarchical set of
     *  values, indexed by strings in the form of "a.b.c",
     *  for example "rbx.ffi.some_fictional_item".
     *
     *  @see  vm_get_config_section().
     */
    // Rubinius.primitive :vm_get_config_item
    static Object*  vm_get_config_item(STATE, String* var);

    /**
     *  Retrieve all values from a VM config section.
     *
     *  For a section such as "rbx.ffi", an Array in the form
     *  of [[key1, val1], ...] is returned.
     *
     *  @see  vm_get_config_item().
     */
    // Rubinius.primitive :vm_get_config_section
    static Object*  vm_get_config_section(STATE, String* section);

    /**
     *  Clears caches for any methods with given name.
     *
     *  Clears both the global cache and the SendSite caches.
     *  Typically used when e.g. a method is added to a Module's
     *  MethodTable in order to ensure that the correct method
     *  will be picked up from calls.
     */
    // Rubinius.primitive :vm_reset_method_cache
    static Object*  vm_reset_method_cache(STATE, Module* mod, Symbol* name, CallFrame* calling_environment);

    /**
     *  Backtrace as an Array.
     */
    // Rubinius.primitive :vm_backtrace
    static Array* vm_backtrace(STATE, Fixnum* skip, Object* inc_vars, CallFrame* calling_environment);

    // Rubinius.primitive :vm_mri_backtrace
    static Array* vm_mri_backtrace(STATE, Fixnum* skip, CallFrame* calling_environment);

    // Load a tool into the VM.
    // Rubinius.primitive :vm_load_tool
    static Object* vm_load_tool(STATE, String* str);

    /** Return true if tooling is enabled */
    // Rubinius.primitive+ :vm_tooling_available_p
    static Object* vm_tooling_available_p(STATE);

    /** Return true if tooling is running. */
    // Rubinius.primitive+ :vm_tooling_active_p
    static Object* vm_tooling_active_p(STATE);

    /** Starts tooling. */
    // Rubinius.primitive+ :vm_tooling_enable
    static Object* vm_tooling_enable(STATE);

    /** Stops tooling. */
    // Rubinius.primitive+ :vm_tooling_disable
    static Object* vm_tooling_disable(STATE);

    /**
     *  Writes String to standard error stream.
     */
    // Rubinius.primitive :vm_write_error
    static Object*  vm_write_error(STATE, String* str);

    /**
     *  Returns information about how the JIT is working.
     */
    // Rubinius.primitive :vm_jit_info
    static Object*  vm_jit_info(STATE);

    // Rubinius.primitive+ :vm_memory_size
    static Fixnum* vm_memory_size(STATE, Object* obj);

    // Rubinius.primitive :vm_watch_signal
    static Object*  vm_watch_signal(STATE, Fixnum* sig, Object* ignored);

    // Rubinius.primitive+ :vm_time
    static Object*  vm_time(STATE);

    // Rubinius.primitive :vm_sleep
    static Object*  vm_sleep(STATE, GCToken gct, Object* duration, CallFrame* calling_environment);

    // Rubinius.primitive :vm_check_interrupts
    static Object*  vm_check_interrupts(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :vm_times
    static Array*   vm_times(STATE);

    // Rubinius.primitive :vm_open_class
    static Class* vm_open_class(STATE, Symbol* name, Object* super, ConstantScope* scope);

    // Rubinius.primitive :vm_open_class_under
    static Class* vm_open_class_under(STATE, Symbol* name, Object* super, Module* under);

    // Rubinius.primitive :vm_open_module
    static Module* vm_open_module(STATE, Symbol* name, ConstantScope* scope);

    // Rubinius.primitive :vm_open_module_under
    static Module* vm_open_module_under(STATE, Symbol* name, Module* under);

    // Rubinius.primitive :vm_find_method
    static Tuple* vm_find_method(STATE, Object* recv, Symbol* name);

    // Rubinius.primitive :vm_find_public_method
    static Tuple* vm_find_public_method(STATE, Object* recv, Symbol* name);

    // Rubinius.primitive :vm_add_method
    static Object* vm_add_method(STATE, GCToken gct, Symbol* name, CompiledCode* meth, ConstantScope* scope, Object* vis, CallFrame* calling_environment);

    // Rubinius.primitive :vm_attach_method
    static Object* vm_attach_method(STATE, GCToken gct, Symbol* name, CompiledCode* meth, ConstantScope* scope, Object* recv, CallFrame* calling_environment);

    // A robust way to get the class of an object, since Object#class can be redefined.
    // Rubinius.primitive+ :vm_object_class
    static Class* vm_object_class(STATE, Object* obj);

    // A robust way to get the singleton class of an object.
    // Rubinius.primitive :vm_object_singleton_class
    static Object* vm_object_singleton_class(STATE, Object* obj);

    // A robust way to get the object for the given singleton class.
    // Rubinius.primitive+ :vm_singleton_class_object
    static Object* vm_singleton_class_object(STATE, Module* mod);

    // A robust way to find out if an object responds to a method, since #respond_to?
    // can be redefined.
    // Rubinius.primitive+ :vm_object_respond_to
    static Object* vm_object_respond_to(STATE, Object* obj, Symbol* sym, Object* include_private);

    // A robust way to find out if two references are the same, since #equal? can
    // be redefined
    // Rubinius.primitive+ :vm_object_equal
    static Object* vm_object_equal(STATE, Object* a, Object* b);

    // A robust way to find out if an object is a kind of a Module, since #kind_of?
    // can be redefined
    // Rubinius.primitive+ :vm_object_kind_of
    static Object* vm_object_kind_of(STATE, Object* obj, Module* mod);

    // Increment the internal global serial number, used for caching
    // Rubinius.primitive :vm_inc_global_serial
    static Object* vm_inc_global_serial(STATE, CallFrame* calling_environment);

    // Print out the current backtrace of ruby code
    // Rubinius.primitive :vm_show_backtrace
    static Object* vm_show_backtrace(STATE, CallFrame* calling_environment);

    // Deoptimize all methods.
    // +disable+ indicates if the methods should also be pulled from being
    // available for JIT.
    // Rubinius.primitive :vm_deoptimize_all
    static Object* vm_deoptimize_all(STATE, Object* disable);

    // Rubinius.primitive :vm_raise_exception
    static Object* vm_raise_exception(STATE, Exception* exc);

    // Rubinius.primitive :vm_throw
    static Object* vm_throw(STATE, Object* dest, Object* value);

    // Rubinius.primitive :vm_catch
    static Object* vm_catch(STATE, Object* dest, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive :vm_set_class
    static Object* vm_set_class(STATE, Object* obj, Class* cls);

    // Rubinius.primitive+ :vm_method_missing_reason
    static Object* vm_method_missing_reason(STATE);

    // Rubinius.primitive+ :vm_constant_missing_reason
    static Object* vm_constant_missing_reason(STATE);

    // Rubinius.primitive :vm_extended_modules
    static Object* vm_extended_modules(STATE, Object* obj);

    // Rubinius.primitive :vm_find_object
    static Object* vm_find_object(STATE, GCToken gct, Array* arg, Object* callable, CallFrame* calling_environment);

    // Rubinius.primitive :vm_set_kcode
    static Object* vm_set_kcode(STATE, String* what);

    // Rubinius.primitive :vm_get_kcode
    static Symbol* vm_get_kcode(STATE);

    // Rubinius.primitive :vm_const_defined
    static Object* vm_const_defined(STATE, Symbol* sym, CallFrame* calling_environment);

    // Rubinius.primitive :vm_const_defined_under
    static Object* vm_const_defined_under(STATE, Module* under, Symbol* sym, Object* send_const_missing, CallFrame* calling_environment);

    // Rubinius.primitive :vm_check_callable
    static Object* vm_check_callable(STATE, Object* obj, Symbol* sym, Object* self, CallFrame* calling_environment);

    // Rubinius.primitive :vm_check_super_callable
    static Object* vm_check_super_callable(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :vm_get_user_home
    static String* vm_get_user_home(STATE, String* name);

    // Rubinius.primitive :vm_agent_start
    static Object* vm_agent_start(STATE);

    // Rubinius.primitive :vm_agent_loopback
    static IO*     vm_agent_loopback(STATE);

    // Rubinius.primitive :vm_dump_heap
    static Object* vm_dump_heap(STATE, String* path);

    // Rubinius.primitive :vm_set_finalizer
    static Object* vm_set_finalizer(STATE, Object* obj, Object* fin);

    // Rubinius.primitive :vm_object_lock
    static Object* vm_object_lock(STATE, GCToken gct, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive :vm_object_uninterrupted_lock
    static Object* vm_object_uninterrupted_lock(STATE, GCToken gct, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive :vm_object_lock_timed
    static Object* vm_object_lock_timed(STATE, GCToken gct, Object* obj, Integer* time, CallFrame* calling_environment);

    // Rubinius.primitive+ :vm_object_trylock
    static Object* vm_object_trylock(STATE, GCToken gct, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive+ :vm_object_locked_p
    static Object* vm_object_locked_p(STATE, GCToken gct, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive :vm_object_unlock
    static Object* vm_object_unlock(STATE, GCToken gct, Object* obj, CallFrame* calling_environment);

    // Rubinius.primitive+ :vm_memory_barrier
    static Object* vm_memory_barrier(STATE);

    // Rubinius.primitive+ :vm_ruby18_p
    static Object* vm_ruby18_p(STATE);

    // Rubinius.primitive+ :vm_ruby19_p
    static Object* vm_ruby19_p(STATE);

    // Rubinius.primitive+ :vm_ruby20_p
    static Object* vm_ruby20_p(STATE);

    // Rubinius.primitive+ :vm_windows_p
    static Object* vm_windows_p(STATE);

    // Rubinius.primitive+ :vm_darwin_p
    static Object* vm_darwin_p(STATE);

    // Rubinius.primitive+ :vm_bsd_p
    static Object* vm_bsd_p(STATE);

    // Rubinius.primitive+ :vm_linux_p
    static Object* vm_linux_p(STATE);

    // Rubinius.primitive :sha1_hash
    static String* sha1_hash(STATE, String* str);

    // Rubinius.primitive :vm_thread_state
    static Tuple* vm_thread_state(STATE);

    // Rubinius.primitive :vm_run_script
    static Object* vm_run_script(STATE, GCToken gct, CompiledCode* code, CallFrame* calling_environment);

    // Rubinius.primitive :vm_hash_trie_item_index
    static Fixnum* vm_hash_trie_item_index(STATE, Fixnum* hash, Fixnum* level, Integer* map);

    // Rubinius.primitive :vm_hash_trie_set_bitmap
    static Integer* vm_hash_trie_set_bitmap(STATE, Fixnum* hash, Fixnum* level, Integer* map);

    // Rubinius.primitive :vm_hash_trie_unset_bitmap
    static Integer* vm_hash_trie_unset_bitmap(STATE, Fixnum* hash, Fixnum* level, Integer* map);

    // Rubinius.primitive :vm_get_module_name
    static String* vm_get_module_name(STATE, Module* mod);

    // Rubinius.primitive :vm_set_module_name
    static Object* vm_set_module_name(STATE, Module* mod, Object* name, Object* under);

    // Rubinius.primitive :vm_set_process_title
    static String* vm_set_process_title(STATE, String* name);

    // Rubinius.primitive :vm_dtrace_fire
    static Object* vm_dtrace_fire(STATE, String* payload);

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
