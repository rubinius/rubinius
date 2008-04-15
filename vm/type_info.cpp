#include "type_info.hpp"
#include "objects.hpp"

namespace rubinius {
  TypeInfo::TypeInfo(Class *cls) {
    type = (object_type)cls->object_type->n2i();
    state = NULL;
  }
}
