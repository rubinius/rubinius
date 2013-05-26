#ifndef RBX_UNWIND_INFO_HPP
#define RBX_UNWIND_INFO_HPP

#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "prelude.hpp"

namespace rubinius {
  const int kMaxUnwindInfos = 4;

  enum UnwindType {
    cRescue = 0,
    cEnsure = 1
  };

  struct UnwindInfo {
    uint32_t target_ip;
    int stack_depth;
    UnwindType type;

    bool for_ensure() const {
      return type == cEnsure;
    }

    bool for_rescue() const {
      return type == cRescue;
    }
  };

  typedef std::vector<UnwindInfo> UnwindOverflow;
  class UnwindInfoSet {

    private:
    UnwindInfo unwinds_[kMaxUnwindInfos];
    UnwindOverflow* overflow_;
    int current_;

    void overflow_set_unwind_info(int i, uint32_t target_ip, int stack_depth, UnwindType type);
    void overflow_push(uint32_t target_ip, int stack_depth, UnwindType type);
    UnwindInfo overflow_pop();
    void overflow_drop();

    public:

    UnwindInfoSet()
      : unwinds_()
      , overflow_(NULL)
      , current_(0)
    {}

    ~UnwindInfoSet() {
      if(overflow_) {
        delete overflow_;
      }
    }

    UnwindInfoSet(const UnwindInfoSet& other);

    int has_unwinds() const {
      return current_ > 0;
    }

    void set_current(int current) {
      current_ = current;
    }

    void set_unwind_info(int i, uint32_t target_ip, int stack_depth, UnwindType type) {
      if(unlikely(i >= kMaxUnwindInfos)) {
        overflow_set_unwind_info(i, target_ip, stack_depth, type);
      } else {
        UnwindInfo& info = unwinds_[i];
        info.target_ip   = target_ip;
        info.stack_depth = stack_depth;
        info.type        = type;
      }
    }

    void push(uint32_t target_ip, int stack_depth, UnwindType type) {
      if(unlikely(current_ >= kMaxUnwindInfos - 1)) {
        overflow_push(target_ip, stack_depth, type);
      } else {
        UnwindInfo& info = unwinds_[current_++];
        info.target_ip   = target_ip;
        info.stack_depth = stack_depth;
        info.type        = type;
      }
    }

    UnwindInfo pop() {
      UnwindInfo info;
      if(unlikely(current_ >= kMaxUnwindInfos)) {
        info = overflow_pop();
      } else {
        info = unwinds_[--current_];
      }
      return info;
    }

    void drop() {
      if(unlikely(current_ >= kMaxUnwindInfos)) {
        overflow_drop();
      } else {
        --current_;
      }
    }

  };

}

#endif
