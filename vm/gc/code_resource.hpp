#ifndef RBX_CODE_RESOURCE
#define RBX_CODE_RESOURCE

namespace rubinius {
  class CodeManager;

  class CodeResource {
    int mark_;

  public:
    virtual ~CodeResource() { };
    virtual void cleanup(CodeManager* cm) { };
    virtual int  size() { return 0; }

    int mark() {
      return mark_ ;
    }

    void set_mark(int m=1) {
      mark_ = m;
    }
  };
}

#endif
