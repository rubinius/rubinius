#ifndef RBX_FSEVENT_HPP
#define RBX_FSEVENT_HPP

#include "class/object.hpp"

#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif HAVE_INOTIFY
#include <unistd.h>
#include <sys/inotify.h>
#else
#error "No file system event utility is available. Please submit an issue."
#endif

namespace rubinius {
  class Fixnum;
  class String;

  class FSEvent : public Object {
  public:
    const static object_type type = FSEventType;

    attr_accessor(fileno, Fixnum);
    attr_accessor(path, String);

#ifdef HAVE_KQUEUE
  private:
    attr_field(kq, int);
    attr_field(filter, struct kevent);

#elif HAVE_INOTIFY
  private:
    attr_field(in, int);
    attr_field(watch_set, bool);
#endif

  public:
    static void bootstrap(STATE);

    static void finalize(STATE, FSEvent* fsevent);
    static FSEvent* create(STATE);

    // Rubinius.primitive :fsevent_allocate
    static FSEvent* allocate(STATE, Object* self);

    // Rubinius.primitive :fsevent_watch_file
    Object* watch_file(STATE, Fixnum* fd, String* path);
    Object* watch_file(STATE, int fd, const char* path);

    // Rubinius.primitive :fsevent_wait_for_event
    Object* wait_for_event(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
