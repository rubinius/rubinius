#ifndef RBX_OBJECTS_HPP
#define RBX_OBJECTS_HPP

#include "vm.hpp"
#include "object.hpp"
#include "type_info.hpp"

#include <iostream>

#include "builtin/immediates.hpp"

namespace rubinius {
  class Numeric : public Object {
  public:
    static const object_type type = NumericType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };

  class Integer : public Numeric {
  public:
    static const object_type type = IntegerType;

    inline native_int n2i();

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };
}

#include "builtin/tuple.hpp"
#include "builtin/array.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/string.hpp"
#include "builtin/float.hpp"

#include "builtin/bignum.hpp"
#include "builtin/fixnum.hpp"

#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"

#include "builtin/hash.hpp"

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
    static inline NormalObject* as<NormalObject>(OBJECT obj) { return (NormalObject*)obj; }
};

#include "builtin/exception.hpp"

#include "builtin/block_environment.hpp"

#include "builtin/io.hpp"

#include "builtin/regexp.hpp"

#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"

#include "builtin/executable.hpp"

#include "builtin/compiledmethod.hpp"

#include "builtin/class.hpp"
#include "builtin/contexts.hpp"
#include "builtin/iseq.hpp"

namespace rubinius {
  template <>
    static bool kind_of<Numeric>(OBJECT obj) {
      return obj->fixnum_p() ||
        (obj->reference_p() && (
          obj->obj_type == Bignum::type ||
          obj->obj_type == Float::type));
    }
}

#include "builtin/list.hpp"
#include "builtin/selector.hpp"
#include "builtin/task.hpp"
#include "builtin/iseq.hpp"
#include "builtin/channel.hpp"

#include "builtin/memorypointer.hpp"
#include "builtin/nativefunction.hpp"

#include "builtin/thread.hpp"

#include "builtin/dir.hpp"

#endif
