#ifndef RBX_PROFILER_HPP
#define RBX_PROFILER_HPP

#include "vm/config.h"
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

#ifndef RBX_HAVE_TR1_HASH
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
      kBlockJIT,
      kFinalizers,
      kScript
    };

    class Method;
    class Node;
    typedef std::tr1::unordered_map<Method*, Fixnum*> KeyMap;
    typedef std::tr1::unordered_map<method_id, Method*> MethodMap;

    class Node {
      int id_;
      int called_;
      uint64_t total_;
      Method*  method_;

      Node* sibling_;
      Node* first_sub_node_;

    public:
      Node(Method* method, int id)
        : id_(id)
        , called_(0)
        , total_(0)
        , method_(method)
        , sibling_(0)
        , first_sub_node_(0)
      { }

      int id() {
        return id_;
      }

      Method* method() {
        return method_;
      }

      uint64_t total() {
        return total_;
      }

      int called() {
        return called_;
      }

      Node* sub_nodes() {
        return first_sub_node_;
      }

      int count_sub_nodes() {
        int count = 0;
        Node* node = first_sub_node_;
        while(node) {
          ++count;
          node = node->sibling();
        }

        return count;
      }

      Node* sibling() {
        return sibling_;
      }

      void set_sibling(Node* node) {
        sibling_ = node;
      }

      void accumulate(uint64_t time) {
        total_ += time;
        called_++;
      }

      Node* find_sub_node(Profiler* profiler, Method* method);
    };

    class Method {
    private:
      method_id id_;
      Symbol*   name_;
      Symbol*   container_;
      Kind      kind_;
      Symbol*   file_;
      int       line_;
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

      Node* find_node(Method* method);
      Method* find_callee(method_id id, Symbol* container,
                          Symbol* name, Kind kind);

      Array* nodes(STATE, KeyMap& keys);
      void merge_nodes(STATE, KeyMap& keys, Array* nodes);
    };

    class Profiler;

    /** Created when a method is being called. Contains a timer that tracks
     * how much time is spent in the method. When the MethodEntry instance
     * goes out of scope, the destructor records the elapsed time and updates
     * the Method and Node objects.
     */
    class MethodEntry {
      VM*           state_;
      Method*       method_;
      Node*         node_;
      Node*         previous_;
      stats::Timer  timer_;

    public:
      MethodEntry(STATE, Dispatch& msg, Arguments& args);
      MethodEntry(STATE, Dispatch& msg, Arguments& args, CompiledMethod* cm, bool jit=false);
      MethodEntry(STATE, Symbol* name, Module* module, CompiledMethod* cm, bool jit=false);
      MethodEntry(STATE, Kind kind, CompiledMethod* cm=0);
      ~MethodEntry();

      void start();

      TEST_CLASS(TestProfiler);
    };

    class Profiler {
      MethodMap methods_;
      Node*     root_;
      Node*     current_;
      VM*       state_;
      int       nodes_;
      uint32_t  threshold_;

    public:
      Profiler(STATE);

      ~Profiler();

      Node* current() {
        return current_;
      }

      void set_current(Node* node) {
        current_ = node;
      }

      int next_node_id() {
        return nodes_++;
      }

      method_id create_id(CompiledMethod* cm, Symbol* container, Symbol* name, Kind kind);
      Method* find_method(CompiledMethod* cm, Symbol* container, Symbol* name, Kind kind);

      Symbol* module_name(Module* module);
      Method* enter_method(Dispatch&, Arguments& args, CompiledMethod* cm, bool jit);
      Method* enter_block(Symbol* name, Module* module, CompiledMethod* cm, bool jit);
      Method* get_method(CompiledMethod* cm, Symbol* name,
          Symbol* container, Kind kind);

      void results(LookupTable* profile, KeyMap& keys, uint64_t runtime);

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
