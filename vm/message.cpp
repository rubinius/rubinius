#include "vm.hpp"
#include "objectmemory.hpp"
#include "message.hpp"

#include "builtin/array.hpp"
#include "builtin/executable.hpp"
#include "builtin/tuple.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"

#include <sstream>

namespace rubinius {

  Message::Message(SendSite* ss, Symbol* name, Object* recv, CallFrame* call_frame,
                   size_t arg_count, Object* block, bool priv, Module* lookup_from)
    : args_(arg_count, call_frame->stack_back_position(arg_count - 1))
    , caller_(call_frame)
    , lookup_from_(lookup_from)
    , send_site(ss)
    , name(name)
    , recv(recv)
    , block(block)
    , priv(priv)
    , method(static_cast<Executable*>(Qnil))
    , module(static_cast<Module*>(Qnil))
    , method_missing(false)
  {}

  Message::Message(CallFrame* call_frame, size_t arg_count)
    : args_(arg_count, call_frame->stack_back_position(arg_count - 1))
    , caller_(call_frame)
    , lookup_from_(NULL)
    , send_site(NULL)
    , name(NULL)
    , recv(NULL)
    , block(NULL)
    , priv(false)
    , method(NULL)
    , module(NULL)
    , method_missing(false)
  {}

  Message::Message(Symbol* name, Object* recv, size_t arg_count,
                   Object* block, Module* lookup_from)
    : args_(arg_count, NULL)
    , caller_(NULL)
    , lookup_from_(lookup_from)
    , send_site(NULL)
    , name(name)
    , recv(recv)
    , block(block)
    , priv(false)
    , method(static_cast<Executable*>(Qnil))
    , module(static_cast<Module*>(Qnil))
    , method_missing(false)
  {}

  /* @todo: is this used anywhere but test_nativefunction? */
  Message::Message(Array* ary)
    : args_(ary)
    , caller_(NULL)
    , lookup_from_(NULL)
    , send_site(NULL)
    , name(NULL)
    , recv(Qnil)
    , block(Qnil)
    , priv(false)
    , method(NULL)
    , module(NULL)
    , method_missing(false)
  {}

  Message::Message(STATE)
    : caller_(NULL)
    , lookup_from_(NULL)
    , send_site(NULL)
    , name(NULL)
    , recv(Qnil)
    , block(Qnil)
    , priv(false)
    , method(NULL)
    , module(NULL)
    , method_missing(false)
  {}

  void Message::set_arguments(STATE, Array* args) {
    args_.use_array(args);
  }

  Object* Message::current_self() {
    return caller_->self();
  }

  /* Only called if send_message can't locate anything to run, which pretty
   * much never happens, since it means even method_missing wasn't available. */
  static void tragic_failure(STATE, Message& msg) {
    std::stringstream ss;
    ss << "unable to locate any method '" << msg.send_site->name()->c_str(state) <<
      "' from '" << msg.lookup_from(state)->name()->c_str(state) << "'";

    Exception::assertion_error(state, ss.str().c_str());
  }

  Object* Message::send(STATE, CallFrame* call_frame) {
    Symbol* original_name = name;
    if(!GlobalCacheResolver::resolve(state, *this)) {
      method_missing = true;
      name = G(sym_method_missing);
      priv = true; // lets us look for method_missing anywhere
      if(!GlobalCacheResolver::resolve(state, *this)) {
        tragic_failure(state, *this);
        return NULL;
      }
    }

    if(method_missing) {
      args_.unshift(state, original_name);
    }

    return method->execute(state, call_frame, *this);
  }

  Module* Message::lookup_from(STATE) {
    if(!lookup_from_) {
      lookup_from_ = recv->lookup_begin(state);
    }

    return lookup_from_;
  }

  void Message::flush_lookup() {
    lookup_from_ = 0;
  }
}
