#include "builtin/sendsite.hpp"
#include "builtin/class.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/selector.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodtable.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"

#include "global_cache.hpp"
#include "objectmemory.hpp"

#include "lookup_data.hpp"

namespace rubinius {
  static bool find_method(STATE, Module* module, Symbol* name, bool priv, Object* self,
                          Executable** found_method, Module** found_module) {
    MethodTableBucket* entry;

    do {
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry->undef_p(state)) return false;

      /* If this was a private send, then we can handle use
       * any method seen. */
      if(priv) {
        /* nil means that the actual method object is 'up' from here */
        if(entry->method()->nil_p()) goto keep_looking;

        *found_method = as<Executable>(entry->method());
        *found_module = module;
        break;
      } else {
        /* The method is private, but this wasn't a private send. */
        if(entry->private_p(state)) {
          return false;
        } else if(entry->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module */
          if(!self->kind_of_p(state, module)) {
            return false;
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks */
        if(entry->method()->nil_p()) {
          priv = true;
          goto keep_looking;
        }

        *found_method = as<Executable>(entry->method());
        *found_module = module;
        break;
      }

keep_looking:
      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    return true;
  }

  bool SendSite::fill(STATE, Module* klass, CallFrame* call_frame,
                                  SendSite::Internal* cache, bool priv,
                                  Module* lookup) {
    SendSite* ss = as<SendSite>(call_frame->cm->literals()->at(state, cache->literal));

    if(!lookup) lookup = klass;

    bool method_missing = false;

    if(find_method(state, lookup, ss->name(), priv, call_frame->self(),
                   &cache->method, &cache->module)) {
      cache->recv_class = klass;
      ss->write_barrier(state, cache->recv_class);
    } else {
      assert(find_method(state, lookup, G(sym_method_missing), true, call_frame->self(),
                         &cache->method, &cache->module));
      method_missing = true;
      cache->recv_class = 0; // Don't cache using method_missing
    }

    ss->write_barrier(state, cache->method);
    ss->write_barrier(state, cache->module);

    cache->execute = cache->method->execute;
    return method_missing;
  }

  namespace performer {
    /*
     * A simple monomorphic cache resolver. Does not support
     * method missing, so it must not be installed if method missing
     * was used.
     */
    Object* mono_performer(STATE, SendSite* ss, CallFrame* call_frame,
                           Dispatch& msg, Arguments& args) {
      if(likely(args.recv()->lookup_begin(state) == ss->recv_class())) {
        msg.module = ss->module();
        msg.method = ss->method();

        ss->hits++;
      } else {
        ss->misses++;
        return basic_performer(state, ss, call_frame, msg, args);
      }

      return msg.method->execute(state, call_frame, msg, args);
    }

    /**
     * A simple monomorphic cache for when the destination method is
     * a method_missing style dispatch.
     */
    Object* mono_mm_performer(STATE, SendSite* ss, CallFrame* call_frame,
                              Dispatch& msg, Arguments& args) {
      if(likely(args.recv()->lookup_begin(state) == ss->recv_class())) {
        msg.module = ss->module();
        msg.method = ss->method();

        ss->hits++;
      } else {
        ss->misses++;
        return basic_performer(state, ss, call_frame, msg, args);
      }

      args.unshift(state, msg.name);

      return msg.method->execute(state, call_frame, msg, args);
    }

    Object* basic_performer(STATE, SendSite* ss, CallFrame* call_frame,
                            Dispatch& msg, Arguments& args) {
      Symbol* original_name = msg.name;

      // priv?
      LookupData lookup(args.recv(), args.recv()->lookup_begin(state));
      bool method_missing = false;

      if(!GlobalCacheResolver::resolve(state, msg.name, msg, lookup)) {
        Dispatch dis(G(sym_method_missing));
        method_missing = true;
        lookup.priv = true; // lets us look for method_missing anywhere

        if(GlobalCacheResolver::resolve(state, dis.name, dis, lookup)) {
          msg.method = dis.method;
          msg.module = dis.module;
        } else {
          std::stringstream ss;
          ss << "could not find method \"" << original_name->c_str(state);
          ss << "\"";
          Assertion::raise(ss.str().c_str());
        }
      }

      // Populate for mono!
      ss->module(state, msg.module);
      ss->method(state, msg.method);
      ss->recv_class(state, lookup.from);
      ss->method_missing = method_missing;

      if(unlikely(method_missing)) {
        args.unshift(state, original_name);
        ss->performer = mono_mm_performer;
      } else {
        ss->performer = mono_performer;
      }

      return msg.method->execute(state, call_frame, msg, args);
    }
  }

  void SendSite::init(STATE) {
    GO(send_site).set(state->new_class_under("SendSite", G(rubinius)));
    G(send_site)->set_object_type(state, SendSiteType);
    G(send_site)->name(state, state->symbol("Rubinius::SendSite"));
  }

  SendSite* SendSite::create(STATE, Object* name) {
    SendSite* ss = state->new_struct<SendSite>(G(send_site));
    ss->name(state, (Symbol*)name);
    ss->sender(state, (CompiledMethod*)Qnil);
    ss->selector(state, Selector::lookup(state, name));

    ss->selector()->associate(state, ss);
    ss->inner_cache_ = 0;

    ss->initialize(state);

    return ss;
  }


  void SendSite::initialize(STATE) {
    performer = performer::basic_performer;

    method(state, (Executable*)Qnil);
    module(state, (Module*)Qnil);
    recv_class(state, (Module*)Qnil);
    method_missing = false;
    hits = misses = 0;

    if(inner_cache_) {
      inner_cache_->recv_class = 0;
    }
  }

  Object* SendSite::set_sender(STATE, CompiledMethod* cm) {
    sender(state, cm);
    return Qnil;
  }

  Object* SendSite::hits_prim(STATE) {
    return Integer::from(state, hits);
  }

  Object* SendSite::misses_prim(STATE) {
    return Integer::from(state, misses);
  }

  /* Fill in details about +msg+ by looking up the class heirarchy
   * and in method tables. Returns true if lookup was successful
   * and +msg+ is now filled in. */

  bool HierarchyResolver::resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup,
                                  bool* was_private)
  {
    Module* module = lookup.from;
    MethodTableBucket* entry;
    bool skip_vis_check = false;

    do {
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry->undef_p(state)) return false;

      /* If this was a private send, then we can handle use
       * any method seen. */
      if(lookup.priv || skip_vis_check) {
        /* nil means that the actual method object is 'up' from here */
        if(entry->method()->nil_p()) goto keep_looking;
        *was_private = entry->private_p(state);
        msg.method = entry->method();
        msg.module = module;
        break;
      } else {
        /* The method is private, but this wasn't a private send. */
        if(entry->private_p(state)) {
          return false;
        } else if(entry->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module */
          if(!lookup.recv->kind_of_p(state, module)) {
            return false;
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks */
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        msg.method = entry->method();
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

  bool GlobalCacheResolver::resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup) {
    struct GlobalCache::cache_entry* entry;

    Module* klass = lookup.from;

    entry = state->global_cache->lookup(klass, name);
    if(entry) {
      if(lookup.priv || entry->is_public) {
        msg.method = entry->method;
        msg.module = entry->module;
        msg.method_missing = entry->method_missing;

        return true;
      }
    }

    bool was_private = false;
    if(HierarchyResolver::resolve(state, name, msg, lookup, &was_private)) {
      state->global_cache->retain(state, lookup.from, name,
          msg.module, msg.method, msg.method_missing, was_private);
      return true;
    }

    return false;
  }

  void SendSite::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    SendSite* ss = as<SendSite>(obj);
    if(ss->inner_cache_) {
      Object* tmp;
      SendSite::Internal* cache = ss->inner_cache_;
      if(cache->module) {
        tmp = mark.call(cache->module);
        if(tmp) {
          cache->module = (Module*)tmp;
          mark.just_set(obj, tmp);
        }
      }

      if(cache->method) {
        tmp = mark.call(cache->method);
        if(tmp) {
          cache->method = (Executable*)tmp;
          mark.just_set(obj, tmp);
        }
      }

      if(cache->recv_class) {
        tmp = mark.call(cache->recv_class);
        if(tmp) {
          cache->recv_class = (Module*)tmp;
          mark.just_set(obj, tmp);
        }
      }
    }
  }

  void SendSite::Info::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);

    SendSite* ss = as<SendSite>(obj);
    if(ss->inner_cache_) {
      SendSite::Internal* cache = ss->inner_cache_;
      if(cache->module) {
        visit.call(cache->module);
      }

      if(cache->method) {
        visit.call(cache->method);
      }

      if(cache->recv_class) {
        visit.call(cache->recv_class);
      }
    }
  }

  void SendSite::Info::show(STATE, Object* self, int level) {
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
