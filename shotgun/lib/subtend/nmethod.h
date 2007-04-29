struct native_method {
#ifdef USE_CINVOKE;
  CInvFunction *prototype;
#endif
  void *entry;
  int args;
};

typedef struct native_method native_method;

#define NMETHOD_FIELDS 7

#define nmethod_allocate(st) (object_memory_new_object_mature(st->om, st->global->nmethod, NMETHOD_FIELDS))
#define nmethod_get_data(obj) cmethod_get_bytecodes(obj)
#define nmethod_set_data(obj, data) cmethod_set_bytecodes(obj, data)
