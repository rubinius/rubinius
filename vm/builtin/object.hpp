#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include <vector>

#include "vm.hpp"
#include "vm/oop.hpp"
#include "vm/type_info.hpp"

#include "executor.hpp"


namespace rubinius {

/**
 *  Create a writer method for a slot.
 *
 *  For attr_writer(foo, SomeClass), creates void foo(STATE, SomeClass* obj)
 *  that sets the instance variable foo_ to the object given and runs the write
 *  barrier.
 */
#define attr_writer(name, type) \
  void name(STATE, type* obj) { \
    name ## _ = obj; \
    if(zone == MatureObjectZone) this->write_barrier(state, obj); \
  }

/**
 *  Create a reader method for a slot.
 *
 *  For attr_reader(foo, SomeClass), creates SomeClass* foo() which returns the
 *  instance variable foo_. A const version is also generated.
 */
#define attr_reader(name, type) type* name() { return name ## _; } \
                                const type* name() const { return name ## _; }

/**
 *  Ruby-like accessor creation for a slot.
 *
 *  Both attr_writer and attr_reader.
 */
#define attr_accessor(name, type) attr_reader(name, type) \
                                  attr_writer(name, type)

  /* Forwards */
  class MetaClass;
  class Fixnum;
  class Integer;
  class String;
  class Module;
  class Executable;
  class Task;
  class Array;
  class Message;
  class TypeInfo;
  class MethodContext;

  class Object;

  typedef std::vector<Object*> ObjectArray;


  /**
   *  Object is the basic Ruby object.
   *
   *  Because of needing to exactly control the in-memory layout of
   *  Objects, none of the subclasses of Object (or ObjectHeader)
   *  is allowed to use define virtual methods. In order to achieve
   *  virtual dispatch for the cases where it is needed (it usually
   *  is not), the TypeInfo class should be used. Each builtin class
   *  defines a contained type Info which inherits--directly or not
   *  from TypeInfo. TypeInfos use virtual methods to achieve dynamism.
   *  Instances of these Info classes are held by the VM, where they
   *  can be retrieved using a given Object's object_type. The Info
   *  object will then dispatch the correct virtual method for the
   *  type. The methods are written to understand the types they are
   *  dealing with.
   *
   *  The class definition layout differs from the normal alphabetic
   *  order in order to group the different aspects involved better
   *  and to simplify finding/adding &c. related methods.
   *
   *  @todo The entire codebase should be reviewed for const
   *        correctness. --rue
   *
   *  @todo Remove the unimplemented definitions? --rue
   *
   *  @see  vm/type_info.hpp
   *
   */
  class Object : public ObjectHeader {
  public:   /* Slots and bookkeeping */

    /** Objects have no index fields past the header by default */
    static const size_t fields = 0;

    /** Class type identifier. */
    static const object_type type = ObjectType;

  public:   /* GC support, bookkeeping &c. */

    /** Calls cleanup() on the TypeInfo for this object's type. */
    void        cleanup(STATE);

    /**
     *  Initialize the object to store bytes.
     *
     *  Sets flag and overwrites body with NULLs.
     */
    void        init_bytes();

    /**
     *  Mark this Object forwarded by the GC.
     *
     *  Sets the forwarded flag and stores the given Object* in
     *  the klass_ field where it can be reached. This object is
     *  no longer valid and should be accessed through the new
     *  Object* (but code outside of the GC framework should not
     *  really run into this much if at all.)
     *
     *  @todo Clarify the scenarios where an Object may exist
     *        forwarded in user code if at all. --rue
     */
    void        set_forward(STATE, Object* fwd);

    /** Provides access to the GC write barrier from any object. */
    void        write_barrier(STATE, void* obj);
    /** Special-case write_barrier() for Fixnums. */
    void        write_barrier(STATE, Fixnum* obj);
    /** Special-case write_barrier() for Symbols. */
    void        write_barrier(STATE, Symbol* obj);


  public:   /* Type information, field access, copy support &c. */

    /** Sets the other Object's flags the same as this. @see vm/oop.hpp. */
    void        copy_flags(STATE, Object* other);
    /** Copies metaclass from original to this one. @see clone(). */
    void        copy_internal_state_from(STATE, Object* original);
    /** NOT IMPLEMENTED. Copies instance variables to the other Object. */
    void        copy_ivars(STATE, Object* other);
    /** NOT IMPLEMENTED. Copies this Object's MetaClass to the other Object. */
    void        copy_metaclass(STATE, Object* other);

    /** True if this Object* is actually a Fixnum, false otherwise. */
    bool        fixnum_p() const;

    /**
     *  Retrieve the Object stored in given field index of this Object.
     *
     *  Uses TypeInfo.
     */
    Object*     get_field(STATE, std::size_t index);

    /** Safely return the object type, even if the receiver is an immediate. */
    object_type get_type() const;

    /** True if given Module is this Object's class, superclass or an included Module. */
    bool        kind_of_p(STATE, Object* module);

    /** Store the given Object at given field index of this Object through TypeInfo. */
    void        set_field(STATE, std::size_t index, Object* val);

    /** True if this Object* is actually a Symbol, false otherwise. */
    bool        symbol_p() const;

    /** Return the TypeInfo for this Object's type. */
    TypeInfo*   type_info(STATE) const;


  public:   /* Method dispatch stuff */

    /** Class object in which to look for the MethodTable for this Object. */
    Class*      lookup_begin(STATE);

    /**
     *  Directly send a method to this Object.
     *
     *  Sets up the current task to send the given method name to this
     *  Object, passing the given number of arguments through varargs.
     *
     *  Uses Task::send_message_slowly().
     */
    bool      send(STATE, Symbol* meth, size_t args, ...);

    /**
     *  Directly send a method on this Object.
     *
     *  Sets up the current task to send the given method name on this
     *  Object, passing arguments in a Ruby Array.
     *
     *  Uses Task::send_message_slowly().
     */
    bool      send_on_task(STATE, Task* task, Symbol* name, Array* args);

    /**
     *  Perform a send from Ruby.
     *
     *  Uses Task::send_message_slowly().
     *
     *  @todo Add more information, when is this used? --rue
     */
    // Ruby.primitive? :object_send
    ExecuteStatus send_prim(STATE, Executable* exec, Task* task, Message& msg);


  public:   /* Ruby interface */

    /**
     *  Ruby #clone.
     *
     *  Creates and returns a new Object that is a copy of this one
     *  including internal state.
     *
     *  @see  dup()
     */
    // Ruby.primitive :object_clone
    Object*   clone(STATE);

    /** Returns the Class object of which this Object is an instance. */
    // Ruby.primitive :object_class
    Class*    class_object(STATE) const;

    /**
     *  Ruby #dup.
     *
     *  Creates and returns a new Object that is a copy of this one
     *  except for internal state.
     *
     *  @see  clone()
     */
    // Ruby.primitive :object_dup
    Object*   dup(STATE);

    /**
     *  Ruby #equal?.
     *
     *  Returns true if and only if this and the other object are
     *  the SAME object, false otherwise.
     */
    // Ruby.primitive :object_equal
    Object*   equal(STATE, Object* other);

    /** Sets the frozen flag. Rubinius does NOT currently support freezing. */
    // Ruby.primitive :object_freeze
    Object*   freeze();

    /** Returns true if this Object's frozen flag set, false otherwise. */
    // Ruby.primitive :object_frozen_p
    Object*   frozen_p();

    /**
     *  Ruby #instance_variable_get.
     *
     *  Return the Object stored as instance variable under the given
     *  identifier.
     */
    // Ruby.primitive :object_get_ivar
    Object*   get_ivar(STATE, Symbol* sym);

    /** Returns the structure containing this object's instance variables. */
    // Ruby.primitive :object_get_ivars
    Object*   get_ivars(STATE);

    /** Calculate a hash value for this object. */
    hashval   hash(STATE);

    /** Returns the hash value as an Integer. @see hash(). */
    // Ruby.primitive :object_hash
    Integer*  hash_prim(STATE);

    /** Returns an Integer ID for this object. Created as needed. */
    // Ruby.primitive :object_id
    Integer*  id(STATE);

    /**
     * Taints other if this is tainted.
     */
    void infect(Object* other);

    /**
     *  Ruby #kind_of?
     *
     *  Returns true if given Module is this Object's class,
     *  superclass or an included Module, false otherwise.
     */
    // Ruby.primitive :object_kind_of
    Object*   kind_of_prim(STATE, Module* klass);

    /** Return object's MetaClass object. Created as needed. */
    Class*    metaclass(STATE);

    /**
     *  Ruby #instance_variable_set
     *
     *  Store the given object in the instance variable by
     *  given identifier.
     */
    // Ruby.primitive :object_set_ivar
    Object*   set_ivar(STATE, Symbol* sym, Object* val);

    /** String describing this object (through TypeInfo.) */
    // Ruby.primitive :object_show
    Object*   show(STATE);
    /** Indented String describing this object (through TypeInfo.) */
    Object*   show(STATE, int level);
    /** Shorter String describing this object (through TypeInfo.) */
    Object*   show_simple(STATE);
    /** Shorter indented String describing this object (through TypeInfo.) */
    Object*   show_simple(STATE, int level);

    /**
     *  Set tainted flag on this object.
     *
     *  Rubinius DOES NOT currently support tainting.
     */
    // Ruby.primitive :object_taint
    Object*   taint();

    /**
     *  Returns true if this object's tainted flag is set.
     *
     *  Rubinius DOES NOT currently support tainting.
     */
    // Ruby.primitive :object_tainted_p
    Object*   tainted_p();

    /**
     *  Clears the tainted flag on this object.
     *
     *  Rubinius DOES NOT currently support tainting.
     */
    // Ruby.primitive :object_untaint
    Object*   untaint();


  public:   /* accessors */

    /* klass_ from ObjectHeader. */
    attr_accessor(klass, Class);

    /* ivars_ from ObjectHeader. */
    attr_accessor(ivars, Object);


  public:   /* TypeInfo */

    /**
     *  Static type information for Object.
     */
    class Info : public TypeInfo {
    public:
      virtual ~Info() {}

      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
    };

  };


/* Object inlines -- Alphabetic, unlike class definition. */

  inline bool Object::fixnum_p() const {
    return FIXNUM_P(this);
  }

  inline Class* Object::lookup_begin(STATE) {
    if(reference_p()) {
      return klass_;
    }

    return state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  inline bool Object::symbol_p() const {
    return SYMBOL_P(this);
  }

  inline void Object::write_barrier(STATE, Fixnum* obj) {
    /* No-op */
  }

  inline void Object::write_barrier(STATE, Symbol* obj) {
    /* No-op */
  }

}

#endif
