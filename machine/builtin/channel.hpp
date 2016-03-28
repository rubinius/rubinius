#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "object_utils.hpp"

#include "builtin/list.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  class IO;
  class IOBuffer;
  class Executable;
  class Thread;

  class Channel : public Object {
  public:

    /** Register class. */
    static void  bootstrap(STATE);

    const static object_type type = ChannelType;

    attr_accessor(value, List);

  private:
    utilities::thread::Condition condition_;
    utilities::thread::Mutex mutex_;
    int waiters_;
    int semaphore_count_;

  public:
    /* interface */
    static void initialize(STATE, Channel* obj) {
      obj->value_ = nil<List>();
      obj->condition_.init();
      obj->mutex_.init();
      obj->waiters_ = 0;
      obj->semaphore_count_ = 0;
    }

    // Rubinius.primitive :channel_new
    static Channel* create(STATE);

    // Rubinius.primitive :channel_send
    Object* send(STATE, Object* val);

    // Rubinius.primitive :channel_receive
    Object* receive(STATE);

    // Rubinius.primitive :channel_try_receive
    Object* try_receive(STATE);

    // Rubinius.primitive :channel_receive_timeout
    Object* receive_timeout(STATE, Object* duration);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };
}

#endif
