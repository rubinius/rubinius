#include "shotgun.h"
#include "cpu.h"
#include "string.h"
#include "bytearray.h"
#include "symbol.h"
#include "tuple.h"
#include "bignum.h"
#include "float.h"
#include <string.h>
#include <stdlib.h>
#include <glib.h>

struct marshal_state {
  int consumed;
  GPtrArray *objects;
};

static int _find_object(OBJECT obj, struct marshal_state *ms) {
  int i;
  for(i = 0; i < ms->objects->len; i++) {
    if(obj == (OBJECT)g_ptr_array_index(ms->objects, i)) {
      return i;
    }
  }
  
  return -1;
}

static void _add_object(OBJECT obj, struct marshal_state *ms) {
  g_ptr_array_add(ms->objects, (gpointer)obj);
}

static OBJECT unmarshal(STATE, char *str, struct marshal_state *ms);
static void   marshal(STATE, OBJECT obj, GString *buf, struct marshal_state *ms);

#define append_c(ch) g_string_append_c(buf, ch)
static void _append_sz(GString *buf, unsigned int i) {
  char bytes[4];
  bytes[0] = ( i >> 24 ) & 0xff;
  bytes[1] = ( i >> 16 ) & 0xff;
  bytes[2] = ( i >> 8  ) & 0xff;
  bytes[3] = i & 0xff;
  g_string_append_len(buf, bytes, 4);
}
#define append_sz(sz) _append_sz(buf, (unsigned int)sz)
#define append_str(str, sz) g_string_append_len(buf, str, sz)

static void marshal_int(STATE, OBJECT obj, GString *buf) {
  int i;
  i = FIXNUM_TO_INT(obj);
  append_c('i');
  if(i < 0) {
    i = abs(i);
    append_c('n');
  } else {
    append_c('p');
  }
  append_sz(i);
}

static int read_int(char *str) {
  return (int)( ( (unsigned char)str[0] << 24 )
              | ( (unsigned char)str[1] << 16 )
              | ( (unsigned char)str[2] << 8  )
              |   (unsigned char)str[3]         );
}

static OBJECT _nth_object(STATE, char *str, struct marshal_state *ms) {
  int ref;
  
  ms->consumed += 5;
  ref = read_int(str + 1);
  return (OBJECT)g_ptr_array_index(ms->objects, ref);
}

static OBJECT unmarshal_int(STATE, char *str, struct marshal_state *ms) {
  int i;
  ms->consumed += 6;
  i = read_int(str + 2);
  if(str[1] == 'n') {
    i = -i;
  }
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
  i = FIXNUM_TO_INT(string_get_bytes(str));
  append_c('x');
  append_sz(i);
  append_str(string_byte_address(state, str), i);
}

static OBJECT unmarshal_sym(STATE, char *str, struct marshal_state *ms) {
  OBJECT obj;
  int sz;
  char *name;
  
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  
  name = calloc(sz + 1, sizeof(char));
  memcpy(name, str + 5, sz);
  name[sz] = 0;
  
  obj = symtbl_lookup_cstr(state, state->global->symbols, name);
  free(name);
  
  return obj;
}

static void marshal_fields_as(STATE, OBJECT obj, GString *buf, char type, struct marshal_state *ms) {
  int sz, i;
  sz = NUM_FIELDS(obj);
  append_c(type);
  append_sz(sz);
  for(i = 0; i < sz; i++) {
    marshal(state, NTH_FIELD(obj, i), buf, ms);
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

/*

static void marshal_object(STATE, OBJECT obj, GString *buf, struct marshal_state *ms) {
  marshal_fields_as(state, obj, buf, 'o', ms);
  marshal_sym(state, class_get_name(object_logical_class(state, obj)));
}

static OBJECT unmarshal_object(STATE, char *str, struct marshal_state *ms) {
  OBJECT tup, name, klass;
  int before;
  
  before = ms->consumed;
  
  sz = unmarshal_num_fields(str);
  tup = tuple_new(state, sz);
  
  tup = unmarshal_into_fields(state, str, ms);
  str += (ms->consumed - before);
  name = unmarshal_sym(state, str, ms);
  klass = module_const_get(state, state->global->object, name);
  if(klass == Qnil || klass == Qundef) {
    printf("Couldn't resolve a generic object's class!\n");
    return Qnil;
  }
  
  HEADER(tup)->klass = klass;
  return tup;
}

*/

static void marshal_tup(STATE, OBJECT obj, GString *buf, struct marshal_state *ms) {
  return marshal_fields_as(state, obj, buf, 'p', ms);
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
  int i;
  char buffer[1024];
  char *s;
  bignum_into_string(state, obj, 10, buffer, 1024);
  append_c('B');
  i = strlen(buffer);
  append_sz(i);
  s = buffer;
  while(*s) {
    append_c(*s++);
  }
  append_c(0);                  /* zero byte */
}

static OBJECT unmarshal_bignum(STATE, char *str, struct marshal_state *ms) {
  int sz;
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  ms->consumed++;               /* zero byte */
  return bignum_from_string(state, str + 5, 10);
}

static void marshal_floatpoint(STATE, OBJECT obj, GString *buf) {
  int i;
  char buffer[26];
  char *s;

  float_into_string(state, obj, buffer, 26);
  append_c('d');
  i = strlen(buffer);
  append_sz(i);
  s = buffer;
  while(*s) {
    append_c(*s++);
  }
  append_c(0);               /* zero byte */
}

static OBJECT unmarshal_floatpoint(STATE, char *str, struct marshal_state *ms) {
  int sz;
  sz = read_int(str + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  ms->consumed++;               /* zero byte */
  return float_from_string(state, str + 5);
}

static void marshal_bytes(STATE, OBJECT obj, GString *buf) {
  int i;
  i = NUM_FIELDS(obj) * REFSIZE;
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

static void marshal_cmethod(STATE, OBJECT obj, GString *buf, struct marshal_state *ms) {
  marshal_fields_as(state, obj, buf, 'm', ms);
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
//  printf("%c\n", tag);
  switch(tag) {
    case 'i':
      o = unmarshal_int(state, str, ms);
      break;
    case 's':
      o = unmarshal_str(state, str, ms);
      _add_object(o, ms);
      break;
    case 'x':
      o = unmarshal_sym(state, str, ms);
      break;
    case 'p':
      o = unmarshal_tup(state, str, ms);
      _add_object(o, ms);
      break;
    case 'b':
      o = unmarshal_bytes(state, str, ms);
      _add_object(o, ms);
      break;
    case 'm':
      o = unmarshal_cmethod(state, str, ms);
      _add_object(o, ms);
      break;
    case 'B':
      o = unmarshal_bignum(state, str, ms);
      _add_object(o, ms);
      break;
    case 'd':
      o = unmarshal_floatpoint(state, str, ms);
      _add_object(o, ms);
      break;
    case 'r':
      o = _nth_object(state, str, ms);
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
      printf("Unknown marshal type '0x%x' at %d!\n", tag, ms->consumed);
      abort();
  }
  return o;
}

static void marshal(STATE, OBJECT obj, GString *buf, struct marshal_state *ms) {
  OBJECT kls;
  int ref;
  
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
    if((ref = _find_object(obj, ms)) > 0) {
      append_c('r');
      append_sz(ref);
    } else {
      _add_object(obj, ms);
      kls = object_class(state, obj);
      if(kls == state->global->string) {
        marshal_str(state, obj, buf);
      } else if(kls == state->global->tuple) {
        marshal_tup(state, obj, buf, ms);
      } else if(kls == state->global->cmethod) {
        marshal_cmethod(state, obj, buf, ms);
      } else if(kls == state->global->bytearray) {
        marshal_bytes(state, obj, buf);
      } else if(kls == BASIC_CLASS(bignum)) {
        marshal_bignum(state, obj, buf);
      } else if(kls == BASIC_CLASS(floatpoint)) {
        marshal_floatpoint(state, obj, buf);
      } else {
        printf("Unable to marshal class %p = %s!\n", (void *)kls, rbs_inspect(state, kls));
      }
    }
  }
}

OBJECT cpu_marshal(STATE, OBJECT obj, int version) {
  GString *buf;
  OBJECT ret;
  struct marshal_state ms;
  ms.consumed = 0;
  ms.objects = g_ptr_array_new();
  
  buf = g_string_new(NULL);
  g_string_append(buf, "RBIX");
  _append_sz(buf, version);
  
  marshal(state, obj, buf, &ms);
  ret = string_new2(state, buf->str, buf->len);
  g_string_free(buf, 1);
  g_ptr_array_free(ms.objects, 1);
  return ret;
}

GString *cpu_marshal_to_gstring(STATE, OBJECT obj, int version) {
  GString *buf;
  struct marshal_state ms;
  
  ms.consumed = 0;
  ms.objects = g_ptr_array_new();
  
  buf = g_string_new(NULL);
  g_string_append(buf, "RBIX");
  _append_sz(buf, version);
  marshal(state, obj, buf, &ms);
  g_ptr_array_free(ms.objects, 1);
  return buf;
}

OBJECT cpu_marshal_to_file(STATE, OBJECT obj, char *path, int version) {
  GString *buf;
  GIOChannel *io;
  GError *err;
  gsize sz;
  struct marshal_state ms;
  
  err = NULL;
  
  io = g_io_channel_new_file(path, "w", &err);
  if(io == NULL) { return Qfalse; }
  
  ms.consumed = 0;
  ms.objects = g_ptr_array_new();
  
  buf = g_string_new(NULL);
  _append_sz(buf, version);
  marshal(state, obj, buf, &ms);
  
  g_io_channel_set_encoding(io, NULL, &err);
  /* TODO do error chceking here */
  g_io_channel_write_chars(io, "RBIX", -1, NULL, &err);
  g_io_channel_write_chars(io, buf->str, buf->len, &sz, &err);
  g_io_channel_shutdown(io, TRUE, &err);
  g_io_channel_unref(io);
  g_string_free(buf, 1);
  g_ptr_array_free(ms.objects, 1);
  return Qtrue;
}

OBJECT cpu_unmarshal(STATE, char *str, int version) {
  struct marshal_state ms;
  OBJECT ret;
  int offset = 4;
  if(!strncmp(str, "RBIS", 4)) {
    version = -1;
  } else if(!strncmp(str, "RBIX", 3)) {
    if(read_int(str + 3) < version) {
      /* file is out of date. */
      return Qnil;
    }
    offset += 4;
  } else {
    printf("Invalid compiled file.\n");
    return Qnil;
  }
  ms.consumed = 0;
  ms.objects = g_ptr_array_new();
  str += offset;
  ret = unmarshal(state, str, &ms);
  g_ptr_array_free(ms.objects, 1);
  return ret;
}

OBJECT cpu_unmarshal_file(STATE, char *path, int version) {
  gchar *data;
  GIOChannel *io;
  GError *err;
  gsize sz, count = 4;
  gchar buf[4];
  OBJECT obj = Qnil;
  struct marshal_state ms;
  ms.consumed = 0;
  
  err = NULL;
  
  io = g_io_channel_new_file(path, "r", &err);
  if(io == NULL) {return Qnil;}
  g_io_channel_set_encoding(io, NULL, &err);
  g_io_channel_read_chars(io, (gchar*)&buf, count, &sz, &err);
  ms.objects = g_ptr_array_new();
  if(!strncmp(buf, "RBIS", 4)) {
    version = -1;
  } else if(!strncmp(buf, "RBIX", 4)) {
    g_io_channel_read_chars(io, (gchar*)&buf, count, &sz, &err);
    if(read_int(buf) < version) {
      /* out of date. */
      goto cleanup;
    }
  } else {
    printf("Invalid compiled file.\n");
    goto cleanup;
  }
  
  g_io_channel_read_to_end(io, &data, &sz, &err);
  obj = unmarshal(state, data, &ms);
cleanup:
  g_io_channel_shutdown(io, TRUE, &err);
  g_io_channel_unref(io);
  g_free(data);
  g_ptr_array_free(ms.objects, 1);
  return obj;
}

