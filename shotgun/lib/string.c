#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <bstrlib.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/string.h"

OBJECT string_new2(STATE, const char *str, int sz) {
  OBJECT obj, data;
  char *ba;

  xassert(sz >= 0);
  obj = string_allocate(state);
  string_set_bytes(obj, I2N(sz));
  string_set_characters(obj, I2N(sz));
  string_set_encoding(obj, Qnil);

  data = bytearray_new_dirty(state, sz+1);
  ba = bytearray_byte_address(state, data);
  memset(ba, 0, SIZE_OF_BODY(data));

  if(str != NULL && sz > 0) {
    memcpy(ba, str, sz);
    ba[sz] = 0;
  }

  string_set_data(obj, data);
  return obj;
}

OBJECT string_new(STATE, const char *str) {
  int sz;

  if(str == NULL) {
    return string_new2(state, str, 0);
  }

  sz = strlen(str);
  return string_new2(state, str, sz);
}

OBJECT string_new_shared(STATE, OBJECT cur) {
  OBJECT obj;

  obj = string_allocate(state);
  string_set_bytes(obj, string_get_bytes(cur));
  string_set_characters(obj, string_get_bytes(cur));
  string_set_encoding(obj, Qnil);

  string_set_data(obj, string_get_data(cur));
  string_set_shared(obj, Qtrue);
  string_set_shared(cur, Qtrue);
  return obj;
}

OBJECT string_newfrombstr(STATE, bstring str)
{
  if(str == NULL) {
    return string_new2(state, NULL, 0);
  }

  return string_new2(state, (const char*)str->data, str->slen);
}

OBJECT string_dup(STATE, OBJECT self) {
  OBJECT obj;
  xassert(STRING_P(self));
  obj = string_new_shared(state, self);
  SET_CLASS(obj, object_class(state, self));
  obj->IsTainted = self->IsTainted;
  return obj;
}

OBJECT string_append(STATE, OBJECT self, OBJECT other) {
  OBJECT cur, obs, nd;
  int cur_sz, oth_sz, ns, tmp, extra;
  char *ba;

  xassert(STRING_P(self));
  xassert(STRING_P(other));

  string_unshare(state, self);

  cur = string_get_data(self);
  obs = string_get_data(other);
  cur_sz = N2I(string_get_bytes(self));
  oth_sz = N2I(string_get_bytes(other));

  ns = cur_sz + oth_sz;
  tmp = bytearray_bytes(state, cur);
  if(ns+1 > tmp) {
    extra = ns * 0.01;
    if(extra < 10) extra = 10;
    nd = bytearray_new_dirty(state, ns+extra);
    object_copy_bytes_into(state, cur, nd, cur_sz, 0);
    object_copy_bytes_into(state, obs, nd, oth_sz, cur_sz);
    ba = bytearray_byte_address(state, nd);
    string_set_data(self, nd);
  } else {
    object_copy_bytes_into(state, obs, cur, oth_sz, cur_sz);
    ba = bytearray_byte_address(state, cur);
  }
  ba[ns] = 0;
  string_set_bytes(self, I2N(ns));
  return self;
}

/* returns pointer to bytearray string based on */
char *string_byte_address(STATE, OBJECT self) {
  OBJECT data;

  xassert(STRING_P(self));
  data = string_get_data(self);
  if(NIL_P(data)) {
    return (char*)"";
  }

  return bytearray_byte_address(state, data);
}

/* Direct pointer to underlying string: handle with care! */
char* rbx_string_as_cstr(STATE, OBJECT self)
{
  xassert(STRING_P(self));

  /* Terminator may be incorrect due to explicit length being used usually */
  char* cstr = string_byte_address(state, self);
  cstr[((size_t)N2I(string_get_bytes(self)))] = '\0';

  return cstr;
}

double string_to_double(STATE, OBJECT self) {
  double value;
  char *p, *n, *ba, *rest;
  int e_seen = 0;
  xassert(STRING_P(self));

  // We'll modify the buffer, so we need our own copy.
  ba = bytearray_as_string(state, string_get_data(self));

  p = ba;
  while (ISSPACE(*p)) p++;
  n = p;

  while (*p) {
    if (*p == '_') {
      p++;
    } else {
      if(*p == 'e' || *p == 'E') {
        if(e_seen) {
          *n = 0;
          break;
        }
        e_seen = 1;
      } else if(!(ISDIGIT(*p) || *p == '.' || *p == '-' || *p == '+')) {
        *n = 0;
        break;
      }

      *n++ = *p++;
    }
  }
  *n = 0;

  /* Some implementations of strtod() don't guarantee to
   * set errno, so we need to reset it ourselves.
   */
  errno = 0;

  value = strtod(ba, &rest);
  if (errno == ERANGE) {
    printf("Float %s out of range\n", ba);
  }

  free(ba);


  return value;
}

static OBJECT tr_replace(STATE, OBJECT string, int bytes, unsigned char *str,
                         unsigned char *data, int size, int steps) {
  if(size > bytes || RTEST(string_get_shared(string))) {
    OBJECT ba;

    ba = bytearray_new_dirty(state, size+1);
    str = bytearray_byte_address(state, ba);
    memset(str, 0, SIZE_OF_BODY(ba));
    string_set_data(string, ba);
    string_set_shared(string, Qnil);
  }

  memcpy(str, data, size);
  str[size] = 0;

  string_set_bytes(string, I2N(size));
  string_set_characters(string, I2N(size));

  return I2N(steps);
}

#define tr_assign(set, tr, last, c) { \
  int _j, _i = set[c]; \
\
  if(lim >= 0 && steps >= lim) { \
    return tr_replace(state, string, bytes, str, tr, last, steps); \
  } \
\
  if(_i < 0) { \
    tr[last] = c; \
  } else { \
    last--; \
    for(_j = _i+1; _j <= last; _j++) { \
      set[tr[_j]]--; \
      tr[_j-1] = tr[_j]; \
    } \
    tr[last] = c; \
  } \
  set[c] = last++; \
  steps++; \
}

OBJECT string_tr_expand(STATE, OBJECT string, OBJECT limit) {
  int i, start, bytes,
      steps = 0, lim = -1,
      last = 0, c, seq, max, set[256];
  unsigned char *str, tr[256];

  if(!NIL_P(limit)) {
    lim = N2I(limit);
  }

  str = (unsigned char *)string_byte_address(state, string);
  bytes = N2I(string_get_bytes(string));
  start = bytes > 1 && str[0] == '^' ? 1 : 0;
  memset(set, -1, sizeof(int) * 256);

  for(i = start; i < bytes;) {
    c = str[i];
    seq = ++i < bytes ? str[i] : -1;

    if(seq == '-') {
      max = ++i < bytes ? str[i] : -1;
      if(max >= 0) {
        while(c <= max) {
          tr_assign(set, tr, last, c);
          c++;
        }
        i++;
      } else {
        tr_assign(set, tr, last, c);
        tr_assign(set, tr, last, seq);
      }
    } else if(c == '\\' && seq >= 0) {
      continue;
    } else {
      tr_assign(set, tr, last, c);
    }
  }

  return tr_replace(state, string, bytes, str, tr, last, steps);
}

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

unsigned int string_hash_str(unsigned char *bp, unsigned int sz) {
  unsigned char *be;
  unsigned int hv;

  be = bp + sz;

  hv = 0;

  while(bp < be) {
    hv *= HashPrime;
    hv ^= *bp++;
  }
  hv = (hv>>28) ^ (hv & MASK_28);

  return hv;
}

unsigned int string_hash_int(STATE, OBJECT self) {
  unsigned char *bp;
  unsigned int sz, h;
  OBJECT data, hsh;

  xassert(STRING_P(self));
  data = string_get_data(self);
  hsh = string_get_hash(self);
  if(hsh != Qnil) {
    return N2I(hsh);
  }
  bp = (unsigned char*)bytearray_byte_address(state, data);
  sz = N2I(string_get_bytes(self));

  h = string_hash_str(bp, sz);
  string_set_hash(self, UI2N(h));

  return h;
}

unsigned int string_hash_cstr(STATE, const char *bp) {
  unsigned int sz = strlen(bp);
  return string_hash_str((unsigned char*)bp, sz);
}

unsigned int string_hash_str_with_size(STATE, const char *bp, int size) {
  return string_hash_str((unsigned char*)bp, size);
}

OBJECT string_to_sym(STATE, OBJECT self) {
  xassert(STRING_P(self));
  return symtbl_lookup(state, state->global->symbols, self);
}

int string_equal_p(STATE, OBJECT self, OBJECT other) {
  OBJECT bytes;

  bytes = string_get_bytes(self);
  if(bytes != string_get_bytes(other)) return FALSE;
  if(strncmp(string_byte_address(state, self), string_byte_address(state, other), N2I(bytes))) {
    return FALSE;
  }

  return TRUE;
}
