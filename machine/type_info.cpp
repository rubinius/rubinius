#include "type_info.hpp"
#include "memory.hpp"
#include "includes.hpp"
#include "class/fixnum.hpp"
#include "class/symbol.hpp"
#include "field_offset.hpp"

#include <sstream>
#include <iomanip>

namespace rubinius {

  void TypeInfo::init(Memory* memory) {
    // Call the automatically generated function
    auto_init(memory);
  }

  TypeInfo::TypeInfo(object_type type)
    : instance_size(sizeof(Object))
    , slots()
    , type_name()
    , type(type)
    , allow_user_allocate(true)
  {}

  TypeInfo::~TypeInfo() { }

  void TypeInfo::set_field(STATE, Object* target, size_t index, Object* val) {
    throw std::runtime_error("field access denied");
  }

  Object* TypeInfo::get_field(STATE, Object* target, size_t index) {
    throw std::runtime_error("unable to access field");
  }

  /* By default, just call auto_mark(). This exists so that
   * other types can overload this to perform work before or
   * after auto_marking is done. */
  void TypeInfo::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    auto_mark(state, obj, f);
  }

  void TypeInfo::class_info(STATE, const Object* self, bool newline) {
    std::cout << const_cast<Object*>(self)->to_s(state, true)->c_str(state);
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

#include "typechecks.hpp"

  /* For use in gdb. */
  extern "C" {
    /* A wrapper because gdb can't do virtual dispatch. */
    void __show__(Object* obj) {
      /* TODO: GC
      rubinius::ThreadState state(rubinius::ThreadState::current());

      if(obj->reference_p()) {
        ObjectPosition pos = rubinius::ThreadState::current()->memory()->validate_object(obj);
        if(pos == cUnknown) {
          std::cout << "<ERROR! Unknown object reference!>\n";
        } else if(pos == cInWrongYoungHalf) {
          std::cout << "<ERROR! Object reference points to old young half!>\n";
        } else {
          obj->show(&state);
        }
      } else {
        obj->show(&state);
      }
      */
    }

    /* Similar to __show__ but only outputs #<SomeClass:0x2428999> */
    void __show_simple__(Object* obj) {
      /* TODO: GC
      rubinius::ThreadState state(rubinius::ThreadState::current());

      if(obj->reference_p()) {
        ObjectPosition pos = rubinius::ThreadState::current()->memory()->validate_object(obj);
        if(pos == cUnknown) {
          std::cout << "<ERROR! Unknown object reference!>\n";
        } else if(pos == cInWrongYoungHalf) {
          std::cout << "<ERROR! Object reference points to old young half!>\n";
        } else {
          obj->show_simple(&state);
        }
      } else {
        obj->show_simple(&state);
      }
      */
    }
  }
}
