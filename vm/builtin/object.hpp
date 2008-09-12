#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include "oop.hpp"
#include "prelude.hpp"

namespace rubinius {
/**
* Create a writer method.
*
* For attr_writer(foo, SomeClass), creates void foo(STATE, SomeClass* obj)
* that sets the instance variable my_foo to the object given and runs the write
* barrier.
*/
#define attr_writer(name, type) void name(STATE, type* obj) { \
                                       name ## _ = obj; \
                                       this->write_barrier(state, (OBJECT)obj); \
                                     }

/**
* Create a reader method.
*
* For attr_reader(foo, SomeClass), creates SomeClass* foo() which returns the
* instance variable my_foo.
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
  class VMExecutable;
  class Task;
  class Array;
  class Message;
  class TypeInfo;
  class MethodContext;

  class Object : public ObjectHeader {
  public:

    /* Objects have no index fields past the header by default */
    const static size_t fields = 0;

    /* body access */
    union {
      OBJECT field[];
      uint8_t bytes[];
    };

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

    bool fixnum_p();
    bool symbol_p();

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init(gc_zone loc, size_t fields);

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields();

    /* Initialize the object as storing bytes, by setting the flag then clearing the
     * body of the object, by setting the entire body as bytes to 0 */
    void init_bytes();

    size_t size_in_bytes();
    size_t body_in_bytes();
    bool reference_p();
    bool stores_bytes_p();
    bool stores_references_p();
    bool young_object_p();
    bool mature_object_p();
    bool forwarded_p();
    void set_forward(STATE, OBJECT fwd);
    OBJECT forward();
    bool marked_p();
    void mark();
    void clear_mark();
    bool nil_p();
    bool undef_p();
    bool true_p();
    bool false_p();
    bool has_ivars_p();
    bool check_type(object_type type);

    /* Provides access to the GC write barrier from any object. */
    void write_barrier(STATE, OBJECT obj);

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

    // Ruby.primitive :object_dup
    OBJECT dup(STATE);

    // Ruby.primitive :object_clone
    OBJECT clone(STATE);

    // Ruby.primitive :object_id
    INTEGER id(STATE);

    OBJECT get_field(STATE, size_t index);
    void   set_field(STATE, size_t index, OBJECT val);
    void cleanup(STATE);

    bool kind_of_p(STATE, OBJECT cls);
    Class* lookup_begin(STATE);

    // Ruby.primitive :object_class
    Class* class_object(STATE);

    hashval hash(STATE);

    // Ruby.primitive :object_hash
    INTEGER hash_prim(STATE);

    Class* metaclass(STATE);

    // Ruby.primitive :object_get_ivar
    OBJECT get_ivar(STATE, OBJECT sym);

    // Ruby.primitive :object_set_ivar
    OBJECT set_ivar(STATE, OBJECT sym, OBJECT val);

    // Ruby.primitive :object_get_ivars
    OBJECT get_ivars(STATE);

    // Ruby.primitive :object_kind_of
    OBJECT kind_of_prim(STATE, Module* klass);

    // Ruby.primitive? :object_send
    bool send_prim(STATE, VMExecutable* exec, Task* task, Message& msg);

    // Setup the current task to send the method +meth+ to +this+ with a
    // variable number of arguments
    bool send(STATE, SYMBOL meth, size_t args, ...);

    // Setup +task+ to send the method +meth+ with +args+ to +this+
    bool send_on_task(STATE, Task* task, SYMBOL name, Array* args);

    void copy_flags(STATE, OBJECT other);
    void copy_ivars(STATE, OBJECT other);
    void copy_metaclass(STATE, OBJECT other);

    static const char* type_to_name(object_type type);

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

    // Ruby.primitive :yield_gdb
    static Object* yield_gdb(STATE, Object* obj);
  };

}

#endif
