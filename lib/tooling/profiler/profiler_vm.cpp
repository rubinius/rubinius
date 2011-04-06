#include <rbxti.hpp>
#include <rbx_config.h>

#include <stdint.h>
#include <stdio.h>

#include <list>
#include <stack>
#include <tr1/unordered_map>
#include <map>
#include <iostream>

#include <time.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

typedef uint64_t method_id;

#ifndef RBX_HAVE_TR1_HASH
namespace std {
  namespace tr1 {
    template <>
      struct hash<method_id> {
        size_t operator()(const method_id id) const {
          return id;
        }
      };
  }
}
#endif

using namespace rbxti;

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

  class VM;
  class Profiler;
  class Method;
  class Node;
  typedef std::tr1::unordered_map<Method*, rinteger> KeyMap;
  typedef std::tr1::unordered_map<method_id, Method*> MethodMap;

  /* An accumulating increment timer. Keeps track of the maximum and minimum
   * intervals recorded. Calculates a cumulative moving average according to
   * the formula:
   *
   *               x(i+1) + i*CA(i)
   *    CA(i+1) = -----------------
   *                   i + 1
   *
   *  where:
   *
   *    CA(n) is the nth cumulative average
   *    x(n) is the nth measured value of x
   *    i is the number of measurements
   */
  class Timer {
  protected:

    uint64_t total_;
    uint64_t timings_;
    uint64_t max_;
    uint64_t min_;
    uint64_t last_;
    uint64_t start_;
    double   moving_average_;
    bool     started_;

  public:

    Timer()
      : total_(0)
      , timings_(0)
      , max_(0)
      , min_(0)
      , last_(0)
      , start_(0)
      , moving_average_(0.0)
      , started_(false)
    { }

    bool started() {
      return started_;
    }

    uint64_t total() {
      return total_;
    }

    uint64_t timings() {
      return timings_;
    }

    uint64_t max() {
      return max_;
    }

    uint64_t min() {
      return min_;
    }

    double moving_average() {
      return moving_average_;
    }

    void start(Env* env) {
      if(started_) return;

      started_ = true;
      start_ = env->time_current_ns();
    }

    void stop(Env* env) {
      if(!started_) return;

      started_ = false;

      last_ = env->time_current_ns() - start_;
      total_ += last_;

      if(min_ == 0 || min_ > last_) min_ = last_;
      if(max_ == 0 || max_ < last_) max_ = last_;

      moving_average_ = (last_ + timings_ * moving_average_) / (timings_ + 1);
      ++timings_;
    }
  };

  class StackTimer : public Timer {
    size_t   entered_;
    uint64_t count_;

  public:
    StackTimer()
      : entered_(0)
      , count_(0)
    { }

    uint64_t count() {
      return count_;
    }

    void start(Env* env) {
      ++entered_;
      Timer::start(env);
    }

    void stop(Env* env) {
      if(!started_) return;

      ++count_;
      if(--entered_ == 0) Timer::stop(env);
    }
  };

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
    rsymbol   name_;
    rsymbol   container_;
    Kind      kind_;
    rsymbol   file_;
    int       line_;
    uint64_t  total_;

  public:
    StackTimer  timer;

  public:
    Method(method_id id, rsymbol name, rsymbol container, Kind kind=kNormal)
      : id_(id)
      , name_(name)
      , container_(container)
      , kind_(kind)
      , file_(0)
      , line_(0)
      , total_(0)
    { }

    method_id id() {
      return id_;
    }

    rsymbol container() {
      return container_;
    }

    rsymbol name() {
      return name_;
    }

    Kind kind() {
      return kind_;
    }

    rstring to_s(Env* env);

    rsymbol file() {
      return file_;
    }

    int line() {
      return line_;
    }

    void set_position(rsymbol file, int line) {
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
    Method* find_callee(method_id id, rsymbol container,
                        rsymbol name, Kind kind);
  };

  class Profiler;

  /** Created when a method is being called. Contains a timer that tracks
   * how much time is spent in the method. When the MethodEntry instance
   * goes out of scope, the destructor records the elapsed time and updates
   * the Method and Node objects.
   */
  class MethodEntry {
    Method*       method_;
    Node*         node_;
    Node*         previous_;
    Timer  timer_;

  public:

    MethodEntry(Method* method)
      : method_(method)
      , node_(0)
      , previous_(0)
    {}

    void start(Profiler* profiler, Env* env);
    void stop(Profiler* profiler, Env* env);
  };

  class Profiler {
    MethodMap methods_;
    Node*     root_;
    Node*     current_;
    int       nodes_;
    uint32_t  threshold_;
    uint64_t  start_time_;
    uint64_t  end_time_;
    int       id_;
    bool      attached_;

  public:
    Profiler(Env* env);

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

    uint64_t start_time() {
      return start_time_;
    }

    uint64_t end_time() {
      return end_time_;
    }

    uint64_t runtime() {
      return end_time_ - start_time_;
    }

    void detach(Env* env) {
      end_time_ = env->time_current_ns();
      attached_ = false;
    }

    bool attached_p() {
      return attached_;
    }

    int id() {
      return id_;
    }

    method_id create_id(Env* env, rmethod cm, rsymbol container, rsymbol name, Kind kind);
    Method* find_method(Env* env, rmethod cm, rsymbol container, rsymbol name, Kind kind);


    Method* enter_method(Env* env, robject recv, rsymbol name, rmodule mod,
                         rmethod cm);
    Method* enter_block(Env* env, rsymbol name, rmodule module, rmethod cm);
    Method* get_method(Env* env, rmethod cm, rsymbol name,
                       rsymbol container, Kind kind);

    void results(Env* env, rtable profile, rtable nodes, rtable methods,
                         KeyMap& keys, uint64_t runtime);
  };

  rstring Method::to_s(Env* env) {
    std::stringstream ss;
    char data[1024];

    if(kind() == kScript) {
      ss << "script:";
      if(file_) {
        env->symbol_cstr(file_, data, 1024);
        ss << data;
      } else {
        ss << "--unknown-file--";
        ss << ":" << line_;
      }

      return env->string_new(ss.str().c_str());
    }

    if(!env->is_nil(container())) {
      env->symbol_cstr(container_, data, 1024);
      ss << data;
    } else {
      ss << "<anonymous>";
    }

    env->symbol_cstr(name(), data, 1024);

    switch(kind()) {
    case kNormal:
      ss << "#" << data;
      break;
    case kNormalJIT:
      ss << "#" << data << " <jit>";
      break;
    case kSingleton:
    case kYoungGC:
    case kMatureGC:
    case kFinalizers:
      ss << "." << data;
      break;
    case kSingletonJIT:
      ss << "." << data << " <jit>";
      break;
    case kBlock:
      ss << "::" << data << "<" << line_ << "> {}";
      break;
    case kBlockJIT:
      ss << "::" << data << " {" << line_ << "} <jit>";
      break;
    case kScript:
      // handled above, just here to make gcc happy.
      abort();
    }

    return env->string_new(ss.str().c_str());
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

  void MethodEntry::start(Profiler* profiler, Env* env) {
    previous_ = profiler->current();
    node_ = previous_->find_sub_node(profiler, method_);
    profiler->set_current(node_);

    method_->timer.start(env);
    timer_.start(env);
  }

  void MethodEntry::stop(Profiler* profiler, Env* env) {
    method_->timer.stop(env);
    timer_.stop(env);
    method_->accumulate(timer_.total());
    node_->accumulate(timer_.total());
    profiler->set_current(previous_);
  }

  Profiler::Profiler(Env* env)
    : root_(0)
    , current_(0)
    , nodes_(0)
    , threshold_((uint32_t)env->config_get_int("profiler.threshold"))
    , start_time_(env->time_current_ns())
    , end_time_(0)
    , id_(env->current_thread_id())
    , attached_(true)
  {
    root_ = current_ = new Node(0, next_node_id());
  }

  Method* Profiler::enter_block(Env* env, rsymbol name, rmodule module, rmethod cm) {
    return get_method(env, cm, name, env->module_name(module), kBlock);
  }

  Method* Profiler::enter_method(Env* env, robject recv, rsymbol name, rmodule mod,
                                 rmethod cm)
  {
    if(env->module_is_metaclass(mod)) {
      robject attached = env->metaclass_attached_instance(mod);

      rmodule as_module = env->cast_to_rmodule(attached);
      if(as_module) {
        return get_method(env, cm, name, env->module_name(as_module), kSingleton);
      } else {
        rstring str = env->to_s(recv);
        return get_method(env, cm, name, env->string_to_symbol(str), kSingleton);
      }
    } else {
      return get_method(env, cm, name, env->module_name(mod), kNormal);
    }
  }

  Method* Profiler::get_method(Env* env, rmethod cm, rsymbol name,
                               rsymbol container, Kind kind)
  {
    Method* method = find_method(env, cm, container, name, kind);

    if(!method->file() && cm && !env->is_nil(cm)) {
      method->set_position(env->method_file(cm), env->method_line(cm));
    }

    return method;
  }

  method_id Profiler::create_id(Env* env, rmethod cm, rsymbol name,
                                rsymbol container, Kind kind)
  {
    // If we have a CompiledMethod, use it's method id.
    if(cm && !env->is_nil(cm)) {
      r_mint i = env->method_id(cm);
      if(i) return i;
    }

    // | -- 32 bits of container -- | -- 29 bits of name -- | -- 2 bits of kind -- | 0

    uint32_t c = env->symbol_id(container) & 0xffffffff;
    uint32_t n = env->symbol_id(name)      & 0x1fffffff;
    uint32_t k = kind               & 0x3;

    return (((uint64_t)c) << 32) |
           (n << 3) |
           k << 1;
  }

  Method* Profiler::find_method(Env* env, rmethod cm, rsymbol container,
                                rsymbol name, Kind kind)
  {
    method_id id = create_id(env, cm, container, name, kind);

    Method* method;

    MethodMap::iterator iter = methods_.find(id);

    if(iter == methods_.end()) {
      method = new Method(id, name, container, kind);
      methods_[method->id()] = method;
    } else {
      method = iter->second;
    }

    return method;
  }

  typedef std::vector<Node*> WorkList;

  static rinteger make_key(Env* env, Method* meth, KeyMap& keys) {
    KeyMap::iterator iter = keys.find(meth);

    if(iter == keys.end()) {
      rinteger key = env->integer_new(keys.size());
      keys[meth] = key;
      return key;
    }

    return iter->second;
  }

  static rinteger add_method(Env* env, rtable methods, Method* meth,
                            KeyMap& keys)
  {
    rinteger key = make_key(env, meth, keys);

    // We already have the method, skip this.
    bool fetched = false;
    env->table_fetch(methods, key, &fetched);
    if(fetched) return env->cast_to_rinteger(key);

    rsymbol cumulative_sym = env->symbol("cumulative");
    rsymbol total_sym = env->symbol("total");
    rsymbol called_sym = env->symbol("called");

    rtable method = env->table_new();
    env->table_store(methods, key, method);

    env->table_store(method, env->symbol("name"), meth->to_s(env));
    env->table_store(method, cumulative_sym, env->integer_new(meth->timer.total()));
    env->table_store(method, total_sym, env->integer_new(meth->total()));
    env->table_store(method, called_sym, env->integer_new(meth->timer.count()));

    if(meth->file()) {
      if(env->is_nil(meth->file())) {
        env->table_store(method, env->symbol("file"), env->string_new("unknown file"));
      } else {
        env->table_store(method, env->symbol("file"),
                         env->symbol_to_string(meth->file()));
      }

      env->table_store(method, env->symbol("line"), env->integer_new(meth->line()));
    }

    return key;
  }

  static void add_node(Env* env, rtable nodes, rtable methods, Node* node,
                       WorkList& work, KeyMap& keys, uint32_t threshold)
  {
    // We haven't exited this method yet, so its stats won't be accurate
    if(node->method()->timer.started()) return;

    rinteger key = env->integer_new(node->id());

    rarray tbl = env->array_new(5);

    env->table_store(nodes, key, tbl);

    robject meth_key = add_method(env, methods, node->method(), keys);

    env->array_set(tbl, 0, meth_key);
    env->array_set(tbl, 1, env->integer_new(node->total()));
    env->array_set(tbl, 2, env->integer_new(node->called()));

    int count = node->count_sub_nodes();
    env->array_set(tbl, 3, env->integer_new(count));

    rarray ary = env->array_new(count);

    int idx = 0;

    Node* sub = node->sub_nodes();

    while(sub) {
      if(sub->total() >= threshold) {
        env->array_set(ary, idx++, env->integer_new(sub->id()));
        work.push_back(sub);
      }

      sub = sub->sibling();
    }

    env->array_set(tbl, 4, ary);
  }

  void Profiler::results(Env* env, rtable profile, rtable nodes, rtable methods,
                         KeyMap& keys, uint64_t runtime)
  {
    WorkList work;

    // If we haven't even gone for a total of longer than 10x the threshold,
    // just disable the threshold.
    if(runtime < 10 * threshold_) threshold_ = 0;

    env->table_store(profile, env->symbol("total_nodes"), env->integer_new(nodes_));

    rarray roots = env->array_new(root_->count_sub_nodes());
    env->table_store(profile, env->symbol("roots"), roots);

    int idx = 0;
    Node* sub = root_->sub_nodes();

    while(sub) {
      if(sub->total() >= threshold_) {
        env->array_set(roots, idx++, env->integer_new(sub->id()));
        work.push_back(sub);
      }

      sub = sub->sibling();
    }

    while(work.size() > 0) {
      Node* node = work.back();
      work.pop_back();

      add_node(env, nodes, methods, node, work, keys, threshold_);
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

  struct GlobalState {
    Profiler* main_profiler;
    std::list<Profiler*> profilers;

    GlobalState()
      : main_profiler(0)
    {}

    void add(Profiler* prof) {
      profilers.push_back(prof);
    }
  };

  static int cProfileToolID = -1;

  namespace {
    void tool_enable(Env* env) {
      GlobalState* st = new GlobalState;
      env->set_global_tool_data(st);

      Profiler* profiler = new Profiler(env);
      st->main_profiler = profiler;

      env->thread_tool_set_data(cProfileToolID, profiler);
      st->add(profiler);

      env->enable_thread_tooling();
    }

    void* tool_enter_method(Env* env, robject recv, rsymbol name, rmodule mod,
                                 rmethod cm)
    {
      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return 0;

      Method* method = profiler->enter_method(env, recv, name, mod, cm);
      MethodEntry* me = new MethodEntry(method);
      me->start(profiler, env);

      return me;
    }

    void tool_leave_entry(Env* env, void* tag) {
      MethodEntry* me = (MethodEntry*)tag;

      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return;

      me->stop(profiler, env);
      delete me;
    }

    void* tool_enter_block(Env* env, rsymbol name, rmodule module, rmethod cm) {
      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return 0;

      Method* method = profiler->enter_block(env, name, module, cm);
      MethodEntry* me = new MethodEntry(method);
      me->start(profiler, env);

      return me;
    }

    void* tool_enter_gc(Env* env, int level) {
      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return 0;

      rsymbol container = env->symbol("GC");
      rsymbol name;

      Kind kind;

      switch(level) {
      case GCYoung:
        kind = kYoungGC;
        name = env->symbol("collect_young");
        break;
      case GCMature:
        kind = kMatureGC;
        name = env->symbol("collect_mature");
        break;
      case GCFinalizer:
        kind = kFinalizers;
        name = env->symbol("run_finalizers");
        break;
      default:
        kind = kFinalizers;
        name = env->symbol("unknown");
        break;
      }

      Method* method = profiler->get_method(env, NULL, name, container, kind);
      MethodEntry* me = new MethodEntry(method);
      me->start(profiler, env);

      return me;
    }

    void* tool_enter_script(Env* env, rmethod cm) {
      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return 0;

      Kind kind = kScript;
      rsymbol container = env->symbol("unknown");
      rsymbol name = container;

      Method* method = profiler->get_method(env, cm, name, container, kind);
      MethodEntry* me = new MethodEntry(method);
      me->start(profiler, env);

      return me;
    }

    void tool_shutdown(Env* env) {
      GlobalState* st = (GlobalState*)env->global_tool_data();
      if(!st) return;

      env->set_global_tool_data(0);

      for(std::list<Profiler*>::iterator i = st->profilers.begin();
          i != st->profilers.end();
          ++i) {

        Profiler* prof = *i;
        if(!prof->attached_p()) delete prof;
      }

      delete st;
    }

    void tool_start_thread(Env* env) {
      GlobalState* st = (GlobalState*)env->global_tool_data();

      // No GlobalState means that the tool isn't currently enabled.
      if(!st) return;

      Profiler* profiler = new Profiler(env);
      st->add(profiler);

      env->thread_tool_set_data(cProfileToolID, profiler);

      env->enable_thread_tooling();
    }

    void tool_stop_thread(Env* env) {
      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);
      if(!profiler) return;

      env->thread_tool_set_data(cProfileToolID, 0);
      profiler->detach(env);

      env->disable_thread_tooling();
    }

    robject tool_results(Env* env) {
      GlobalState* st = (GlobalState*)env->global_tool_data();

      Profiler* profiler = (Profiler*)env->thread_tool_data(cProfileToolID);

      // Ignore results requests that don't come from the thread that
      // started profiling.
      if(st->main_profiler != profiler) return env->nil();

      profiler->detach(env);

      env->thread_tool_set_data(cProfileToolID, 0);

      rtable profile = env->table_new();

      for(std::list<Profiler*>::iterator i = st->profilers.begin();
          i != st->profilers.end();
          ++i) {

        Profiler* prof = *i;

        rtable thread = env->table_new();

        env->table_store(profile, env->integer_new(prof->id()), thread);

        rtable methods = env->table_new();
        rtable nodes = env->table_new();

        env->table_store(thread, env->symbol("methods"), methods);
        env->table_store(thread, env->symbol("nodes"),   nodes);

        uint64_t runtime = profiler->runtime();
        env->table_store(thread, env->symbol("runtime"), env->integer_new(runtime));

        KeyMap keys;
        prof->results(env, thread, nodes, methods, keys, runtime);
      }

      tool_shutdown(env);

      env->disable_thread_tooling();
      return profile;
    }

  }

  extern "C" int Tool_Init(Env* env) {
    env->config_set("tool.require", "tooling/profiler/profiler.rb");

    cProfileToolID = env->thread_tool_new_id();

    env->set_tool_results(tool_results);
    env->set_tool_enable(tool_enable);

    env->set_tool_enter_method(tool_enter_method);
    env->set_tool_leave_method(tool_leave_entry);

    env->set_tool_enter_block(tool_enter_block);
    env->set_tool_leave_block(tool_leave_entry);

    env->set_tool_enter_gc(tool_enter_gc);
    env->set_tool_leave_gc(tool_leave_entry);

    env->set_tool_enter_script(tool_enter_script);
    env->set_tool_leave_script(tool_leave_entry);

    env->set_tool_shutdown(tool_shutdown);

    env->set_tool_thread_start(tool_start_thread);
    env->set_tool_thread_stop(tool_stop_thread);

    return 1;
  }

}
