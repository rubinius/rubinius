#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

namespace rubinius {
  class Channel : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = ChannelType;

    OBJECT value;
    List* waiting;

    static Channel* create(STATE);
    void send(STATE, OBJECT);
    void receive(STATE);
    bool has_readers_p();
  };

  class ChannelCallback : public ObjectCallback {
    TypedRoot<Channel*> channel;

    ChannelCallback(STATE, Channel* chan) : ObjectCallback(state) {
      channel.set(chan, &GO(roots));
    }

    virtual void call(OBJECT obj) {
      channel->send(state, obj);
    }
  };

}

#include "builtin_thread.hpp"

#endif
