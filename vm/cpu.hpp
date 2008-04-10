#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

namespace rubinius {
  class Message {
  public:
    Array* arguments;
    STATE;
    
    Message(STATE, Array* ary);
    OBJECT get_argument(size_t index);
    
  };
}

#endif
