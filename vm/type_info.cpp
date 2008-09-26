
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


  TypeInfo::TypeInfo(object_type type, bool cleanup)
    : instances_need_cleanup(cleanup)
    , slots()
    , state(NULL)
    , type(type)
    , type_name()
  {
  }

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
  void TypeInfo::cleanup(OBJECT obj)
  {
    /* Nada */
  }

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
    // HACK copied from Tuple;
    OBJECT tmp;
    Tuple* tup = static_cast<Tuple*>(obj);

    for(size_t i = 0; i < tup->num_fields(); i++) {
      tmp = tup->field[i];
      if(tmp->reference_p()) {
        tmp = mark.call(tmp);
        if(tmp) {
          tup->field[i] = tmp;
          mark.just_set(obj, tmp);
        }
      }
    }
  }

  void TypeInfo::class_info(STATE, OBJECT self, bool newline) {
    if(Module* mod = try_as<Module>(self)) {
      const char *name = mod->name()->nil_p() ? "<anonymous>" : mod->name()->c_str(state);
      std::cout << "#<" << name << "(" <<
        self->class_object(state)->name()->c_str(state) << ")";
    } else {
      std::cout << "#<" << self->class_object(state)->name()->c_str(state);
    }
    std::cout << ":" << (void*)self;
    if(newline) std::cout << ">\n";
  }

  void TypeInfo::class_header(STATE, OBJECT self) {
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

  void TypeInfo::show(STATE, OBJECT self, int level) {
    class_info(state, self, true);
  }

   void TypeInfo::show_simple(STATE, OBJECT self, int level) {
     class_info(state, self, true);
   }

#include "gen/typechecks.gen.cpp"

  /* For use in gdb. */
  extern "C" {
    /* A wrapper because gdb can't do virtual dispatch. */
    void __show__(OBJECT obj) {
      obj->show(VM::current_state());
    }

    /* Similar to __show__ but only outputs #<SomeClass:0x2428999> */
    void __show_simple__(OBJECT obj) {
      obj->show_simple(VM::current_state());
    }
  }
}
