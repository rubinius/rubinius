#ifndef RBS_AR_HPP
#define RBS_AR_HPP

#include <fcntl.h>
#include <cstring>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>
#include <cstdlib>

namespace rubinius {
  class Ar {
  public:
    const char* path;
    int fd;

    Ar(const char* path);
    virtual ~Ar();
    bool validate();
    bool open();
    char* file_name(char* name, long* size);
    bool process();
    virtual bool each_file(const char* path, uint8_t*, long) { return true; }
  };
}

#endif

