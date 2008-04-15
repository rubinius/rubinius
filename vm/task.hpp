#ifndef RBX_TASK_HPP
#define RBX_TASK_HPP

namespace rubinius {
  class Task : public BuiltinType {
  public:
    const static size_t fields = 0;
    const static object_type type = TaskType;

    static void init(STATE);
  };

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
