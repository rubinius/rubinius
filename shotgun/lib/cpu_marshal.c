#include "shotgun.h"
#include "cpu.h"
#include "string.h"
#include "bytearray.h"
#include "symbol.h"
#include "tuple.h"
#include "bignum.h"
#include <string.h>
#include <stdlib.h>
#include <glib.h>

struct marshal_state {
  int consumed;
};

static OBJECT unmarshal(STATE, char *str, struct marshal_state *ms);
static void   marshal(STATE, OBJECT obj, GString *buf);

#define append_c(ch) g_string_append_c(buf, ch)
#define append_sz(sz) g_string_append_len(buf, (char*)&sz, 4)
#define append_str(str, sz) g_string_append_len(buf, str, sz)

static void marshal_int(STATE, OBJECT obj, GString *buf) {
  int i;
  i = FIXNUM_TO_INT(obj);
  append_c('i');
  append_sz(i);
}

static int read_int(char *str) {
  int i;
  memcpy(&i, str, 4);
#ifndef __BIG_ENDIAN__
  i = swap32(i);
#endif
  return i;
}

static OBJECT unmarshal_int(STATE, char *str, struct marshal_state *ms) {
  int i;
  ms->consumed += 5;
  i = read_int(str + 1);
  return I2N(i);
}

static void marshal_str(STATE, OBJECT obj, GString *buf) {
  int i;
  i = FIXNUM_TO_INT(string_get_bytes(obj));
  append_c('s');
  append_sz(i);
  append_str(string_byte_address(state, obj), i);
}

static OBJECT unmarshal_str(STATE, char *str, struct marshal_state *ms) {
  int sz;
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  return string_new2(state, str + 5, sz);
}

static void marshal_sym(STATE, OBJECT obj, GString *buf) {
  OBJECT str;
  int i;
  str = symtbl_find_string(state, state->global->symbols, obj);
  i = FIXNUM_TO_INT(string_get_bytes(obj));
  append_c('x');
  append_sz(i);
  append_str(string_byte_address(state, obj), i);
}

static OBJECT unmarshal_sym(STATE, char *str, struct marshal_state *ms) {
  OBJECT obj;
  obj = unmarshal_str(state, str, ms);
  return string_to_sym(state, obj);
}

static void marshal_fields_as(STATE, OBJECT obj, GString *buf, char type) {
  int sz, i;
  sz = NUM_FIELDS(obj);
  append_c(type);
  append_sz(sz);
  for(i = 0; i < sz; i++) {
    marshal(state, NTH_FIELD(obj, i), buf);    
  }
}

static OBJECT unmarshal_into_fields(STATE, char *str, int sz, OBJECT tup, struct marshal_state *ms) {
  char *buf;
  int i, j, cur;
  OBJECT o;
  ms->consumed += 5;
  buf = str + 5;
  cur = ms->consumed;
  for(i = 0; i < sz; i++) {
    o = unmarshal(state, buf, ms);
    j = ms->consumed - cur;
    buf += j;
    cur = ms->consumed;
    SET_FIELD(tup, i, o);
  }
  
  return tup;
}

static int unmarshal_num_fields(char *str) {
  int i;
  i = read_int(str + 1);
  return i;
}

static void marshal_tup(STATE, OBJECT obj, GString *buf) {
  return marshal_fields_as(state, obj, buf, 'p');
}

static OBJECT unmarshal_tup(STATE, char *str, struct marshal_state *ms) {
  int sz;
  OBJECT tup;
  sz = unmarshal_num_fields(str);
  tup = tuple_new(state, sz);
  unmarshal_into_fields(state, str, sz, tup, ms);
  return tup;
}

static void marshal_bignum(STATE, OBJECT obj, GString *buf) {
  int i, j;
  char buffer[1024];
  char *s;
  i = NUM_FIELDS(obj);
  bignum_into_string(state, obj, 10, buffer, 1024);
  append_c('B');
  i = strlen(buffer);
  append_sz(i);
  s = buffer;
  while(*s) {
    append_c(*s++);
  }
  append_c(0);
}

static OBJECT unmarshal_bignum(STATE, char *str, struct marshal_state *ms) {
  OBJECT obj;
  int i, sz;
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  return bignum_from_string(state, str + 5, 10);
}

static void marshal_bytes(STATE, OBJECT obj, GString *buf) {
  int i;
  i = NUM_FIELDS(obj);
  append_c('b');
  append_sz(i);
  append_str(bytearray_byte_address(state, obj), i);
}

static OBJECT unmarshal_bytes(STATE, char *str, struct marshal_state *ms) {
  int sz;
  OBJECT obj;
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  obj = bytearray_new(state, sz);
  
  memcpy(bytearray_byte_address(state, obj), str + 5, sz);
  
  // printf("Unmarshaled bytes: %p / %d / %d\n", bytearray_byte_address(state, obj), sz, bytearray_bytes(state, obj));
  
  return obj;
}

static void marshal_cmethod(STATE, OBJECT obj, GString *buf) {
  marshal_fields_as(state, obj, buf, 'm');
}

static OBJECT unmarshal_cmethod(STATE, char *str, struct marshal_state *ms) {
  int sz;
  OBJECT cm;
  sz = unmarshal_num_fields(str);
  cm = cmethod_allocate(state);
  unmarshal_into_fields(state, str, sz, cm, ms);
  return cm;
}

static OBJECT unmarshal(STATE, char *str, struct marshal_state *ms) {
  char tag;
  OBJECT o;
  tag = str[0];
  // printf("%c\n", tag);
  switch(tag) {
    case 'i':
      o = unmarshal_int(state, str, ms);
      break;
    case 's':
      o = unmarshal_str(state, str, ms);
      break;
    case 'x':
      o = unmarshal_sym(state, str, ms);
      break;
    case 'p':
      o = unmarshal_tup(state, str, ms);
      break;
    case 'b':
      o = unmarshal_bytes(state, str, ms);
      break;
    case 'm':
      o = unmarshal_cmethod(state, str, ms);
      break;
    case 'B':
      o = unmarshal_bignum(state, str, ms);
      break;
    case 'n':
      ms->consumed += 1;
      o = Qnil;
      break;
    case 't':
      ms->consumed += 1;
      o = Qtrue;
      break;
    case 'f':
      ms->consumed += 1;
      o = Qfalse;
      break;
    default:
      printf("Unknown type '%c'!\n", tag);
      abort();
  }
  return o;
}

static void marshal(STATE, OBJECT obj, GString *buf) {
  OBJECT kls;
  if(FIXNUM_P(obj)) {
    marshal_int(state, obj, buf);
  } else if(SYMBOL_P(obj)) {
    marshal_sym(state, obj, buf);
  } else if(obj == Qnil) {
    append_c('n');
  } else if(obj == Qtrue) {
    append_c('t');
  } else if(obj == Qfalse) {
    append_c('f');
  } else if(REFERENCE_P(obj)) {
    kls = object_class(state, obj);
    if(kls == state->global->string) {
      marshal_str(state, obj, buf);
    } else if(kls == state->global->tuple) {
      marshal_tup(state, obj, buf);
    } else if(kls == state->global->cmethod) {
      marshal_cmethod(state, obj, buf);
    } else if(kls == state->global->bytearray) {
      marshal_bytes(state, obj, buf);
    } else if(kls == BASIC_CLASS(bignum)) {
      marshal_bignum(state, obj, buf);
    } else {
      printf("Unable to marshal!\n");
      abort();
    }
  }
}

GString *cpu_marshal(STATE, OBJECT obj) {
  GString *buf;
  
  buf = g_string_new(NULL);
  marshal(state, obj, buf);
  return buf;
}

OBJECT cpu_unmarshal(STATE, char *str) {
  struct marshal_state ms;
  ms.consumed = 0;
  return unmarshal(state, str, &ms);
}

OBJECT cpu_unmarshal_file(STATE, char *path) {
  gchar *data;
  GIOChannel *io;
  GError *err;
  gsize sz, count = 4;
  gchar buf[4];
  OBJECT obj;
  
  err = NULL;
  
  io = g_io_channel_new_file(path, "r", &err);
  if(io == NULL) {return Qnil;}
  g_io_channel_set_encoding(io, NULL, &err);
  g_io_channel_read_chars(io, (gchar*)&buf, count, &sz, &err);
  if(strncmp(buf, "RBIS", 4)) {
    printf("Invalid compiled file.\n");
    return Qnil;
  }
  g_io_channel_read_to_end(io, &data, &sz, &err);
  
  obj = cpu_unmarshal(state, data);
  g_io_channel_shutdown(io, TRUE, &err);
  g_io_channel_unref(io);
  g_free(data);
  return obj;
}
