#ifndef RBX_CODE_RESOURCE
#define RBX_CODE_RESOURCE

namespace rubinius {
  class CodeManager;
  class VM;
  class State;

  class CodeResource {
    bool mark_;
    bool detached_;

  public:
    CodeResource()
      : mark_(false)
      , detached_(false)
    {}

    virtual ~CodeResource() { };
    virtual void cleanup(State* state, CodeManager* cm) = 0;
    virtual int  size() { return 0; }
    virtual const char* kind() { return "resource"; }

    bool marked() {
      return mark_ ;
    }

    void set_mark() {
      mark_ = true;
    }

    void clear_mark() {
      mark_ = false;
    }

    bool detached() {
      return detached_;
    }

    void set_detached() {
      detached_ = true;
    }

  };
}

#endif
