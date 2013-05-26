#include "builtin/call_custom_cache.hpp"
#include "builtin/call_unit.hpp"
#include "builtin/class.hpp"
#include "ontology.hpp"

namespace rubinius {

  void CallCustomCache::init(STATE) {
    GO(call_custom_cache).set(
        ontology::new_class(state, "CallCustomCache",
          G(call_site), G(rubinius)));
    G(call_custom_cache)->set_object_type(state, CallCustomCacheType);
  }

  CallCustomCache* CallCustomCache::create(STATE, CallSite* call_site, CallUnit* call_unit) {
    CallCustomCache* cache = state->new_object_dirty<CallCustomCache>(G(call_custom_cache));
    cache->name_      = call_site->name();
    cache->executable(state, call_site->executable());
    cache->ip_        = call_site->ip();
    cache->executor_  = check_cache;
    cache->fallback_  = call_site->fallback_;
    cache->updater_   = NULL;
    cache->call_unit(state, call_unit);
    cache->hits_      = 0;
    return cache;
  }

  Object* CallCustomCache::check_cache(STATE, CallSite* call_site,
      CallFrame* call_frame, Arguments& args)
  {
    CallCustomCache* cache = static_cast<CallCustomCache*>(call_site);

    CallUnit* cu = cache->call_unit_;
    return cu->execute(state, call_frame, cu,
                       cu->executable(), cu->module(), args);
  }

  void CallCustomCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }
}

