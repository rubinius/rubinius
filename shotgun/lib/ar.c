#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/array.h"

static OBJECT shotgun_ar_validate(int fd) {
  char data[8];
  long bytes;
  OBJECT valid = Qtrue;

  bzero(data, 8);

  bytes = read(fd, data, 8);

  if(bytes != 8 || strncmp(data, "!<arch>\n", 8)) {
    valid = Qfalse;
  }

  return valid;
}

static char *shotgun_ar_file_name(STATE, int fd, char *name, long *size) {
  long bytes, length;
  char *ptr;

  if(strncmp(name, "#1/", 3) != 0) {
    ptr = strchr(name, ' ');
    *ptr = (char)0;
    return strdup(name);
  } else {
    length = atoi(name + 3);

    ptr = (char *)calloc(length, sizeof(char));
    if(ptr == NULL) return NULL;

    bytes = read(fd, ptr, length);

    if(bytes != length) {
      free(ptr);
      return NULL;
    }

    *size -= length;

    return ptr;
  }
}

/*
 * List files in the ar(5) archive +path+.
 */
OBJECT shotgun_ar_list_files(STATE, char *path) {
  int err;
  long bytes, size;
  char *file_name, data_size[11], name_data[17];
  int fd;
  OBJECT files;

  bzero(data_size, 11);
  bzero(name_data, 17);

  fd = open(path, O_RDONLY);

  if(fd == -1) {
    return Qnil;
  }

  if(!RTEST(shotgun_ar_validate(fd))) {
    return Qfalse;
  }

  files = array_new(state, 0);

  for(;;) {
    bytes = read(fd, name_data, 16);
    if(bytes != 16) break;

    err = lseek(fd, 32, SEEK_CUR); /* mtime, uid, gid, mode */
    if(err < 0) break;

    bytes = read(fd, data_size, 10);
    if(bytes != 10) break;

    size = atoi(data_size);

    err = lseek(fd, 2, SEEK_CUR);
    if(err < 0) break;

    file_name = shotgun_ar_file_name(state, fd, name_data, &size);
    array_append(state, files, string_new(state, file_name));

    free(file_name);

    if(size % 2 == 1) size++;

    lseek(fd, size, SEEK_CUR);
    if(err < 0) break;
  }

  close(fd);

  return files;
}

/*
 * Retrieve +file+ from ar(5) archive +path+.
 */
OBJECT shotgun_ar_get_file(STATE, char *path, char *file_name) {
  int err;
  long bytes, size;
  char *data, *name, data_size[11], name_data[17];
  int fd;
  OBJECT file_data = Qnil;

  bzero(data_size, 11);
  bzero(name_data, 17);

  fd = open(path, O_RDONLY);

  if(fd == -1) {
    return Qnil;
  }

  if(!RTEST(shotgun_ar_validate(fd))) {
    return Qfalse;
  }

  for(;;) {
    bytes = read(fd, name_data, 16);
    if(bytes != 16) break;

    err = lseek(fd, 32, SEEK_CUR); /* mtime, uid, gid, mode */
    if(err < 0) break;

    bytes = read(fd, data_size, 10);
    if(bytes != 10) break;

    size = atoi(data_size);

    err = lseek(fd, 2, SEEK_CUR);
    if(err < 0) break;

    name = shotgun_ar_file_name(state, fd, name_data, &size);

    if(!strcmp(name, file_name)) {
      data = (char *)calloc(size, sizeof(char));
      if(data == NULL) break;

      bytes = read(fd, data, size);
      if(bytes != size) {
        free(data);
        break;
      }

      file_data = string_new2(state, data, size);
    } else {
      lseek(fd, size, SEEK_CUR);
      if(err < 0) break;
    }

    if(size % 2 == 1) {
      lseek(fd, 1, SEEK_CUR);
      if(err < 0) break;
    }
  }

  close(fd);

  return file_data;
}

