#include "oop.hpp"
#include "builtin/object.hpp"

#include <cassert>

namespace rubinius {
  void ObjectHeader::initialize_copy(Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    obj_type_    = other->obj_type_;
    age          = new_age;
    bytes_       = other->bytes_;
    klass_       = other->klass_;
    ivars_       = other->ivars_;

    Forwarded    = 0;
    RequiresCleanup = other->RequiresCleanup;

    RefsAreWeak  = other->RefsAreWeak;
  }

  void ObjectHeader::copy_body(Object* other) {
    assert(this->bytes_ == other->bytes_);

    void** src = other->__body__;
    void** dst = this->__body__;
    size_t field_count = num_fields();

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = src[counter];
    }
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void ObjectHeader::clear_fields() {
    ivars_ = Qnil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    void** dst = this->__body__;
    size_t field_count = num_fields();

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = Qnil;
    }
  }

  void ObjectHeader::clear_body_to_null() {
    void** dst = this->__body__;
    size_t field_count = num_fields();

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = 0;
    }

  }

}
