#ifndef RBX_BUILTIN_DIAGNOSTICS_HPP
#define RBX_BUILTIN_DIAGNOSTICS_HPP

#include "class/class.hpp"
#include "class/object.hpp"

#include "object_utils.hpp"
#include "diagnostics.hpp"

namespace rubinius {
  class LookupTable;

  class Diagnostics : public Object {
  public:
    const static object_type type = DiagnosticsType;

    static void bootstrap(STATE);

    class Info : public TypeInfo {
      public:
        Info(object_type type)
          : TypeInfo(type)
        {
          allow_user_allocate = false;
        }

        void mark(Object* obj, memory::ObjectMark& mark) { }
        void auto_mark(Object* obj, memory::ObjectMark& mark) { }
        void set_field(STATE, Object* target, size_t index, Object* val) { }
        Object* get_field(STATE, Object* target, size_t index) { return cNil; }
        void populate_slot_locations() { }
    };
  };
}

#endif
