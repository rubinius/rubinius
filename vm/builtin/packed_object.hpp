#ifndef RBX_PACKED_OBJECT_HPP
#define RBX_PACKED_OBJECT_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class PackedObject : public Object {
  public:
    const static object_type type = PackedObjectType;

  public:
    Object** body_as_array() {
      return reinterpret_cast<Object**>(pointer_to_body());
    }

    Object* get_packed_ivar(STATE, Symbol* sym);
    Object* set_packed_ivar(STATE, Symbol* sym, Object* val);
    Object* packed_ivar_defined(STATE, Symbol* sym);
    Object* packed_ivar_delete(STATE, Symbol* sym, bool* removed = 0);

    void add_packed_ivars(STATE, Array* ary);

  public:
    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false)
        : TypeInfo(type, cleanup)
      {}

      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
      virtual void visit(Object* obj, ObjectVisitor& visit);
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
      virtual size_t object_size(const ObjectHeader* object);
    };
  };
}

#endif
