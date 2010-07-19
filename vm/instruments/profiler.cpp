#include "instruments/profiler.hpp"

#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/module.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "detection.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "vmmethod.hpp"

#include "instruments/timing.hpp"

#include <time.h>

#include <iostream>
#include <vector>
#include <algorithm>

namespace rubinius {

  namespace profiler {

    Fixnum* Edge::find_key(KeyMap& keys) {
      return method_->find_key(keys);
    }

    Method::~Method() {
      for(Edges::iterator i = edges_.begin();
          i != edges_.end();
          i++) {
        delete i->second;
      }
    }

    String* Method::to_s(STATE) {
      std::stringstream ss;

      const char *module = "<anonymous>";
      const char *method_name = name()->c_str(state);

      if(Symbol* klass = try_as<Symbol>(container())) {
        module = klass->c_str(state);
      }

      ss << module;

      switch(kind()) {
      case kNormal:
        ss << "#" << method_name;
        break;
      case kNormalJIT:
        ss << "#" << method_name << " <jit>";
        break;
      case kSingleton:
      case kYoungGC:
      case kMatureGC:
        ss << "." << method_name;
        break;
      case kSingletonJIT:
        ss << "." << method_name << " <jit>";
        break;
      case kBlock:
        ss << "::" << method_name << "<" << line_ << "> {}";
        break;
      case kBlockJIT:
        ss << "::" << method_name << " {" << line_ << "} <jit>";
        break;
      }

      return String::create(state, ss.str().c_str());
    }

    Edge* Method::find_edge(Method* method) {
      Edges::iterator iter = edges_.find(method->id());

      Edge* edge;
      if(likely(iter != edges_.end())) {
        edge = iter->second;
      } else {
        edge = new Edge(method);
        edges_[method->id()] = edge;
      }

      return edge;
    }

    Fixnum* Method::find_key(KeyMap& keys) {
      KeyMap::iterator iter = keys.find(id_);

      if(iter == keys.end()) {
        Fixnum* key = Fixnum::from(keys.size());
        keys[id_] = key;
        return key;
      }

      return iter->second;
    }

    Array* Method::edges(STATE, KeyMap& keys) {
      Array* edges = Array::create(state, edges_.size());

      size_t idx = 0;
      for(Edges::iterator i = edges_.begin();
          i != edges_.end();
          i++) {
        Edge* edge = i->second;

        Array* ary = Array::create(state, 3);
        edges->set(state, idx++, ary);

        ary->set(state, 0, edge->find_key(keys));
        ary->set(state, 1, Integer::from(state, edge->called()));
        ary->set(state, 2, Integer::from(state, edge->total()));
      }

      return edges;
    }

    void Method::merge_edges(STATE, KeyMap& keys, Array* edges) {
      for(Edges::iterator i = edges_.begin();
          i != edges_.end();
          i++) {
        Edge* edge = i->second;
        Fixnum* key = edge->find_key(keys);

        size_t j;
        for(j = 0; j < edges->size(); j++) {
          Array* ary = try_as<Array>(edges->get(state, j));
          if(!ary) continue;

          Fixnum* fix = try_as<Fixnum>(ary->get(state, 0));
          if(!fix) continue;

          if(key != fix) continue;

          Integer* igr = try_as<Integer>(ary->get(state, 1));
          if(!igr) continue;

          uint64_t called = igr->to_native();
          called += edge->called();
          ary->set(state, 1, Integer::from(state, called));

          igr = try_as<Integer>(ary->get(state, 2));
          if(!igr) continue;

          uint64_t total = igr->to_ulong_long();
          total += edge->total();
          ary->set(state, 2, Integer::from(state, total));

          break;
        }

        // We did not find an existing entry to update, create a new one
        if(j == edges->size()) {
          Array* ary = Array::create(state, 3);
          edges->append(state, ary);

          ary->set(state, 0, edge->find_key(keys));
          ary->set(state, 1, Integer::from(state, edge->called()));
          ary->set(state, 2, Integer::from(state, edge->total()));
        }
      }
    }

    MethodEntry::MethodEntry(STATE, Dispatch& msg, Arguments& args)
      : state_(state)
      , edge_(0)
    {
      method_ = state->profiler()->enter_method(
          msg, args, reinterpret_cast<CompiledMethod*>(Qnil), false);
      start();
    }

    MethodEntry::MethodEntry(STATE, Dispatch& msg, Arguments& args, CompiledMethod* cm, bool jit)
      : state_(state)
      , edge_(0)
    {
      method_ = state->profiler()->enter_method(msg, args, cm, jit);
      start();
    }

    MethodEntry::MethodEntry(STATE, Symbol* name, Module* module, CompiledMethod* cm, bool jit)
      : state_(state)
      , edge_(0)
    {
      method_ = state->profiler()->enter_block(name, module, cm, jit);
      start();
    }

    MethodEntry::MethodEntry(STATE, Kind kind)
      : state_(state)
      , edge_(0)
    {
      Symbol* name;

      switch(kind) {
      case kYoungGC:
        name = state_->symbol("collect_young");
        break;
      case kMatureGC:
        name = state_->symbol("collect_mature");
        break;
      default:
        name = state_->symbol("unknown");
      }

      CompiledMethod* cm = reinterpret_cast<CompiledMethod*>(Qnil);
      method_ = state->profiler()->find_method(cm, state_->symbol("GC"), name, kind);
      start();
    }

    void MethodEntry::start() {
      previous_ = state_->profiler()->current();
      if(previous_) {
        edge_ = previous_->find_edge(method_);
      }
      state_->profiler()->set_current(method_);
      method_->timer.start();
      timer_.start();
    }

    MethodEntry::~MethodEntry() {
      if(!state_->shared.profiling()) return;

      method_->timer.stop();
      timer_.stop();
      method_->accumulate(timer_.total());
      if(edge_) edge_->accumulate(timer_.total());
      state_->profiler()->set_current(previous_);
    }

    Profiler::~Profiler() {
      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        delete i->second;
      }
    }

    Symbol* Profiler::module_name(Module* module) {
      if(IncludedModule* im = try_as<IncludedModule>(module)) {
        return im->module()->name();
      } else {
        return module->name();
      }
    }

    Method* Profiler::enter_block(Symbol* name, Module* module, CompiledMethod* cm, bool jit) {
      return get_method(cm, name, module_name(module), jit ? kBlockJIT : kBlock);
    }

    Method* Profiler::enter_method(Dispatch &msg, Arguments& args, CompiledMethod* cm, bool jit) {
      if(MetaClass* mc = try_as<MetaClass>(msg.module)) {
        Object* attached = mc->attached_instance();

        if(Module* mod = try_as<Module>(attached)) {
          return get_method(cm, msg.name, mod->name(), jit ? kSingletonJIT : kSingleton);
        } else {
          Symbol* name = args.recv()->to_s(state_)->to_sym(state_);
          return get_method(cm, msg.name, name, jit ? kSingletonJIT : kSingleton);
        }
      } else {
        return get_method(cm, msg.name, module_name(msg.module), jit ? kNormalJIT : kNormal);
      }
    }

    Method* Profiler::get_method(CompiledMethod* cm, Symbol* name,
                                 Symbol* container, Kind kind) {
      Method* method = find_method(cm, container, name, kind);

      if(!method->file() && !cm->nil_p()) {
        method->set_position(cm->file(), cm->start_line(state_));
      }

      return method;
    }

    method_id Profiler::create_id(CompiledMethod* cm, Symbol* container,
                                  Symbol* name, Kind kind)
    {
      // If we have a CompiledMethod, use it's method id.
      if(!cm->nil_p()) {
        if(VMMethod* vmm = cm->backend_method()) {
          return (vmm->method_id() << 1) | 1;
        }
      }

      // | -- 32 bits of container -- | -- 29 bits of name -- | -- 2 bits of kind -- | 0

      uint32_t c = container->index() & 0xffffffff;
      uint32_t n = name->index()      & 0x1fffffff;
      uint32_t k = kind               & 0x3;

      return (((uint64_t)c) << 32) |
             (n << 3) |
             k << 1;
    }

    Method* Profiler::find_method(CompiledMethod* cm, Symbol* container,
                                  Symbol* name, Kind kind)
    {
      method_id id = create_id(cm, container, name, kind);

      Method* method;
      MethodMap::iterator iter = methods_.find(id);
      if(unlikely(iter == methods_.end())) {
        method = new Method(id, name, container, kind);
        methods_[method->id()] = method;
      } else {
        method = iter->second;
      }

      return method;
    }

    // internal helper method
    static void update_method(STATE, LookupTable* profile, KeyMap& keys, Method* meth) {
      LookupTable* methods = try_as<LookupTable>(profile->fetch(
            state, state->symbol("methods")));

      if(!methods) return;

      Symbol* cumulative_sym = state->symbol("cumulative");
      Symbol* total_sym = state->symbol("total");
      Symbol* called_sym = state->symbol("called");
      Symbol* edges_sym = state->symbol("edges");

      LookupTable* method;
      Fixnum* key = meth->find_key(keys);
      if((method = try_as<LookupTable>(methods->fetch(state, key)))) {
        Integer* igr = try_as<Integer>(method->fetch(state, cumulative_sym));
        if(!igr) igr = Integer::from(state, 0);

        uint64_t cumulative = igr->to_ulong_long();
        method->store(state, cumulative_sym,
            Integer::from(state, cumulative + meth->timer.total()));

        igr = try_as<Integer>(method->fetch(state, total_sym));
        if(!igr) igr = Integer::from(state, 0);

        uint64_t total = igr->to_ulong_long();
        method->store(state, total_sym,
            Integer::from(state, total + meth->total()));

        Fixnum* fix = try_as<Fixnum>(method->fetch(state, called_sym));
        if(!fix) fix = Fixnum::from(0);

        size_t called = fix->to_native();
        method->store(state, called_sym, Fixnum::from(called + meth->timer.count()));

        Array* ary = try_as<Array>(method->fetch(state, edges_sym));
        if(!ary) return;

        meth->merge_edges(state, keys, ary);
      } else {
        method = LookupTable::create(state);
        methods->store(state, key, method);

        method->store(state, state->symbol("name"), meth->to_s(state));
        method->store(state, cumulative_sym, Integer::from(state, meth->timer.total()));
        method->store(state, total_sym, Integer::from(state, meth->total()));
        method->store(state, called_sym, Fixnum::from(meth->timer.count()));

        if(meth->file()) {
          const char *file;
          if(meth->file()->nil_p()) {
            file = "unknown file";
          } else {
            file = meth->file()->c_str(state);
          }

          method->store(state, state->symbol("file"), String::create(state, file));
          method->store(state, state->symbol("line"), Fixnum::from(meth->line()));
        }

        method->store(state, edges_sym, meth->edges(state, keys));
      }
    }

    void Profiler::results(LookupTable* profile, KeyMap& keys) {
      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        Method* method = i->second;

        // We haven't exited this method yet, so its stats won't be accurate
        if(method->timer.started()) continue;

        update_method(state_, profile, keys, method);
      }
    }

    ProfilerCollection::ProfilerCollection(STATE)
      : profile_(state, (LookupTable*)Qnil)
    {
      LookupTable* profile = LookupTable::create(state);
      LookupTable* methods = LookupTable::create(state);
      profile->store(state, state->symbol("methods"), methods);
      profile->store(state, state->symbol("method"),
                     String::create(state, TIMING_METHOD));

      profile_.set(profile);

      start_time_ = get_current_time();
    }

    ProfilerCollection::~ProfilerCollection() {
      for(ProfilerMap::iterator iter = profilers_.begin();
          iter != profilers_.end();
          iter++) {
        iter->first->remove_profiler();
        delete iter->second;
      }
    }

    void ProfilerCollection::add_profiler(VM* vm, Profiler* profiler) {
      profilers_[vm] = profiler;
    }

    void ProfilerCollection::remove_profiler(VM* vm, Profiler* profiler) {
      ProfilerMap::iterator iter = profilers_.find(vm);
      if(iter != profilers_.end()) {
        Profiler* profiler = iter->second;
        profiler->results(profile_.get(), keys_);

        iter->first->remove_profiler();

        delete iter->second;
        profilers_.erase(iter);
      }
    }

    LookupTable* ProfilerCollection::results(STATE) {
      LookupTable* profile = profile_.get();

      profile->store(state, state->symbol("runtime"),
          Integer::from(state, get_current_time() - start_time_));

      for(ProfilerMap::iterator iter = profilers_.begin();
          iter != profilers_.end();
          iter++) {
        iter->second->results(profile, keys_);
      }

      return profile;
    }
  }
}
