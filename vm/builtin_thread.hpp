#ifndef RBX_THREAD_HPP
#define RBX_THREAD_HPP

namespace rubinius {
  class Thread : public BuiltinType {
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
