#include "shotgun.h"
#include "string.h"
#include "symbol.h"
#include "module.h"
#include "class.h"
#include <string.h>

OBJECT rbs_const_set(STATE, OBJECT module, char *name, OBJECT obj) {
  OBJECT sym, str;
  
  str = string_new(state, name);
  sym = string_to_sym(state, str);
  
  module_const_set(state, module, sym, obj);
  return obj;
}

OBJECT rbs_const_get(STATE, OBJECT module, char *name) {
  OBJECT sym, str;
  
  str = string_new(state, name);
  sym = string_to_sym(state, str);
  
  return module_const_get(state, module, sym);
}

OBJECT rbs_class_new(STATE, char *name, int fields, OBJECT sup) {
  return class_new(state, name, fields, sup, BASIC_CLASS(object));
}

OBJECT rbs_class_new_with_namespace(STATE, char *name, int fields, OBJECT sup, OBJECT ns) {
  return class_new(state, name, fields, sup, ns);
}

OBJECT rbs_module_new(STATE, char *name, OBJECT ns) {
  OBJECT mod;
  mod = module_allocate_mature(state, 0);
  module_setup_with_namespace(state, mod, name, ns);
  return mod;
}

OBJECT rbs_symbol_to_string(STATE, OBJECT sym) {
  OBJECT str;
  str = symtbl_find_string(state, state->global->symbols, sym);
  return str;
}

char *rbs_symbol_to_cstring(STATE, OBJECT sym) {
  return string_as_string(state, rbs_symbol_to_string(state, sym));
}

OBJECT rbs_get_field_raw(OBJECT obj, int fel) {
  return NTH_FIELD_DIRECT(obj, fel);
}

int rbs_num_fields(OBJECT obj) {
  return NUM_FIELDS(obj);
}

OBJECT rbs_raw_class(OBJECT obj) {
  if(REFERENCE_P(obj)) {
    return HEADER(obj)->klass;
  }
  return Qnil;
}

OBJECT rbs_class_new_instance(STATE, OBJECT cls) {
  OBJECT t1;
  t1 = class_get_instance_fields(cls);
  return NEW_OBJECT(cls, FIXNUM_TO_INT(t1));
}

char *rbs_inspect(STATE, OBJECT obj) {
  OBJECT kls;
  static char buf[1024];
  
  kls = object_logical_class(state, obj);
  
  if(NIL_P(kls)) {
    assert(RTEST(kls) && "class is nil");
    sprintf(buf, "<(NilClass!!):%p>", (void*)obj);
  } else if(kls == state->global->class) {
    sprintf(buf, "%s", rbs_symbol_to_cstring(state, module_get_name(obj)));
  } else {
    sprintf(buf, "<%s:%p>", rbs_symbol_to_cstring(state, module_get_name(kls)), (void*)obj);
  }
  return buf;
}

char *rbs_inspect_verbose(STATE, OBJECT obj) {
  OBJECT kls;
  static char buf[1024];
  
  kls = object_logical_class(state, obj);
  
  if(NIL_P(kls)) {
    assert(RTEST(kls) && "class is nil");
    sprintf(buf, "<(NilClass!!):%p>", (void*)obj);
  } else if(kls == state->global->class) {
    sprintf(buf, "%s", rbs_symbol_to_cstring(state, module_get_name(obj)));
  } else if(kls == state->global->symbol || kls == state->global->string) {
    char *s = 0;
    sprintf(buf, "<%s:%p '", rbs_symbol_to_cstring(state, module_get_name(kls)), (void*)obj);
    
    if (kls == state->global->symbol) {
      s = rbs_symbol_to_cstring(state, obj);
    }
    
    if (kls == state->global->string) {
      s = string_as_string(state, obj);
    }
    
    if (s) {
      int l;
      if ((l = strlen(s)) > 40) {
          strncat(buf, s, 20);
          strcat(buf, "...");
          strncat(buf, s+l-20, 20);
      } else
          strncat(buf, s, 40);
    }
    strcat(buf, "'>");
  } else {
    sprintf(buf, "<%s:%p>", rbs_symbol_to_cstring(state, module_get_name(kls)), (void*)obj);
  }
  return buf;
}
