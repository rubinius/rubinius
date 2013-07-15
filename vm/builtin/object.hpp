#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include <vector>

#include "vm.hpp"
#include "oop.hpp"
#include "type_info.hpp"
#include "executor.hpp"
#include "objectmemory.hpp"

namespace rubinius {

/**
 *  Create a writer method for a slot.
 *
 *  For attr_writer(foo, SomeClass), creates void foo(STATE, SomeClass* obj)
 *  that sets the instance variable foo_ to the object given and runs the write
 *  barrier.
 */
#define attr_writer(name, type) \
  template <class T> \
  void name(T state, type* obj) { \
    name ## _ = obj; \
    this->write_barrier(state, obj); \
  }

/**
 *  Create a reader method for a slot.
 *
 *  For attr_reader(foo, SomeClass), creates SomeClass* foo() which returns the
 *  instance variable foo_. A const version is also generated.
 */
#define attr_reader(name, type) \
  type* name() const { return name ## _; }

/**
 *  Ruby-like accessor creation for a slot.
 *
 *  Both attr_writer and attr_reader.
 */
#define attr_accessor(name, type) \
  attr_reader(name, type) \
  attr_writer(name, type)

  /* Forwards */
  class Fixnum;
  class Integer;
  class String;
  class Module;
  class Executable;
  class Array;
  class Object;

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

    /** Class type identifier. */
    static const object_type type = ObjectType;

    static void bootstrap_methods(STATE);

  public:   /* GC support, bookkeeping &c. */

    /** Provides access to the GC write barrier from any object. */
    void        write_barrier(STATE, void* obj);
    void        write_barrier(VM*, void* obj);

    /** Special-case write_barrier() for Fixnums. */
    void        write_barrier(STATE, Fixnum* obj);
    /** Special-case write_barrier() for Symbols. */
    void        write_barrier(STATE, Symbol* obj);

    void        write_barrier(ObjectMemory* om, void* obj);

    void        setup_allocation_site(STATE, CallFrame* call_frame = NULL);

  public:   /* Type information, field access, copy support &c. */

    /**
     * Returns a copy of this object. This is NOT the same as Ruby
     * Kernel#dup. Code that needs Kernel#dup semantics MUST call
     * #dup from Ruby. This method is used in the VM to duplicate
     * data structures. It will not call the Ruby allocate() or
     * initialize_copy() methods.
     */
    Object* duplicate(STATE);

    Object* copy_object(STATE, Object* other);

    /**
     *  Copies the object including any instance variables. Called by
     *  Kernel#dup.
     */

    // Rubinius.primitive :object_copy_object
    Object* copy_object_prim(STATE, Object* other, CallFrame* calling_environment);

    /**
     * Copies this Object's MetaClass to the other Object. Called
     * by Kernel#clone.
     */
    // Rubinius.primitive :object_copy_singleton_class
    Object* copy_singleton_class(STATE, GCToken gct, Object* other, CallFrame* calling_environment);

    /** True if this Object* is actually a Fixnum, false otherwise. */
    bool fixnum_p() const;

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
     */
    Object* send(STATE, CallFrame* caller, Symbol* name, Array* args,
        Object* block = cNil, bool allow_private = true);
    Object* send(STATE, CallFrame* caller, Symbol* name, bool allow_private = true);

    Object* send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args,
        Symbol* min_visibility);

    /**
     *  Ruby #send/#__send__
     */
    // Rubinius.primitive? :object_send
    Object* private_send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    /**
     *  Ruby #public_send
     */
    // Rubinius.primitive? :object_public_send
    Object* public_send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);


  public:   /* Ruby interface */

    /** Returns the Class object of which this Object is an instance. */
    // Rubinius.primitive+ :object_class
    Class*    class_object(STATE) const;

    /**
     *  Ruby #equal?.
     *
     *  Returns true if and only if this and the other object are
     *  the SAME object, false otherwise.
     */
    // Rubinius.primitive+ :object_equal
    Object*   equal(STATE, Object* other);

    /** Sets the frozen flag. Rubinius does NOT currently support freezing. */
    // Rubinius.primitive :object_freeze
    Object*   freeze(STATE);

    /** Returns true if this Object's frozen flag set, false otherwise. */
    // Rubinius.primitive+ :object_frozen_p
    Object*   frozen_p(STATE);

    /**
     *  Ruby #instance_variable_get.
     *
     *  Return the Object stored as instance variable under the given
     *  identifier.
     */
    // Rubinius.primitive :object_get_ivar
    Object*   get_ivar_prim(STATE, Symbol* sym);

    Object*   get_ivar(STATE, Symbol* sym);

    // A version that only attempts to find +sym+ in the ivars slot
    Object*   get_table_ivar(STATE, Symbol* sym);

    // Rubinius.primitive :object_del_ivar
    Object*   del_ivar(STATE, Symbol* sym);
    Object*   del_table_ivar(STATE, Symbol* sym, bool* removed = 0);

    Object*   table_ivar_defined(STATE, Symbol* sym);

    Object*   ivar_defined(STATE, Symbol* sym);

    // Rubinius.primitive :object_ivar_defined
    Object*   ivar_defined_prim(STATE, Symbol* sym);

    /** Returns the structure containing this object's instance variables. */
    // Rubinius.primitive :object_ivar_names
    Array*   ivar_names(STATE);

    Array*   ivar_names(STATE, Array* ary);

    /** Calculate a hash value for this object. */
    hashval   hash(STATE);

    /** Returns the hash value as an Integer. @see hash(). */
    // Rubinius.primitive+ :object_hash
    Integer*  hash_prim(STATE);

    /** Returns an Integer ID for this object. Created as needed. */
    // Rubinius.primitive+ :object_id
    Integer*  id(STATE);

    /** Indicates if this object has been assigned an object id. */
    bool has_id(STATE);

    /** Reset the object id */
    void reset_id(STATE);

    // Rubinius.primitive :object_infect
    Object* infect_prim(STATE, Object* obj, Object* other) {
      other->infect(state, obj);
      return obj;
    }

    /**
     * Taints other if this is tainted.
     */
    void infect(STATE, Object* other);

    /**
     *  Ruby #kind_of?
     *
     *  Returns true if given Module is this Object's class,
     *  superclass or an included Module, false otherwise.
     */
    // Rubinius.primitive+ :object_kind_of
    Object*   kind_of_prim(STATE, Module* klass);

    /**
     *  Ruby #instance_of?
     *
     *  Returns true if given Module is this Object's class,
     *  false otherwise.
     */
    // Rubinius.primitive+ :object_instance_of
    Object*   instance_of_prim(STATE, Module* klass);

    /** Return object's MetaClass object. Created as needed.
     *  Also fixes up the parent hierarchy if needed */
    Class* singleton_class(STATE);

    /** Return object's MetaClass object. Created as needed. */
    Class* singleton_class_instance(STATE);

    /**
     *  Ruby #instance_variable_set
     *
     *  Store the given object in the instance variable by
     *  given identifier.
     */
    // Rubinius.primitive :object_set_ivar
    Object*   set_ivar_prim(STATE, Symbol* sym, Object* val);

    Object*   set_ivar(STATE, Symbol* sym, Object* val);

    // Specialized version that only checks ivars_
    Object*   set_table_ivar(STATE, Symbol* sym, Object* val);

    /** String describing this object (through TypeInfo.) */
    // Rubinius.primitive :object_show
    Object*   show(STATE);
    /** Indented String describing this object (through TypeInfo.) */
    Object*   show(STATE, int level);
    /** Shorter String describing this object (through TypeInfo.) */
    Object*   show_simple(STATE);
    /** Shorter indented String describing this object (through TypeInfo.) */
    Object*   show_simple(STATE, int level);

    /**
     *  Set tainted flag on this object.
     */
    // Rubinius.primitive :object_taint
    Object*   taint(STATE);

    /**
     *  Returns true if this object's tainted flag is set.
     */
    // Rubinius.primitive+ :object_tainted_p
    Object*   tainted_p(STATE);

    /**
     *  Clears the tainted flag on this object.
     */
    // Rubinius.primitive :object_untaint
    Object*   untaint(STATE);

    /**
     * Returns true if this object's untrusted flag is set.
     */
    // Rubinius.primitive+ :object_untrusted_p
    Object* untrusted_p(STATE);

    /**
     * Sets the untrusted flag on this object.
     */
    // Rubinius.primitive :object_untrust
    Object* untrust(STATE);

    /**
     * Clears the untrusted flag on this object.
     */
    // Rubinius.primitive :object_trust
    Object* trust(STATE);

    /**
     *  Returns an #inspect-like representation of an Object for
     *  use in C++ code.
     *
     *  If address is true, uses the actual address of the object.
     *  Otherwise, uses the object's id().
     */
    std::string to_string(STATE, bool address = false);

    /**
     *  Returns an #inspect-like representation of an Object for
     *  use in C++ code. Not called from Ruby code.
     *
     *  If address is true, uses the actual address of the object.
     *  Otherwise, uses the object's id().
     */
    String* to_s(STATE, bool address = false);

    /**
     *
     * Returns cTrue if this responds to method +meth+
     */
    // Rubinius.primitive+ :object_respond_to
    Object* respond_to(STATE, Symbol* name, Object* priv, CallFrame* calling_environment);

    Object* respond_to(STATE, Symbol* name, Object* priv);

    /**
     * Checks if object is frozen and raises RuntimeError if it is.
     * Similar to CRuby rb_check_frozen
     */
    void check_frozen(STATE);

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
      Info(object_type type)
        : TypeInfo(type)
      {}

      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

/* Object inlines -- Alphabetic, unlike class definition. */

  inline bool Object::fixnum_p() const {
    return __FIXNUM_P__(this);
  }

  inline Class* Object::lookup_begin(STATE) {
    if(reference_p()) {
      return klass_;
    }

    return state->globals().special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  inline bool Object::symbol_p() const {
    return __SYMBOL_P__(this);
  }

  inline void Object::write_barrier(STATE, Fixnum* obj) {
    /* No-op */
  }

  inline void Object::write_barrier(STATE, Symbol* obj) {
    /* No-op */
  }

  inline void Object::write_barrier(STATE, void* ptr) {
    Object* obj = reinterpret_cast<Object*>(ptr);
    state->memory()->write_barrier(this, obj);
  }

  inline void Object::write_barrier(VM* vm, void* ptr) {
    Object* obj = reinterpret_cast<Object*>(ptr);
    vm->om->write_barrier(this, obj);
  }

  // Used in filtering APIs
  class ObjectMatcher {
  public:
    virtual ~ObjectMatcher() {}
    virtual bool match_p(STATE, Object* obj) = 0;
  };

  class ObjectMatchAll : public ObjectMatcher {
  public:
    virtual ~ObjectMatchAll() {}
    virtual bool match_p(STATE, Object* obj) { return true; }
  };

}

#endif
