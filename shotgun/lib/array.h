OBJECT array_new(STATE, int count);
OBJECT array_from_tuple(STATE, OBJECT tuple);
OBJECT array_set(STATE, OBJECT self, int idx, OBJECT val);
OBJECT array_get(STATE, OBJECT self, int idx);
OBJECT array_append(STATE, OBJECT self, OBJECT val);
OBJECT array_pop(STATE, OBJECT self);
