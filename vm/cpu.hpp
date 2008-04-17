#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

namespace rubinius {
  class SendSite;

  class Message {
  public:
    STATE;
    Array* arguments;

    SendSite* send_site;
    SYMBOL name;
    OBJECT recv;
    OBJECT block;
    OBJECT splat;
    size_t args;
    size_t stack;
    bool   priv;

    Module* lookup_from;
    Executable* method;
    Module* module;

    Message(STATE, Array* ary);
    Message(STATE);
    OBJECT get_argument(size_t index);

    void combine_with_splat(OBJECT splat, int flags);

  };
}

#endif
