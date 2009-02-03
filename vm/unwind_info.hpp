#ifndef RBX_UNWIND_INFO_HPP
#define RBX_UNWIND_INFO_HPP

namespace rubinius {
  // TODO figure out if this is a good number
  const int kMaxUnwindInfos = 20;

  struct UnwindInfo {
    uint32_t target_ip;
    uint32_t stack_depth;
  };
}

#endif
