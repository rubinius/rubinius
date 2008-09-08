#ifndef RBX_TYPE_INFO_HPP
#define RBX_TYPE_INFO_HPP

#include "prelude.hpp"
#include "object.hpp"

#include <map>
#include <stdexcept>

#include "gc.hpp"

namespace rubinius {
  class VM;
  class Class;

  /* TypeInfo contains varies operations that are registered by types */
  class TypeInfo {
  public:

    typedef std::map<native_int, long> Slots;

    VM* state;
    object_type type;
    std::string type_name;
    Slots slots;

    static void init(STATE);
    static void auto_init(STATE);
    TypeInfo(object_type type);
    virtual void auto_mark(OBJECT obj, ObjectMark& mark);

    // These are virtual methods that are 're-dispatched' using an object's
    // 'obj_type' field to determine the correct type info
    virtual ~TypeInfo();
    virtual void cleanup(OBJECT obj);
    virtual void mark(OBJECT obj, ObjectMark& mark);
    virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
    virtual OBJECT get_field(STATE, OBJECT target, size_t index);

    /**
     * Default output for any object. Prints just the class name
     * and address. It is expected that classes will define their own
     * show output.
     */
    virtual void show(STATE, OBJECT self, int level);

    /**
     * Currently prints the same output as show. Is specialized by
     * complex classes to e.g. limit the recursion into nested
     * objects to make the output more manageable. See e.g. Tuple
     * and CompiledMethod. Immediates and numeric classes print
     * their value for both show and show_simple.
     */
    virtual void show_simple(STATE, OBJECT self, int level);

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
    virtual void class_info(STATE, OBJECT self, bool newline = false);

    /**
     * Prints out the class name and address followed by a newline. Used
     * for complex classes that will print additional member info.
     *
     *   #<SomeClass:0x3287648\n
     */
    virtual void class_header(STATE, OBJECT self);

    /**
     * Prints "..." + endl at the requested indent level.
     */
    virtual void ellipsis(int level);

    /**
     * Indents to level-1 and prints ">" + endl.
     */
    virtual void close_body(int level);
  };

#define BASIC_TYPEINFO(super) \
  Info(object_type type) : super(type) { } \
  virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val); \
  virtual OBJECT get_field(STATE, OBJECT target, size_t index); \
  virtual void auto_mark(OBJECT obj, ObjectMark& mark);

}

#endif
