#include "shotgun.h"
#include "module.h"

OBJECT metaclass_s_attach(STATE, OBJECT obj) {
  OBJECT meta;
  meta = metaclass_allocate(state);
  meta->IsMeta = TRUE;
  metaclass_set_attached_instance(meta, obj);
  if(RTEST(state->global->hash) || RTEST(state->global->methtbl)) {
    module_setup_fields(state, meta);
  }
  SET_CLASS(obj, meta);
  return meta;
}

int metaclass_s_metaclass_p(STATE, OBJECT obj) {
  return obj->IsMeta;
}

