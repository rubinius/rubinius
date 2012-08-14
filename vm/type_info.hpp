#ifndef RBX_VM_TYPE_INFO_HPP
#define RBX_VM_TYPE_INFO_HPP

#include <map>
#include <stdexcept>
#include <vector>

#include "object_types.hpp"
#include "prelude.hpp"
#include "executor.hpp"

namespace rubinius {

  class Class;
  class Object;
  class ObjectMark;
  class ObjectMemory;
  class ObjectHeader;

  class GCTokenImpl {
  private:
    int dummy;
  };

  typedef GCTokenImpl& GCToken;

  /**
   *  Static type information for the VM.
   *
   *  Due to memory layout restrictions, virtual methods cannot exist
   *  on builtin types. This class abstracts those operations out of
   *  the classes themselves. Using virtual dispatch here allows the
   *  correct method implementation to be invoked, based on the object's
   *  obj_type field.
   *
   *  @see  doc/builtin_internal_resource_releasing.txt
   *  @see  vm/object_types.hpp
   *  @see  builtin/object.hpp
   */
  class TypeInfo {
  public: // Types

    typedef std::map<native_int, long> Slots;
    typedef std::vector<executor> AccessorPrimitives;
    typedef std::vector<uintptr_t> SlotLocations;

  private: /* Instance vars */
    VM*         state_;

  public:

    size_t      instance_size;
    static size_t instance_sizes[(int)LastObjectType];
    Slots       slots;
    AccessorPrimitives slot_accessors;
    SlotLocations slot_locations;
    object_type type;
    std::string type_name;
    bool        allow_user_allocate;

  public: /* Class initializers */

    static void init(ObjectMemory* om);
    static void auto_init(ObjectMemory* om);
    static void auto_learn_fields(STATE);
    virtual void auto_mark(Object* obj, ObjectMark& mark) = 0;

  public:   /* Ctors */

    /**
     *  Make a new TypeInfo.
     *
     *  To support TypeInfo hierarchies where some classes may
     *  and some may not need e.g. a cleanup, the instantiation
     *  will set the cleanup flag if _any_ of the classes in
     *  the chain request it.
     */
    TypeInfo(object_type type);

    virtual ~TypeInfo();

  public:   /* Interface */

    void set_state(STATE);

    VM* state() {
      return state_;
    }

    virtual void mark(Object* obj, ObjectMark& mark);

    virtual void set_field(STATE, Object* target, size_t index, Object* val);
    virtual Object* get_field(STATE, Object* target, size_t index);

    virtual void populate_slot_locations() { }

    /**
     * Slow case, should be called only if instance_size is zero
     */
    virtual size_t object_size(const ObjectHeader * obj);

    /**
     * Currently prints the same output as show_simple. Is specialized by
     * complex classes to e.g. limit the recursion into nested
     * objects to make the output more manageable. See e.g. Tuple
     * and CompiledCode. Immediates and numeric classes print
     * their value for both show and show_simple.
     */
    virtual void show(STATE, Object* self, int level);

    /**
     * Default output for any object. Prints just the class name
     * and address. It is expected that classes will define their own
     * show output.
     */
    virtual void show_simple(STATE, Object* self, int level);

    /**
     * Prints spaces to indent the following text to the requested
     * level. Levels are specified as 0, 1, 2, ... and the multiplier
     * is 2. So, text at level 2 will have 2 * 2 = 4 spaces in front.
     */
    virtual void indent(int level);

    /**
     * Indents the attribute name to the requested level. @see indent.
     */
    virtual void indent_attribute(int level, const char* name);

    /**
     * Prints out the class name and address. Used for simple classes
     * that may append other info on the same line.
     *
     *   #<SomeClass:0x346882
     */
    virtual void class_info(STATE, const Object* self, bool newline = false);

    /**
     * Prints out the class name and address followed by a newline. Used
     * for complex classes that will print additional member info.
     *
     *   #<SomeClass:0x3287648\n
     */
    virtual void class_header(STATE, const Object* self);

    /**
     * Prints "..." + endl at the requested indent level.
     */
    virtual void ellipsis(int level);

    /**
     * Indents to level-1 and prints ">" + endl.
     */
    virtual void close_body(int level);

  };

}


#define BASIC_TYPEINFO(super) \
  Info(object_type type) : super(type) { } \
  virtual void auto_mark(Object* obj, ObjectMark& mark); \
  virtual void set_field(STATE, Object* target, size_t index, Object* val); \
  virtual Object* get_field(STATE, Object* target, size_t index); \
  virtual void populate_slot_locations();

#endif
