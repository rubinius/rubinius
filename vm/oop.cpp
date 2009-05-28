#include "oop.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"

#include "objectmemory.hpp"

#include <cassert>

namespace rubinius {
  size_t ObjectHeader::slow_size_in_bytes(STATE) const {
    return state->om->type_info[type_id()]->object_size(this);
  }

  void ObjectHeader::initialize_copy(Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    // DO NOT, EVER, CHANGE obj_type_.
    // obj_type_ indicates the shape of the object in memory. There are
    // fucked up cases where this is called where other is another type
    // (that, btw, is likely a bug in itself), but we should never,
    // MUST never change obj_type_ to make them match. This causes the GC
    // to get confused about the memory shape of the object!
    assert(obj_type_ == other->obj_type_);
    age          = new_age;
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

  void ObjectHeader::copy_body(STATE, Object* other) {
    void* src = other->__body__;
    void* dst = this->__body__;

    memcpy(dst, src, other->body_in_bytes(state));
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void ObjectHeader::clear_fields(size_t bytes) {
    ivars_ = Qnil;

    /* HACK: this case seems like a reasonable exception
     * to using accessor functions
     */
    void** dst = this->__body__;
    size_t field_count = bytes_to_fields(bytes);

    for(register size_t counter = 0; counter < field_count; ++counter) {
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
