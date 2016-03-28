#ifndef VM_LLVM_BASIC_BLOCK_HPP
#define VM_LLVM_BASIC_BLOCK_HPP

#include "jit/llvm/state.hpp"

namespace rubinius {
  struct JITBasicBlock {
    llvm::BasicBlock* block;
    llvm::BasicBlock* prologue;
    JITBasicBlock* exception_handler;
    int sp;
    int start_ip;
    int end_ip;
    bool reachable;
    bool landing_pad;
    int exception_type;

    LocalMap local_info_;

  public:
    JITBasicBlock()
      : block(0)
      , prologue(0)
      , exception_handler(0)
      , sp(-1)
      , start_ip(0)
      , end_ip(0)
      , reachable(false)
      , landing_pad(false)
      , exception_type(-1)
    {}

    llvm::BasicBlock* entry() {
      if(prologue) return prologue;
      return block;
    }

    void add_local(int which, type::KnownType kt) {
      LocalInfo li(which);
      li.set_known_type(kt);

      local_info_[which] = li;
    }

    LocalInfo* get_local(int which) {
      LocalMap::iterator i = local_info_.find(which);
      if(i == local_info_.end()) return 0;
      return &i->second;
    }
  };

  typedef std::map<int, JITBasicBlock> BlockMap;
}

#endif
