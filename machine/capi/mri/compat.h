#ifndef RUBY_COMPAT_H
#define RUBY_COMPAT_H

/*
 * Rules for better ruby C extensions:
 *
 * Never use the R<TYPE> macros directly, always use R<TYPE>_<FIELD>
 *
 * Never compare with RBASIC(obj)->klass, always use
 *   rb_obj_is_instance_of()
 *
 * Never use RHASH(obj)->tbl or RHASH_TBL().
 *
 */


#ifndef RUBY_COMPAT_LEVEL

// Set COMPAT_LEVEL based on the version

#include "version.h"
#if RUBY_VERSION_MAJOR >= 1
#if  RUBY_VERSION_MINOR >= 8

#if   RUBY_VERSION_TEENY >= 7
#define RUBY_COMPAT_LEVEL 1
#endif

#elif RUBY_VERSION_MINOR >= 9

#define RUBY_COMPAT_LEVEL 3

#endif
#endif

// Fall back and at least set it to 0

#ifndef RUBY_COMPAT_LEVEL
#define RUBY_COMPAT_LEVEL 0
#endif

#endif

// Array
#ifndef RARRAY_PTR
#define RARRAY_PTR(obj) RARRAY(obj)->ptr
#endif

#ifndef RARRAY_LEN
#define RARRAY_LEN(obj) RARRAY(obj)->len
#endif

// String
#ifndef RSTRING_PTR
#define RSTRING_PTR(obj) RSTRING(obj)->ptr
#endif

#ifndef RSTRING_LEN
#define RSTRING_LEN(obj) RSTRING(obj)->len
#endif

#ifndef rb_str_ptr
#define rb_str_ptr(str) RSTRING_PTR(str)
#endif

#ifndef rb_str_ptr_readonly
#define rb_str_ptr_readonly(str) RSTRING_PTR(str)
#endif

#ifndef rb_str_len
#define rb_str_len(str) RSTRING_LEN(str)
#endif

// Hash

#ifndef RHASH_TBL
#define RHASH_TBL(obj) RHASH(obj)->tbl
#endif

#ifndef RHASH_ITER_LEV
#define RHASH_ITER_LEV(obj) RHASH(obj)->iter_lev
#endif

#ifndef RHASH_IFNONE
#define RHASH_IFNONE(obj) RHASH(obj)->ifnone
#endif

#ifndef RHASH_SIZE
#define RHASH_SIZE(obj) RHASH(hash)->tbl->num_entries
#endif

#ifndef RHASH_EMPTY_P
#define RHASH_EMPTY_P(obj) (RHASH_SIZE(obj) == 0)
#endif

#if RUBY_COMPAT_LEVEL < 1
#include "st.h"

static VALUE
rb_hash_lookup(hash, key)
    VALUE hash, key;
{
  VALUE val;

  if(!st_lookup(RHASH(hash)->tbl, key, &val)) {
    return Qnil; /* without Hash#default */
  }
  return val;
}

#endif

#if RUBY_COMPAT_LEVEL < 2
static VALUE
rb_hash_size(hash)
    VALUE hash;
{
  return INT2FIX(RHASH_SIZE(hash));
}
#endif

#if RUBY_COMPAT_LEVEL < 2
#include "st.h"

static int
__rb_hash_clear_clear_i(key, value, dummy)
    VALUE key, value, dummy;
{
    return ST_DELETE;
}

static VALUE
rb_hash_clear(hash)
    VALUE hash;
{
  if(RHASH(hash)->tbl->num_entries > 0) {
    rb_hash_foreach(hash, __rb_hash_clear_clear_i, 0);
  }

  return hash;
}
#endif

#endif
