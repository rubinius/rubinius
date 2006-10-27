#include "rubinius.h"
#include "bootstrap.h"
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc/malloc.h>

#define rassert(e) ({ assert(e); printf("%s:%d: <%s> => success\n", __FUNCTION__, __LINE__, #e); })

void test_bootstrap(STATE) {
  OBJECT tmp, sym, cls, obj, mod, meta;
  char *str;
  cpu_bootstrap(state);
  
  tmp = rbs_const_get(state, state->global->object, "LoadError");
  printf("LoadError: 0x%x => 0x%x\n", state->global->exc_loe, tmp);
  rassert(tmp == state->global->exc_loe);
  sym = module_get_name(tmp);
  str = rbs_symbol_to_cstring(state, sym);
  rassert(!strcmp(str, "LoadError"));
  
  cls = state->global->class;
  obj = state->global->object;
  mod = state->global->module;
  
  #define CO(obj) object_class(state, obj)
  #define assert_equal(a,b) rassert(a == b)
  #define SP(cls) class_get_superclass(cls)
  
  printf("%x =?= %x (%x)\n", cls, CO(cls), cls);
  printf("%x =?= %x (%x)\n", cls, CO(obj), obj);
  printf("%x =?= %x (%x)\n", cls, CO(mod), mod);
  
  assert_equal(cls, CO(cls));
  assert_equal(cls, CO(obj));
  assert_equal(cls, CO(mod));
  
  assert_equal(mod, SP(cls));
  assert_equal(obj, SP(mod));
  rassert(NIL_P(SP(obj)));
  
  meta = state->global->metaclass;
  assert_equal(cls, CO(meta));
  assert_equal(cls, SP(meta));
  
  OBJECT cls_meta, mod_meta, obj_meta;
  
  cls_meta = object_metaclass(state, cls);
  mod_meta = object_metaclass(state, mod);
  obj_meta = object_metaclass(state, obj);
  
  rassert(REFERENCE_P(cls_meta));
  rassert(REFERENCE_P(mod_meta));
  rassert(REFERENCE_P(obj_meta));
  
  assert_equal(mod_meta, SP(cls_meta));
  assert_equal(obj_meta, SP(mod_meta));
  assert_equal(cls, SP(obj_meta));

}

void test_bootstrap_secondary(STATE) {
  OBJECT obj, sym, tup, tmp;
  
  obj = state->global->object;
  sym = state->global->symbol;
  tup = state->global->tuple;
  
  assert_equal(obj, SP(sym));
  assert_equal(0, FIXNUM_TO_INT(class_get_instance_fields(sym)));
  
  assert_equal(obj, SP(tup));
  assert_equal(0, FIXNUM_TO_INT(class_get_instance_fields(tup)));
  
  OBJECT obj_meta, tup_meta, sym_meta;
  
  obj_meta = object_metaclass(state, obj);
  tup_meta = object_metaclass(state, tup);
  
  // printf("obj_meta: %x (%x) => %x\n", obj_meta, obj, SP(tup_meta));
  rassert(REFERENCE_P(obj_meta));
  tmp = SP(tup_meta);
  assert_equal(obj_meta, tmp);
  
  sym_meta = object_metaclass(state, sym);
  rassert(REFERENCE_P(sym_meta));
  assert_equal(obj_meta, SP(sym_meta));
  
}

int main(int argc, char **argv) {
  STATE;
  OBJECT obj, obj2, cls;
  GPtrArray *roots;
  int tmp;
  
  roots = g_ptr_array_new();
  
  state = rubinius_state_new();
  rassert(state->om != 0);
  printf("OM: %p\n", state->om);
  
  rassert(state->om->gc != 0);
  printf("GC: %p\n", state->om->gc);
  obj = object_memory_new_object(state->om, Qnil, 4);
  rassert(obj != 0);
  printf("obj: %p\n", (void*)obj);
  rassert(baker_gc_contains_p(state->om->gc, obj));
  tmp = malloc_size((void*)(state->om->gc->current->address));
  rassert(tmp >= OMDefaultSize);
  rassert(state->om->gc->current->address == obj);
  printf("memory: 0x%x, %d\n", state->om->gc->current->address, tmp);
  rassert(state->om->gc->space_a == state->om->gc->current);
  rassert(object_memory_used(state->om) > 0);
  object_memory_collect(state->om, roots);
  rassert(state->om->gc->space_b == state->om->gc->current);
  rassert(object_memory_used(state->om) == 0);
  obj = object_memory_new_object(state->om, Qnil, 8);
  rassert(object_memory_used(state->om) > 0);
  rassert(object_memory_used(state->om) == SIZE_IN_BYTES(obj));
  
  g_ptr_array_add(roots, (gpointer)obj);
  object_memory_collect(state->om, roots);
  rassert(object_memory_used(state->om) == SIZE_IN_BYTES(obj));
  obj2 = (OBJECT)g_ptr_array_index(roots, 0);
  rassert(obj2 != obj);
  rassert(state->om->gc->current->address == obj2);
  object_memory_collect(state->om, roots);
  obj2 = (OBJECT)g_ptr_array_index(roots, 0);
  rassert(obj == obj2);
  cls = obj;
  
  g_ptr_array_remove_index(roots, 0);
  obj = object_memory_new_object(state->om, cls, 4);
  g_ptr_array_add(roots, (gpointer)obj);
  object_memory_collect(state->om, roots);
  
  rassert(HEADER(obj)->klass != cls);
  rassert(!baker_gc_contains_p(state->om->gc, cls));
  rassert(baker_gc_contains_p(state->om->gc, HEADER(obj)->klass));
  
  test_bootstrap(state);
  test_bootstrap_secondary(state);
  
  printf("Everything checks out.\n");
  return 0;
}
