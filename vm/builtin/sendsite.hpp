#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class CompiledMethod;
  class Selector;
  class Message;
  class SendSite;

  typedef bool (*MethodResolver)(STATE, Message& msg);

  class SendSite : public Object {
  public:
    static const size_t fields = 10;
    static const object_type type = SendSiteType;

    typedef bool (*Performer)(STATE, Task* task, Message& msg);

  private:
    SYMBOL name_;            // slot
    CompiledMethod* sender_; // slot
    Selector* selector_;     // slot
    Executable* method_;     // slot
    Module* module_;         // slot
    Module* recv_class_;     // slot

  public:
    // TODO: fix up data members that aren't slots
    bool   method_missing;
    size_t hits;
    size_t misses;
    MethodResolver resolver;
    Performer performer;

  public:
    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(sender, CompiledMethod);
    attr_accessor(selector, Selector);
    attr_accessor(method, Executable);
    attr_accessor(module, Module);
    attr_accessor(recv_class, Module);

    /* interface */

    static void init(STATE);

    // Ruby.primitive :sendsite_create
    static SendSite* create(STATE, OBJECT name);

    // Ruby.primitive :sendsite_set_sender
    OBJECT set_sender(STATE, CompiledMethod* cm);

    // Ruby.primitive :sendsite_hits
    OBJECT hits_prim(STATE);

    // Ruby.primitive :sendsite_misses
    OBJECT misses_prim(STATE);

    void initialize(STATE);
    bool locate(STATE, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };

  /**
   *  Standard Ruby method lookup path.
   *
   *  The normal, slow method lookup path. Given the Module in which
   *  to start looking and the method name, the lookup progresses up
   *  the inheritance tree until the method is located or no more
   *  method tables exist. If the method was found, it is verified
   *  that the method is visible (see MethodVisibility) for this
   *  call. If everything checks out, the Executable object representing
   *  the method and the Module in which it was found are stored in
   *  the Message.
   *
   *  @returns  true if method is found, false otherwise.
   */
  class HierarchyResolver {
  public:
    static bool resolve(STATE, Message& msg);
  };

  /**
   *  Cached method lookup path.
   *
   *  The method is first attempted to be located in the globally
   *  which the lookup *starts* as the key. If the method is found
   *  in the cache, the cached information is used. If not, the
   *  fallback is to look it up using HierarchyResolver. If the
   *  slower path locates the method, it is stored in the cache.
   *  Either way, the Executable object and the Module in which
   *  the Executable was originally located are stored in the
   *  Message.
   *
   *  @note     the cache is indexed using the name and Module where
   *            the lookup *started*. The information stored includes
   *            the Module in which the Executable was *found*.
   *
   *  @returns  true if the method was found, false otherwise.
   */
  class GlobalCacheResolver {
  public:
    static bool resolve(STATE, Message& msg);
  };

  /**
   *  Monomorphic inline method lookup.
   *
   *  First checks if the cached receiver class matchs msg.lookup_from.
   *  If so, set the +msg+ method and module to the cached ones. If not,
   *  invoke the GlobalCacheResolver::resolve method. If the method is
   *  found, cache it in +ss+.
   *
   *  @returns true if the method was found, false otherwise.
   */
  class MonomorphicInlineCacheResolver {
  public:
    static bool resolve(STATE, Message& msg);
  };
};

#endif
