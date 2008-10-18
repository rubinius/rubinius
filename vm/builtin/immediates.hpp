#ifndef RBX_BUILTIN_IMMEDIATES_HPP
#define RBX_BUILTIN_IMMEDIATES_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class NilClass : public Object {
  public:
    static const object_type type = NilType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type, false) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };

  class TrueClass : public Object {
  public:
    static const object_type type = TrueType;

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type, false) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };
  };

  class FalseClass : public Object {
  public:
    static const object_type type = FalseType;

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
