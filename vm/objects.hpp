#ifndef RBX_OBJECTS_HPP
#define RBX_OBJECTS_HPP

#include "vm.hpp"
#include "object.hpp"
#include "type_info.hpp"

#include <iostream>

namespace rubinius {
  class BuiltinType : public Object {

  };
}

#include "builtin_immediates.hpp"

namespace rubinius {
  class Numeric : public BuiltinType {
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

#include "builtin_bignum.hpp"
#include "builtin_fixnum.hpp"

#include "builtin_staticscope.hpp"
#include "builtin_symbol.hpp"

#include "builtin_tuple.hpp"

#include "builtin_hash.hpp"

namespace rubinius {
  class NormalObject : public BuiltinType {
  public:
    const static size_t fields = 1;
    const static object_type type = ObjectType;

    OBJECT instance_variables;
    
    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
    };
  };
};

#include "builtin_array.hpp"
#include "builtin_exception.hpp"

#include "builtin_block_environment.hpp"

#include "builtin_io.hpp"

#include "builtin_regexp.hpp"
#include "builtin_bytearray.hpp"
#include "builtin_string.hpp"

#include "builtin_lookuptable.hpp"
#include "builtin_methodtable.hpp"

#include "builtin_executable.hpp"

#include "builtin_compiledmethod.hpp"

#include "builtin_class.hpp"
#include "builtin_contexts.hpp"
#include "builtin_iseq.hpp"
#include "builtin_float.hpp"

namespace rubinius {
  template <>
    static bool kind_of<Numeric>(OBJECT obj) {
      return obj->fixnum_p() || 
        (obj->reference_p() && (
          obj->obj_type == Bignum::type ||
          obj->obj_type == Float::type));
    }
}
  
#include "builtin_list.hpp"
#include "builtin_selector.hpp"
#include "builtin_task.hpp"
#include "builtin_iseq.hpp"
#include "builtin_channel.hpp"

#include "builtin_memorypointer.hpp"
#include "builtin_nativefunction.hpp"

#endif
