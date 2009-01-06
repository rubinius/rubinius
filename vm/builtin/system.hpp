#ifndef RBX_BUILTIN_SYSTEM_HPP
#define RBX_BUILTIN_SYSTEM_HPP

#include "vm/object_types.hpp"
#include "vm/type_info.hpp"

#include "builtin/object.hpp"


namespace rubinius {

  class Array;
  class Fixnum;
  class String;


  /**
   *  VM primitives etc.
   */
  class System : public Object {

  public:   /* Bookkeeping */

    /** No particular type needed. @todo Add VMType? --rue */
    static const object_type type = ObjectType;


  public:   /* Primitives */

    /** Load a compiled file. */
    // Ruby.primitive :compiledfile_load
    static Object*  compiledfile_load(STATE, String* path, Object* version);

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
     *  The tenure flag is NOT USED currently, but its
     *  intention is to indicate that all young objects
     *  should be tenured now.
     */
    // Ruby.primitive :vm_gc_start
    static Object*  vm_gc_start(STATE, Object* tenure);

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
     *  Writes backtrace to standard output.
     */
    // Ruby.primitive :vm_show_backtrace
    static Object*  vm_show_backtrace(STATE, Object* ctx);

    /**
     *  Starts the profiler.
     */
    // Ruby.primitive :vm_start_profiler
    static Object*  vm_start_profiler(STATE);

    /**
     *  Stops the profiler.
     */
    // Ruby.primitive :vm_stop_profiler
    static Object*  vm_stop_profiler(STATE, String* path);

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


  public:   /* Type info */

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) {}
      virtual ~Info() {}
    };


  private:  /* Disallowed operations */

    System();
    ~System();

    System(const System& other);
    System& operator=(System& other);

  };

}

#endif
