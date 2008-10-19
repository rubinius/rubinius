#ifndef RBX_AR_HPP
#define RBX_AR_HPP

#include <fcntl.h>
#include <cstring>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>
#include <cstdlib>


namespace rubinius {

  /*
   * @todo  Name parameters to each_file(). --rue
   * @todo  Add comments for methods.
   */
  class Ar {
  public:   /* Ctors */

    Ar(const char* path);
    virtual ~Ar();


  public:   /* Interface */

    virtual bool  each_file(const char* /* path */, uint8_t*, long);
    char*         file_name(char* name, long* size);
    bool          open();
    bool          process();
    bool          validate();


  public:   /* Instance vars */

    const char* path;
    int fd;


  private:  /* Disallowed */

    Ar(const Ar& other);
    Ar& operator=(Ar& other);
  };


  inline bool Ar::each_file(const char* /* path */, uint8_t*, long) {
    return true;
  }
}

#endif

