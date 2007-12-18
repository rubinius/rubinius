#define class_create(st) class_allocate_mature(st, 0)
OBJECT class_new(STATE, const char *name, int fields, OBJECT sup, OBJECT ns);
OBJECT class_new_instance(STATE, OBJECT self);
OBJECT class_constitute(STATE, OBJECT sup, OBJECT under);
OBJECT class_superclass(STATE, OBJECT cls);

#define included_module_get_module(mod) NTH_FIELD(mod, 7)
