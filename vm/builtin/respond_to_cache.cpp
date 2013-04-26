#include "detection.hpp"

#include "arguments.hpp"
#include "call_frame.hpp"
#include "global_cache.hpp"
#include "ontology.hpp"

#include "builtin/respond_to_cache.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  void RespondToCache::init(STATE) {
    GO(respond_to_cache).set(
        ontology::new_class(state, "RespondToCache",
          G(call_site), G(rubinius)));

  }

  RespondToCache* RespondToCache::empty(STATE, CallSite* fallback, Executable* executable, int ip) {
    RespondToCache* cache =
      state->vm()->new_object<RespondToCache>(G(respond_to_cache));
    cache->fallback_call_site(state, fallback);
    cache->executable(state, executable);
    cache->ip_              = ip;
    cache->name_            = fallback->name();
    cache->receiver_class_  = nil<Class>();
    cache->visibility_      = nil<Symbol>();
    cache->responds_        = cNil;
    cache->executor_        = check_cache;
    cache->fallback_        = check_cache;
    cache->clear_receiver_data();
    return cache;
  }

  Object* RespondToCache::check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args) {
    RespondToCache* cache = static_cast<RespondToCache*>(call_site);

    Class* const recv_class = args.recv()->lookup_begin(state);
    Object* const message   = args.get_argument(0);
    Object* visibility      = cFalse;
    if(args.total() > 1) {
      visibility            = args.get_argument(1);
    }

    register uint64_t recv_data = recv_class->data_id();
    if(likely(recv_data == cache->receiver_data() && message == cache->message_ && visibility == cache->visibility_)) {
      return cache->responds_;
    }

    return cache->fallback_call_site_->execute(state, call_frame, args);
  }

  void RespondToCache::update(STATE, Object* recv, Symbol* msg, Object* priv, Object* res) {
    Class* const recv_class = recv->lookup_begin(state);
    uint64_t recv_data = recv_class->data_id();
    receiver_class(state, recv_class);
    message(state, msg);
    visibility(state, priv);
    responds(state, res);
    set_receiver_data(recv_data);
    executor_ = check_cache;
  }

  void RespondToCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

