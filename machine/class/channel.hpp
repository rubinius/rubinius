#ifndef RBX_CHANNEL_HPP
#define RBX_CHANNEL_HPP

#include "object_utils.hpp"

#include "class/list.hpp"
#include "class/object.hpp"
#include "class/string.hpp"

#include <mutex>

namespace rubinius {
  class IO;
  class IOBuffer;
  class Executable;
  class Thread;

  class Channel : public Object {
  public:
    const static object_type type = ChannelType;

    static void  bootstrap(STATE);

    attr_accessor(value, List);

  private:
    std::condition_variable _condition_;
    std::mutex _mutex_;

    attr_field(waiters, int);
    attr_field(semaphore_count, int);

  public:
    static void initialize(STATE, Channel* obj) {
      obj->value(nil<List>());
      new(&obj->_condition_) std::condition_variable;
      new(&obj->_mutex_) std::mutex;
      obj->waiters(0);
      obj->semaphore_count(0);
    }

    void inc_semaphore_count() {
      _semaphore_count_++;
    }

    void dec_semaphore_count() {
      _semaphore_count_--;
    }

    void inc_waiters() {
      _waiters_++;
    }

    void dec_waiters() {
      _waiters_--;
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
