#include "oop.hpp"
#include "builtin/object.hpp"

#include <cassert>

namespace rubinius {
  /* Initialize the objects data with the most basic info. This is done
   * right after an object is created. */
  void ObjectHeader::init(gc_zone loc, size_t fields) {
    all_flags = 0;
    zone = loc;
    field_count = fields;
  }

  void ObjectHeader::initialize_copy(Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    obj_type     = other->obj_type;
    age          = new_age;
    field_count  = other->field_count;
    klass_       = other->klass_;
    ivars_       = other->ivars_;

    Forwarded    = 0;
    ForeverYoung = other->ForeverYoung;
    StoresBytes  = other->StoresBytes;
    RequiresCleanup = other->RequiresCleanup;
    IsBlockContext = other->IsBlockContext;
    IsMeta       = other->IsMeta;
    IsTainted    = other->IsTainted;
    IsFrozen     = other->IsFrozen;
    RefsAreWeak  = other->RefsAreWeak;
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
