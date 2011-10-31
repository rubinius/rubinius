#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "virtual.hpp" // ObjectCallback
#include "gc/root.hpp" // TypedRoot

#include "builtin/thread.hpp"

namespace rubinius {
  class Float;
  class List;
  class IO;
  class IOBuffer;
  class Message;
  class Executable;

  class Channel : public Object {
  public:

    /** Register class. */
    static void  init(STATE);

    const static object_type type = ChannelType;

  private:
    List* value_;  // slot

    thread::Condition condition_;
    thread::Mutex mutex_;
    int waiters_;
    int semaphore_count_;

  public:
    /* accessors */

    attr_accessor(value, List);

    /* interface */

    // Rubinius.primitive :channel_new
    static Channel* create(STATE);
    static Channel* create_primed(STATE);

    /** Remove Thread from waiting list, if it is there. */
    void    cancel_waiter(STATE, const Thread* waiter);

    // Rubinius.primitive :channel_send
    Object* send(STATE, GCToken gct, Object*);

    // Rubinius.primitive :channel_receive
    Object* receive(STATE, GCToken gct, CallFrame* calling_environment);

    // Rubinius.primitive :channel_try_receive
    Object* try_receive(STATE, GCToken gct);

    // Rubinius.primitive :channel_receive_timeout
    Object* receive_timeout(STATE, GCToken gct, Object* duration, CallFrame* calling_environment);

    bool has_readers_p();


    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

  class ChannelCallback : public ObjectCallback {
  public:
    TypedRoot<Channel*> channel;

    ChannelCallback(STATE, Channel* chan);
    virtual void call(Object* obj);
  };

}

#endif
