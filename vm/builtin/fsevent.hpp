#ifndef RBX_FSEVENT_HPP
#define RBX_FSEVENT_HPP

#include "builtin/object.hpp"

#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif HAVE_INOTIFY
#else
#error "No file system event utility is available. Please submit an issue."
#endif

namespace rubinius {
  class Fixnum;
  class String;

  class FSEvent : public Object {
  public:
    const static object_type type = FSEventType;

  private:
    Fixnum* fileno_;    // slot
    String* path_;      // slot

  public:
    /* accessors */

    attr_accessor(fileno, Fixnum);
    attr_accessor(path, String);

#ifdef HAVE_KQUEUE
  private:
    int kq_;
    struct kevent filter_;
    struct kevent event_;

#elif HAVE_INOTIFY
#endif

  public:
    static void init(STATE);

    static FSEvent* create(STATE);

    // Rubinius.primitive+ :fsevent_allocate
    static FSEvent* allocate(STATE, Object* self);

    // Rubinius.primitive :fsevent_watch_file
    Object* watch_file(STATE, Fixnum* fd, String* path);
    Object* watch_file(STATE, int fd, const char* path);

    // Rubinius.primitive :fsevent_wait_for_event
    Object* wait_for_event();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
