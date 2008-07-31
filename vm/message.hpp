#ifndef RBX_MESSAGE_HPP
#define RBX_MESSAGE_HPP

namespace rubinius {
  class Array;
  class Executable;
  class SendSite;
  class Task;

  class Message {
  public:
    STATE;
    Array* arguments;
    Task*  task;
    size_t argument_start;

    SendSite* send_site;
    SYMBOL name;
    OBJECT recv;
    OBJECT block;
    OBJECT splat;
    OBJECT current_self;
    size_t args;
    size_t stack;
    bool   priv;

    Module* lookup_from;
    Executable* method;
    Module* module;

    Message(STATE, Array* ary);
    Message(STATE);
    OBJECT get_argument(size_t index);

    void import_arguments(STATE, Task* task, size_t args);
    void combine_with_splat(STATE, Task* task, Array* splat);

    void use_from_task(Task* task, size_t args) {
      this->task = task;
      this->args = args;
    }
  };
}

#endif
