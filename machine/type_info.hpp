#ifndef RBX_VM_TYPE_INFO_HPP
#define RBX_VM_TYPE_INFO_HPP

#include <stdlib.h>
#include <functional>
#include <map>
#include <stdexcept>
#include <vector>
#include <string>

#include "object_types.hpp"
#include "defines.hpp"
#include "executor.hpp"

namespace rubinius {

  class Class;
  class Object;
  class Memory;
  class DataHeader;
  class ObjectHeader;

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
   *  @see  machine/object_types.hpp
   *  @see  builtin/object.hpp
   */
  class TypeInfo {
  public: // Types

    typedef std::map<intptr_t, long> Slots;
    typedef std::vector<object_type> SlotTypes;
    typedef std::vector<executor> AccessorPrimitives;
    typedef std::vector<uintptr_t> SlotLocations;

  public:

    size_t      instance_size;
    static size_t instance_sizes[(int)LastObjectType];
    Slots       slots;
    SlotTypes   slot_types;
    AccessorPrimitives slot_accessors;
    SlotLocations slot_locations;
    std::string type_name;
    object_type type;
    bool        allow_user_allocate;

  public: /* Class initializers */

    // TODO: Can these be reworked to be self-contained?
    static void init(Memory* memory);
    static void auto_init(Memory* memory);
    static void auto_learn_fields(STATE);

    virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) = 0;
    virtual void update_weakref(STATE, Object* obj) {}
    virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
    virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
    virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {
      before_visit(state, o, f);
      after_visit(state, o, f);
    }

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

    virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);

    virtual void set_field(STATE, Object* target, size_t index, Object* val);
    virtual Object* get_field(STATE, Object* target, size_t index);

    virtual void populate_slot_locations() { }

    /**
     * Objects whose size is static generate a entry in the types data
     * structure for that size, which is accessed directly via the data
     * structure instead of via a method call. Objects whose size is defined
     * at runtime must implement this method.
     */
    virtual size_t object_size(const ObjectHeader* obj) {
      ::abort();
    }

    virtual size_t object_size(const DataHeader* obj) {
      ::abort();
    }

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
  virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f); \
  virtual void set_field(STATE, Object* target, size_t index, Object* val); \
  virtual Object* get_field(STATE, Object* target, size_t index); \
  virtual void populate_slot_locations(); \
  virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {} \
  virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {} \
  virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f); \

#endif
