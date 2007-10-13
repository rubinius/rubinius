#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "quark.h"
#include "hashtable.h"
#include "ptr_array.h"

static struct hashtable *quark_string_hash = NULL; /* const char * -> int */
static ptr_array quark_string_index = NULL;
static pthread_mutex_t quark_static_lock = PTHREAD_MUTEX_INITIALIZER;

enum {QUARK_NOT_FOUND = ~0L};

static unsigned int string_hash(const void *val)
{
  unsigned int retval = 0;
  const char *str = (const char *)val;
  while (*str)
  {
    retval += 5* (*str);
    str++;
  }
  return retval;
}
static int string_eq(const void* a1, const void* a2)
{
  return strcmp((const char*) a1, (const char*) a2) == 0;
}


/* assumes static lock is held */
static inline bool quark_init_if_needed()
{
  if (quark_string_hash == NULL)
  {
    quark_string_hash = create_hashtable(31, string_hash, string_eq);
    quark_string_index = ptr_array_new(10);
    ptr_array_append(quark_string_index, NULL);
    return true;
  }
  return false;
}

quark quark_from_string(const char *string)
{
  if (string == NULL) return 0;
  pthread_mutex_lock(&quark_static_lock);
  if (!quark_init_if_needed())
  {
    uintptr_t value = (uintptr_t)hashtable_search(quark_string_hash, string);
    if (value != 0)
    {
      pthread_mutex_unlock(&quark_static_lock);
      return value;
    }
  }
  string = strdup(string);
  if (!ptr_array_append(quark_string_index, string))
  { 
    pthread_mutex_unlock(&quark_static_lock);
    return QUARK_NOT_FOUND; 
  }
  size_t index = ptr_array_length(quark_string_index) - 1;
  hashtable_insert(quark_string_hash, (char*) string, (void*) index);
  pthread_mutex_unlock(&quark_static_lock);
  return index;
}

quark quark_from_static_string(const char *string)
{
  if (string == NULL) return 0;
  pthread_mutex_lock(&quark_static_lock);
  if (!quark_init_if_needed())
  {
    uintptr_t value = (uintptr_t)hashtable_search(quark_string_hash, string);
    if (value != 0)
    {
      pthread_mutex_unlock(&quark_static_lock);
      return value;
    }
  }
  if (!ptr_array_append(quark_string_index, string))
  { 
    pthread_mutex_unlock(&quark_static_lock);
    return QUARK_NOT_FOUND;
  }
  size_t index = ptr_array_length(quark_string_index) - 1;
  hashtable_insert(quark_string_hash, (char *)string, (void*) index);
  pthread_mutex_unlock(&quark_static_lock);
  return index;
}

const char * quark_to_string(quark q)
{
  const char *retval;
  if (q == 0) return NULL;
  pthread_mutex_lock(&quark_static_lock);
  if (quark_init_if_needed() || q >= ptr_array_length(quark_string_index))
  {
    pthread_mutex_unlock(&quark_static_lock);
    return NULL;
  }
  retval = ptr_array_get_index(quark_string_index, q);
  pthread_mutex_unlock(&quark_static_lock);
  return retval;
}
