#include <rbxti.hpp>
#include <rbxti/atomic.hpp>
#include <rbx_config.h>

#include <iostream>
#ifdef RBX_HAVE_TR1
#include <tr1/unordered_map>
#define std_unordered_map std::tr1::unordered_map
#else
#include <unordered_map>
#define std_unordered_map std::unordered_map
#endif

using namespace rbxti;

#if defined(RBX_HAVE_TR1) && !defined(RBX_HAVE_TR1_HASH)
namespace std {
  namespace tr1 {
    template <>
      struct hash<r_mint> {
        size_t operator()(const r_mint id) const {
          return id;
        }
      };
  }
}
#endif

namespace coverage {

  typedef std_unordered_map<int, int> InstructionCoverageMap;
  typedef std_unordered_map<r_mint, InstructionCoverageMap*> CoverageMap;

  class Coverage {
    CoverageMap coverage_map_;
    rbxti::SpinLock lock_;

  public:
    Coverage() { }
    ~Coverage();

    void lock() {
      lock_.lock();
    }

    void unlock() {
      lock_.unlock();
    }

    void add(Env* env, r_mint id, int ip);
    robject results(Env* env);
  };

  Coverage::~Coverage() {
    for(CoverageMap::iterator i = coverage_map_.begin();
        i != coverage_map_.end();
        i++) {
      delete i->second;
    }
  }

  void Coverage::add(Env* env, r_mint id, int ip) {
    lock();

    InstructionCoverageMap *map;
    CoverageMap::iterator i = coverage_map_.find(id);

    if(i == coverage_map_.end()) {
      map = new InstructionCoverageMap;
      coverage_map_[id] = map;
    } else {
      map = i->second;
    }

    InstructionCoverageMap::iterator j = map->find(ip);

    if(j == map->end()) {
      (*map)[ip] = 1;
    } else {
      (*map)[ip] += 1;
    }

    unlock();
  }

  static void ccode_iterator(Env* env, rcompiled_code code, void* data) {
    rtable map = (rtable)data;
    r_mint id = env->method_id(code);

    bool found = false;
    robject obj = env->table_fetch(map, env->integer_new(id), &found);
    if(!found) return;

    rtable attr = env->cast_to_rtable(obj);
    if(!attr) return;

    env->table_store(attr, env->symbol("code"), code);
  }

  robject Coverage::results(Env* env) {
    rtable coverage_map = env->table_new();

    for(CoverageMap::iterator i = coverage_map_.begin();
        i != coverage_map_.end();
        i++) {
      InstructionCoverageMap* map = i->second;
      rtable method_map = env->table_new();

      for(InstructionCoverageMap::iterator j = map->begin();
          j != map->end();
          j++) {
        env->table_store(method_map,
                         env->integer_new(j->first),
                         env->integer_new(j->second));
      }

      rtable attr = env->table_new();
      env->table_store(attr, env->symbol("counts"), method_map);

      env->table_store(coverage_map, env->integer_new(i->first), attr);
    }

    env->find_all_compiled_code(ccode_iterator, (void*)coverage_map);

    return coverage_map;
  }

  namespace {
    void coverage_enable(Env* env) {
      Coverage* coverage = new Coverage;
      env->set_global_tool_data(coverage);
    }

    robject coverage_results(Env* env) {
      Coverage* coverage = reinterpret_cast<Coverage*>(env->global_tool_data());

      env->set_tool_at_ip(NULL);
      env->set_global_tool_data(NULL);

      robject results = coverage->results(env);
      delete coverage;

      return results;
    }

    void coverage_at_ip(Env* env, rmachine_code mcode, int ip) {
      Coverage* coverage = reinterpret_cast<Coverage*>(env->global_tool_data());

      if(!coverage) return;

      coverage->add(env, env->machine_code_id(mcode), ip);
    }
  }

  extern "C" int Tool_Init(Env* env) {
    env->set_tool_enable(coverage_enable);
    env->set_tool_results(coverage_results);

    env->set_tool_at_ip(coverage_at_ip);

    return 1;
  }
}
