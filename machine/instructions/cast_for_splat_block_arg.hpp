#include "instructions.hpp"

#include "on_stack.hpp"

namespace rubinius {
  namespace instructions {
    inline bool cast_for_splat_block_arg(STATE, CallFrame* call_frame) {
      if(!call_frame->arguments) {
        Exception::internal_error(state, "no arguments object");
        return false;
      }

      if(call_frame->arguments->total() == 1) {
        Object* obj = call_frame->arguments->get_argument(0);
        if(!rubinius::kind_of<Array>(obj)) {
          /* Yes, you are reading this code correctly: In Ruby 1.8, calling a
           * block with these forms { |*| } and { |*a| } with a single argument
           * that is not an Array and which responds to #to_ary will cause #to_ary
           * to be called and its return value ignored. Ultimately, the original
           * object itself is wrapped in an Array and passed to the block.
           */
          if(CBOOL(obj->respond_to(state, G(sym_to_ary), cFalse))) {
            OnStack<1> os(state, obj);
            Object* ignored = obj->send(state, G(sym_to_ary));
            if(!ignored) return false;
            if(!ignored->nil_p() && !rubinius::kind_of<Array>(ignored)) {
              Exception::type_error(state, "to_ary must return an Array");
              return false;
            }
          }
        }
        Array* ary = Array::create(state, 1);
        ary->set(state, 0, obj);
        stack_push(ary);
      } else {
        Array* ary = Array::create(state, call_frame->arguments->total());
        for(size_t i = 0; i < call_frame->arguments->total(); i++) {
          ary->set(state, i, call_frame->arguments->get_argument(i));
        }
        stack_push(ary);
      }

      return true;
    }
  }
}
