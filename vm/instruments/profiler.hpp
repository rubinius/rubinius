#ifndef RBX_PROFILER_HPP
#define RBX_PROFILER_HPP

#include "vm.hpp"
#include "instruments/stats.hpp"
#include "testable.hpp"

#include <stdint.h>
#include <stdio.h>

#include <stack>
#include <tr1/unordered_map>
#include <map>
#include <iostream>

namespace rubinius {
  namespace profiler {
    typedef uint64_t method_id;
  }
}

#if (__GNUC__ <= 4 && __GNUC_MINOR__ <= 1)
namespace std {
  namespace tr1 {
    template <>
      struct hash<rubinius::profiler::method_id> {
        size_t operator()(const rubinius::profiler::method_id id) const {
          return id;
        }
      };
  }
}
#endif

class TestProfiler;

namespace rubinius {
  class VM;
  class Symbol;
  class Object;
  class LookupTable;
  class Fixnum;
  class String;
  class CompiledMethod;
  class Dispatch;
  class Arguments;
  class Message;

  namespace profiler {

    enum Kind {
      kNormal,
      kSingleton,
      kBlock,
      kYoungGC,
      kMatureGC,
      kNormalJIT,
      kSingletonJIT,
      kBlockJIT
    };

    class Method;
    typedef std::tr1::unordered_map<method_id, Fixnum*> KeyMap;

    class Edge {
      Method*  method_;
      uint64_t total_;
      uint64_t called_;

    public:
      Edge(Method* method)
        : method_(method)
        , total_(0)
        , called_(0)
      { }

      Method* method() {
        return method_;
      }

      uint64_t total() {
        return total_;
      }

      uint64_t called() {
        return called_;
      }

      void accumulate(uint64_t time) {
        total_ += time;
        called_++;
      }

      Fixnum* find_key(KeyMap& keys);
    };

    typedef std::tr1::unordered_map<method_id, Edge*> Edges;

    class Method {
    private:
      method_id id_;
      Symbol*   name_;
      Symbol*   container_;
      Kind      kind_;
      Symbol*   file_;
      int       line_;
      Edges     edges_;
      uint64_t  total_;

    public:
      stats::StackTimer  timer;

    public:
      Method(method_id id, Symbol* name, Symbol* container, Kind kind=kNormal)
        : id_(id)
        , name_(name)
        , container_(container)
        , kind_(kind)
        , file_(0)
        , line_(0)
        , total_(0)
      { }
      ~Method();

      method_id id() {
        return id_;
      }

      Symbol* container() {
        return container_;
      }

      Symbol* name() {
        return name_;
      }

      Kind kind() {
        return kind_;
      }

      String* to_s(STATE);

      Symbol* file() {
        return file_;
      }

      int line() {
        return line_;
      }

      void set_position(Symbol* file, int line) {
        file_ = file;
        line_ = line;
      }

      uint64_t total() {
        return total_;
      }

      void accumulate(uint64_t time) {
        total_ += time;
      }

      Edge* find_edge(Method* method);

      Fixnum* find_key(KeyMap& keys);
      Array* edges(STATE, KeyMap& keys);
      void merge_edges(STATE, KeyMap& keys, Array* edges);
    };

    class Profiler;

    /** Created when a method is being called. Contains a timer that tracks
     * how much time is spent in the method. When the MethodEntry instance
     * goes out of scope, the destructor records the elapsed time and updates
     * the Method and Edge objects.
     */
    class MethodEntry {
      VM*           state_;
      Edge*         edge_;
      Method*       method_;
      Method*       previous_;
      stats::Timer  timer_;

    public:
      MethodEntry(STATE, Dispatch& msg, Arguments& args);
      MethodEntry(STATE, Dispatch& msg, Arguments& args, CompiledMethod* cm, bool jit=false);
      MethodEntry(STATE, Symbol* name, Module* module, CompiledMethod* cm, bool jit=false);
      MethodEntry(STATE, Kind kind);
      ~MethodEntry();

      void start();

      TEST_CLASS(TestProfiler);
    };

    class Profiler {
      typedef std::tr1::unordered_map<method_id, Method*> MethodMap;

    private:
      MethodMap methods_;
      Method*   current_;
      VM*       state_;

    public:
      Profiler(STATE)
        : current_(0)
        , state_(state)
      { }
      ~Profiler();

      Method* current() {
        return current_;
      }

      void set_current(Method* method) {
        current_ = method;
      }

      method_id create_id(CompiledMethod* cm, Symbol* container, Symbol* name, Kind kind);
      Method* find_method(CompiledMethod* cm, Symbol* container, Symbol* name, Kind kind);

      Symbol* module_name(Module* module);
      Method* enter_method(Dispatch&, Arguments& args, CompiledMethod* cm, bool jit);
      Method* enter_block(Symbol* name, Module* module, CompiledMethod* cm, bool jit);
      Method* get_method(CompiledMethod* cm, Symbol* name,
          Symbol* container, Kind kind);

      void results(LookupTable* profile, KeyMap& keys);

      TEST_CLASS(TestProfiler);
    };

    class ProfilerCollection {
      typedef std::tr1::unordered_map<VM*, Profiler*> ProfilerMap;

    private:
      uint64_t start_time_;
      KeyMap keys_;
      ProfilerMap profilers_;
      TypedRoot<LookupTable*> profile_;

    public:
      ProfilerCollection(STATE);
      ~ProfilerCollection();

      void add_profiler(VM* vm, Profiler* profiler);
      void remove_profiler(VM* vm, Profiler* profiler);

      LookupTable* results(STATE);
    };
  }
}

#endif
