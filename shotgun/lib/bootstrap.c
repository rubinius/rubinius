#include "shotgun.h"
#include "object.h"
#include "symbol.h"
#include "module.h"
#include "hash.h"
#include <assert.h>

#include "rubinius.h"

#define BC(o) BASIC_CLASS(o)

void cpu_bootstrap_exceptions(STATE) {
  int sz;
  sz = 2;
  
  OBJECT exc, std, arg, loe, stk, sxp;
  
  #define dexc(name, sup) rbs_class_new(state, #name, sz, sup)
  
  exc = dexc(Exception, BC(object));
  dexc(fatal, exc);
  std = dexc(StandardError, exc);
  arg = dexc(ArgumentError, exc);
  dexc(NameError, std);
  dexc(NoMethodError, exc);
  dexc(SyntaxError, exc);
  loe = dexc(LoadError, exc);
  dexc(RuntimeError, std);
  dexc(SystemCallError, std);
  stk = dexc(StackError, exc);
  sxp = dexc(StackExploded, stk);
  
  state->global->exc_arg = arg;
  state->global->exc_loe = loe;
  
  state->global->exc_stack_explosion = sxp;
}

void cpu_bootstrap(STATE) {
  OBJECT cls, obj, tmp, tmp2;
  
  cls = NEW_OBJECT(Qnil, CLASS_FIELDS);
  HEADER(cls)->klass = cls;
  class_set_instance_fields(cls, CLASS_FIELDS);
  BC(class) = cls;
  assert(cls == CLASS_OBJECT(cls));
  obj = _object_basic_class(state, Qnil);
  BC(object) = obj;
  BC(module) = _module_basic_class(state, obj);
  class_set_superclass(cls, BC(module));
  BC(metaclass) = _metaclass_basic_class(state, cls);
  object_create_metaclass(state, obj, cls);
  object_create_metaclass(state, BC(module), object_metaclass(state, obj));
  object_create_metaclass(state, BC(class), object_metaclass(state, BC(module)));
  
  BC(hash) = _hash_class(state, obj);
  BC(methtbl) = _methtbl_class(state, BC(hash));
  module_setup_fields(state, object_metaclass(state, obj));
  module_setup_fields(state, object_metaclass(state, BC(module)));
  module_setup_fields(state, object_metaclass(state, BC(class)));
  module_setup_fields(state, object_metaclass(state, BC(hash)));
  module_setup_fields(state, object_metaclass(state, BC(methtbl)));
  BC(symbol) = _symbol_class(state, obj);
  BC(tuple) = _tuple_class(state, obj);
  BC(array) = _array_class(state, obj);
  BC(bytearray) = _bytearray_class(state, obj);
  BC(string) = _string_class(state, obj);
  BC(symtbl) = _symtbl_class(state, obj);
  BC(cmethod) = _cmethod_class(state, obj);
  BC(io) = _io_class(state, obj);
  BC(methctx) = _methctx_class(state, obj);
  BC(blokenv) = _blokenv_class(state, obj);
  BC(blokctx) = _blokctx_class(state, obj);
  
  state->global->symbols = symtbl_new(state);
  
  module_setup(state, obj, "Object");
  module_setup(state, cls, "Class");
  module_setup(state, BC(module), "Module");
  module_setup(state, BC(metaclass), "MetaClass");
  module_setup(state, BC(symbol), "Symbol");
  module_setup(state, BC(tuple), "Tuple");
  module_setup(state, BC(array), "Array");
  module_setup(state, BC(bytearray), "ByteArray");
  module_setup(state, BC(hash), "Hash");
  module_setup(state, BC(string), "String");
  module_setup(state, BC(symtbl), "SymbolTable");
  module_setup(state, BC(methtbl), "MethodTable");
  module_setup(state, BC(cmethod), "CompiledMethod");
  module_setup(state, BC(io), "IO");
  module_setup(state, BC(methctx), "MethodContext");
  module_setup(state, BC(blokenv), "BlockEnvironment");
  module_setup(state, BC(blokctx), "BlockContext");
  
  rbs_const_set(state, obj, "Symbols", state->global->symbols);
  BC(nil_class) = rbs_class_new(state, "NilClass", 0, obj);
  BC(true_class) = rbs_class_new(state, "TrueClass", 0, obj);
  BC(false_class) = rbs_class_new(state, "FalseClass", 0, obj);
  tmp = rbs_class_new(state, "Numeric", 0, obj);
  tmp2 = rbs_class_new(state, "Integer", 0, tmp);
  BC(fixnum_class) = rbs_class_new(state, "Fixnum", 0, tmp2);
  BC(bignum) = rbs_class_new(state, "Bignum", 0, tmp2);
  BC(undef_class) = rbs_class_new(state, "UndefClass", 0, obj);
  
  #define bcs(name, sup, string) BC(name) = _ ## name ## _class(state, sup); \ 
    module_setup(state, BC(name), string);
  
  bcs(regexp, obj, "Regexp");
  bcs(regexpdata, obj, "RegexpData");
  bcs(matchdata, obj, "MatchData");
  
  cpu_bootstrap_exceptions(state);
  
  state->global->external_ivars = hash_new(state);
}
