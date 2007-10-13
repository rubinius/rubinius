#include <stdlib.h>
#include "config_hash.h"


DEFINE_HASHTABLE_INSERT(ht_config_insert, struct tagbstring, struct tagbstring);
DEFINE_HASHTABLE_SEARCH(ht_config_search, struct tagbstring, struct tagbstring);
DEFINE_HASHTABLE_REMOVE(ht_config_remove, struct tagbstring, struct tagbstring);
unsigned int bstring_hash(const void * value)
{
  unsigned int retval = 0;
  int i = 0;
  for (; i < blength((const_bstring)value); ++i)
    {
      retval += 5*bchar((const_bstring)value, i);
    }
  return retval;
}

int bstring_eq(const void * value1, const void * value2)
{
  return biseq((const_bstring)value1, (const_bstring)value2);
}

struct hashtable * ht_config_create(unsigned int minsize)
{
  return create_hashtable(minsize, bstring_hash, bstring_eq);
}

void ht_config_destroy(struct hashtable *ht_config)
{
  struct hashtable_itr * iter = hashtable_iterator(ht_config);
  do 
    {
      bdestroy((bstring)hashtable_iterator_key(iter));
      bdestroy((bstring)hashtable_iterator_value(iter));
    }
  while (hashtable_iterator_remove(iter));
  free(iter);
  hashtable_destroy(ht_config, 0);
}
