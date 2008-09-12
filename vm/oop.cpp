#include "oop.hpp"
#include "builtin/object.hpp"

namespace rubinius {
  void ObjectHeader::initialize_copy(Object* other, unsigned int age) {
    this->all_flags   = other->all_flags;
    this->field_count = other->field_count;
    this->Forwarded   = 0;
    this->age         = age;
    this->klass       = other->klass;
    this->ivars_      = other->ivars();
  }
}
