#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/ar.h"

static int ar_validate(int fd) {
  char data[8];
  long bytes;

  bzero(data, 8);

  bytes = read(fd, data, 8);

  if(bytes != 8 || strncmp(data, "!<arch>\n", 8)) {
    return FALSE;
  }

  return TRUE;
}

static char *ar_file_name(int fd, char *name, long *size) {
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
 * Retrieves each file from from ar(5) archive +path+ and processes it with
 * +callback+.
 */
int ar_each_file(machine m, const char *path, int(*callback)(machine, char *, uint8_t *, long)) {
  int err;
  long bytes, size;
  char *name, data_size[11], name_data[17];
  uint8_t *data;
  int fd;
  int ret = TRUE;

  bzero(data_size, 11);
  bzero(name_data, 17);

  fd = open(path, O_RDONLY);

  if(fd == -1) {
    return FALSE;
  }

  if(!ar_validate(fd)) {
    return FALSE;
  }

  for(;;) {
    bytes = read(fd, name_data, 16);
    if(bytes == 0) { /* end of file */
      break;
    } else if(bytes != 16) {
      ret = FALSE;
      break;
    }

    err = lseek(fd, 32, SEEK_CUR); /* mtime, uid, gid, mode */
    if(err < 0) {
      ret = FALSE;
      break;
    }

    bytes = read(fd, data_size, 10);
    if(bytes != 10) {
      ret = FALSE;
      break;
    }

    size = atoi(data_size);

    err = lseek(fd, 2, SEEK_CUR);
    if(err < 0) {
      ret = FALSE;
      break;
    }

    name = ar_file_name(fd, name_data, &size);

    data = (uint8_t *)calloc(size, sizeof(uint8_t));
    if(data == NULL) {
      ret = FALSE;
      break;
    }

    bytes = read(fd, data, size);
    if(bytes != size) {
      free(data);
      ret = FALSE;
      break;
    }

    ret = callback(m, name, data, bytes);

    free(data);

    if(!RTEST(ret)) {
      break;
    }

    if(size % 2 == 1) {
      lseek(fd, 1, SEEK_CUR);
      if(err < 0) {
        ret = FALSE;
        break;
      }
    }
  }

  close(fd);

  return ret;
}

