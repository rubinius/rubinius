#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/methtbl.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/module.h"

void module_setup_fields(STATE, OBJECT module) {
  if(NIL_P(module_get_constants(module))) {
    module_set_constants(module, hash_new(state));
  }
  if(NIL_P(module_get_method_table(module))) {
    module_set_method_table(module, methtbl_new(state));
  }
}

void module_setup_name(STATE, OBJECT module, const char *name, OBJECT ns) {
  OBJECT str, sym;
  
  str = string_new(state, name);
  sym = string_to_sym(state, str);
  module_set_name(module, sym);
  module_const_set(state, ns, sym, module);
  sassert(NIL_P(module_get_encloser(module)));
  module_set_encloser(module, ns);
  return;
}

void module_setup_with_namespace(STATE, OBJECT module, const char *name, OBJECT ns) {
  module_setup_fields(state, module);
  module_setup_name(state, module, name, ns);
  module_setup_fields(state, object_metaclass(state, module));
}

void module_setup(STATE, OBJECT module, const char *name) {
  module_setup_with_namespace(state, module, name, BASIC_CLASS(object));
}

void module_const_set(STATE, OBJECT self, OBJECT sym, OBJECT obj) {
  OBJECT cnt;
  
  cnt = module_get_constants(self);
  hash_add(state, cnt, object_hash_int(state, sym), sym, obj);
}

OBJECT module_const_get(STATE, OBJECT self, OBJECT sym) {
  OBJECT cnt, obj;
  cnt = module_get_constants(self);
  obj = hash_get(state, cnt, object_hash_int(state, sym));
  return obj;
}
