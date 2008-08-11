#ifndef RBX_OBJECTS_HPP
#define RBX_OBJECTS_HPP

#include "vm.hpp"
#include "object.hpp"
#include "type_info.hpp"

#include <iostream>

namespace rubinius {
  class NormalObject : public Object {
  public:
    const static size_t fields = 1;
    const static object_type type = ObjectType;

    OBJECT instance_variables;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(OBJECT t, ObjectMark& mark);
    };
  };

  template <>
  static inline NormalObject* as<NormalObject>(OBJECT obj) {
    return (NormalObject*)obj;
  }
};

#endif
