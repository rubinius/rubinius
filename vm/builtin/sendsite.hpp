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

    SYMBOL name; // slot
    CompiledMethod* sender; // slot
    Selector* selector; // slot
    size_t hits, misses;
    bool specialized;

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
    };
  };
};

#endif
