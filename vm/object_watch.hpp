#ifndef RBX_OBJECT_WATCH
#define RBX_OBJECT_WATCH

// #define ENABLE_OBJECT_WATCH

namespace rubinius {
  class Object;

  extern Object* object_watch;

#ifdef ENABLE_OBJECT_WATCH
  static inline bool watched_p(Object* obj) {
    return obj == object_watch;
  }
#else
  static inline bool watched_p(Object* obj) {
    return false;
  }
#endif
}

#endif

