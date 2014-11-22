#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/fsevent.hpp"
#include "builtin/string.hpp"

#include "ontology.hpp"

#include "util/logger.hpp"

namespace rubinius {
  using namespace utilities;

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

  void FSEvent::finalize(STATE, FSEvent* fsevent) {
    if(fsevent->kq_ > 0) ::close(fsevent->kq_);
  }

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = state->new_object_pinned<FSEvent>(G(fsevent));
    if((fsevent->kq_ = kqueue()) < 0) {
      logger::error("%s: unable to create kqueue", strerror(errno));
    } else {
      state->memory()->needs_finalization(fsevent, (FinalizerFunction)&FSEvent::finalize);
    }

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

  void FSEvent::finalize(STATE, FSEvent* fsevent) {
    if(fsevent->in_ > 0) ::close(fsevent->in_);
  }

  FSEvent* FSEvent::create(STATE) {
    FSEvent* fsevent = state->new_object_pinned<FSEvent>(G(fsevent));
    fsevent->watch_set_ = false;
    if((fsevent->in_ = inotify_init()) < 0) {
      logger::error("%s: unable to create inotify", strerror(errno));
    } else {
      state->memory()->needs_finalization(fsevent, (FinalizerFunction)&FSEvent::finalize);
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

    fsevent_add_watch(in_, path->c_str(state));
    watch_set_ = true;

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

    if(!watch_set_) {
      fsevent_add_watch(in_, path_->c_str(state));
    } else {
      watch_set_ = false;
    }

    status = read(in_, &fsevent, RBX_FSEVENT_BUF_LEN);

    if(status <= 0 || !(fsevent.u.event.mask & IN_MODIFY)) {
      return cNil;
    }

    return cTrue;
  }

#endif
}
