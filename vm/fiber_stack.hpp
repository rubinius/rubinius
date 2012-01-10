#ifndef RBX_VM_FIBER_STACK_HPP
#define RBX_VM_FIBER_STACK_HPP

namespace rubinius {
  class FiberData;

  class FiberStack {
    void* address_;
    size_t size_;
    int refs_;
    FiberData* user_;

  public:
    FiberStack(size_t size);

    void* address() {
      return address_;
    }

    void* top_address() {
      return (void*)((char*)address_ + size_);
    }

    size_t size() {
      return size_;
    }

    void inc_ref() {
      refs_++;
    }

    void dec_ref() {
      refs_--;
    }

    int refs() {
      return refs_;
    }

    bool unused_p() {
      return refs_ == 0;
    }

    bool shared_p() {
      return user_ && refs_ > 1;
    }

    FiberData* user() {
      return user_;
    }

    void set_user(FiberData* d) {
      user_ = d;
    }

    void allocate();
    void free();
    void flush(STATE);
    void orphan(STATE, FiberData* user);
  };

  class FiberStacks {
    typedef std::list<FiberStack> Stacks;
    const static size_t cStackSize = 128 * 1024;
    const static size_t cMaxStacks = 1;
    const static size_t cTrampolineSize = 4096;

    Stacks stacks_;
    void* trampoline_;

  public:
    FiberStacks()
      : trampoline_(0)
    {}

    FiberStack* allocate();

    void* trampoline();
  };
}

#endif
