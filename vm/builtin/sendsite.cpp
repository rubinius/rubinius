#include "builtin/sendsite.hpp"
#include "builtin/class.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/selector.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"
#include "global_cache.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  namespace performer {
    /*
     * A simple monomorphic cache resolver. Does not support
     * method missing, so it must not be installed if method missing
     * was used.
     */
    ExecuteStatus mono_performer(STATE, Task* task, Message& msg) {
      if(likely(msg.lookup_from == msg.send_site->recv_class())) {
        msg.module = msg.send_site->module();
        msg.method = msg.send_site->method();

        msg.send_site->hits++;
      } else {
        msg.send_site->misses++;
        return basic_performer(state, task, msg);
      }

      return msg.method->execute(state, task, msg);
    }

    /**
     * A simple monomorphic cache for when the destination method is
     * a method_missing style dispatch.
     */
    ExecuteStatus mono_mm_performer(STATE, Task* task, Message& msg) {
      if(likely(msg.lookup_from == msg.send_site->recv_class())) {
        msg.module = msg.send_site->module();
        msg.method = msg.send_site->method();

        msg.send_site->hits++;
      } else {
        msg.send_site->misses++;
        return basic_performer(state, task, msg);
      }

      msg.unshift_argument(state, msg.name);

      return msg.method->execute(state, task, msg);

    }

    ExecuteStatus basic_performer(STATE, Task* task, Message& msg) {
      SYMBOL original_name = msg.name;

      if(!GlobalCacheResolver::resolve(state, msg)) {
        msg.method_missing = true;
        msg.name = G(sym_method_missing);
        msg.priv = true; // lets us look for method_missing anywhere
        sassert(GlobalCacheResolver::resolve(state, msg));
      }

      // Populate for mono!
      msg.send_site->module(state, msg.module);
      msg.send_site->method(state, msg.method);
      msg.send_site->recv_class(state, msg.lookup_from);
      msg.send_site->method_missing = msg.method_missing;

      if(unlikely(msg.method_missing)) {
        msg.unshift_argument(state, original_name);
        msg.send_site->performer = mono_mm_performer;
      } else {
        msg.send_site->performer = mono_performer;
      }

      return msg.method->execute(state, task, msg);
    }
  }

  void SendSite::init(STATE) {
    GO(send_site).set(state->new_class("SendSite"));
    G(send_site)->set_object_type(state, SendSiteType);
  }

  SendSite* SendSite::create(STATE, OBJECT name) {
    SendSite* ss = (SendSite*)state->new_struct(G(send_site), sizeof(SendSite));
    ss->name(state, (SYMBOL)name);
    ss->sender(state, (CompiledMethod*)Qnil);
    ss->selector(state, Selector::lookup(state, name));

    ss->initialize(state);
    ss->selector()->associate(state, ss);

    return ss;
  }


  void SendSite::initialize(STATE) {
    resolver = MonomorphicInlineCacheResolver::resolve;
    performer = performer::basic_performer;

    method(state, (Executable*)Qnil);
    module(state, (Module*)Qnil);
    recv_class(state, (Module*)Qnil);
    method_missing = false;
    hits = misses = 0;
  }

  OBJECT SendSite::set_sender(STATE, CompiledMethod* cm) {
    sender(state, cm);
    return Qnil;
  }

  OBJECT SendSite::hits_prim(STATE) {
    return Integer::from(state, hits);
  }

  OBJECT SendSite::misses_prim(STATE) {
    return Integer::from(state, misses);
  }

  /* Use the information within +this+ to populate +msg+. Returns
   * true if +msg+ was populated. */

  bool SendSite::locate(STATE, Message& msg) {
    SYMBOL original_name = msg.name;

    if(!resolver(state, msg)) {
      msg.method_missing = true;
      msg.name = G(sym_method_missing);
      msg.priv = true; // lets us look for method_missing anywhere
      if(!resolver(state, msg)) {
        return false;
      }
    }

    if(msg.method_missing) {
      msg.unshift_argument(state, original_name);
    }

    return true;
  }

  /* Fill in details about +msg+ by looking up the class heirarchy
   * and in method tables. Returns true if lookup was successful
   * and +msg+ is now filled in. */

  bool HierarchyResolver::resolve(STATE, Message& msg) {
    Module* module = msg.lookup_from;
    OBJECT entry;
    MethodVisibility* vis;

    do {
      entry = module->method_table()->fetch(state, msg.name);

      /* Nothing, there? Ok, keep looking. */
      if(entry->nil_p()) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry == Qfalse) return false;

      vis = try_as<MethodVisibility>(entry);

      /* If this was a private send, then we can handle use
       * any method seen. */
      if(msg.priv) {
        /* nil means that the actual method object is 'up' from here */
        if(vis && vis->method()->nil_p()) goto keep_looking;

        msg.method = as<Executable>(vis ? vis->method() : entry);
        msg.module = module;
        break;
      } else if(vis) {
        /* The method is private, but this wasn't a private send. */
        if(vis->private_p(state)) {
          return false;
        } else if(vis->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module */
          if(!msg.current_self()->kind_of_p(state, module)) {
            return false;
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks */
        if(vis->method()->nil_p()) {
          msg.priv = true;
          goto keep_looking;
        }

        msg.method = as<Executable>(vis->method());
        msg.module = module;
        break;
      } else {
        msg.method = as<Executable>(entry);
        msg.module = module;
        break;
      }

keep_looking:
      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    return true;
  }

  bool GlobalCacheResolver::resolve(STATE, Message& msg) {
    struct GlobalCache::cache_entry* entry;

    entry = state->global_cache->lookup(msg.lookup_from, msg.name);
    if(entry) {
      if(msg.priv || entry->is_public) {
        msg.method = entry->method;
        msg.module = entry->module;
        msg.method_missing = entry->method_missing;

        return true;
      }
    }

    if(HierarchyResolver::resolve(state, msg)) {
      state->global_cache->retain(state, msg.lookup_from, msg.name,
          msg.module, msg.method, msg.method_missing);
      return true;
    }

    return false;
  }

  bool MonomorphicInlineCacheResolver::resolve(STATE, Message& msg) {
    if(msg.lookup_from == msg.send_site->recv_class()) {
      msg.module = msg.send_site->module();
      msg.method = msg.send_site->method();
      msg.method_missing = msg.send_site->method_missing;

      msg.send_site->hits++;
      return true;
    }

    msg.send_site->misses++;
    if(GlobalCacheResolver::resolve(state, msg)) {
      msg.send_site->module(state, msg.module);
      msg.send_site->method(state, msg.method);
      msg.send_site->recv_class(state, msg.lookup_from);
      msg.send_site->method_missing = msg.method_missing;

      return true;
    }

    return false;
  }

  void SendSite::Info::show(STATE, OBJECT self, int level) {
    SendSite* ss = as<SendSite>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); ss->name()->show(state, level);
    indent_attribute(level, "sender"); class_info(state, ss->sender(), true);
    indent_attribute(level, "selector"); class_info(state, ss->selector(), true);
    indent_attribute(level, "hits"); std::cout << ss->hits << std::endl;
    indent_attribute(level, "misses"); std::cout << ss->misses << std::endl;
    indent_attribute(level, "module"); class_info(state, ss->module(), true);
    indent_attribute(level, "method"); class_info(state, ss->method(), true);
    indent_attribute(level, "recv_class"); class_info(state, ss->recv_class(), true);
    close_body(level);
  }
};
