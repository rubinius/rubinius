
#include "type_info.hpp"
#include "objectmemory.hpp"
#include "gen/includes.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {

  void TypeInfo::init(STATE) {
    // Call the automatically generated function
    auto_init(state);

    // Give Object a TypeInfo entry
    TypeInfo* object_type_info = new TypeInfo(ObjectType);
    object_type_info->type_name = std::string("Object");
    state->add_type_info(object_type_info);
  }


  TypeInfo::~TypeInfo() { }

  TypeInfo::TypeInfo(object_type type) : type(type) {
    state = NULL;
  }

  void TypeInfo::cleanup(OBJECT obj) { }

  void TypeInfo::set_field(STATE, OBJECT target, size_t index, OBJECT val) {
    throw std::runtime_error("field access denied");
  }

  OBJECT TypeInfo::get_field(STATE, OBJECT target, size_t index) {
    throw std::runtime_error("unable to access field");
  }

  /* By default, just call auto_mark(). This exists so that
   * other types can overload this to perform work before or
   * after auto_marking is done. */
  void TypeInfo::mark(OBJECT obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

  /* For each type, there is an automatically generated version
   * of this function (called via virtual dispatch) that marks
   * all slots. */
  void TypeInfo::auto_mark(OBJECT obj, ObjectMark& mark) {
    for(size_t i = 0; i < obj->field_count; i++) {
      Object* slot = obj->field[i];
      if(slot->reference_p()) {
        OBJECT res = mark.call(slot);
        if(res) {
          obj->field[i] = as<Object>(res);
          mark.just_set(obj, res);
        }
      }
    }
  }

  /**
   * Prints out the class name and address. Used for simple classes
   * that may append other info on the same line.
   *
   *   #<SomeClass:0x346882
   */
  void TypeInfo::class_info(OBJECT self, bool newline) {
    std::cout << "#<" << self->class_object(state)->name->c_str(state) <<
      ":" << (void*) self;
    if(newline) std::cout << ">\n";
  }

  /**
   * Prints out the class name and address followed by a newline. Used
   * for complex classes that will print additional member info.
   *
   *   #<SomeClass:0x3287648\n
   */
  void TypeInfo::class_header(OBJECT self) {
    std::cout << "#<" << self->class_object(state)->name->c_str(state) <<
      ":" << (void*) self << "\n";
  }

  /**
   * Prints spaces to indent the following text to the requested
   * level. Levels are specified as 0, 1, 2, ... and the multiplier
   * is 2. So, text at level 2 will have 2 * 2 = 4 spaces in front.
   */
  void TypeInfo::indent(int level) {
    int offset = level * 2;

    if(offset > 0) {
      std::cout << std::setfill(' ') << std::setw(offset) << " ";
    }
  }

  /**
   * Default output for any object. Prints just the class name
   * and address.
   */
  void TypeInfo::show(STATE, OBJECT self, int level) {
    class_info(self, true);
  }
#include "gen/typechecks.gen.cpp"

  /* For use in gdb. */
  extern "C" {
    /* A wrapper because gdb can't do virtual dispatch. */
    void __show__(OBJECT obj) {
      obj->show(VM::current_state());
    }
  }
}
