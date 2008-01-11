#include <hashtable.h>
#include <hashtable_itr.h>
#include <bstrlib.h>

int ht_config_insert(struct hashtable *ht, bstring key, bstring value);
bstring ht_config_search(const struct hashtable *ht, const_bstring key);
bstring ht_config_remove(struct hashtable *ht, bstring key);

struct hashtable * ht_config_create(unsigned int minsize);
void ht_config_destroy(struct hashtable *ht_config);

int ht_vconfig_insert(struct hashtable *ht, int *key, void *value);
void *ht_vconfig_search(const struct hashtable *ht, const int *key);
void *ht_vconfig_remove(struct hashtable *ht, int *key);

struct hashtable * ht_vconfig_create(unsigned int minsize);
void ht_vconfig_destroy(struct hashtable *ht_config);
