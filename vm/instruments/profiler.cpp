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
#include "configuration.hpp"

#include "instruments/timing.hpp"

#include <time.h>

#include <iostream>
#include <vector>
#include <algorithm>

namespace rubinius {

  namespace profiler {

    Method::~Method() { }

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

    Node* Node::find_sub_node(Profiler* profiler, Method* method) {
      Node* sub = first_sub_node_;

      while(sub) {
        if(sub->method() == method) return sub;
        sub = sub->sibling();
      }

      Node* node = new Node(method, profiler->next_node_id());
      node->set_sibling(first_sub_node_);
      first_sub_node_ = node;

      return node;
    }

    MethodEntry::MethodEntry(STATE, Dispatch& msg, Arguments& args)
      : state_(state)
      , node_(0)
    {
      method_ = state->profiler()->enter_method(
          msg, args, reinterpret_cast<CompiledMethod*>(Qnil), false);
      start();
    }

    MethodEntry::MethodEntry(STATE, Dispatch& msg, Arguments& args, CompiledMethod* cm, bool jit)
      : state_(state)
      , node_(0)
    {
      method_ = state->profiler()->enter_method(msg, args, cm, jit);
      start();
    }

    MethodEntry::MethodEntry(STATE, Symbol* name, Module* module, CompiledMethod* cm, bool jit)
      : state_(state)
      , node_(0)
    {
      method_ = state->profiler()->enter_block(name, module, cm, jit);
      start();
    }

    MethodEntry::MethodEntry(STATE, Kind kind, CompiledMethod* cm)
      : state_(state)
      , node_(0)
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
      Profiler* profiler = state_->profiler();

      previous_ = profiler->current();
      node_ = previous_->find_sub_node(profiler, method_);
      profiler->set_current(node_);

      method_->timer.start();
      timer_.start();
    }

    MethodEntry::~MethodEntry() {
      if(!state_->shared.profiling()) return;

      method_->timer.stop();
      timer_.stop();
      method_->accumulate(timer_.total());
      node_->accumulate(timer_.total());
      state_->profiler()->set_current(previous_);
    }

    Profiler::Profiler(STATE)
      : state_(state)
      , nodes_(0)
    {
      root_ = current_ = new Node(0, next_node_id());
      threshold_ = (uint32_t)state->shared.config.profiler_threshold;
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

    typedef std::vector<Node*> WorkList;

    static Fixnum* make_key(Method* meth, KeyMap& keys) {
      KeyMap::iterator iter = keys.find(meth);

      if(iter == keys.end()) {
        Fixnum* key = Fixnum::from(keys.size());
        keys[meth] = key;
        return key;
      }

      return iter->second;
    }

    static Fixnum* add_method(STATE, LookupTable* profile, Method* meth,
                              KeyMap& keys)
    {
      LookupTable* methods = try_as<LookupTable>(profile->fetch(
            state, state->symbol("methods")));

      if(!methods) return 0;

      Fixnum* key = make_key(meth, keys);

      // We already have the method, skip this.
      if(!methods->fetch(state, key)->nil_p()) return key;

      Symbol* cumulative_sym = state->symbol("cumulative");
      Symbol* total_sym = state->symbol("total");
      Symbol* called_sym = state->symbol("called");

      LookupTable* method = LookupTable::create(state);
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

      return key;
    }

    static void add_node(STATE, LookupTable* profile, Node* node,
                           WorkList& work, KeyMap& keys, uint32_t threshold)
    {
      LookupTable* nodes = try_as<LookupTable>(profile->fetch(
            state, state->symbol("nodes")));

      if(!nodes) return;

      // We haven't exited this method yet, so its stats won't be accurate
      if(node->method()->timer.started()) return;

      Fixnum* key = Fixnum::from(node->id());

      Array* tbl = Array::create(state, 5);

      nodes->store(state, key, tbl);

      Fixnum* meth_key = add_method(state, profile, node->method(), keys);

      tbl->set(state, 0, meth_key);
      tbl->set(state, 1, Integer::from(state, node->total()));
      tbl->set(state, 2, Fixnum::from(node->called()));

      int count = node->count_sub_nodes();
      tbl->set(state, 3, Fixnum::from(count));

      Array* ary = Array::create(state, count);

      int idx = 0;

      Node* sub = node->sub_nodes();

      while(sub) {
        if(sub->total() >= threshold) {
          ary->set(state, idx++, Fixnum::from(sub->id()));
          work.push_back(sub);
        }

        sub = sub->sibling();
      }

      tbl->set(state, 4, ary);
    }

    void Profiler::results(LookupTable* profile, KeyMap& keys, uint64_t runtime) {
      WorkList work;

      // If we haven't even gone for a total of longer than 10x the threshold,
      // just disable the threshold.
      if(runtime < 10 * threshold_) threshold_ = 0;

      profile->store(state_, state_->symbol("total_nodes"), Fixnum::from(nodes_));

      Array* roots = Array::create(state_, root_->count_sub_nodes());
      profile->store(state_, state_->symbol("roots"), roots);

      int idx = 0;
      Node* sub = root_->sub_nodes();

      while(sub) {
        if(sub->total() >= threshold_) {
          roots->set(state_, idx++, Fixnum::from(sub->id()));
          work.push_back(sub);
        }

        sub = sub->sibling();
      }

      while(work.size() > 0) {
        Node* node = work.back();
        work.pop_back();

        add_node(state_, profile, node, work, keys, threshold_);
      }
    }

    Profiler::~Profiler() {
      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        delete i->second;
      }

      WorkList work;

      work.push_back(root_);

      while(work.size() > 0) {
        Node* node = work.back();
        work.pop_back();

        Node* sub = node->sub_nodes();

        while(sub) {
          work.push_back(sub);
          sub = sub->sibling();
        }

        delete node;
      }
    }

    ProfilerCollection::ProfilerCollection(STATE)
      : profile_(state, (LookupTable*)Qnil)
    {
      LookupTable* profile = LookupTable::create(state);
      LookupTable* methods = LookupTable::create(state);
      LookupTable* nodes   = LookupTable::create(state);
      profile->store(state, state->symbol("methods"), methods);
      profile->store(state, state->symbol("nodes"), nodes);
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
      uint64_t runtime = get_current_time() - start_time_;

      ProfilerMap::iterator iter = profilers_.find(vm);
      if(iter != profilers_.end()) {
        Profiler* profiler = iter->second;
        profiler->results(profile_.get(), keys_, runtime);

        iter->first->remove_profiler();

        delete iter->second;
        profilers_.erase(iter);
      }
    }

    LookupTable* ProfilerCollection::results(STATE) {
      LookupTable* profile = profile_.get();

      uint64_t runtime = get_current_time() - start_time_;
      profile->store(state, state->symbol("runtime"),
                     Integer::from(state, runtime));

      for(ProfilerMap::iterator iter = profilers_.begin();
          iter != profilers_.end();
          iter++) {
        iter->second->results(profile, keys_, runtime);
      }

      return profile;
    }
  }
}
