#include "oop.hpp"
#include "builtin/object.hpp"

namespace rubinius {
  void ObjectHeader::initialize_copy(Object* other, unsigned int age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */
    gc_zone z         = this->zone;
    this->all_flags   = other->all_flags;
    this->zone        = z;
    assert(z != UnspecifiedZone);

    this->field_count = other->field_count;
    this->Forwarded   = 0;
    this->age         = age;
    this->klass_      = other->klass();
    this->ivars_      = other->ivars();
  }

  void ObjectHeader::copy_body(Object* other) {
    assert(this->field_count == other->field_count);

    void** src = other->__body__;
    void** dst = this->__body__;

    for(size_t counter = 0; counter < field_count; counter++) {
      *dst++ = *src++;
    }
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void ObjectHeader::clear_fields() {
    ivars_ = Qnil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    Object** dst = reinterpret_cast<Object**>(__body__);

    for(size_t counter = 0; counter < field_count; counter++) {
      *dst++ = Qnil;
    }
  }

  void ObjectHeader::clear_body_to_null() {
    uintptr_t** dst = reinterpret_cast<uintptr_t**>(__body__);

    for(size_t counter = 0; counter < field_count; counter++) {
      *dst++ = 0;
    }

  }

}
