#ifndef RBX_C_API_HPP
#define RBX_C_API_HPP

#include "memory.hpp"
#include "spinlock.hpp"

#include "class/native_method.hpp"

#include "capi/capi_constants.h"

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rubinius {
  class C_API {
  public:
    typedef std::unordered_set<std::string> CApiBlackList;
    typedef std::vector<std::mutex*> CApiLocks;
    typedef std::unordered_map<std::string, int> CApiLockMap;

    typedef std::vector<std::string> CApiConstantNameMap;
    typedef std::unordered_map<int, MemoryHandle*> CApiConstantHandleMap;

  private:
    CApiConstantNameMap capi_constant_name_map_;
    CApiConstantHandleMap capi_constant_map_;
    CApiBlackList capi_black_list_;
    CApiLocks capi_locks_;
    CApiLockMap capi_lock_map_;

    bool use_capi_lock_;

    locks::spinlock_mutex capi_ds_lock_;
    locks::spinlock_mutex capi_locks_lock_;
    locks::spinlock_mutex capi_constant_lock_;

  public:
    C_API();
    virtual ~C_API() { }

    void after_fork_child(STATE);

    void set_use_capi_lock(bool s) {
      use_capi_lock_ = s;
    }

    locks::spinlock_mutex& capi_ds_lock() {
      return capi_ds_lock_;
    }

    locks::spinlock_mutex& capi_constant_lock() {
      return capi_constant_lock_;
    }

    int capi_lock_index(std::string name);

    void enter_capi(STATE, const char* file, int line);
    void leave_capi(STATE);

    void setup_capi_constant_names();
    CApiConstantNameMap& capi_constant_name_map() {
      return capi_constant_name_map_;
    }

    CApiConstantHandleMap& capi_constant_map() {
      return capi_constant_map_;
    }

    void initialize_capi_black_list();
  };
}

#endif
