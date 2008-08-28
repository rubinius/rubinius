#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "virtual.hpp" // ObjectCallback
#include "gc_root.hpp" // TypedRoot

namespace rubinius {
  class List;
  class IO;
  class IOBuffer;

  class Channel : public Object {
  public:
    const static size_t fields = 2;
    const static object_type type = ChannelType;

    OBJECT value; // slot
    List* waiting; // slot

    // Ruby.primitive :channel_new
    static Channel* create(STATE);

    // Ruby.primitive :channel_send
    OBJECT send(STATE, OBJECT);

    // Ruby.primitive :channel_receive
    OBJECT receive(STATE);
    bool has_readers_p();

    // Ruby.primitive :channel_send_on_signal
    static OBJECT send_on_signal(STATE, Channel* chan, FIXNUM signal);

    // Ruby.primitive :channel_send_on_readable
    static OBJECT send_on_readable(STATE, Channel* chan, IO* io, IOBuffer* buffer, FIXNUM bytes);

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
