#ifndef RBX_UNWIND_INFO_HPP
#define RBX_UNWIND_INFO_HPP

#include <stdint.h>

namespace rubinius {
  // TODO figure out if this is a good number
  const int kMaxUnwindInfos = 20;

  enum UnwindType {
    cRescue = 0,
    cEnsure = 1
  };

  struct UnwindInfo {
    uint32_t target_ip;
    int stack_depth;
    UnwindType type;

    bool for_ensure() {
      return type == cEnsure;
    }

    bool for_rescue() {
      return type == cRescue;
    }
  };
}

#endif
