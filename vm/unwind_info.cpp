#include "unwind_info.hpp"

namespace rubinius {

  UnwindInfoSet::UnwindInfoSet(const UnwindInfoSet& other) {
    for(int i = 0; i < kMaxUnwindInfos; ++i) {
      unwinds_[i] = other.unwinds_[i];
    }
    if(other.overflow_) {
      overflow_ = new UnwindOverflow(*other.overflow_);
    } else {
      overflow_ = NULL;
    }
    current_ = other.current_;
  }

  void UnwindInfoSet::overflow_set_unwind_info(int i, uint32_t target_ip, int stack_depth, UnwindType type) {
    if(!overflow_) {
      overflow_ = new UnwindOverflow();
    }

    UnwindInfo info;
    info.target_ip   = target_ip;
    info.stack_depth = stack_depth;
    info.type        = type;
    (*overflow_)[i - kMaxUnwindInfos] = info;
  }

  void UnwindInfoSet::overflow_push(uint32_t target_ip, int stack_depth, UnwindType type) {
    if(!overflow_) {
      overflow_ = new UnwindOverflow();
    }
    current_++;
    UnwindInfo info;
    info.target_ip   = target_ip;
    info.stack_depth = stack_depth;
    info.type        = type;
    overflow_->push_back(info);
  }

  UnwindInfo UnwindInfoSet::overflow_pop() {
    current_--;
    UnwindInfo info = overflow_->back();
    overflow_->pop_back();
    return info;
  }

  void UnwindInfoSet::overflow_drop() {
    current_--;
    overflow_->pop_back();
  }
}
