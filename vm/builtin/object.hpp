#ifndef RBX_VM_BUILTIN_OBJECT_HPP
#define RBX_VM_BUILTIN_OBJECT_HPP

#include <assert.h>
#include "oop.hpp"
#include "prelude.hpp"

namespace rubinius {
  class MetaClass;
  class Integer;

  class Object : public ObjectHeader {
  public:

    // Ruby.primitive :object_equal
    OBJECT equal(STATE, OBJECT other);

    // Ruby.primitive :object_show
    OBJECT show(STATE);

    /* body access */
    union {
      OBJECT field[];
      uint8_t bytes[];
    };

    /* WARNING. Do not use this version if +num+ has the chance of being
     * greater than FIXNUM_MAX. */
    static FIXNUM i2n(native_int num) {
      return (FIXNUM)APPLY_TAG(num, TAG_FIXNUM);
    }

    static Integer* i2n(STATE, native_int num);
    static Integer* ui2n(STATE, unsigned int num);
    static Integer* ll2n(STATE, long long num);
    static Integer* ull2n(STATE, unsigned long long num);

    bool fixnum_p() {
      return FIXNUM_P(this);
    }

    bool symbol_p() {
      return SYMBOL_P(this);
    }

    /* Initialize the objects data with the most basic info. This is done
     * right after an object is created. */
    void init(gc_zone loc, size_t fields) {
      all_flags = 0;
      zone = loc;
      field_count = fields;
    }

    /* Clear the body of the object, by setting each field to Qnil */
    void clear_fields() {
      for(size_t i = 0; i < field_count; i++) {
        field[i] = Qnil;
      }
    }

    /* Initialize the object as storing bytes, by setting the flag then clearing the
     * body of the object, by setting the entire body as bytes to 0 */
    void init_bytes() {
      this->StoresBytes = 1;
      std::memset((void*)(this->field), field_count * sizeof(OBJECT), 0);
    }

    size_t size_in_bytes() {
      return SIZE_IN_BYTES(this);
    }

    size_t body_in_bytes() {
      return field_count * sizeof(OBJECT);
    }

    bool reference_p() {
      return REFERENCE_P(this);
    }

    bool stores_bytes_p() {
      return StoresBytes;
    }

    bool stores_references_p() {
      return !StoresBytes;
    }

    bool young_object_p() {
      return zone == YoungObjectZone;
    }

    bool mature_object_p() {
      return zone == MatureObjectZone;
    }

    bool forwarded_p() {
      return Forwarded == 1;
    }

    void set_forward(OBJECT fwd) {
      assert(zone == YoungObjectZone);
      Forwarded = 1;
      klass = (Class*)fwd;
    }

    OBJECT forward() {
      return (OBJECT)klass;
    }

    bool marked_p() {
      return Marked == 1;
    }

    void mark() {
      Marked = 1;
    }

    void clear_mark() {
      Marked = 0;
    }

    bool nil_p() {
      return this == Qnil;
    }

    bool undef_p() {
      return this == Qundef;
    }

    bool true_p() {
      return this == Qtrue;
    }

    bool false_p() {
      return this == Qfalse;
    }

    bool has_ivars_p() {
      return CanStoreIvars == TRUE;
    }

    bool check_type(object_type type) {
      return reference_p() && obj_type == type;
    }

    // Safely return the object type, even if the receiver is an immediate
    object_type type() {
      if(reference_p()) return obj_type;
      if(fixnum_p()) return FixnumType;
      if(symbol_p()) return SymbolType;
      if(nil_p()) return NilType;
      if(true_p()) return TrueType;
      if(false_p()) return FalseType;
      return ObjectType;
    }

    // Ruby.primitive :object_tainted_p
    OBJECT tainted_p() {
      if(this->IsTainted && reference_p()) {
        return Qtrue;
      } else {
        return Qfalse;
      }
    }

    // Ruby.primitive :object_taint
    OBJECT taint() {
      if(reference_p()) {
        this->IsTainted = TRUE;
      }
      return this;
    }

    // Ruby.primitive :object_untaint
    OBJECT untaint() {
      if(reference_p()) {
        this->IsTainted = FALSE;
      }
      return this;
    }

    // Ruby.primitive :object_freeze
    OBJECT freeze() {
      if(reference_p()) {
        this->IsFrozen = TRUE;
      }
      return this;
    }

    // Ruby.primitive :object_frozen_p
    OBJECT frozen_p() {
      if(this->IsFrozen && reference_p()) {
        return Qtrue;
      } else {
        return Qfalse;
      }
    }

    // Ruby.primitive :object_dup
    OBJECT dup(STATE);
    // Ruby.primitive :object_id
    uintptr_t id(STATE);

    OBJECT get_field(STATE, size_t index);
    void   set_field(STATE, size_t index, OBJECT val);
    void cleanup(STATE);

    bool kind_of_p(STATE, OBJECT cls);
    Class* lookup_begin(STATE);
    Class* class_object(STATE);

    hashval hash(STATE);

    Class* metaclass(STATE);

    OBJECT get_ivar(STATE, OBJECT sym);
    OBJECT set_ivar(STATE, OBJECT sym, OBJECT val);

    void copy_flags(STATE, OBJECT other);
    void copy_ivars(STATE, OBJECT other);
    void copy_metaclass(STATE, OBJECT other);

    static const char* type_to_name(object_type type);
  };

}

#endif
