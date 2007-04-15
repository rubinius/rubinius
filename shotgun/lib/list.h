
void Init_list(STATE);
OBJECT list_new(STATE);
void list_append(STATE, OBJECT self, OBJECT obj);
OBJECT list_shift(STATE, OBJECT self);
void list_delete(STATE, OBJECT self, OBJECT obj);

#define ListFields 3
#define ListNodeFields 2

#define list_set_count(obj, val) SET_FIELD(obj, 0, val)
#define list_set_first(obj, val) SET_FIELD(obj, 1, val)
#define list_set_last(obj, val)  SET_FIELD(obj, 2, val)

#define list_get_count(obj) NTH_FIELD(obj, 0)
#define list_get_first(obj) NTH_FIELD(obj, 1)
#define list_get_last(obj)  NTH_FIELD(obj, 2)


#define list_node_set_object(obj, val) SET_FIELD(obj, 0, val)
#define list_node_set_next(obj, val) SET_FIELD(obj, 1, val)

#define list_node_get_object(obj) NTH_FIELD(obj, 0)
#define list_node_get_next(obj) NTH_FIELD(obj, 1)

#define list_empty_p(lst) (list_get_count(lst) == I2N(0))