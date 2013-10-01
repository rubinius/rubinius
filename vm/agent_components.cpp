#include "missing/unordered_map.hpp"

#include "config.h"

#include "util/bert.hpp"
#include "agent_components.hpp"

#include "object_memory.hpp"

#include "builtin/thread.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"

#include "windows_compat.h"

namespace rubinius {
namespace agent {
  class Item {
  public:
    enum Kind {
      eTree,
      eStaticVariable,
      eDynamicVariable
    };

  private:
    Kind kind_;
    const char* name_;

  public:
    Item(Kind k, const char* name)
      : kind_(k)
      , name_(strdup(name))
    {}

    virtual ~Item() {
      free(const_cast<char*>(name_));
    }

    Kind kind() const {
      return kind_;
    }

    bool tree_p() const {
      return kind_ == eTree;
    }

    const char* name() const {
      return name_;
    }

    virtual void read(Output& output) {
      output.error("unimplemented");
    }

    virtual bool read_path(Output& output, const char* ipath) {
      return false;
    }

    virtual void set(Output& output, bert::Value* val) {
      output.error("unimplemented");
    }

    virtual bool set_path(Output& output, const char* path, bert::Value* val) {
      return false;
    }
  };

  typedef std_unordered_map<std::string, Item*> NamedItems;

  class DynamicVariable : public Item {
  public:
    DynamicVariable(const char* name)
      : Item(Item::eDynamicVariable, name)
    {}
  };

  class Tree : public Item {
    NamedItems hash_;

  public:
    Tree(const char* name)
      : Item(Item::eTree, name)
    {}

    virtual ~Tree() {
      for(NamedItems::iterator i = hash_.begin();
          i != hash_.end();
          ++i) {
        delete i->second;
      }
    }

    Tree* get_tree(const char* name) {
      NamedItems::iterator i = hash_.find(name);
      if(i == hash_.end()) {
        Tree* t = new Tree(name);
        hash_[t->name()] = t;
        return t;
      }

      if(i->second->tree_p()) {
        return reinterpret_cast<Tree*>(i->second);
      }

      rubinius::bug("failed to create or find tree");
      return 0;
    }

    void add(Item* item) {
      hash_[item->name()] = item;
    }

    Item* get(const char* name) {
      NamedItems::iterator i = hash_.find(name);
      if(i == hash_.end()) {
        return 0;
      }

      return i->second;
    }

    virtual void read(Output& output) {
      output.ok("tree");
      output.e().write_tuple(hash_.size());

      for(NamedItems::iterator i = hash_.begin();
          i != hash_.end();
          ++i) {
        Item* item = i->second;
        output.e().write_binary(item->name());
      }
    }

    virtual bool read_path(Output& output, const char* ipath) {
      char* path = strdup(ipath);
      char* pos = strchr(path, '.');

      if(pos) *pos = 0;

      Item* item = get(path);

      bool ok;

      if(item) {
        if(pos) {
          ok = item->read_path(output, pos+1);
        } else {
          item->read(output);
          ok = true;
        }
      } else {
        ok = false;
      }

      free(path);

      return ok;
    }

    virtual bool set_path(Output& output, const char* ipath, bert::Value* val) {
      char* path = strdup(ipath);
      char* pos = strchr(path, '.');

      if(pos) *pos = 0;

      Item* item = get(path);

      bool ok;

      if(item) {
        if(pos) {
          ok = item->set_path(output, pos+1, val);
        } else {
          item->set(output, val);
          ok = true;
        }
      } else {
        ok = false;
      }

      free(path);

      return ok;
    }
  };

  class SystemName : public DynamicVariable {
  public:
    SystemName(const char* name)
      : DynamicVariable(name)
    {}

    virtual void read(Output& output) {
      struct utsname name;
      if(uname(&name)) {
        output.error("unknown");
      } else {
        output.ok("list");
        output.e().write_tuple(5);
        output.e().write_binary(name.sysname);
        output.e().write_binary(name.nodename);
        output.e().write_binary(name.release);
        output.e().write_binary(name.version);
        output.e().write_binary(name.machine);
      }
    }
  };

  class ProcessId : public DynamicVariable {
  public:
    ProcessId(const char* name)
      : DynamicVariable(name)
    {}

    virtual void read(Output& output) {
      pid_t pid = getpid();
      output.ok("value");
      output.e().write_tuple(1);
      output.e().write_integer(pid);
    }
  };

  class VMConfig : public DynamicVariable {
    State* state_;

  public:
    VMConfig(State* state, const char* name)
      : DynamicVariable(name)
      , state_(state)
    {}

    virtual void read(Output& output) {
      output.ok("list");
      output.e().write_tuple(state_->shared().config.items_size());

      for(config::Items::iterator i = state_->shared().config.items_begin();
          i != state_->shared().config.items_end();
          ++i) {
        config::ConfigItem* item = *i;
        output.e().write_binary(item->name());
      }
    }

    virtual bool read_path(Output& output, const char* path) {
      if(config::ConfigItem* item = state_->shared().config.find(path)) {
        output.ok("value");

        std::ostringstream ss;
        item->print_value(ss);

        if(item->integer_p()) {
          config::Integer* i = static_cast<config::Integer*>(item);
          output.e().write_integer(i->value);
        } else {
          output.e().write_binary(ss.str().c_str());
        }
      } else {
        output.error("unknown key");
      }
      return true;
    }

    virtual bool set_path(Output& output, const char* path, bert::Value* val) {
      if(val->string_p()) {
        output.ok("value");
        if(state_->shared().config.import(path, val->string())) {
          output.e().write_atom("ok");
        } else {
          output.e().write_atom("unknown_key");
        }
      } else {
        output.error("format");
      }

      return true;
    }
  };

  class Backtrace : public DynamicVariable {
    State* state_;

  public:
    Backtrace(State* state, const char* name)
      : DynamicVariable(name)
      , state_(state)
    {}

    virtual void read(Output& output) {
      state_->shared().stop_threads_externally();

      std::list<ManagedThread*>* threads = state_->shared().threads();

      output.ok("value");

      output.e().write_tuple(threads->size());

      for(std::list<ManagedThread*>::iterator i = threads->begin();
          i != threads->end();
          ++i) {
        if(VM* vm = (*i)->as_vm()) {
          std::ostringstream ss;
          vm->saved_call_frame()->print_backtrace(state_, ss);
          output.e().write_binary(ss.str().c_str());
        } else {
          output.e().write_binary("");
        }
      }

      state_->shared().restart_threads_externally();
    }
  };

  class ThreadBacktrace : public DynamicVariable {
    State* state_;

  public:
    ThreadBacktrace(State* state, const char* name)
      : DynamicVariable(name)
      , state_(state)
    {}

    virtual void read(Output& output) {
      state_->shared().stop_threads_externally();

      output.ok("list");

      std::list<ManagedThread*>* thrs = state_->shared().threads();

      output.e().write_tuple(thrs->size());

      for(std::list<ManagedThread*>::iterator i = thrs->begin();
          i != thrs->end();
          ++i) {
        ManagedThread* thr = *i;

        if(VM* vm = thr->as_vm()) {
          Thread* ruby_thread = vm->thread.get();
          if(ruby_thread->system_thread()) {
            output.e().write_tuple(2);
            output.e().write_atom("system");
            output.e().write_binary(thr->name().c_str());
          } else {
            std::ostringstream ss;
            vm->saved_call_frame()->print_backtrace(state_, ss);

            output.e().write_tuple(3);
            output.e().write_atom("user");
            output.e().write_atom(CBOOL(vm->thread->sleep()) ? "sleep" : "run");
            output.e().write_binary(ss.str().c_str());
          }
        } else {
          output.e().write_tuple(2);
          output.e().write_atom("system");
          output.e().write_binary(thr->name().c_str());
        }
      }

      state_->shared().restart_threads_externally();
    }
  };

  class ThreadCount : public DynamicVariable {
    State* state_;

  public:
    ThreadCount(State* state, const char* name)
      : DynamicVariable(name)
      , state_(state)
    {}

    virtual void read(Output& output) {
      state_->shared().stop_threads_externally();

      output.ok("value");

      std::list<ManagedThread*>* thrs = state_->shared().threads();

      output.e().write_integer(thrs->size());

      state_->shared().restart_threads_externally();
    }
  };
  class DumpHeap: public DynamicVariable {
    State* state_;

  public:
    DumpHeap(State* state, const char* name)
      : DynamicVariable(name)
      , state_(state)
    {}

    virtual void set(Output& output, bert::Value* val) {
      state_->vm()->shared.stop_threads_externally();

      if(val->string_p()) {
        output.ok("value");
        String* path = String::create(state_, val->string());
        if(CBOOL(System::vm_dump_heap(state_, path))) {
          output.e().write_atom("ok");
        } else {
          output.e().write_atom("error");
        }
      } else {
        output.error("format");
      }

      state_->vm()->shared.restart_threads_externally();
    }
  };


  template <typename T>
  class ReadInteger : public DynamicVariable {
    T* ptr_;

  public:
    ReadInteger(const char* name, T* ptr)
      : DynamicVariable(name)
      , ptr_(ptr)
    {}

    virtual void read(Output& output) {
      output.ok("value");
      output.e().write_integer(*ptr_);
    }
  };

  class ReadAtomicInteger : public DynamicVariable {
    atomic::integer& ai_;

  public:
    ReadAtomicInteger(const char* name, atomic::integer& ai)
      : DynamicVariable(name)
      , ai_(ai)
    {}

    virtual void read(Output& output) {
      output.ok("value");
      output.e().write_integer(ai_.read());
    }
  };

  class ReadDouble : public DynamicVariable {
    double* ptr_;

  public:
    ReadDouble(const char* name, double* ptr)
      : DynamicVariable(name)
      , ptr_(ptr)
    {}

    virtual void read(Output& output) {
      output.ok("value");
      output.e().write_float(*ptr_);
    }
  };

  template <typename T>
  class StaticInteger : public DynamicVariable {
    T val_;

  public:
    StaticInteger(const char* name, T val)
      : DynamicVariable(name)
      , val_(val)
    {}

    virtual void read(Output& output) {
      output.ok("value");
      output.e().write_integer(val_);
    }
  };

  VariableAccess::VariableAccess(State* state)
    : root_(new Tree(""))
  {
    root_->add(new StaticInteger<int>("version", 2));
    root_->add(new VMConfig(state, "config"));

    system_ = root_->get_tree("system");
    system_->add(new SystemName("name"));
    system_->add(new ProcessId("pid"));
    system_->add(new Backtrace(state, "backtrace"));

    Tree* mem = system_->get_tree("memory");

    mem->add(new DumpHeap(state, "dump"));

    Tree* young = mem->get_tree("young");
    young->add(new ReadInteger<size_t>("bytes", &state->memory()->young_usage()));

    Tree* mature = mem->get_tree("mature");
    mature->add(new ReadInteger<size_t>("bytes", &state->memory()->immix_usage()));

    Tree* large = mem->get_tree("large");
    large->add(new ReadInteger<size_t>("bytes", &state->memory()->loe_usage()));

    Tree* code = mem->get_tree("code");
    code->add(new ReadInteger<size_t>("bytes", &state->memory()->code_usage()));

    Tree* symbols = mem->get_tree("symbols");
    symbols->add(new ReadInteger<size_t>("bytes", &state->shared().symbols.bytes_used()));

    Tree* counter = mem->get_tree("counter");
    counter->add(new ReadAtomicInteger("young_objects",
                       state->memory()->gc_stats.young_objects_allocated));
    counter->add(new ReadAtomicInteger("young_bytes",
                       state->memory()->gc_stats.young_bytes_allocated));
    counter->add(new ReadAtomicInteger("promoted_objects",
                       state->memory()->gc_stats.promoted_objects_allocated));
    counter->add(new ReadAtomicInteger("promoted_bytes",
                       state->memory()->gc_stats.promoted_bytes_allocated));
    counter->add(new ReadAtomicInteger("mature_objects",
                       state->memory()->gc_stats.mature_objects_allocated));
    counter->add(new ReadAtomicInteger("mature_bytes",
                       state->memory()->gc_stats.mature_bytes_allocated));

    Tree* gc_young = system_->get_tree("gc")->get_tree("young");

    gc_young->add(new ReadAtomicInteger("count",
                       state->memory()->gc_stats.young_collection_count));
    gc_young->add(new ReadAtomicInteger("total_wallclock",
                       state->memory()->gc_stats.total_young_collection_time));
    gc_young->add(new ReadAtomicInteger("last_wallclock",
                       state->memory()->gc_stats.last_young_collection_time));

    Tree* gc_full = system_->get_tree("gc")->get_tree("full");
    gc_full->add(new ReadAtomicInteger("count",
                       state->memory()->gc_stats.full_collection_count));
    gc_full->add(new ReadAtomicInteger("total_stop_wallclock",
                       state->memory()->gc_stats.total_full_stop_collection_time));
    gc_full->add(new ReadAtomicInteger("total_concurrent_wallclock",
                       state->memory()->gc_stats.total_full_concurrent_collection_time));
    gc_full->add(new ReadAtomicInteger("last_stop_wallclock",
                       state->memory()->gc_stats.last_full_stop_collection_time));
    gc_full->add(new ReadAtomicInteger("last_concurrent_wallclock",
                       state->memory()->gc_stats.last_full_concurrent_collection_time));

    Tree* jit = system_->get_tree("jit");
    jit->add(new ReadAtomicInteger("cache_resets", state->shared().stats.methods_cache_resets));
    jit->add(new ReadInteger<int>("global_serial", state->shared().global_serial_address()));
    jit->add(new ReadAtomicInteger("methods", state->shared().stats.jitted_methods));
    jit->add(new ReadAtomicInteger("time", state->shared().stats.jit_time_spent));

    Tree* threads = system_->get_tree("threads");
    threads->add(new ThreadBacktrace(state, "backtrace"));
    threads->add(new ThreadCount(state, "count"));
  }

  VariableAccess::~VariableAccess() {
    delete root_;
  }

  bool VariableAccess::read_path(Output& output, const char* ipath) {
    if(strlen(ipath) == 1 && ipath[0] == '.') {
      root_->read(output);
    } else {
      return root_->read_path(output, ipath);
    }
    return true;
  }

  bool VariableAccess::set_path(Output& output, const char* ipath, bert::Value* val) {
    if(strlen(ipath) == 1 && ipath[0] == '.') {
      root_->set(output, val);
    } else {
      return root_->set_path(output, ipath, val);
    }
    return true;
  }
}}
