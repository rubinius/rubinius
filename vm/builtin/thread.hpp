#ifndef RBX_THREAD_HPP
#define RBX_THREAD_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Channel;

  class Thread : public Object {
  public:
    const static size_t fields = 5;
    const static object_type type = ThreadType;

    OBJECT ivars; // slot
    Task* task; // slot
    Channel* channel; // slot
    FIXNUM priority; // slot

    static void init(STATE);
    static Thread* create(STATE);
    void boot_task(STATE);
    void sleep_for(STATE, Channel* chan);
    void set_top(STATE, OBJECT val);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class DeadLock : public Assertion {
  public:
    DeadLock(const char* msg) : Assertion(msg) { }
  };
}

#endif
