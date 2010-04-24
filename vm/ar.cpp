#include "ar.hpp"

/*
 * A simple ar(5) implementation. Used to load data from .rba's */

namespace rubinius {

  /* Create an AR object for +path+ as an ar format file. */
  Ar::Ar(const char* path) : path(path), fd(-1) { }

  /* Closes any open descriptors. */
  Ar::~Ar() {
    if(fd > -1) ::close(fd);
  }

  /* Open up the archive, returning false if we were unable to open
   * the path. */
  bool Ar::open() {
    fd = ::open(path, O_RDONLY);

    if(fd == -1) {
      return false;
    }

    if(!validate()) {
      ::close(fd);
      fd = -1;
      return false;
    }

    return true;
  }

  bool Ar::validate() {
    char data[8];
    long bytes;

    memset(data, 0, 8);

    bytes = read(fd, data, 8);

    if(bytes != 8 || strncmp(data, "!<arch>\n", 8)) {
      return false;
    }

    return true;
  }

  char* Ar::file_name(char* name, long* size) {
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
  bool Ar::process() {
    int err;
    long bytes, size;
    char *name, data_size[11], name_data[17];
    uint8_t *data;
    bool ret = true;

    if(fd < 0) return false;

    memset(data_size, 0, 11);
    memset(name_data, 0, 17);

    for(;;) {
      bytes = read(fd, name_data, 16);
      if(bytes == 0) { /* end of file */
        break;
      } else if(bytes != 16) {
        return false;
      }

      err = lseek(fd, 32, SEEK_CUR); /* mtime, uid, gid, mode */
      if(err < 0) return false;

      bytes = read(fd, data_size, 10);
      if(bytes != 10) return false;

      size = atoi(data_size);

      err = lseek(fd, 2, SEEK_CUR);
      if(err < 0) return false;

      name = file_name(name_data, &size);

      data = (uint8_t *)calloc(size, sizeof(uint8_t));
      if(data == NULL) return false;

      bytes = read(fd, data, size);
      if(bytes != size) {
        free(data);
        return false;
      }

      ret = each_file(name, data, bytes);

      free(data);

      if(!ret) break;

      if(size % 2 == 1) {
        lseek(fd, 1, SEEK_CUR);
        if(err < 0) return false;
      }
    }

    return true;
  }

}
