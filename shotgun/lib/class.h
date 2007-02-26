#define class_create(st) class_allocate_mature(st, 0)
OBJECT class_new(STATE, char *name, int fields, OBJECT sup, OBJECT ns);
OBJECT class_new_instance(STATE, OBJECT self);
