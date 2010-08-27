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

  public:
    /* accessors */

    attr_accessor(value, List);

    /* interface */

    // Ruby.primitive :channel_new
    static Channel* create(STATE);

    /** Remove Thread from waiting list, if it is there. */
    void    cancel_waiter(STATE, const Thread* waiter);

    // Ruby.primitive :channel_send
    Object* send(STATE, Object*);

    // Ruby.primitive :channel_receive
    Object* receive(STATE, CallFrame* calling_environment);

    // Ruby.primitive :channel_try_receive
    Object* try_receive(STATE);

    // Ruby.primitive :channel_receive_timeout
    Object* receive_timeout(STATE, Object* duration, CallFrame* calling_environment);

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
