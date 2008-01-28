#include <string.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/module.h"
#include "shotgun/lib/class.h"

OBJECT rbs_const_set(STATE, OBJECT module, const char *name, OBJECT obj) {
  OBJECT sym, str;
  
  str = string_new(state, name);
  sym = string_to_sym(state, str);
  
  module_const_set(state, module, sym, obj);
  return obj;
}

OBJECT rbs_const_get(STATE, OBJECT module, const char *name) {
  OBJECT sym, str;
  
  str = string_new(state, name);
  sym = string_to_sym(state, str);
  
  return module_const_get(state, module, sym);
}

OBJECT rbs_class_new(STATE, const char *name, int fields, OBJECT sup) {
  return class_new(state, name, fields, sup, BASIC_CLASS(object));
}

OBJECT rbs_class_new_with_namespace(STATE, const char *name, int fields, OBJECT sup, OBJECT ns) {
  return class_new(state, name, fields, sup, ns);
}

OBJECT rbs_module_new(STATE, const char *name, OBJECT ns) {
  OBJECT mod;

  mod = rbs_const_get(state, ns, name);

  if (!NIL_P(mod)) /* HACK check for module */
    return mod;

  mod = module_allocate_mature(state, 0);
  module_setup_with_namespace(state, mod, name, ns);

  return mod;
}

OBJECT rbs_symbol_to_string(STATE, OBJECT sym) {
  OBJECT str;
  str = symtbl_find_string(state, state->global->symbols, sym);
  return str;
}

const char *rbs_symbol_to_cstring(STATE, OBJECT sym) {
  return string_byte_address(state, rbs_symbol_to_string(state, sym));
}

OBJECT rbs_get_field_raw(OBJECT obj, int fel) {
  return NTH_FIELD_DIRECT(obj, fel);
}

int rbs_num_fields(OBJECT obj) {
  return NUM_FIELDS(obj);
}

OBJECT rbs_raw_class(OBJECT obj) {
  if(REFERENCE_P(obj)) {
    return obj->klass;
  }
  return Qnil;
}

OBJECT rbs_class_new_instance(STATE, OBJECT cls) {
  OBJECT t1;
  t1 = class_get_instance_fields(cls);
  return NEW_OBJECT(cls, N2I(t1));
}

const char *rbs_inspect(STATE, OBJECT obj) {
  OBJECT kls;
  static char buf[1024];
  
  kls = object_class(state, obj);
  
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

const char *rbs_inspect_verbose(STATE, OBJECT obj) {
  OBJECT kls;
  static char buf[1024];
  
  kls = object_class(state, obj);
  
  if(NIL_P(kls)) {
    assert(RTEST(kls) && "class is nil");
    sprintf(buf, "<(NilClass!!):%p>", (void*)obj);
  } else if(kls == state->global->class) {
    sprintf(buf, "<Class:%s>", rbs_symbol_to_cstring(state, module_get_name(obj)));
  } else if(kls == state->global->module) {
    sprintf(buf, "<Module:%s>", rbs_symbol_to_cstring(state, module_get_name(obj)));
  } else if(kls == state->global->cmethod) {
    sprintf(buf, "<CompiledMethod:%p %s>", (void*)obj, rbs_symbol_to_cstring(state, cmethod_get_name(obj)));
  } else if(kls == state->global->symbol || kls == state->global->string) {
    const char *s = NULL;
    sprintf(buf, "<%s:%p '", rbs_symbol_to_cstring(state, module_get_name(kls)), (void*)obj);
    
    if (kls == state->global->symbol) {
      s = rbs_symbol_to_cstring(state, obj);
    }
    
    if (kls == state->global->string) {
      s = string_byte_address(state, obj);
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

/* Debugging functions */
void _print_stack(int cnt, int start) {
  OBJECT *stk;
  int i;
  
  stk = current_machine->c->sp_ptr;
  printf("Current Stack: %p\n", stk);
  
  if(start < 0) {
    stk -= start;    
    for(i = start; i < 0; i++) {
      printf("  %3d:\t%s\t\t\t%p\n", i, _inspect(*stk), stk);
      stk--;
    }
  }
  
  stk = current_machine->c->sp_ptr;
  
  printf(">   0:\t%s\n", _inspect(*stk--));  
  
  for(i = 1; i <= cnt; i++) {
    printf("  %3d:\t%s\n", i, _inspect(*stk));
    stk--;
  }
}

const char *_inspect(OBJECT obj) {
  if(SYMBOL_P(obj)) {
    return rbs_symbol_to_cstring(current_machine->s, obj);
  }
  return rbs_inspect_verbose(current_machine->s, obj);
}

