#include "builtin/exception.hpp"
#include "builtin/task.hpp"
#include "builtin/contexts.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"

namespace rubinius {
  Exception* Exception::create(STATE) {
    return (Exception*)state->new_object(G(exception));
  }

  Exception* Exception::arg_error(STATE, int expected, int given) {
    Exception* exc = (Exception*)state->new_object(G(exc_arg));
    MethodContext* ctx = G(current_task)->active();
    ctx->reference(state);
    exc->context(state, ctx);
    exc->message(state, (String*)Qnil);
    exc->set_ivar(state, state->symbol("@expected"), Fixnum::from(expected));
    exc->set_ivar(state, state->symbol("@given"), Fixnum::from(given));

    return exc;
  }
}
