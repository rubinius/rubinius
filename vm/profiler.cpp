#include "vm/profiler.hpp"

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

    Profiler::Profiler() {
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

    /*
    void Profiler::enter_block(STATE, MethodContext* ctx, CompiledMethod* cm) {
      record_method(state, cm, as<Symbol>(ctx->name()),
          module_name(ctx->module()), kBlock);
    }
    */

    void Profiler::enter_primitive(STATE, Dispatch& msg, Arguments& args) {
      enter_method(state, msg, args, (CompiledMethod*)Qnil);
    }

    void Profiler::enter_method(STATE, Dispatch &msg, Arguments& args, CompiledMethod* cm) {
      if(MetaClass* mc = try_as<MetaClass>(msg.module)) {
        Object* attached = mc->attached_instance();

        if(Module* mod = try_as<Module>(attached)) {
          record_method(state, cm, msg.name, mod->name(), kSingleton);
        } else {
          Symbol* name = args.recv()->to_s(state)->to_sym(state);
          record_method(state, cm, msg.name, name, kSingleton);
        }
      } else {
        record_method(state, cm, msg.name, module_name(msg.module), kNormal);
      }
    }

    Method* Profiler::record_method(STATE, CompiledMethod* cm, Symbol* name,
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
        method->set_position(cm->file(), cm->start_line(state));
      }

      Invocation invoke(leaf);
      invoke.start();
      running_.push(invoke);

      return method;
    }

    void Profiler::leave_method() {
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

    LookupTable* Profiler::results(STATE) {
      std::vector<Method*> all_methods(0);

      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        all_methods.push_back(i->second);
      }

      LookupTable* profile = LookupTable::create(state);
      profile->store(state, state->symbol("num_methods"),
                     Integer::from(state, methods_.size()));
      profile->store(state, state->symbol("method"),
                     String::create(state, TIMING_METHOD));

      LookupTable* methods = LookupTable::create(state);
      profile->store(state, state->symbol("methods"), methods);

      for(std::vector<Method*>::iterator i = all_methods.begin();
          i != all_methods.end();
          i++) {
        Method* meth = *i;
        LookupTable* method = LookupTable::create(state);

        methods->store(state, Fixnum::from(meth->id()), method);

        method->store(state, state->symbol("name"), meth->name(state));
        method->store(state, state->symbol("total"),
                      Integer::from(state, meth->total_time_in_ns()));
        method->store(state, state->symbol("called"),
                      Fixnum::from(meth->called_times()));

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
        method->store(state, state->symbol("leaves"), leaves);

        size_t idx = 0;
        for(Leaves::iterator li = meth->leaves_begin();
            li != meth->leaves_end();
            li++) {
          Leaf* leaf = li->second;

          Array* l = Array::create(state, 2);
          leaves->set(state, idx++, l);

          l->set(state, 0, Fixnum::from(leaf->method()->id()));
          l->set(state, 1, Integer::from(state, leaf->total_time_in_ns()));
        }
      }

      return profile;
    }
  }
}
