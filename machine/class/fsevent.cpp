#include "memory.hpp"
#include "thread_phase.hpp"

#include "class/class.hpp"
#include "class/fixnum.hpp"
#include "class/fsevent.hpp"
#include "class/string.hpp"

#include "memory/collector.hpp"

#include "logger.hpp"

#include <unistd.h>

namespace rubinius {
  using namespace utilities;

  void FSEvent::bootstrap(STATE) {
    GO(fsevent).set(state->memory()->new_class<Class, FSEvent>(
          state, G(rubinius), "FSEvent"));
  }

  FSEvent* FSEvent::allocate(STATE, Object* self) {
    return state->memory()->new_object_pinned<FSEvent>(state, as<Class>(self));
  }

#ifdef HAVE_KQUEUE

  void FSEvent::finalize(STATE, FSEvent* fsevent) {
    if(fsevent->kq() > 0) ::close(fsevent->kq());
  }

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = FSEvent::allocate(state, G(fsevent));

    fsevent->kq(kqueue());

    if(fsevent->kq() < 0) {
      logger::error("%s: unable to create kqueue", strerror(errno));
    } else {
      state->collector()->native_finalizer(state, fsevent,
          (memory::FinalizerFunction)&FSEvent::finalize);
    }

    return fsevent;
  }

  Object* FSEvent::watch_file(STATE, Fixnum* fd, String* path) {
    this->fileno(state, fd);
    this->path(state, path);

    EV_SET(&_filter_, fd->to_native(), EVFILT_VNODE,
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
      UnmanagedPhase unmanaged(state);

      status = kevent(kq(), &_filter_, 1, &event, 1, NULL);
    }

    if(status < 0) return cNil;

    return cTrue;
  }

#elif HAVE_INOTIFY

  void FSEvent::finalize(STATE, FSEvent* fsevent) {
    if(fsevent->in() > 0) ::close(fsevent->in());
  }

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = state->memory()->new_object_pinned<FSEvent>(state, G(fsevent));
    fsevent->watch_set(false);
    fsevent->in(inotify_init());
    if(fsevent->in() < 0) {
      logger::error("%s: unable to create inotify", strerror(errno));
    } else {
      state->collector()->native_finalizer(state, fsevent,
          (memory::FinalizerFunction)&FSEvent::finalize);
    }

    return fsevent;
  }

#define RBX_FSEVENT_MASK    (IN_MODIFY \
                             | IN_CLOSE_WRITE \
                             | IN_CLOSE_NOWRITE \
                             | IN_DELETE \
                             | IN_ONESHOT)

  static int fsevent_add_watch(int fd, const char* path) {
    return inotify_add_watch(fd, path, RBX_FSEVENT_MASK);
  }

  Object* FSEvent::watch_file(STATE, Fixnum* fd, String* path) {
    this->fileno(state, fd);
    this->path(state, path);

    fsevent_add_watch(in(), path->c_str(state));
    watch_set(true);

    return cNil;
  }

  Object* FSEvent::watch_file(STATE, int fd, const char* path) {
    return watch_file(state, Fixnum::from(fd), String::create(state, path));
  }

#ifdef NAME_MAX
#define RBX_FSEVENT_NAME_MAX  NAME_MAX
#else
#define RBX_FSEVENT_NAME_MAX  65535
#endif

#define RBX_FSEVENT_BUF_LEN   (sizeof(struct inotify_event) + RBX_FSEVENT_NAME_MAX)

  struct fsevent_inotify_event {
    union {
      unsigned char buf[RBX_FSEVENT_BUF_LEN];
      struct inotify_event event;
    } u;
  };

  Object* FSEvent::wait_for_event(STATE) {
    struct fsevent_inotify_event fsevent;
    int status;

    if(!watch_set()) {
      fsevent_add_watch(in(), path()->c_str(state));
    } else {
      watch_set(false);
    }

    {
      UnmanagedPhase unmanaged(state);

      status = read(in(), &fsevent, RBX_FSEVENT_BUF_LEN);
    }

    if(status < 0) return cNil;

    return cTrue;
  }

#endif
}
