
void selector_init(STATE);
OBJECT selector_new(STATE, OBJECT name);
OBJECT selector_lookup(STATE, OBJECT name);
OBJECT selector_associate(STATE, OBJECT self, OBJECT ss);

void selector_clear(STATE, OBJECT self);
void selector_clear_by_name(STATE, OBJECT name);
