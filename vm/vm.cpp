#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"

namespace rubinius {
  VM::VM(size_t bytes) {
    om = new ObjectMemory(bytes);
    bootstrap_ontology();
  }

  VM::~VM() {
    delete om;
  }

  OBJECT VM::new_object(Class *cls) {
    return om->new_object(cls, cls->instance_fields->n2i());
  }

  Symbol* VM::symbol(const char* str, size_t size) {
    return (Symbol*)globals.symbols->lookup(this, str, size);
  }

  OBJECT VM::new_struct(Class* cls, size_t bytes) {
    size_t fields = bytes / sizeof(OBJECT);
    if(fields % sizeof(OBJECT) != 0) fields++;

    return om->new_object_bytes(cls, fields);
  }

  TypeInfo* VM::get_type_info(Class* cls) {
    TypeInfo *ti = new TypeInfo(cls);
    type_info[cls->object_type->n2i()] = ti;
    return ti;
  }

  TypeInfo::TypeInfo(Class *cls) {
    type = (object_type)cls->object_type->n2i();
    cleanup = NULL;
  }
  
  void type_assert(OBJECT obj, object_type type, char* reason) {
    if(obj->reference_p() && obj->obj_type != type) {
      throw new TypeError(type, obj, reason);
    } else if(type == FixnumType && !obj->fixnum_p()) {
      throw new TypeError(type, obj, reason);
    }
  }
};
