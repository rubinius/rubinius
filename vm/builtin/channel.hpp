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

    // Ruby.primitive :scheduler_send_on_signal
    static Object* send_on_signal(STATE, Channel* chan, Fixnum* signal);

    // Ruby.primitive :scheduler_send_on_readable
    static Object* send_on_readable(STATE, Channel* chan, IO* io, Object* maybe_buffer, Fixnum* bytes);

    // Ruby.primitive :scheduler_send_on_writable
    static Object* send_on_writable(STATE, Channel* chan, IO* io);

    // Ruby.primitive :scheduler_send_in_microseconds
    static Object* send_in_microseconds(STATE, Channel* chan, Integer* useconds, Object* tag);

    // Ruby.primitive :scheduler_send_in_seconds
    static Object* send_in_seconds(STATE, Channel* chan, Float* seconds, Object* tag);
    static Object* send_in_seconds(STATE, Channel* chan, double seconds, Object* tag);

    /**
     *  Event child process ending.
     */
    // Ruby.primitive :scheduler_send_on_stopped
    static Object* send_on_stopped(STATE, Channel* channel, Fixnum* pid, Fixnum* flags);


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
