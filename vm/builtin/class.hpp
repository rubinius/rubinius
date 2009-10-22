#ifndef RBX_BUILTIN_CLASS_HPP
#define RBX_BUILTIN_CLASS_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/module.hpp"
#include "type_info.hpp"

namespace rubinius {
  class LookupTable;

  class Class : public Module {
  public:
    const static object_type type = ClassType;

  private:
    Fixnum* instance_type_;   // slot
    LookupTable* packed_ivar_info_; // slot

    TypeInfo* type_info_;

    int class_id_;
    size_t packed_size_;

  public:
    /* accessors */

    attr_accessor(packed_ivar_info, LookupTable);
    attr_accessor(instance_type, Fixnum);

    TypeInfo* type_info() {
      return type_info_;
    }

    void set_type_info(TypeInfo* ti) {
      type_info_ = ti;
    }

    int class_id() {
      return class_id_;
    }

    void set_class_id(int id) {
      class_id_ = id;
    }

    size_t packed_size() {
      return packed_size_;
    }

    void set_packed_size(size_t s) {
      packed_size_ = s;
    }

    /* static */
    static void bootstrap_methods(STATE);

    /* interface */

    /** Returns actual superclass, skipping over IncludedModules */
    Class* true_superclass(STATE);

    void set_object_type(STATE, size_t type);

    static Class* create(STATE, Class* super);

    // Ruby.primitive :class_s_allocate
    static Class* s_allocate(STATE);

    // Ruby.primitive+ :class_allocate
    Object* allocate(STATE);

    // Ruby.primitive :class_set_superclass
    Object* set_superclass(STATE, Class* sup);

    // Ruby.primitive :class_set_packed
    Object* set_packed(STATE, Array* info);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };

  class CompiledMethod;
  class StaticScope;

  class MetaClass : public Class {
  public:
    const static object_type type = MetaClassType;

  private:
    Object* attached_instance_; // slot

  public:
    /* accessors */

    attr_accessor(attached_instance, Object);

    /* interface */

    static MetaClass* attach(STATE, Object* obj, Object* sup = NULL);

    // Ruby.primitive :metaclass_attach_method
    Object* attach_method(STATE, Symbol* name, CompiledMethod* method, StaticScope* scope);

    class Info : public Class::Info {
    public:
      BASIC_TYPEINFO(Class::Info)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
