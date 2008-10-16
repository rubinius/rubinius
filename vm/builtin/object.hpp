#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include "vm.hpp"
#include "vm/oop.hpp"
#include "vm/prelude.hpp"
#include "vm/type_info.hpp"

namespace rubinius {

/**
* Create a writer method.
*
* For attr_writer(foo, SomeClass), creates void foo(STATE, SomeClass* obj)
* that sets the instance variable foo_ to the object given and runs the write
* barrier.
*/
#define attr_writer(name, type) \
  void name(STATE, type* obj) { \
    name ## _ = obj; \
    if(zone == MatureObjectZone) this->write_barrier(state, obj); \
  }

/**
* Create a reader method.
*
* For attr_reader(foo, SomeClass), creates SomeClass* foo() which returns the
* instance variable foo_.
*/
#define attr_reader(name, type) type* name() { return name ## _; }

/**
* Ruby-like accessor creation.
*
* Both attr_writer and attr_reader.
*/
#define attr_accessor(name, type) attr_reader(name, type) \
                                  attr_writer(name, type)

  class MetaClass;
  class Integer;
  class String;
  class Module;
  class Executable;
  class Task;
  class Array;
  class Message;
  class TypeInfo;
  class MethodContext;

  class Object : public ObjectHeader {
  public:

    /* Objects have no index fields past the header by default */
    static const size_t fields = 0;
    static const object_type type = ObjectType;

    /* accessors */
    attr_accessor(klass, Class);
    attr_accessor(ivars, Object);

    // Ruby.primitive :object_equal
    OBJECT equal(STATE, OBJECT other);

    // Ruby.primitive :object_show
    OBJECT show(STATE);
    OBJECT show(STATE, int level);
    OBJECT show_simple(STATE);
    OBJECT show_simple(STATE, int level);

    /* Initialize the object as storing bytes, by setting the flag then clearing the
     * body of the object, by setting the entire body as bytes to 0 */
    void init_bytes();

    void set_forward(STATE, OBJECT fwd);

    /* Provides access to the GC write barrier from any object.
     * We use void* as the type for obj to work around C++'s type system
     * that requires full definitions of classes to be present for it
     * figure out if you can properly pass an object (the superclass
     * has to be known).
     *
     * If we have OBJECT obj here, then we either have to cast to call
     * write_barrier (which means we lose the ability to have type specific
     * write_barrier versions, which we do), or we have to include
     * every header up front. We opt for the former. */
    void write_barrier(STATE, void* obj);

    // Safely return the object type, even if the receiver is an immediate
    object_type get_type();

    // Return the TypeInfo object used to reflect on an object of this
    // type.
    TypeInfo* type_info(STATE);

    // Ruby.primitive :object_tainted_p
    OBJECT tainted_p();

    // Ruby.primitive :object_taint
    OBJECT taint();

    // Ruby.primitive :object_untaint
    OBJECT untaint();

    // Ruby.primitive :object_freeze
    OBJECT freeze();

    // Ruby.primitive :object_frozen_p
    OBJECT frozen_p();

    /** Copies metaclass from original to this one. @see clone(). */
    void    copy_internal_state_from(STATE, Object* original);

    // Ruby.primitive :object_clone
    OBJECT  clone(STATE);

    // Ruby.primitive :object_dup
    OBJECT  dup(STATE);

    // Ruby.primitive :object_id
    INTEGER id(STATE);

    OBJECT get_field(STATE, size_t index);
    void   set_field(STATE, size_t index, OBJECT val);
    void cleanup(STATE);

    bool kind_of_p(STATE, OBJECT cls);

    // Ruby.primitive :object_class
    Class* class_object(STATE);

    hashval hash(STATE);

    // Ruby.primitive :object_hash
    INTEGER hash_prim(STATE);

    Class* metaclass(STATE);

    // Ruby.primitive :object_get_ivar
    OBJECT get_ivar(STATE, SYMBOL sym);

    // Ruby.primitive :object_set_ivar
    OBJECT set_ivar(STATE, SYMBOL sym, OBJECT val);

    // Ruby.primitive :object_get_ivars
    OBJECT get_ivars(STATE);

    // Ruby.primitive :object_kind_of
    OBJECT kind_of_prim(STATE, Module* klass);

    // Ruby.primitive? :object_send
    bool send_prim(STATE, Executable* exec, Task* task, Message& msg);

    // Setup the current task to send the method +meth+ to +this+ with a
    // variable number of arguments
    bool send(STATE, SYMBOL meth, size_t args, ...);

    // Setup +task+ to send the method +meth+ with +args+ to +this+
    bool send_on_task(STATE, Task* task, SYMBOL name, Array* args);

    void copy_flags(STATE, OBJECT other);
    void copy_ivars(STATE, OBJECT other);
    void copy_metaclass(STATE, OBJECT other);

    /* VM level primitives. It's dumb to have them here, but it's more
     * complicated to make field_extract parse non-Object classes. */

    // Ruby.primitive :vm_get_config_item
    static OBJECT vm_get_config_item(STATE, String* var);

    // Ruby.primitive :vm_get_config_section
    static OBJECT vm_get_config_section(STATE, String* section);

    // Ruby.primitive :vm_write_error
    static OBJECT vm_write_error(STATE, String* str);

    // Ruby.primitive :vm_reset_method_cache
    static OBJECT vm_reset_method_cache(STATE, SYMBOL name);

    // Ruby.primitive :vm_exit
    static OBJECT vm_exit(STATE, FIXNUM code);

    // Ruby.primitive :vm_show_backtrace
    static OBJECT vm_show_backtrace(STATE, MethodContext* ctx);

    // Ruby.primitive :vm_start_profiler
    static OBJECT vm_start_profiler(STATE);

    // Ruby.primitive :vm_stop_profiler
    static OBJECT vm_stop_profiler(STATE, String* path);

    // Perform GC as soon as possible. Tenure is an older flag
    // which means "please tenure all young objects now". It
    // is currently not enabled.
    //
    // Ruby.primitive :vm_gc_start
    static OBJECT vm_gc_start(STATE, OBJECT tenure);

    // Ruby.primitive :yield_gdb
    static Object* yield_gdb(STATE, Object* obj);

    // Hackety HACK, don't talk back; just remove compiledfile_load
    // when ruby performance is better

    // Ruby.primitive :compiledfile_load
    static OBJECT compiledfile_load(STATE, String* path, OBJECT version);

    // Overloads to shortcut (using C++'s type system) the write
    // barrier when storing Fixnums or Symbols
    void write_barrier(STATE, Fixnum* obj) { }
    void write_barrier(STATE, Symbol* obj) { }

    bool fixnum_p() {
      return FIXNUM_P(this);
    }

    bool symbol_p() {
      return SYMBOL_P(this);
    }

    Class* lookup_begin(STATE) {
      if(reference_p()) return klass_;
      return state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
    }

  public:

    /**
     *  Static type information for Object.
     */
    class Info : public TypeInfo {
    public:
      virtual ~Info() {}

      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
    };

  };

}

#endif
