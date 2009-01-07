#include "vm/profiler.hpp"

#include "vm/object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "detection.hpp"

#include "timing.hpp"

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
      const char *module = "unknown";

      if(Symbol* klass = try_as<Symbol>(container())) {
        module = klass->c_str(state);
      }

      String* name = String::create(state, module);

      switch(kind()) {
      case kNormal:
        name->append(state, ".");
        break;
      case kSingleton:
        name->append(state, "#");
        break;
      case kBlock:
        name->append(state, "#.");
        break;
      }

      name->append(state, method()->c_str(state));

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

    Method* Profiler::enter_method(Symbol* name, Object* container, Kind kind) {
      Key key(name, container, kind);

      Method* meth;

      meth = find_key(key);
      if(!meth) {
        meth = new Method(methods_.size(), name, container, kind);
        methods_[key] = meth;
      }

      Leaf* leaf = current_->find_leaf(meth);

      current_ = meth;

      meth->called();

      Invocation invoke(leaf);
      invoke.start();
      running_.push(invoke);

      return meth;
    }

    void Profiler::leave_method() {
      if(running_.empty()) return;

      // Re-enable when enter/leave calls are correct
      // assert(!running_.empty());

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
