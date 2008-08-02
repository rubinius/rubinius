#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Selector;
  class MethodResolver;
  class Message;

  class SendSite : public Object {
    public:

    static const size_t object_fields = 6;
    static const object_type type = SendSiteType;

    SYMBOL name; // slot
    OBJECT sender; // slot
    Selector* selector; // slot
    size_t hits, misses;
    bool specialized;

    MethodResolver* resolver;

    static void init(STATE);
    static SendSite* create(STATE, OBJECT name);
    void initialize(STATE);
    void set_sender(STATE, OBJECT cm);
    bool basic_p(STATE);
    bool locate(STATE, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
