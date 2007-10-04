#include "shotgun.h"
#include "cpu.h"
#include "string.h"
#include "bytearray.h"
#include "symbol.h"
#include "tuple.h"
#include "bignum.h"
#include "float.h"
#include "flags.h"
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static OBJECT string_newfrombstr(STATE, bstring output)
{
	return string_new2(state, bdata(output), blength(output));
}

struct marshal_state {
  int consumed;
  ptr_array objects;
  uint8_t *buf;
};

static int _find_object(OBJECT obj, struct marshal_state *ms) {
  int i;
  for(i = 0; i < ptr_array_length(ms->objects); i++) {
    if(obj == (OBJECT)ptr_array_get_index(ms->objects, i)) {
      return i;
    }
  }
  
  return -1;
}

static void _add_object(OBJECT obj, struct marshal_state *ms) {
  ptr_array_append(ms->objects, (gpointer)obj);
}

static OBJECT unmarshal(STATE, struct marshal_state *ms);
static void   marshal(STATE, OBJECT obj, bstring buf, struct marshal_state *ms);

#define append_c(ch) bconchar(buf, ch)
static void _append_sz(bstring buf, unsigned int i) {
  char bytes[4];
  bytes[0] = ( i >> 24 ) & 0xff;
  bytes[1] = ( i >> 16 ) & 0xff;
  bytes[2] = ( i >> 8  ) & 0xff;
  bytes[3] = i & 0xff;
  bcatblk(buf, bytes, 4);
}
#define append_sz(sz) _append_sz(buf, (unsigned int)sz)
#define append_str(str, sz) bcatblk(buf, str, sz)

static void marshal_int(STATE, OBJECT obj, bstring buf) {
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

static int read_int(uint8_t *str) {
  return (int)( (str[0] << 24)
              | (str[1] << 16)
              | (str[2] << 8 )
              |  str[3]      );
}

static OBJECT _nth_object(STATE, struct marshal_state *ms) {
  int ref;
  
  ms->consumed += 5;
  ref = read_int(ms->buf + 1);
  return (OBJECT)ptr_array_get_index(ms->objects, ref);
}

static OBJECT unmarshal_int(STATE, struct marshal_state *ms) {
  int i;
  ms->consumed += 6;
  i = read_int(ms->buf + 2);
  if(ms->buf[1] == 'n') {
    i = -i;
  }
  return I2N(i);
}

static void marshal_str(STATE, OBJECT obj, bstring buf) {
  int i;
  i = FIXNUM_TO_INT(string_get_bytes(obj));
  append_c('s');
  append_sz(i);
  append_str(string_byte_address(state, obj), i);
}

static OBJECT unmarshal_str(STATE, struct marshal_state *ms) {
  int sz;
  sz = read_int(ms->buf + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  return string_new2(state, (char *) ms->buf + 5, sz);
}

static void marshal_sym(STATE, OBJECT obj, bstring buf) {
  OBJECT str;
  int i;
  str = symtbl_find_string(state, state->global->symbols, obj);
  i = FIXNUM_TO_INT(string_get_bytes(str));
  append_c('x');
  append_sz(i);
  append_str(string_byte_address(state, str), i);
}

static OBJECT unmarshal_sym(STATE, struct marshal_state *ms) {
  int sz;
  
  sz = read_int(ms->buf + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  
  return symtbl_lookup_str_with_size(state, state->global->symbols,
                                     (char *) ms->buf + 5, sz);
}

static void marshal_fields_as(STATE, OBJECT obj, bstring buf, char type, struct marshal_state *ms) {
  int sz, i;
  sz = NUM_FIELDS(obj);
  append_c(type);
  append_sz(sz);
  for(i = 0; i < sz; i++) {
    marshal(state, NTH_FIELD(obj, i), buf, ms);
  }
}

static OBJECT unmarshal_into_fields(STATE, int sz, OBJECT tup, struct marshal_state *ms) {
  int i, j, cur;
  OBJECT o;
  cur = ms->consumed;
  for(i = 0; i < sz; i++) {
    uint8_t *old = ms->buf;

    o = unmarshal(state, ms);
    j = ms->consumed - cur;
    ms->buf = old + j;
    cur = ms->consumed;
    SET_FIELD(tup, i, o);
  }
  
  return tup;
}

static int unmarshal_num_fields(struct marshal_state *ms) {
  int i;
  i = read_int(ms->buf + 1);

  ms->consumed += 5;
  ms->buf += 5;

  return i;
}

/*

static void marshal_object(STATE, OBJECT obj, bstring buf, struct marshal_state *ms) {
  marshal_fields_as(state, obj, buf, 'o', ms);
  marshal_sym(state, class_get_name(object_class(state, obj)));
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

static void marshal_tup(STATE, OBJECT obj, bstring buf, struct marshal_state *ms) {
  return marshal_fields_as(state, obj, buf, 'p', ms);
}

static OBJECT unmarshal_tup(STATE, struct marshal_state *ms) {
  int sz;
  OBJECT tup;
  sz = unmarshal_num_fields(ms);
  tup = tuple_new(state, sz);
  unmarshal_into_fields(state, sz, tup, ms);
  return tup;
}

static void marshal_bignum(STATE, OBJECT obj, bstring buf) {
  int i;
  char buffer[1024];
  bignum_into_string(state, obj, 10, buffer, 1024);
  append_c('B');
  i = strlen(buffer);
  append_sz(i);
  bcatblk(buf, buffer, i);
  append_c(0);                  /* zero byte */
}

static OBJECT unmarshal_bignum(STATE, struct marshal_state *ms) {
  int sz;
  sz = read_int(ms->buf + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  ms->consumed++;               /* zero byte */
  return bignum_from_string(state, (char *) ms->buf + 5, 10);
}

static void marshal_floatpoint(STATE, OBJECT obj, bstring buf) {
  int i;
  char buffer[26];

  float_into_string(state, obj, buffer, 26);
  append_c('d');
  i = strlen(buffer);
  append_sz(i);
  bcatblk(buf, buffer, i);
  append_c(0);               /* zero byte */
}

static OBJECT unmarshal_floatpoint(STATE, struct marshal_state *ms) {
  int sz;
  sz = read_int(ms->buf + 1);
  ms->consumed += 5;
  ms->consumed += sz;
  ms->consumed++;               /* zero byte */
  return float_from_string(state, (char *) ms->buf + 5);
}

static void marshal_bytes(STATE, OBJECT obj, bstring buf) {
  int i;
  i = NUM_FIELDS(obj) * REFSIZE;
  append_c('b');
  append_sz(i);
  append_str(bytearray_byte_address(state, obj), i);
}

static OBJECT unmarshal_bytes(STATE, struct marshal_state *ms) {
  int sz;
  OBJECT obj;
  sz = read_int(ms->buf + 1);
  
  ms->consumed += 5;
  ms->consumed += sz;
  obj = bytearray_new(state, sz);
  
  memcpy(bytearray_byte_address(state, obj), ms->buf + 5, sz);
  
  // printf("Unmarshaled bytes: %p / %d / %d\n", bytearray_byte_address(state, obj), sz, bytearray_bytes(state, obj));
  
  return obj;
}

static void marshal_iseq(STATE, OBJECT obj, bstring buf) {
  int i;
  append_c('I');
  if(FLAG2_SET_P(obj, IsLittleEndianFlag)) {
    append_c('l');
  } else {
    append_c('b');
  }
  
  i = NUM_FIELDS(obj) * REFSIZE;
  append_sz(i);
  append_str(bytearray_byte_address(state, obj), i);
}

static OBJECT unmarshal_iseq(STATE, struct marshal_state *ms) {
  int sz;
  char endian;
  OBJECT obj;
  endian = ms->buf[1];
    
  sz = read_int(ms->buf + 2);
  
  ms->consumed += 6;
  ms->consumed += sz;
  obj = iseq_new(state, sz / REFSIZE);
  
  memcpy(bytearray_byte_address(state, obj), ms->buf + 6, sz);
  
  #if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
    if(endian != 'b') iseq_flip(state, obj);
  #else
    if(endian != 'l') iseq_flip(state, obj);
  #endif
  
  return obj;
}

static void marshal_cmethod(STATE, OBJECT obj, bstring buf, struct marshal_state *ms) {
  marshal_fields_as(state, obj, buf, 'm', ms);
}

static OBJECT unmarshal_cmethod(STATE, struct marshal_state *ms) {
  int sz;
  OBJECT cm;
  sz = unmarshal_num_fields(ms);
  cm = cmethod_allocate(state);
  unmarshal_into_fields(state, sz, cm, ms);
  return cm;
}

static OBJECT unmarshal(STATE, struct marshal_state *ms) {
  uint8_t tag = *ms->buf;
  OBJECT o;

//  printf("%c\n", tag);
  switch(tag) {
    case 'i':
      o = unmarshal_int(state, ms);
      break;
    case 's':
      o = unmarshal_str(state, ms);
      _add_object(o, ms);
      break;
    case 'x':
      o = unmarshal_sym(state, ms);
      break;
    case 'p':
      o = unmarshal_tup(state, ms);
      _add_object(o, ms);
      break;
    case 'b':
      o = unmarshal_bytes(state, ms);
      _add_object(o, ms);
      break;
    case 'I':
      o = unmarshal_iseq(state, ms);
      _add_object(o, ms);
      break;
    case 'm':
      o = unmarshal_cmethod(state, ms);
      _add_object(o, ms);
      break;
    case 'B':
      o = unmarshal_bignum(state, ms);
      _add_object(o, ms);
      break;
    case 'd':
      o = unmarshal_floatpoint(state, ms);
      _add_object(o, ms);
      break;
    case 'r':
      o = _nth_object(state, ms);
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

static void marshal(STATE, OBJECT obj, bstring buf, struct marshal_state *ms) {
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
      } else if(kls == state->global->iseq) {
        marshal_iseq(state, obj, buf);
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
  bstring buf;
  OBJECT ret;

  buf = cpu_marshal_to_bstring(state, obj, version);
  ret = string_newfrombstr(state, buf);
  bdestroy(buf);
  return ret;
}

bstring cpu_marshal_to_bstring(STATE, OBJECT obj, int version) {
  bstring buf;
  struct marshal_state ms;
  
  ms.consumed = 0;
  ms.objects = ptr_array_new(8);
  
  buf = cstr2bstr("RBIX");
  _append_sz(buf, version);
  marshal(state, obj, buf, &ms);
  ptr_array_free(ms.objects);
  return buf;
}

OBJECT cpu_marshal_to_file(STATE, OBJECT obj, char *path, int version) {
  bstring buf;
  FILE *f;
  struct marshal_state ms;
  
  f = fopen(path, "wb");
  if(!f) {
    return Qfalse;
  }
  
  ms.consumed = 0;
  ms.objects = ptr_array_new(8);
  
  buf = cstr2bstr("");
  _append_sz(buf, version);
  marshal(state, obj, buf, &ms);
  
  /* TODO do error chceking here */
  fwrite("RBIX", 1, 4, f);
  fwrite(bdatae(buf,""), 1, blength(buf), f);
  fclose(f);

  bdestroy(buf);
  ptr_array_free(ms.objects);
  return Qtrue;
}

OBJECT cpu_unmarshal(STATE, uint8_t *str, int version) {
  struct marshal_state ms;
  OBJECT ret;
  int offset = 4;
  if(!memcmp(str, "RBIS", 4)) {
    version = -1;
  } else if(!memcmp(str, "RBIX", 4)) {
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
  ms.objects = ptr_array_new(8);
  ms.buf = str + offset;

  ret = unmarshal(state, &ms);
  ptr_array_free(ms.objects);
  return ret;
}

OBJECT cpu_unmarshal_file(STATE, const char *path, int version) {
  uint8_t *ptr, *data = NULL;
  FILE *f = NULL;
  size_t read;
  off_t left;
  uint8_t buf[4];
  OBJECT obj = Qnil;
  struct marshal_state ms;
  struct stat st;
  int s;

  s = stat(path, &st);
  if(s == -1) {
    return Qnil;
  }

  left = st.st_size;

  ms.consumed = 0;
  ms.objects = NULL;

  f = fopen(path, "rb");
  if(!f) {
    goto cleanup;
  }

  read = fread(buf, 1, 4, f);
  if(read != 4) {
    goto cleanup;
  }

  left -= 4;

  ms.objects = ptr_array_new(8);
  if(!memcmp(buf, "RBIS", 4)) {
    version = -1;
  } else if(!memcmp(buf, "RBIX", 4)) {
    read = fread(buf, 1, 4, f);
    if(read != 4) {
      goto cleanup;
    }

    left -= 4;

    if(read_int(buf) < version) {
      /* out of date. */
      goto cleanup;
    }
  } else {
    printf("Invalid compiled file.\n");
    goto cleanup;
  }

  /* read the payload of the file */
  data = ptr = malloc(st.st_size);
  if(!data) {
    return Qnil;
  }

  do {
    read = fread(ptr, 1, left, f);
    ptr += read;
    left -= read;
  } while (read > 0);

  ms.buf = data;
  obj = unmarshal(state, &ms);
cleanup:
  if(f) {
    fclose (f);
  }

  if(data) {
    free(data);
  }

  if(ms.objects) {
    ptr_array_free(ms.objects);
  }

  return obj;
}

