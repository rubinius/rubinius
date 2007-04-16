#include "shotgun.h"
#include "module.h"
#include "flags.h"

OBJECT metaclass_s_attach(STATE, OBJECT obj) {
  OBJECT meta;
  meta = metaclass_allocate(state);
  FLAG_SET(meta, IsMetaFlag);
  metaclass_set_attached_instance(meta, obj);
  if(RTEST(state->global->hash) || RTEST(state->global->methtbl)) {
    module_setup_fields(state, meta);
  }
  HEADER(obj)->klass = meta;
  return meta;
}

int metaclass_s_metaclass_p(STATE, OBJECT obj) {
  if(FLAG_SET_P(obj, IsMetaFlag)) {
    return TRUE;
  }
  return FALSE;
}

