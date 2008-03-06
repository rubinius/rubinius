#ifndef RBS_LOOKUPTABLE_H
#define RBS_LOOKUPTABLE_H

OBJECT lookuptable_new(STATE, int size);
OBJECT lookuptable_store(STATE, OBJECT tbl, OBJECT key, OBJECT val);
OBJECT lookuptable_fetch(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_delete(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_has_key(STATE, OBJECT tbl, OBJECT key);
OBJECT lookuptable_keys(STATE, OBJECT tbl);
OBJECT lookuptable_values(STATE, OBJECT tbl);

#endif
