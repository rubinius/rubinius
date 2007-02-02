void module_setup_fields(STATE, OBJECT module);
void module_setup_name(STATE, OBJECT module, char *name, OBJECT ns);
void module_setup(STATE, OBJECT module, char *name);
void module_setup_with_namespace(STATE, OBJECT module, char *name, OBJECT ns);
void module_const_set(STATE, OBJECT self, OBJECT sym, OBJECT obj);
OBJECT module_const_get(STATE, OBJECT self, OBJECT sym);
void module_setup_fields(STATE, OBJECT module);

