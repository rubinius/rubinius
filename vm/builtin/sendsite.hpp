#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "dispatch.hpp"

namespace rubinius {
  class CompiledMethod;
  class Selector;
  class Message;
  class SendSite;
  class MethodContext;
  class LookupData;

  typedef bool (*MethodResolver)(STATE, Message& msg);

  class SendSite : public Object {
  public:
    static const object_type type = SendSiteType;

    typedef Object* (*Performer)(STATE, SendSite* ss, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    // A class thats C heap allocated and stores a bunch of SendSite data,
    // to improve the speed of method dispatch.
    struct Internal : public Dispatch {
      size_t literal;
      Module* recv_class;
      executor execute;

      Internal(size_t lit)
        : literal(lit)
        , recv_class(0)
        , execute(0)
      {}
    };

    static bool fill(STATE, Module* klass, CallFrame* call_frame, Internal* cache, bool priv,
                     Module* lookup = 0);

  private:
    Symbol* name_;           // slot
    CompiledMethod* sender_; // slot
    Selector* selector_;     // slot
    Executable* method_;     // slot
    Module* module_;         // slot
    Module* recv_class_;     // slot

  public:
    Internal* inner_cache_;

    // @todo fix up data members that aren't slots
    bool   method_missing;
    size_t hits;
    size_t misses;
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
    static SendSite* create(STATE, Object* name);

    // Ruby.primitive :sendsite_set_sender
    Object* set_sender(STATE, CompiledMethod* cm);

    // Ruby.primitive :sendsite_hits
    Object* hits_prim(STATE);

    // Ruby.primitive :sendsite_misses
    Object* misses_prim(STATE);

    void initialize(STATE);

    // Check and see if the method referenced has the given serial
    // Sideffect: populates the sendsite if empty
    bool check_serial(STATE, CallFrame* call_frame, Object* reciever, int serial);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void visit(Object*, ObjectVisitor& visit);
      virtual void show(STATE, Object* self, int level);
    };
  };

  namespace performer {
    Object* basic_performer(STATE, SendSite* ss, CallFrame*, Dispatch& msg, Arguments& args);
    Object* mono_performer(STATE, SendSite* ss, CallFrame*, Dispatch& msg, Arguments& args);
    Object* mono_mm_performer(STATE, SendSite* ss, CallFrame*, Dispatch& msg, Arguments& args);
  }

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
    static bool resolve(STATE, Dispatch& msg, LookupData& lookup, bool* was_private);
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
    static bool resolve(STATE, Dispatch& msg, LookupData& lookup);
  };
};

#endif
