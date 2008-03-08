#ifndef RBS_LOOKUPTABLE_H
#define RBS_LOOKUPTABLE_H

#define lookuptable_new(s) lookuptable_new_sized(s, 0)

OBJECT lookuptable_new_sized(STATE, size_t size);
OBJECT lookuptable_setup(STATE, OBJECT tbl, size_t size);
OBJECT lookuptable_store(STATE, OBJECT tbl, OBJECT key, OBJECT val);
OBJECT lookuptable_fetch(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_delete(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_has_key(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_keys(STATE, OBJECT tbl);
OBJECT lookuptable_values(STATE, OBJECT tbl);
OBJECT lookuptable_entries(STATE, OBJECT tbl);
OBJECT lookuptable_dup(STATE, OBJECT tbl);
OBJECT lookuptable_find(STATE, OBJECT tbl, OBJECT key);

#endif
