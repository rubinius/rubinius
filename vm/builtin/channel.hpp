#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "virtual.hpp" // ObjectCallback
#include "gc_root.hpp" // TypedRoot

namespace rubinius {
  class Float;
  class List;
  class IO;
  class IOBuffer;
  class Task;
  class Message;
  class Executable;

  class Channel : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = ChannelType;

  private:
    Object* value_;  // slot
    List* waiting_; // slot

  public:
    /* accessors */

    attr_accessor(value, Object);
    attr_accessor(waiting, List);

    /* interface */

    // Ruby.primitive :channel_new
    static Channel* create(STATE);

    /** Remove Thread from waiting list, if it is there. */
    void    cancel_waiter(STATE, const Thread* waiter);

    // Ruby.primitive :channel_send
    Object* send(STATE, Object*);

    // Ruby.primitive? :channel_receive
    ExecuteStatus receive_prim(STATE, Executable* exec, Task* task, Message& msg);

    Object* receive(STATE);
    bool has_readers_p();

    // Ruby.primitive :scheduler_send_on_signal
    static Object* send_on_signal(STATE, Channel* chan, Fixnum* signal);

    // Ruby.primitive :scheduler_send_on_readable
    static Object* send_on_readable(STATE, Channel* chan, IO* io, Object* maybe_buffer, Fixnum* bytes);

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
