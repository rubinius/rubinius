#include <hashtable.h>
#include <hashtable_itr.h>
#include <bstrlib.h>

int ht_config_insert(struct hashtable *ht, bstring key, bstring value);
bstring ht_config_search(struct hashtable *ht, bstring key);
bstring ht_config_remove(struct hashtable *ht, bstring key);

struct hashtable * ht_config_create(unsigned int minsize);
void ht_config_destroy(struct hashtable *ht_config);
