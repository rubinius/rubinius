#ifndef RBX_RESOLVER_HPP
#define RBX_RESOLVER_HPP

namespace rubinius {
  class Executable;

  class MethodResolver {
  public:
    virtual bool resolve(STATE, Message& msg) = 0;
    virtual ~MethodResolver() { }
  };

  class HierarchyResolver : public MethodResolver {
  public:
    virtual bool resolve(STATE, Message& msg);
    virtual ~HierarchyResolver() { }
  };

  class GlobalCacheResolver : public HierarchyResolver {
  public:
    virtual bool resolve(STATE, Message& msg);
    virtual ~GlobalCacheResolver() { }
  };

  class SpecializedResolver : public GlobalCacheResolver {
  public:
    Module* klass;
    Module* mod;
    Executable* method;

    virtual bool resolve(STATE, Message& msg);
    virtual ~SpecializedResolver() { }
  };
}

#endif
