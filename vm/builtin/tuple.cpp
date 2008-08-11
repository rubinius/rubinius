#include "builtin/tuple.hpp"
#include "vm.hpp"
#include "objects.hpp"
#include "objectmemory.hpp"
#include "builtin/fixnum.hpp"

#include <cstdarg>

namespace rubinius {
  OBJECT Tuple::at(size_t index) {
    if(field_count <= index) {
      throw new ObjectBoundsExceeded(this, index);
    }
    return field[index];
  }

  /* The Tuple#at primitive. */
  OBJECT Tuple::at_prim(STATE, FIXNUM index_obj) {
    size_t index = index_obj->n2i();
    if(field_count <= index) return Qnil;

    return field[index];
  }

  OBJECT Tuple::put(STATE, size_t idx, OBJECT val) {
    state->om->store_object(this, idx, val);
    return val;
  }

  /* The Tuple#put primitive. */
  OBJECT Tuple::put_prim(STATE, FIXNUM index, OBJECT val) {
    return put(state, index->n2i(), val);
  }

  /* The Tuple#fields primitive. */
  OBJECT Tuple::fields_prim(STATE) {
    return Integer::from(state, field_count);
  }

  Tuple* Tuple::create(STATE, size_t fields) {
    return (Tuple*)state->om->new_object(G(tuple), fields);
  }

  Tuple* Tuple::allocate(STATE, Fixnum* fields) {
    return Tuple::create(state, fields->n2i());
  }

  Tuple* Tuple::from(STATE, size_t fields, ...) {
    va_list ar;
    Tuple* tup = create(state, fields);

    va_start(ar, fields);
    for(size_t i = 0; i < fields; i++) {
      tup->put(state, i, va_arg(ar, OBJECT));
    }
    va_end(ar);

    return tup;
  }

  void Tuple::copy_from(STATE, Tuple* other, int start, int end) {

  }


  void Tuple::Info::mark(OBJECT obj, ObjectMark& mark) {
    OBJECT tmp;
    Tuple* tup = as<Tuple>(obj);

    for(size_t i = 0; i < tup->field_count; i++) {
      tmp = mark.call(tup->field[i]);
      if(tmp) mark.set(obj, &tup->field[i], tmp);
    }
  }
}
