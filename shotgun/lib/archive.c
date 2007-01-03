#include <zip.h>
#include "shotgun.h"
#include "string.h"
#include "cpu.h"
#include "array.h"

OBJECT archive_list_files(STATE, char *path) {
  struct zip *za;
  struct zip_stat st;
  int i, err, count;
  OBJECT ary;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  count = zip_get_num_files(za);
  ary = array_new(state, count);
  for(i=0; i < count; i++) {
    zip_stat_index(za, i, 0, &st);
    array_set(state, ary, i, string_new(state, (char *)st.name));
  }
  
  zip_close(za);
  
  return ary;
}

OBJECT archive_get_file(STATE, char *path, char* name) {
  struct zip *za;
  struct zip_stat st;
  struct zip_file *zf;
  OBJECT str;
  int n, total, err, file;
  char *buf;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  if((file = zip_name_locate(za, name, 0)) < 0) {
    printf("Unknown file %s\n", name);
    return Qnil;
  }
  
  if((zf=zip_fopen_index(za, file, 0)) == NULL) {
    return Qnil;
  }
  
  zip_stat_index(za, file, 0, &st);
  total = st.size;
  printf("Bringing in file %s (%d) of %d bytes.\n", name, file, total);
  str = string_new2(state, NULL, total);
  buf = string_byte_address(state, str);
  
  while((n=zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  zip_close(za);
  
  return str;
}

OBJECT archive_get_object(STATE, char *path, char* name) {
  struct zip *za;
  struct zip_stat st;
  struct zip_file *zf;
  char *str;
  OBJECT ret;
  int n, total, err, file;
  char *buf;
  
  if((za=zip_open(path, 0, &err)) == NULL) {
    return Qnil;
  }
  
  if((file = zip_name_locate(za, name, 0)) < 0) {
    printf("Unknown file %s\n", name);
    return Qnil;
  }
  
  if((zf=zip_fopen_index(za, file, 0)) == NULL) {
    return Qnil;
  }
  
  zip_stat_index(za, file, 0, &st);
  total = st.size;
  printf("Bringing in file %s (%d) of %d bytes.\n", name, file, total);
  str = malloc(sizeof(char) * total);
  buf = str;
  
  while((n=zip_fread(zf, buf, total)) > 0) {
    buf += n;
    total -= n;
  }
  
  zip_fclose(zf);
  zip_close(za);
  
  ret = cpu_unmarshal(state, str);
  free(str);
  return ret;
}


