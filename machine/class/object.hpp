#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include <vector>

#include "defines.hpp"
#include "memory/header.hpp"

#include "thread_state.hpp"
#include "type_info.hpp"
#include "executor.hpp"

namespace rubinius {
  class Fixnum;
  class Integer;
  class String;
  class Module;
  class Executable;
  class Array;

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
   *  @see  machine/type_info.hpp
   *
   */
  class Object : public ObjectHeader {
  public:   /* Slots and bookkeeping */

    static void bootstrap(STATE);
    static void initialize(STATE, Object* obj) { }
    static void initialize(STATE, Object* obj, intptr_t bytes, object_type type) {
      obj->initialize_fields(bytes);
    }

    /** Class type identifier. */
    static const object_type type = ObjectType;

    static void bootstrap_methods(STATE);

  public:   /* GC support, bookkeeping &c. */

    void setup_allocation_site(STATE = NULL);

  public:   /* Type information, field access, copy support &c. */

    MemoryHandle* get_handle(STATE) {
      return memory_handle(state, this);
    }

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
    Object* copy_object_prim(STATE, Object* other);

    /**
     * Copies this Object's MetaClass to the other Object. Called
     * by Kernel#clone.
     */
    // Rubinius.primitive :object_copy_singleton_class
    Object* copy_singleton_class(STATE, Object* other);

    /**
     *  Retrieve the Object stored in given field index of this Object.
     *
     *  Uses TypeInfo.
     */
    Object* get_field(STATE, std::size_t index);

    /** Safely return the object type, even if the receiver is an immediate. */
    object_type get_type() const;

    /** True if given Module is this Object's class, superclass or an included Module. */
    bool kind_of_p(STATE, Object* module);

    /** Store the given Object at given field index of this Object through TypeInfo. */
    void set_field(STATE, std::size_t index, Object* val);

    /** Return the TypeInfo for this Object's type. */
    TypeInfo* type_info(STATE) const;


  public:   /* Method dispatch stuff */

    /** Actual class for this object. Also handles immediates */
    Class* direct_class(STATE) const;

    /** Module where to start looking for the MethodTable for this Object. */
    Module* lookup_begin(STATE) const;

    /**
     *  Directly send a method to this Object.
     *
     *  Sets up the current task to send the given method name to this
     *  Object, passing the given number of arguments through varargs.
     */
    Object* send(STATE, Symbol* name, Array* args,
        Object* block = cNil, bool allow_private = true);
    Object* send(STATE, Symbol* name, bool allow_private = true);

    Object* send(STATE, Executable* exec, Module* mod, Arguments& args,
        Symbol* min_visibility);

    /**
     *  Ruby #send/#__send__
     */
    // Rubinius.primitive? :object_send
    Object* private_send_prim(STATE, Executable* exec, Module* mod, Arguments& args);

    /**
     *  Ruby #public_send
     */
    // Rubinius.primitive? :object_public_send
    Object* public_send_prim(STATE, Executable* exec, Module* mod, Arguments& args);


  public:   /* Ruby interface */

    /** Returns the Class object of which this Object is an instance. */
    // Rubinius.primitive+ :object_class
    Class* class_object(STATE) const;

    /**
     *  Ruby #equal?.
     *
     *  Returns true if and only if this and the other object are
     *  the SAME object, false otherwise.
     */
    // Rubinius.primitive+ :object_equal
    Object* equal(STATE, Object* other);

    /** Sets the frozen flag. Rubinius does NOT currently support freezing. */
    // Rubinius.primitive :object_freeze
    Object* freeze(STATE);

    /** Returns true if this Object's frozen flag set, false otherwise. */
    // Rubinius.primitive+ :object_frozen_p
    Object* object_frozen_p(STATE);

    /**
     *  Ruby #instance_variable_get.
     *
     *  Return the Object stored as instance variable under the given
     *  identifier.
     */
    // Rubinius.primitive :object_get_ivar
    Object* get_ivar_prim(STATE, Symbol* sym);

    Object* get_ivar(STATE, Symbol* sym);

    // A version that only attempts to find +sym+ in the ivars slot
    Object* get_table_ivar(STATE, Symbol* sym);

    // Rubinius.primitive :object_del_ivar
    Object* del_ivar(STATE, Symbol* sym);
    Object* del_table_ivar(STATE, Symbol* sym, bool* removed = 0);

    Object* table_ivar_defined(STATE, Symbol* sym);

    Object* ivar_defined(STATE, Symbol* sym);

    // Rubinius.primitive :object_ivar_defined
    Object* ivar_defined_prim(STATE, Symbol* sym);

    // Rubinius.primitive :object_instance_fields
    Array* instance_fields(STATE);

    /** Returns the structure containing this object's instance variables. */
    // Rubinius.primitive :object_instance_variables
    Array* ivar_names(STATE);

    Array* ivar_names(STATE, Array* ary);

    /** Calculate a hash value for this object. */
    hashval hash(STATE);

    /** Returns the hash value as an Integer. @see hash(). */
    // Rubinius.primitive+ :object_hash
    Integer* hash_prim(STATE);

    /** Returns an Integer ID for this object. Created as needed. */
    // Rubinius.primitive+ :object_id
    Integer* object_id(STATE);

    /** Indicates if this object has been assigned an object id. */
    bool object_id_p(STATE);

    // Rubinius.primitive+ :object_infect
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
    Object* kind_of_prim(STATE, Module* klass);

    /**
     *  Ruby #instance_of?
     *
     *  Returns true if given Module is this Object's class,
     *  false otherwise.
     */
    // Rubinius.primitive+ :object_instance_of
    Object* instance_of_prim(STATE, Module* klass);

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
    Object* set_ivar_prim(STATE, Symbol* sym, Object* val);

    Object* set_ivar(STATE, Symbol* sym, Object* val);

    // Specialized version that only checks _ivars_
    Object* set_table_ivar(STATE, Symbol* sym, Object* val);

    /** String describing this object (through TypeInfo.) */
    // Rubinius.primitive :object_show
    Object* show(STATE);
    /** Indented String describing this object (through TypeInfo.) */
    Object* show(STATE, int level);
    /** Shorter String describing this object (through TypeInfo.) */
    Object* show_simple(STATE);
    /** Shorter indented String describing this object (through TypeInfo.) */
    Object* show_simple(STATE, int level);

    /**
     *  Set tainted flag on this object.
     */
    // Rubinius.primitive :object_taint
    Object* taint(STATE);

    /**
     *  Returns true if this object's tainted flag is set.
     */
    // Rubinius.primitive+ :object_tainted_p
    Object* object_tainted_p(STATE);

    /**
     *  Clears the tainted flag on this object.
     */
    // Rubinius.primitive :object_untaint
    Object* untaint(STATE);

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
    Object* respond_to_prim(STATE, Symbol* name, Object* priv);

    Object* respond_to(STATE, Symbol* name, Object* priv);

    // Rubinius.primitive :object_become
    Object* become(STATE, Object* other);

    // Rubinius.primitive :object_nil_code_id
    Object* nil_code_id(STATE);

    // Rubinius.primitive :object_nil_ip
    Object* nil_ip(STATE);

    /**
     * Checks if object is frozen and raises RuntimeError if it is.
     * Similar to CRuby rb_check_frozen
     */
    void check_frozen(STATE);

    /**
     * Returns true unless one of the objects is nil, true, or
     * false. Those objects are allowed to be modified when
     * frozen.
     */
    bool frozen_mod_disallowed();

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

      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
    };
  };

/* Object inlines -- Alphabetic, unlike class definition. */

  inline Class* Object::direct_class(STATE) const {
    if(reference_p()) {
      return klass();
    }

    return state->globals().special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  inline Module* Object::lookup_begin(STATE) const {
    return reinterpret_cast<Module*>(direct_class(state));
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
