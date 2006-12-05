OBJECT tuple_new(STATE, int size);
#define tuple_fields(state, tup) NUM_FIELDS(tup)
#define tuple_at(state, tup, idx) NTH_FIELD(tup, idx)
#define tuple_put(state, tup, idx, val) SET_FIELD(tup, idx, val)
OBJECT tuple_new2(STATE, int n, ...);
OBJECT tuple_enlarge(STATE, OBJECT tup, int inc);
