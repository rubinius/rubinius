namespace rubinius {
namespace jit {
  class Context {
    LLVMState* ls_;
    JITMethodInfo* root_info_;
    bool inlined_block_;

  public:
    Context(LLVMState* ls)
      : ls_(ls)
      , root_info_(0)
      , inlined_block_(false)
    {}

    LLVMState* state() {
      return ls_;
    }

    void set_root(JITMethodInfo* info) {
      root_info_ = info;
    }

    JITMethodInfo* root() {
      return root_info_;
    }

    void set_inlined_block(bool val=true) {
      inlined_block_ = val;
    }

    bool inlined_block() {
      return inlined_block_;
    }
  };
}
}
