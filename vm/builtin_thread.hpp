#ifndef RBX_THREAD_HPP
#define RBX_THREAD_HPP

namespace rubinius {
  class Thread : public BuiltinType {
  public:
    const static size_t fields = 5;
    const static object_type type = ThreadType;

    Task* task;
    Channel* channel;
    FIXNUM priority;

    static void init(STATE);
    static Thread* create(STATE);
    void sleep_for(STATE, Channel* chan);
  };

  class DeadLock : public Assertion {
  public:
    DeadLock(char* msg) : Assertion(msg) { }
  };
}

#endif
