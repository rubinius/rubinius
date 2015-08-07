#include "arguments.hpp"
#include "call_frame.hpp"
#include "object_memory.hpp"

#include "builtin/class.hpp"
#include "builtin/respond_to_cache.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  void RespondToCache::bootstrap(STATE) {
    GO(respond_to_cache).set(state->memory()->new_class<Class, RespondToCache>(
          state, G(call_site), G(rubinius), "RespondToCache"));
  }

  RespondToCache* RespondToCache::create(STATE, CallSite* fallback,
                    Object* recv, Symbol* msg, Object* priv, Object* res, int hits) {

    RespondToCache* cache =
      state->memory()->new_object<RespondToCache>(state, G(respond_to_cache));

    Class* recv_class = recv->direct_class(state);

    cache->fallback_call_site(state, fallback);
    cache->executable(state, fallback->executable());
    cache->name(state, fallback->name());
    cache->receiver_class(state, recv_class);
    cache->visibility(state, priv);
    cache->message(state, msg);
    cache->responds(state, res);

    cache->ip_ = fallback->ip();
    cache->executor_ = check_cache;
    cache->fallback_ = check_cache;
    cache->updater_ = NULL;
    cache->hits_ = hits;
    cache->set_receiver_data(recv_class->data_raw());

    return cache;
  }

  Object* RespondToCache::check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args) {
    RespondToCache* cache = static_cast<RespondToCache*>(call_site);

    Class* const recv_class = args.recv()->direct_class(state);
    if(args.total() > 0) {
      Object* const message   = args.get_argument(0);
      Object* visibility      = cFalse;
      if(args.total() > 1) {
        visibility            = args.get_argument(1);
      }

      uint64_t recv_data = recv_class->data_raw();
      if(likely(recv_data == cache->receiver_data_raw() && message == cache->message_ && visibility == cache->visibility_)) {
        cache->hit();
        return cache->responds_;
      }
    }

    return cache->fallback_call_site_->execute(state, call_frame, args);
  }

  Integer* RespondToCache::hits_prim(STATE) {
    return Integer::from(state, hits_);
  }

  void RespondToCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

