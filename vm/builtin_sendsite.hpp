#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "objects.hpp"
#include "message.hpp"

namespace rubinius {
  class Selector;

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
    OBJECT  method;

    virtual bool resolve(STATE, Message& msg);
    virtual ~SpecializedResolver() { }
  };

  class SendSite : public BuiltinType {
    public:

    static const size_t object_fields = 6;
    static const object_type type = SendSiteType;

    SYMBOL name;
    OBJECT sender;
    Selector* selector;
    size_t hits, misses;
    bool specialized;

    MethodResolver* resolver;

    static void init(STATE);
    static SendSite* create(STATE, OBJECT name);
    void initialize(STATE);
    void set_sender(STATE, OBJECT cm);
    bool basic_p(STATE);
    bool locate(STATE, Message& msg);
  };
};

#endif
