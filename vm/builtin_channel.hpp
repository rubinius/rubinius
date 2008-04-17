#ifndef RBX_TASK_HPP
#define RBX_TASK_HPP

namespace rubinius {
  class Channel : public BuiltinType {
  public:
    const static size_t fields = 3;
    const static object_type type = ChannelType;

    void send(OBJECT);
  };

  class ChannelCallback : public ObjectCallback {
    Channel *channel;

    ChannelCallback(STATE, Channel* chan) : 
      ObjectCallback(state), channel(chan) { }

    virtual void call(OBJECT obj) {
      channel->send(obj);
    }

    virtual OBJECT object() { return channel; }
  };

  class Thread : public BuiltinType {
  public:
    const static size_t fields = 5;
    const static object_type type = ThreadType;
  };
}

#endif
