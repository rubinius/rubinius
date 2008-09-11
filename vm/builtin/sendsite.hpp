#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class CompiledMethod;
  class Selector;
  class MethodResolver;
  class Message;

  class SendSite : public Object {
  public:
    static const size_t object_fields = 6;
    static const object_type type = SendSiteType;

  private:
    SYMBOL name_;            // slot
    CompiledMethod* sender_; // slot
    Selector* selector_;     // slot

  public:
    // TODO: fix up data members that aren't slots
    size_t hits, misses;
    bool specialized;

  public:
    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(sender, CompiledMethod);
    attr_accessor(selector, Selector);

    /* interface */

    MethodResolver* resolver;

    static void init(STATE);

    // Ruby.primitive :sendsite_create
    static SendSite* create(STATE, OBJECT name);
    void initialize(STATE);
    // Ruby.primitive :sendsite_set_sender
    OBJECT set_sender(STATE, CompiledMethod* cm);
    bool basic_p(STATE);
    bool locate(STATE, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
