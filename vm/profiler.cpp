#include "vm/profiler.hpp"

#include "vm/object_utils.hpp"
#include "vm/vm.hpp"

#include "builtin/symbol.hpp"
#include "detection.hpp"

// HACK figure out a better way to detect if we should use
// mach_absolute_time
#if defined(OS_X_ANCIENT) || \
	defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
  #define USE_MACH_TIME
#endif

#ifdef USE_MACH_TIME

#include <mach/mach_time.h>
#define current_time() mach_absolute_time()
#define METHOD "mach_absolute_time"

#else

uint64_t current_time() {
  timespec tp;
  if (clock_gettime(CLOCK_REALTIME, &tp)) {
    // error! Do something about it?
    return 0U;
  }
  return tp.tv_sec * 1000000000UL + tp.tv_nsec;
}
#define METHOD "clock_gettime"

#endif

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
      start_time_ = current_time();
    }

    void Invocation::stop() {
      leaf_->add_total_time(current_time() - start_time_);
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
      assert(!running_.empty());

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

    static bool method_cmp(Method* a, Method* b) {
      return a->total_time() > b->total_time();
    }

    void Profiler::print_results(STATE, std::ostream& stream) {
      std::vector<Method*> all_methods(0);

      for(MethodMap::iterator i = methods_.begin();
          i != methods_.end();
          i++) {
        all_methods.push_back(i->second);
      }

      std::sort(all_methods.begin(), all_methods.end(), method_cmp);

      stream << "<profile methods='" << methods_.size() <<
        "' method='" << METHOD << "'>\n";

      for(std::vector<Method*>::iterator i = all_methods.begin();
          i != all_methods.end();
          i++) {
        Method* meth = *i;
        stream << "<method id='" << meth->id() << "' name='";

        if(Symbol* klass = try_as<Symbol>(meth->container())) {
          stream << klass->c_str(state);
        } else {
          stream << "unknown";
        }

        Kind kind = meth->kind();
        if(kind == kNormal) {
          stream << ".";
        } else if(kind == kSingleton) {
          stream << "#";
        } else if(kind == kBlock) {
          stream << "#.";
        }

        stream << meth->method()->c_str(state);

        stream << "' total='" << meth->total_time_in_ns() <<
          "' called='" << meth->called_times();

        if(meth->file()) {
          const char *file;
          if(meth->file()->nil_p()) {
            file = "unknown file";
          } else {
            file = meth->file()->c_str(state);
          }
          stream << "' file='" << file <<
                    "' line='" << meth->line();
        }

        stream << "'>\n";

        for(Leaves::iterator li = meth->leaves_begin();
            li != meth->leaves_end();
            li++) {
          Leaf* leaf = li->second;
          stream << "  <leaf id='" << leaf->method()->id() <<
            "' total='" << leaf->total_time_in_ns() << "'/>\n";
        }
        stream << "</method>\n";
      }

      stream << "</profile>\n";
    }
  }
}
