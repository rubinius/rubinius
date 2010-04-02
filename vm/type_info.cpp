
#include "type_info.hpp"
#include "objectmemory.hpp"
#include "gen/includes.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "field_offset.hpp"

#include "gc/gc.hpp"

#include <iostream>
#include <iomanip>

namespace rubinius {

  void TypeInfo::init(ObjectMemory* om) {
    // Call the automatically generated function
    auto_init(om);
  }


  TypeInfo::TypeInfo(object_type type, bool cleanup)
    : state_(NULL)
    , instances_need_cleanup(cleanup)
    , instance_size(sizeof(Object))
    , slots()
    , type(type)
    , type_name()
  {}

  TypeInfo::~TypeInfo() { }

  /**
   *  Default resource cleanup.
   *
   *  This method is and should remain empty so that it is safe
   *  for subclasses to call super without knowing whether the
   *  superclass implements a cleanup since it is possible to
   *  get "skipped" classes, i.e. A < B < C < Object where A
   *  and C have a cleanup but B does not.
   *
   *  The method is still only called on objects who have at
   *  least one superclass requiring cleanup.
   */
  void TypeInfo::cleanup(Object* obj)
  {
    /* Nada */
  }

  void TypeInfo::set_field(STATE, Object* target, size_t index, Object* val) {
    throw std::runtime_error("field access denied");
  }

  Object* TypeInfo::get_field(STATE, Object* target, size_t index) {
    throw std::runtime_error("unable to access field");
  }

  /* By default, just call auto_mark(). This exists so that
   * other types can overload this to perform work before or
   * after auto_marking is done. */
  void TypeInfo::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

  /* By default, just call auto_mark(). This exists so that
   * other types can overload this to perform work before or
   * after auto_marking is done. */
  void TypeInfo::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);
  }

  void TypeInfo::auto_visit(Object* obj, ObjectVisitor& visit) {
    // Must be implemented in subclasses!
  }

  size_t TypeInfo::object_size(const ObjectHeader* obj) {
    abort();
    // Must be implemented, if goes here
    return 0;
  }

  void TypeInfo::class_info(STATE, const Object* self, bool newline) {
    std::cout << const_cast<Object*>(self)->to_s(state, true)->c_str();
    if(newline) std::cout << std::endl;
  }

  void TypeInfo::class_header(STATE, const Object* self) {
    class_info(state, self);
    std::cout << "\n";
  }

  void TypeInfo::indent(int level) {
    int offset = level * 2;

    if(offset > 0) {
      std::cout << std::setfill(' ') << std::setw(offset) << " ";
    }
  }

  void TypeInfo::indent_attribute(int level, const char* name) {
    indent(level);
    std::cout << name << ": ";
  }

  void TypeInfo::ellipsis(int level) {
    indent(level);
    std::cout << "..." << std::endl;
  }

  void TypeInfo::close_body(int level) {
    indent(level-1);
    std::cout << ">" << std::endl;
  }

  void TypeInfo::show(STATE, Object* self, int level) {
    class_info(state, self, true);
  }

   void TypeInfo::show_simple(STATE, Object* self, int level) {
     class_info(state, self, true);
   }

#include "gen/typechecks.gen.cpp"

  /* For use in gdb. */
  extern "C" {
    /* A wrapper because gdb can't do virtual dispatch. */
    void __show__(Object* obj) {
      if(obj->reference_p()) {
        ObjectPosition pos = rubinius::VM::current_state()->om->validate_object(obj);
        if(pos == cUnknown) {
          std::cout << "<ERROR! Unknown object reference!>\n";
        } else if(pos == cInWrongYoungHalf) {
          std::cout << "<ERROR! Object reference points to old young half!>\n";
        } else {
          obj->show(VM::current_state());
        }
      } else {
        obj->show(VM::current_state());
      }
    }

    /* Similar to __show__ but only outputs #<SomeClass:0x2428999> */
    void __show_simple__(Object* obj) {
      if(obj->reference_p()) {
        ObjectPosition pos = rubinius::VM::current_state()->om->validate_object(obj);
        if(pos == cUnknown) {
          std::cout << "<ERROR! Unknown object reference!>\n";
        } else if(pos == cInWrongYoungHalf) {
          std::cout << "<ERROR! Object reference points to old young half!>\n";
        } else {
          obj->show_simple(VM::current_state());
        }
      } else {
        obj->show_simple(VM::current_state());
      }
    }
  }
}
