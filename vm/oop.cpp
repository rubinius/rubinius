#include "oop.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"

#include <cassert>

namespace rubinius {
  size_t ObjectHeader::size_in_bytes() const {
    return klass_->type_info()->object_size(this);
  }

  void ObjectHeader::initialize_copy(Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    obj_type_    = other->obj_type_;
    age          = new_age;
    bytes_       = other->bytes_;
    klass_       = other->klass_;
    ivars_       = other->ivars_;

    Forwarded = 0;
    RefsAreWeak = other->RefsAreWeak;
    RequiresCleanup = other->RequiresCleanup;
  }

  void ObjectHeader::copy_flags(Object* source) {
    this->obj_type_       = source->obj_type_;
    this->RequiresCleanup = source->RequiresCleanup;
  }

  void ObjectHeader::copy_body(Object* other) {
    assert(this->bytes_ == other->bytes_);

    void** src = other->__body__;
    void** dst = this->__body__;

    memcpy(dst, src, body_in_bytes());
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void ObjectHeader::clear_fields(size_t bytes) {
    ivars_ = Qnil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = Qnil;
    }
  }

  void ObjectHeader::clear_body_to_null(size_t bytes) {
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(size_t counter = 0; counter < field_count; counter++) {
      dst[counter] = 0;
    }

  }

}
