#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#endif

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "quark.h"
#include "hashtable.h"
#include "ptr_array.h"

static struct hashtable *quark_string_hash = NULL; /* const char * -> int */
static ptr_array quark_string_index = NULL;

#ifdef _WIN32
static CRITICAL_SECTION quark_static_cs;
static bool cs_initialized = false;

static void mutex_lock()
{
  if (!cs_initialized) {
    InitializeCriticalSection(&quark_static_cs);

    cs_initialized = true;	  
  }

  EnterCriticalSection(&quark_static_cs);
}

static void mutex_unlock()
{
  LeaveCriticalSection(&quark_static_cs);
}
#else
static pthread_mutex_t quark_static_lock = PTHREAD_MUTEX_INITIALIZER;

static void mutex_lock()
{
  pthread_mutex_lock(&quark_static_lock);
}

static void mutex_unlock()
{
  pthread_mutex_unlock(&quark_static_lock);
}
#endif

enum {QUARK_NOT_FOUND = ~0L};

/* djb2 hash function */
static unsigned int string_hash(const void *val)
{
  unsigned int hash = 5381;
  const char *str = (const char *)val;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
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
  mutex_lock();
  if (!quark_init_if_needed())
  {
    uintptr_t value = (uintptr_t)hashtable_search(quark_string_hash, string);
    if (value != 0)
    {
      mutex_unlock();
      return value;
    }
  }
  string = strdup(string);
  if (!ptr_array_append(quark_string_index, string))
  { 
    mutex_unlock();
    return QUARK_NOT_FOUND; 
  }
  size_t index = ptr_array_length(quark_string_index) - 1;
  hashtable_insert(quark_string_hash, (char*) string, (void*) index);
  mutex_unlock();
  return index;
}

quark quark_from_static_string(const char *string)
{
  if (string == NULL) return 0;
  mutex_lock();
  if (!quark_init_if_needed())
  {
    uintptr_t value = (uintptr_t)hashtable_search(quark_string_hash, string);
    if (value != 0)
    {
      mutex_unlock();
      return value;
    }
  }
  if (!ptr_array_append(quark_string_index, string))
  { 
    mutex_unlock();
    return QUARK_NOT_FOUND;
  }
  size_t index = ptr_array_length(quark_string_index) - 1;
  hashtable_insert(quark_string_hash, (char *)string, (void*) index);
  mutex_unlock();
  return index;
}

const char * quark_to_string(quark q)
{
  const char *retval;
  if (q == 0) return NULL;
  mutex_lock();
  if (quark_init_if_needed() || q >= ptr_array_length(quark_string_index))
  {
    mutex_unlock();
    return NULL;
  }
  retval = ptr_array_get_index(quark_string_index, q);
  mutex_unlock();
  return retval;
}
