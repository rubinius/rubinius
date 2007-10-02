#ifndef __CLEANUP_HASH__
#define __CLEANUP_HASH__

#include <hashtable.h>
#include "state.h"
#include "oop.h"

inline int ht_cleanup_insert(struct hashtable *h, OBJECT k, state_cleanup_func v)
{
  return hashtable_insert(h, (void*)k, v);
}
inline state_cleanup_func ht_cleanup_search(struct hashtable *h, OBJECT k)
{
  return (state_cleanup_func) hashtable_search(h,(void*)k);
}

inline state_cleanup_func ht_cleanup_remove(struct hashtable *h, OBJECT k)
{
  return (state_cleanup_func) hashtable_remove(h,(void*)k);
}

inline unsigned int pointer_hash(void * value)
{
  return (unsigned int) (intptr_t)  value;
}

inline int pointer_eq(void * value1, void * value2)
{
  return value1 == value2;
}

inline struct hashtable * ht_cleanup_create(unsigned int minsize)
{
  return create_hashtable(minsize, pointer_hash, pointer_eq);
}

inline void ht_cleanup_destroy(struct hashtable *ht_cleanup)
{
  hashtable_destroy(ht_cleanup, FALSE);
}

#endif /* __CLEANUP_HASH__ */
