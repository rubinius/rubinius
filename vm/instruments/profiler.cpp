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

    Method::~Method() {
      for(Edges::iterator i = edges_.begin();
          i != edges_.end();
          i++) {
        delete i->second;
      }
    }

    String* Method::to_s(STATE) {
      std::stringstream ss;

      if(kind() == kScript) {
        ss << "script:";
        if(file_) {
          ss << file_->c_str(state);
        } else {
          ss << "--unknown-file--";
          ss << ":" << line_;
        }

        return String::create(state, ss.str().c_str());
      }

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
      case kFinalizers:
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
      case kScript:
        // handled above, just here to make gcc happy.
        abort();
      }

      return String::create(state, ss.str().c_str());
    }

    Method* Method::find_callee(method_id id, Symbol* container,
                                Symbol* name, Kind kind)
    {
      Edges::iterator iter = edges_.find(id);

      if(iter != edges_.end()) return iter->second->method();

      Method* method = new Method(id, name, container, kind);
      edges_[id] = new Edge(method);

      return method;
    }

    Edge* Method::find_edge(Method* meth) {
      Edges::iterator iter = edges_.find(meth->id());

      if(iter != edges_.end()) return iter->second;
      return 0;
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

    MethodEntry::MethodEntry(STATE, Kind kind, CompiledMethod* cm)
      : state_(state)
      , edge_(0)
    {
      Symbol* container;
      Symbol* name;

      switch(kind) {
      case kYoungGC:
        container = state_->symbol("GC");
        name = state_->symbol("collect_young");
        break;
      case kMatureGC:
        container = state_->symbol("GC");
        name = state_->symbol("collect_mature");
        break;
      case kFinalizers:
        container = state_->symbol("GC");
        name = state_->symbol("run_finalizers");
        break;
      case kScript:
        container = state_->symbol("__script__");
        name = state_->symbol("__script__");
        break;
      default:
        container = state_->symbol("unknown");
        name = state_->symbol("unknown");
      }

      if(!cm) cm = reinterpret_cast<CompiledMethod*>(Qnil);
      method_ = state->profiler()->get_method(cm, name, container, kind);
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

    Profiler::Profiler(STATE)
      : state_(state)
    {
      // Symbol* root = state->symbol("__root__");
      current_ = 0; root_ = 0;
      // root_ = current_ = new Method(0, root, root);
    }

    Profiler::~Profiler() {
      for(MethodMap::iterator i = root_methods_.begin();
          i != root_methods_.end();
          i++) {
        delete i->second;
      }

      // delete root_;
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

      if(current_) {
        method = current_->find_callee(id, container, name, kind);
      } else {
        MethodMap::iterator iter = root_methods_.find(id);

        if(unlikely(iter == root_methods_.end())) {
          method = new Method(id, name, container, kind);
          root_methods_[method->id()] = method;
        } else {
          method = iter->second;
        }
      }

      return method;
    }

    typedef std::vector<Method*> WorkList;

    static Fixnum* make_key(Method* meth, KeyMap& keys) {
      KeyMap::iterator iter = keys.find(meth);

      if(iter == keys.end()) {
        Fixnum* key = Fixnum::from(keys.size());
        keys[meth] = key;
        return key;
      }

      return iter->second;
    }

    static void add_method(STATE, LookupTable* profile, Method* meth,
                           WorkList& work, KeyMap& keys)
    {
      LookupTable* methods = try_as<LookupTable>(profile->fetch(
            state, state->symbol("methods")));

      if(!methods) return;

      Symbol* cumulative_sym = state->symbol("cumulative");
      Symbol* total_sym = state->symbol("total");
      Symbol* called_sym = state->symbol("called");
      Symbol* edges_sym = state->symbol("edges");

      LookupTable* method = LookupTable::create(state);
      Fixnum* key = make_key(meth, keys);
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

      Edges& edges_map = meth->edges();

      Array* edges = Array::create(state, edges_map.size());

      size_t idx = 0;
      for(Edges::iterator i = edges_map.begin();
          i != edges_map.end();
          i++) {
        Edge* edge = i->second;

        // We haven't exited this method yet, so its stats won't be accurate
        if(edge->method()->timer.started()) continue;

        Array* ary = Array::create(state, 3);
        edges->set(state, idx++, ary);

        ary->set(state, 0, make_key(edge->method(), keys));
        work.push_back(edge->method());

        ary->set(state, 1, Integer::from(state, edge->called()));
        ary->set(state, 2, Integer::from(state, edge->total()));
      }

      method->store(state, edges_sym, edges);
    }

    void Profiler::results(LookupTable* profile, KeyMap& keys) {
      WorkList work;

      for(MethodMap::iterator i = root_methods_.begin();
          i != root_methods_.end();
          i++) {

        Method* method = i->second;

        // We haven't exited this method yet, so its stats won't be accurate
        if(method->timer.started()) continue;
        work.push_back(method);
      }

      while(work.size() > 0) {
        Method* method = work.back();
        work.pop_back();

        add_method(state_, profile, method, work, keys);
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
      printf("writing results...\n");
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
