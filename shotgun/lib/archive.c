#include <zip.h>
#include <string.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/archive.h"

archive_handle archive_open(STATE, const char *path) {
  int err = 0;

  return (archive_handle) zip_open(path, 0, &err);
}

OBJECT archive_close(STATE, archive_handle za) {
  zip_close((struct zip *) za);

  return Qnil;
}

OBJECT archive_list_files(STATE, char *path) {
  struct zip *za;
  struct zip_stat st;
  int i, err, count;
  OBJECT ary, tup;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  count = zip_get_num_files(za);
  ary = array_new(state, count);
  for(i=0; i < count; i++) {
    zip_stat_index(za, i, 0, &st);
    tup = tuple_new2(state, 3, string_new(state, (char *)st.name), ML2N(st.mtime), ML2N(st.size));
    array_set(state, ary, i, tup);
  }
  
  zip_close(za);
  
  return ary;
}

OBJECT archive_get_file(STATE, const char *path, const char* name) {
  struct zip *za;
  struct zip_stat st;
  struct zip_file *zf;
  OBJECT str;
  int err, file = -1;
  native_int n, total;
  char *buf;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  if((file = zip_name_locate(za, (const char*)name, 0)) < 0) {
    zip_close(za);    
    return Qnil;
  }
  
  if((zf=zip_fopen_index(za, file, 0)) == NULL) {
    zip_close(za);
    return Qnil;
  }
  
  zip_stat_index(za, file, 0, &st);
  total = st.size;
  str = string_new2(state, NULL, total);
  buf = string_byte_address(state, str);
  
  while((n = zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  zip_close(za);
  
  return str;
}

OBJECT archive_get_file2(STATE, archive_handle za, const char *name) {
  struct zip_stat st;
  struct zip_file *zf;
  OBJECT str;
  int file = -1;
  native_int n, total;
  char *buf;

  if((file = zip_name_locate((struct zip *)za, name, 0)) < 0) {
    return Qnil;
  }
  
  if((zf = zip_fopen_index((struct zip *)za, file, 0)) == NULL) {
    return Qnil;
  }
  
  zip_stat_index((struct zip *)za, file, 0, &st);
  total = st.size;
  str = string_new2(state, NULL, total);
  buf = string_byte_address(state, str);
  
  while((n = zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  
  return str;
}

OBJECT archive_get_object(STATE, const char *path, char* name, int version) {
  struct zip *za;
  struct zip_stat st;
  struct zip_file *zf;
  uint8_t *str, *buf;
  OBJECT ret;
  int err, file = -1;
  native_int n, total;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  if((file = zip_name_locate(za, name, 0)) < 0) {
    zip_close(za);
    return Qnil;
  }
  
  if((zf = zip_fopen_index(za, file, 0)) == NULL) {
    zip_close(za);
    return Qnil;
  }
  
  zip_stat_index(za, file, 0, &st);
  total = st.size;
  str = ALLOC_N(uint8_t, total);
  buf = str;
  
  while((n = zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  zip_close(za);
  
  ret = cpu_unmarshal(state, str, (int)st.size, version);
  XFREE(str);
  return ret;
}

OBJECT archive_get_object2(STATE, archive_handle za,
                           const char *name, int version) {
  struct zip_stat st;
  struct zip_file *zf;
  uint8_t *str, *buf;
  OBJECT ret;
  int file = -1;
  native_int n, total;

  if((file = zip_name_locate((struct zip *)za, name, 0)) < 0) {
    return Qnil;
  }
  
  if((zf = zip_fopen_index((struct zip *)za, file, 0)) == NULL) {
    return Qnil;
  }
  
  zip_stat_index((struct zip *)za, file, 0, &st);
  total = st.size;
  str = ALLOC_N(uint8_t, total);
  buf = str;
  
  while((n = zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  
  ret = cpu_unmarshal(state, str, (int)st.size, version);
  XFREE(str);

  return ret;
}

static OBJECT add_or_replace(struct zip *za, char *name, struct zip_source *zs) {
  OBJECT ret;
  int idx = zip_name_locate(za, name, 0);
  
  if(idx < 0) {
    if(zip_add(za, name, zs) < 0) {
      ret = Qfalse;
    } else {
      ret = Qtrue;
    }
  } else {
    if(zip_replace(za, idx, zs) < 0) {
      ret = Qfalse;
    } else {
      ret = Qtrue;
    }
  }
  return ret;
}

OBJECT archive_add_file(STATE, char *path, char *name, char *file) {
  struct zip *za;
  struct zip_source *zs;
  OBJECT ret;
  int err;
  
  if((za = zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  if((zs = zip_source_file(za, file, 0, 0)) == NULL) {
    zip_close(za);
    return Qfalse;
  }
  
  ret = add_or_replace(za, name, zs);
  
  zip_close(za);
  return ret;
}

OBJECT archive_add_object(STATE, char *path, char *name, OBJECT obj, int version) {
  struct zip *za;
  struct zip_source *zs;
  bstring buf;
  OBJECT ret;
  int err;
  
  if((za = zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  buf = cpu_marshal_to_bstring(state, obj, version);
  
  zs = zip_source_buffer(za, bdata(buf), blength(buf), 0);

  ret = add_or_replace(za, name, zs);
  
  zip_close(za);
  bdestroy(buf);
  return ret;
}

OBJECT archive_delete_file(STATE, char *path, int idx) {
  struct zip *za;
  OBJECT ret;
  int err;
  
  if((za = zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
    
  if(zip_delete(za, idx) < 0) {
    ret = Qfalse;
  } else {
    ret = Qtrue;
  }
  
  zip_close(za);
  return ret;
}

