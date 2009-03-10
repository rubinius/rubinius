#ifndef RBX_PROFILER_HPP
#define RBX_PROFILER_HPP

#include "vm/vm.hpp"

#include <stdint.h>
#include <stdio.h>

#include <stack>
#include <tr1/unordered_map>
#include <map>
#include <iostream>

namespace rubinius {
  class VM;
  class Symbol;
  class Object;
  class LookupTable;
  class String;

  namespace profiler {

    enum Kind {
      kNormal,
      kSingleton,
      kBlock
    };

    class Key {
    private:
      Symbol* meth_;
      Object* container_;
      Kind    kind_;

    public:
      Key(Symbol* meth, Object* container, Kind kind = kNormal) :
        meth_(meth), container_(container), kind_(kind) { }

      bool operator==(const Key& other) const;
      size_t hash() const;
    };

    class Method;
    class Leaf {
    private:
      Method* method_;
      uint64_t total_time_;

    public:
      Leaf(Method* meth) : method_(meth), total_time_(0) { }

      Method* method() {
        return method_;
      }

      void add_total_time(uint64_t diff);

      uint64_t total_time() {
        return total_time_;
      }

      uint64_t total_time_in_ns();
    };

    typedef std::map<Method*, Leaf*> Leaves;

    class Method {
    private:
      uint64_t id_;
      Symbol*  meth_;
      Object*  container_;
      Kind     kind_;
      uint64_t total_time_;
      Leaves   leaves_;
      uint64_t called_times_;
      Symbol*  file_;
      int      line_;

    public:
      Method(uint64_t id, Symbol* meth, Object* container, Kind kind = kNormal):
        id_(id),
        meth_(meth),
        container_(container),
        kind_(kind),
        total_time_(0),
        called_times_(0),
        file_(0),
        line_(0)
      { }

      ~Method();

      uint64_t id() {
        return id_;
      }

      uint64_t called_times() {
        return called_times_;
      }

      void called() {
        called_times_++;
      }

      Object* container() {
        return container_;
      }

      Symbol* method() {
        return meth_;
      }

      Kind kind() {
        return kind_;
      }

      String* name(STATE);

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

      uint64_t total_time() {
        return total_time_;
      }

      uint64_t total_time_in_ns();

      void add_total_time(uint64_t diff) {
        total_time_ += diff;
      }

      Leaf* find_leaf(Method* meth);

      size_t number_of_leaves() {
        return leaves_.size();
      }

      Leaves::iterator leaves_begin() {
        return leaves_.begin();
      }

      Leaves::iterator leaves_end() {
        return leaves_.end();
      }
    };

    class Invocation {
    private:
      uint64_t start_time_;
      Leaf*    leaf_;

    public:
      Invocation(Leaf* meth) : leaf_(meth) { }
      void start();
      void stop();

      Leaf* leaf() {
        return leaf_;
      }
    };
  }
}

namespace std {
  namespace tr1 {
    template <>
      struct hash<rubinius::profiler::Key> {
        size_t operator()(const rubinius::profiler::Key& key) const {
          return key.hash();
        }
      };
  }
}

namespace rubinius {
    class CompiledMethod;
    class Message;

  namespace profiler {

    class Profiler {
      typedef std::tr1::unordered_map<Key, Method*> MethodMap;

    private:
      MethodMap methods_;
      std::stack<Invocation> running_;
      Method* top_;
      Method* current_;

    public:
      Profiler();
      ~Profiler();

      Symbol* module_name(Module* module);
      void enter_method(STATE, Message&, CompiledMethod*);
      void enter_primitive(STATE, Message&);
      // void enter_block(STATE, MethodContext*, CompiledMethod*);
      Method* record_method(STATE, CompiledMethod*, Symbol*, Object*, Kind kind = kNormal);
      void leave_method();

      size_t number_of_entries();
      Method* find_key(Key& key);
      size_t depth();

      LookupTable* results(STATE);

      Method* current_method() {
        return current_;
      }
    };
  }
}



#endif
