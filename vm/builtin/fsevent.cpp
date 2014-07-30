#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/fsevent.hpp"
#include "builtin/string.hpp"

#include "ontology.hpp"

namespace rubinius {
  void FSEvent::init(STATE) {
    GO(fsevent).set(ontology::new_class_under(state, "FSEvent", G(rubinius)));
    G(fsevent)->set_object_type(state, FSEventType);
  }

  FSEvent* FSEvent::allocate(STATE, Object* self) {
    FSEvent* fsevent = create(state);
    fsevent->klass(state, as<Class>(self));
    return fsevent;
  }

#ifdef HAVE_KQUEUE

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = state->new_object<FSEvent>(G(fsevent));
    fsevent->kq_ = kqueue();
    return fsevent;
  }

  Object* FSEvent::watch_file(STATE, Fixnum* fd, String* path) {
    this->fileno(state, fd);
    this->path(state, path);

    EV_SET(&filter_, fd->to_native(), EVFILT_VNODE,
        EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_WRITE, 0, NULL);

    return cNil;
  }

  Object* FSEvent::watch_file(STATE, int fd, const char* path) {
    return watch_file(state, Fixnum::from(fd), String::create(state, path));
  }

  Object* FSEvent::wait_for_event(STATE) {
    struct kevent event;
    int status = 0;

    {
      GCIndependent guard(state, 0);

      status = kevent(kq_, &filter_, 1, &event, 1, NULL);
    }

    if(status < 0 || !(event.fflags & NOTE_WRITE)) {
      return cNil;
    }

    return cTrue;
  }

#elif HAVE_INOTIFY

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = state->new_object<FSEvent>(G(fsevent));
    // TODO: set inotify state
    return fsevent;
  }

  Object* FSEvent::watch_file(STATE, Fixnum* fd, String* path) {
    return cNil;
  }

  Object* FSEvent::watch_file(STATE, int fd, const char* path) {
    return watch_file(state, Fixnum::from(fd), String::create(state, path));
  }

  Object* FSEvent::wait_for_event(STATE) {
    return cNil;
  }

#endif
}
