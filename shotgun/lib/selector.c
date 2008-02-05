#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/module.h"

void selector_init(STATE) {
  state->global->selectors = hash_new(state);
  BASIC_CLASS(selector) = _selector_class(state, BASIC_CLASS(object));
  module_setup(state, BASIC_CLASS(selector), "Selector");
  class_set_object_type(BASIC_CLASS(selector), I2N(SelectorType));
}

OBJECT selector_new(STATE, OBJECT name) {
  OBJECT sel = selector_allocate(state);

  selector_set_name(sel, name);
  selector_set_send_sites(sel, array_new(state, 1));

  return sel;
}

OBJECT selector_lookup(STATE, OBJECT name) {
  OBJECT sel;
  sel = hash_find(state, state->global->selectors, name);
  if(!NIL_P(sel)) return sel;

  sel = selector_new(state, name);
  hash_set(state, state->global->selectors, name, sel);
  return sel;
}

OBJECT selector_associate(STATE, OBJECT self, OBJECT ss) {
  OBJECT ary;

  ary = selector_get_send_sites(self);
  array_append(state, ary, ss);

  return ss;
}
