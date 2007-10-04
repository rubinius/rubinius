struct rni_handle {
  int flags;
  int handle_id;
  int table_idx;
  void *data;
};

typedef struct rni_handle rni_handle;

struct rni_ht_entry {
  int handle_id;
  OBJECT object;
};

typedef struct rni_ht_entry rni_ht_entry;

struct rni_handle_table {
  int total;
  int used;
  int id;
  int next;
  rni_ht_entry **entries;
};

typedef struct rni_handle_table rni_handle_table;

#define AS_STR(handle) ((bstring)(handle->data))
#define AS_ARY(handle) ((ptr_array)handle->data)
#define SET_FLAG(ha,fl) ha->flags |= fl
#define IS_FLAG(ha, fl) ((ha->flags & fl) == fl)
#define CLEAR_FLAG(ha, fl) (ha->flags ^= fl)

#define GLOBAL_FLAG (1<<0)
#define STRING_FLAG (1<<1)
#define ARRAY_FLAG  (1<<2)

rni_handle *handle_new(rni_handle_table *tbl, OBJECT obj);
int handle_is_global(rni_handle *h);
OBJECT handle_remove(rni_handle_table *tbl, rni_handle *h);
void handle_delete(rni_handle *h);
OBJECT handle_to_object(STATE, rni_handle_table *tbl, rni_handle *h);
void handle_make_global(rni_handle *h);
void handle_clear_global(rni_handle *h);

