#ifndef RBX_BUILTIN_SYSTEM_HPP
#define RBX_BUILTIN_SYSTEM_HPP

#include "class/object.hpp"

namespace rubinius {

  class Array;
  class Fixnum;
  class String;
  class BlockEnvironment;
  class CompiledCode;
  class LexicalScope;
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
    static void attach_primitive(STATE, Module* mod, bool meta, Symbol* name, Symbol* prim);

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
    static Object*  vm_exec(STATE, String* path, Array* args);

    // Rubinius.primitive :vm_spawn_setup
    static Object*  vm_spawn_setup(STATE, Object* spawn_state);

    // Rubinius.primitive :vm_spawn
    static Object*  vm_spawn(STATE, Object* spawn_state, String* path, Array* args);

    // Rubinius.primitive :vm_backtick
    static Object*  vm_backtick(STATE, String* str);

    // Rubinius.primitive :vm_waitpid
    static Object*  vm_waitpid(STATE, Fixnum* pid, Object* no_hang);

    // Rubinius.primitive :vm_exit
    static Object*  vm_exit(STATE, Fixnum* code);

    /**
     *  System fork()
     */
    // Rubinius.primitive :vm_fork
    static Fixnum*  vm_fork(STATE);

    /**
     *  Force garbage collection as soon as possible.
     *
     */
    // Rubinius.primitive :vm_gc_start
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
    static Object*  vm_reset_method_cache(STATE, Module* mod, Symbol* name);

    /**
     *  Backtrace as an Array.
     */
    // Rubinius.primitive :vm_backtrace
    static Array* vm_backtrace(STATE, Fixnum* skip);

    // Rubinius.primitive :vm_mri_backtrace
    static Array* vm_mri_backtrace(STATE, Fixnum* skip);

    /**
     *  Writes String to standard error stream.
     */
    // Rubinius.primitive :vm_write_error
    static Object*  vm_write_error(STATE, String* str);

    // Rubinius.primitive+ :vm_memory_size
    static Fixnum* vm_memory_size(STATE, Object* obj);

    // Rubinius.primitive :vm_watch_signal
    static Object*  vm_watch_signal(STATE, Fixnum* sig, Object* ignored);

    // Rubinius.primitive :vm_signal_thread
    static Object* vm_signal_thread(STATE);

    // Rubinius.primitive+ :vm_time
    static Object*  vm_time(STATE);

    // Rubinius.primitive :vm_times
    static Array*   vm_times(STATE);

    // Rubinius.primitive :vm_open_class
    static Class* vm_open_class(STATE, Symbol* name, Object* super, LexicalScope* scope);

    // Rubinius.primitive :vm_open_class_under
    static Class* vm_open_class_under(STATE, Symbol* name, Object* super, Module* under);

    // Rubinius.primitive :vm_open_module
    static Module* vm_open_module(STATE, Symbol* name, LexicalScope* scope);

    // Rubinius.primitive :vm_open_module_under
    static Module* vm_open_module_under(STATE, Symbol* name, Module* under);

    // Rubinius.primitive :vm_find_method
    static Tuple* vm_find_method(STATE, Object* recv, Symbol* name);

    // Rubinius.primitive :vm_find_public_method
    static Tuple* vm_find_public_method(STATE, Object* recv, Symbol* name);

    // Rubinius.primitive :vm_add_method
    static Object* vm_add_method(STATE, Symbol* name, Object* method, LexicalScope* scope, Object* vis);

    // Rubinius.primitive :vm_attach_method
    static Object* vm_attach_method(STATE, Symbol* name, Object* method, LexicalScope* scope, Object* recv);

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

    // Return the internal global serial number, used for caching
    // Rubinius.primitive :vm_global_serial
    static Object* vm_global_serial(STATE);

    // Increment the internal global serial number, used for caching
    // Rubinius.primitive :vm_inc_global_serial
    static Object* vm_inc_global_serial(STATE);

    // Print out the current backtrace of ruby code
    // Rubinius.primitive :vm_show_backtrace
    static Object* vm_show_backtrace(STATE);

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
    static Object* vm_catch(STATE, Object* dest, Object* obj);

    // Rubinius.primitive :vm_set_class
    static Object* vm_set_class(STATE, Object* obj, Class* cls);

    // Rubinius.primitive+ :vm_method_missing_reason
    static Object* vm_method_missing_reason(STATE);

    // Rubinius.primitive+ :vm_constant_missing_reason
    static Object* vm_constant_missing_reason(STATE);

    // Rubinius.primitive :vm_extended_modules
    static Object* vm_extended_modules(STATE, Object* obj);

    // Rubinius.primitive :vm_find_object
    static Object* vm_find_object(STATE, Array* arg, Object* callable);

    // Rubinius.primitive :vm_check_callable
    static Object* vm_check_callable(STATE, Object* obj, Symbol* sym, Object* self);

    // Rubinius.primitive :vm_check_super_callable
    static Object* vm_check_super_callable(STATE);

    // Rubinius.primitive :vm_get_user_home
    static String* vm_get_user_home(STATE, String* name);

    // Rubinius.primitive :vm_set_finalizer
    static Object* vm_set_finalizer(STATE, Object* obj, Object* fin);

    // Rubinius.primitive :vm_object_lock
    static Object* vm_object_lock(STATE, Object* obj);

    // Rubinius.primitive+ :vm_object_trylock
    static Object* vm_object_trylock(STATE, Object* obj);

    // Rubinius.primitive+ :vm_object_locked_p
    static Object* vm_object_locked_p(STATE, Object* obj);

    // Rubinius.primitive+ :vm_object_lock_owned_p
    static Object* vm_object_lock_owned_p(STATE, Object* obj);

    // Rubinius.primitive :vm_object_unlock
    static Object* vm_object_unlock(STATE, Object* obj);

    // Rubinius.primitive+ :vm_memory_barrier
    static Object* vm_memory_barrier(STATE);

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
    static Object* vm_run_script(STATE, CompiledCode* code);

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
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
    };


  private:  /* Disallowed operations */

    System();
    ~System();

    System(const System& other);
    System& operator=(System& other);

  };

}

#endif
