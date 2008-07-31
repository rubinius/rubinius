#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

namespace rubinius {
  class List;

  class Channel : public Object {
  public:
    const static size_t fields = 3;
    const static object_type type = ChannelType;

    OBJECT __ivars__; // slot
    OBJECT value; // slot
    List* waiting; // slot

    static Channel* create(STATE);

    // Ruby.primitive :channel_send
    OBJECT send(STATE, OBJECT);

    // Ruby.primitive :channel_receive
    OBJECT receive(STATE);
    bool has_readers_p();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class ChannelCallback : public ObjectCallback {
  public:
    TypedRoot<Channel*> channel;

    ChannelCallback(STATE, Channel* chan) : ObjectCallback(state) {
      channel.set(chan, &GO(roots));
    }

    virtual void call(OBJECT obj) {
      channel->send(state, obj);
    }
  };

}

#include "builtin/thread.hpp"

#endif
