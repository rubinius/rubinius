#include "type_info.hpp"
#include "objects.hpp"

namespace rubinius {
  TypeInfo::TypeInfo(Class *cls) {
    type = (object_type)cls->instance_type->n2i();
    state = NULL;
  }
}
