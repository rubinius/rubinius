#ifndef RBX_GC_MANAGED_THREAD
#define RBX_GC_MANAGED_THREAD

namespace rubinius {
  class SharedState;
  class VM;

  class ManagedThread {
  public:
    enum Kind {
      eRuby, eSystem
    };

  private:
    SharedState& shared_;
    Roots roots_;
    Kind kind_;
    const char* name_;

  public:
    ManagedThread(SharedState& ss, Kind kind)
      : shared_(ss)
      , kind_(kind)
      , name_(kind == eRuby ? "<ruby>" : "<system>")
    {}

    Roots& roots() {
      return roots_;
    }

    Kind kind() {
      return kind_;
    }

    VM* as_vm() {
      if(kind_ == eRuby) return reinterpret_cast<VM*>(this);
      return 0;
    }

    const char* name() {
      return name_;
    }

    void set_name(const char* name) {
      name_ = name;
    }
  };
}

#endif
