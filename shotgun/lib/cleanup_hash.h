#ifndef RBS_CLEANUP_HASH_H
#define RBS_CLEANUP_HASH_H

#include <hashtable.h>
#include "state.h"
#include "oop.h"

inline int ht_cleanup_insert(struct hashtable *h, OBJECT k, state_cleanup_func v)
{
  return hashtable_insert(h, (void*)k, v);
}
inline state_cleanup_func ht_cleanup_search(const struct hashtable *h, const OBJECT k)
{
  return (state_cleanup_func) hashtable_search(h,(void*)k);
}

inline state_cleanup_func ht_cleanup_remove(struct hashtable *h, OBJECT k)
{
  return (state_cleanup_func) hashtable_remove(h,(void*)k);
}

inline unsigned int pointer_hash(const void * value)
{
  return (unsigned int) (intptr_t)  value;
}

inline int pointer_eq(const void * value1, const void * value2)
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

#endif /* RBS_CLEANUP_HASH_H */
