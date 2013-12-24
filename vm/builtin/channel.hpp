#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class List;
  class IO;
  class IOBuffer;
  class Executable;
  class Thread;

  class Channel : public Object {
  public:

    /** Register class. */
    static void  init(STATE);

    const static object_type type = ChannelType;

  private:
    List* value_;  // slot

    utilities::thread::Condition condition_;
    utilities::thread::Mutex mutex_;
    int waiters_;
    int semaphore_count_;

  public:
    /* accessors */

    attr_accessor(value, List);

    /* interface */

    // Rubinius.primitive :channel_new
    static Channel* create(STATE);

    // Rubinius.primitive :channel_send
    Object* send(STATE, GCToken gct, Object* val, CallFrame* calling_environment);

    // Rubinius.primitive :channel_receive
    Object* receive(STATE, GCToken gct, CallFrame* calling_environment);

    // Rubinius.primitive :channel_try_receive
    Object* try_receive(STATE, GCToken gct, CallFrame* calling_environment);

    // Rubinius.primitive :channel_receive_timeout
    Object* receive_timeout(STATE, GCToken gct, Object* duration, CallFrame* calling_environment);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

}

#endif
