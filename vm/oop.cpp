#include "oop.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"

#include "objectmemory.hpp"

#include <cassert>

namespace rubinius {
  void ObjectHeader::set_inflated_header(InflatedHeader* ih) {
    HeaderWord orig = header;

    ih->reset_object(this);
    ih->update(header);

    HeaderWord new_val = header;
    new_val.all_flags = ih;
    new_val.f.meaning = eAuxWordInflated;

    // Do a spin update so if someone else is trying to update it at the same time
    // we catch that and keep trying until we get our version in.
    while(!__sync_bool_compare_and_swap(&header.flags64, orig.flags64, new_val.flags64)) {
      orig = header;
      ih->update(header);
    }
  }

  InflatedHeader* ObjectHeader::deflate_header() {
    // Probably needs to CAS, but this only used by immix and in a place
    // we don't hit currently, so don't worry about it for now.
    InflatedHeader* ih = inflated_header();
    header.f = ih->flags();
    header.f.meaning = eAuxWordEmpty;
    header.f.aux_word = 0;

    return ih;
  }

  void InflatedHeader::update(HeaderWord header) {
    flags_ = header.f;

    switch(flags_.meaning) {
    case eAuxWordObjID:
      object_id_ = flags_.aux_word;
      flags_.meaning = eAuxWordEmpty;
      flags_.aux_word = 0;
      break;
    case eAuxWordLock:
      // fine.
    case eAuxWordInflated:
      abort();
    }
  }

  void ObjectHeader::set_object_id(ObjectMemory* om, uint32_t id) {
    // Just ignore trying to reset it to 0 for now.
    if(id == 0) return;

    // Construct 2 new headers: one is the version we hope that
    // is in use and the other is what we want it to be. The CAS
    // the new one into place.
    HeaderWord orig = header;

    orig.f.meaning = eAuxWordEmpty;
    orig.f.aux_word = 0;

    HeaderWord new_val = orig;

    new_val.f.meaning = eAuxWordObjID;
    new_val.f.aux_word = id;

    if(__sync_bool_compare_and_swap(&header.flags64, orig.flags64, new_val.flags64)) {
      return;
    }

    // not inflated, and the aux_word is being used for locking.
    // Inflate!

    om->inflate_for_id(this, id);
  }

  size_t ObjectHeader::slow_size_in_bytes(STATE) const {
    return state->om->type_info[type_id()]->object_size(this);
  }

  void ObjectHeader::initialize_copy(ObjectMemory* om, Object* other, unsigned int new_age) {
    /* Even though we dup it, we have to be careful to maintain
     * the zone. */

    // DO NOT, EVER, CHANGE obj_type_.
    // obj_type_ indicates the shape of the object in memory. There are
    // fucked up cases where this is called where other is another type
    // (that, btw, is likely a bug in itself), but we should never,
    // MUST never change obj_type_ to make them match. This causes the GC
    // to get confused about the memory shape of the object!
    assert(type_id() == other->type_id());
    set_age(new_age);
    klass_ = other->klass_;
    ivars_ = other->ivars_;

    clear_forwarded();

    if(other->is_tainted_p()) set_tainted();
  }

  void ObjectHeader::initialize_full_state(STATE, Object* other, unsigned int age) {
    assert(type_id() == other->type_id());
    set_age(age);
    klass_ = other->klass_;
    ivars_ = other->ivars_;

    if(other->object_id() > 0) {
      set_object_id(state->om, other->object_id());
    }

    clear_forwarded();

    if(other->is_tainted_p()) set_tainted();

    copy_body(state, other);

    state->om->write_barrier((Object*)this, ivars_);
    state->om->write_barrier((Object*)this, klass_);

    // This method is only used by the GC to move an object, so must retain
    // the settings flags.
    flags().Frozen =  other->flags().Frozen;
    flags().Tainted = other->flags().Tainted;
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

  void InflatedHeader::set_object(ObjectHeader* obj) {
    flags_ = obj->flags();
    object_ = obj;
  }

}
