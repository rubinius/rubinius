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

#include "instruments/timing.hpp"

#include <time.h>

#include <iostream>
#include <vector>
#include <algorithm>

namespace rubinius {

  namespace profiler {
    bool Key::operator==(const Key& other) const {
      return meth_ == other.meth_ &&
        container_ == other.container_ &&
        kind_ == other.kind_;
    }

    size_t Key::hash() const {
      return (size_t)meth_ ^ (size_t)container_;
    }

    void Invocation::start() {
      start_time_ = get_current_time();
    }

    void Invocation::stop() {
      leaf_->add_total_time(get_current_time() - start_time_);
    }

    Method::~Method() {
      for(Leaves::iterator i = leaves_.begin();
          i != leaves_.end();
          i++) {
        delete i->second;
      }
    }

    static uint64_t in_nanoseconds(uint64_t time) {
#ifdef USE_MACH_TIME
      static mach_timebase_info_data_t timebase = {0, 0};

      if(timebase.denom == 0) {
        mach_timebase_info(&timebase);
      }

      return time * timebase.numer / timebase.denom;
#else
      return time;
#endif

    }

    String* Method::name(STATE) {
      const char *module = "";
      const char *method_name = method()->c_str(state);

      if(Symbol* klass = try_as<Symbol>(container())) {
        module = klass->c_str(state);
      }

      String* name = String::create(state, module);

      switch(kind()) {
      case kNormal:
        name->append(state, "#");
        name->append(state, method_name);
        break;
      case kSingleton:
        name->append(state, ".");
        name->append(state, method_name);
        break;
      case kBlock:
        name->append(state, "#");
        name->append(state, method_name);
        name->append(state, " {}");
        break;
      }

      return name;
    }

    uint64_t Method::total_time_in_ns() {
      return in_nanoseconds(total_time_);
    }

    uint64_t Leaf::total_time_in_ns() {
      return in_nanoseconds(total_time_);
    }

    Leaf* Method::find_leaf(Method* callee) {
      Leaves::iterator i = leaves_.find(callee);

      if(i != leaves_.end()) {
        return i->second;
      }

      Leaf* leaf = new Leaf(callee);
      leaves_[callee] = leaf;
      return leaf;
    }

    void Leaf::add_total_time(uint64_t diff) {
      total_time_ += diff;
      method_->add_total_time(diff);
    }

    Profiler::Profiler(STATE)
      : state_(state)
    {
      top_ = new Method(0, 0, false);
      current_ = top_;
    }

    Profiler::~Profiler() {
      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        delete i->second;
      }

      delete top_;
    }

    Symbol* Profiler::module_name(Module* module) {
      if(IncludedModule* im = try_as<IncludedModule>(module)) {
        return im->module()->name();
      } else {
        return module->name();
      }
    }

    void Profiler::enter_block(Symbol* name, Module* module, CompiledMethod* cm) {
      record_method(cm, name, module_name(module), kBlock);
    }

    void Profiler::enter_method(Dispatch& msg, Arguments& args) {
      enter_method(msg, args, reinterpret_cast<CompiledMethod*>(Qnil));
    }

    void Profiler::enter_method(Dispatch &msg, Arguments& args, CompiledMethod* cm) {
      if(MetaClass* mc = try_as<MetaClass>(msg.module)) {
        Object* attached = mc->attached_instance();

        if(Module* mod = try_as<Module>(attached)) {
          record_method(cm, msg.name, mod->name(), kSingleton);
        } else {
          Symbol* name = args.recv()->to_s(state_)->to_sym(state_);
          record_method(cm, msg.name, name, kSingleton);
        }
      } else {
        record_method(cm, msg.name, module_name(msg.module), kNormal);
      }
    }

    Method* Profiler::record_method(CompiledMethod* cm, Symbol* name,
                                 Object* container, Kind kind) {
      Key key(name, container, kind);

      Method* method = find_key(key);
      if(!method) {
        method = new Method(methods_.size(), name, container, kind);
        methods_[key] = method;
      }

      Leaf* leaf = current_->find_leaf(method);

      current_ = method;

      method->called();

      if(!method->file() && !cm->nil_p()) {
        method->set_position(cm->file(), cm->start_line(state_));
      }

      Invocation invoke(leaf);
      invoke.start();
      running_.push(invoke);

      return method;
    }

    void Profiler::leave() {
      // Depending on when we started profiling, there could be calls
      // above the first time we entered a method, so ignore these.
      if(running_.empty()) return;

      Invocation& invoke = running_.top();
      invoke.stop();

      running_.pop();

      if(running_.empty()) {
        current_ = top_;
      } else {
        current_ = running_.top().leaf()->method();
      }
    }

    size_t Profiler::number_of_entries() {
      return methods_.size();
    }

    size_t Profiler::depth() {
      return running_.size();
    }

    Method* Profiler::find_key(Key& key) {
      return methods_[key];
    }

    // internal helper method
    static Array* update_method(STATE, LookupTable* profile, Method* meth) {
      LookupTable* methods = as<LookupTable>(profile->fetch(
            state, state->symbol("methods")));

      Symbol* total_sym = state->symbol("total");
      Symbol* called_sym = state->symbol("called");
      Symbol* leaves_sym = state->symbol("leaves");

      LookupTable* method;
      Fixnum* method_id = Fixnum::from(meth->id());
      if((method = try_as<LookupTable>(methods->fetch(state, method_id)))) {
        uint64_t total = as<Integer>(method->fetch(state, total_sym))->to_ulong_long();
        method->store(state, total_sym,
            Integer::from(state, total + meth->total_time_in_ns()));
        size_t called = as<Fixnum>(method->fetch(state, called_sym))->to_native();
        method->store(state, called_sym, Fixnum::from(called + meth->called_times()));

        return as<Array>(method->fetch(state, leaves_sym));
      } else {
        method = LookupTable::create(state);
        methods->store(state, method_id, method);

        method->store(state, state->symbol("name"), meth->name(state));
        method->store(state, total_sym, Integer::from(state, meth->total_time_in_ns()));
        method->store(state, called_sym, Fixnum::from(meth->called_times()));

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

        Array* leaves = Array::create(state, meth->number_of_leaves());
        method->store(state, leaves_sym, leaves);

        return leaves;
      }
    }

    void Profiler::results(LookupTable* profile) {
      std::vector<Method*> all_methods(0);

      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        all_methods.push_back(i->second);
      }

      for(std::vector<Method*>::iterator i = all_methods.begin();
          i != all_methods.end();
          i++) {
        Method* meth = *i;

        Array* leaves = update_method(state_, profile, meth);

        size_t idx = 0;
        for(Leaves::iterator li = meth->leaves_begin();
            li != meth->leaves_end();
            li++) {
          Leaf* leaf = li->second;

          Array* l = Array::create(state_, 2);
          leaves->set(state_, idx++, l);

          l->set(state_, 0, Fixnum::from(leaf->method()->id()));
          l->set(state_, 1, Integer::from(state_, leaf->total_time_in_ns()));
        }
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
        profiler->results(profile_.get());

        iter->first->remove_profiler();

        delete iter->second;
        profilers_.erase(iter);
      }
    }

    LookupTable* ProfilerCollection::results(STATE) {
      LookupTable* profile = profile_.get();

      for(ProfilerMap::iterator iter = profilers_.begin();
          iter != profilers_.end();
          iter++) {
        iter->second->results(profile);
      }

      return profile;
    }
  }
}
