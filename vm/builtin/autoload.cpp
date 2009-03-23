#include "prelude.hpp"
#include "call_frame.hpp"
#include "message.hpp"
#include "builtin/class.hpp"
#include "builtin/autoload.hpp"

namespace rubinius {
  Autoload* Autoload::create(STATE) {
    return state->new_object<Autoload>(G(autoload));
  }

  void Autoload::init(STATE) {
    GO(autoload).set(state->new_class("Autoload"));
    G(autoload)->set_object_type(state, AutoloadType);
  }

  Object* Autoload::resolve(STATE, CallFrame* call_frame) {
    Message msg(state,
                G(sym_call),
                this,
                0,
                Qnil,
                this->lookup_begin(state));

    return msg.send(state, call_frame);
  }
}
