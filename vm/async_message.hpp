#ifndef RBX_ASYNC_MESSAGE_HPP
#define RBX_ASYNC_MESSAGE_HPP

#include "util/mailbox.hpp"

namespace rubinius {
  class ASyncMessage {
  public:
    enum Type {
      cSignal
    };

  public:
    ASyncMessage(Type type, int data)
      : type_(type)
      , data_(data)
    {}

    Type type() {
      return type_;
    }

    int data() {
      return data_;
    }

  private:
    Type type_;
    int data_;
  };

  typedef thread::Mailbox<ASyncMessage> ASyncMessageMailbox;
}

#endif
