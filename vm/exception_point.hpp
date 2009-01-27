#ifndef RBX_EXCEPTION_POINT

#include <setjmp.h>

namespace rubinius {
  class ExceptionPoint {
    bool jumped_to_;
    ExceptionPoint* previous_;

  public:
    jmp_buf __jump_buffer;

  public:
    ExceptionPoint(Task* task)
      : jumped_to_(false)
      , previous_(task->current_ep)
    {
      task->current_ep = this;
    }

    bool jumped_to() {
      return jumped_to_;
    }

    void reset() {
      jumped_to_ = false;
    }

    void return_to(Task* task) {
      jumped_to_ = true;
      task->current_ep = this;
      _longjmp(__jump_buffer, 1);
      abort(); // HUH!
    }

    void unwind_to_previous(Task* task) {
      previous_->return_to(task);
    }

    void pop(Task* task) {
      task->current_ep = previous_;
    }

  };
}
#define PLACE_EXCEPTIONPOINT(ep) _setjmp(ep.__jump_buffer)

#endif
