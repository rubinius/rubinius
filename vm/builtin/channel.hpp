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
    OBJECT value_;  // slot
    List* waiting_; // slot

  public:
    /* accessors */

    attr_accessor(value, Object);
    attr_accessor(waiting, List);

    /* interface */

    // Ruby.primitive :channel_new
    static Channel* create(STATE);

    // Ruby.primitive :channel_send
    OBJECT send(STATE, OBJECT);

    // Ruby.primitive? :channel_receive
    bool receive_prim(STATE, Executable* exec, Task* task, Message& msg);

    OBJECT receive(STATE);
    bool has_readers_p();

    // Ruby.primitive :scheduler_send_on_signal
    static OBJECT send_on_signal(STATE, Channel* chan, FIXNUM signal);

    // Ruby.primitive :scheduler_send_on_readable
    static OBJECT send_on_readable(STATE, Channel* chan, IO* io, IOBuffer* buffer, FIXNUM bytes);

    // Ruby.primitive :scheduler_send_in_microseconds
    static OBJECT send_in_microseconds(STATE, Channel* chan, Integer* useconds, OBJECT tag);

    // Ruby.primitive :scheduler_send_in_seconds
    static OBJECT send_in_seconds(STATE, Channel* chan, Float* seconds, OBJECT tag);
    static OBJECT send_in_seconds(STATE, Channel* chan, double seconds, OBJECT tag);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class ChannelCallback : public ObjectCallback {
  public:
    TypedRoot<Channel*> channel;

    ChannelCallback(STATE, Channel* chan);
    virtual void call(OBJECT obj);
  };

}

#endif
