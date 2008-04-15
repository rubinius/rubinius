#ifndef RBX_BUILTIN_SENDSITE_HPP
#define RBX_BUILTIN_SENDSITE_HPP

#include "prelude.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"

namespace rubinius {
  class Selector;

  class SendSite : public BuiltinType {
    public:

    static const size_t object_fields = 6;
    static const object_type type = SendSiteType;

    SYMBOL name;
    OBJECT sender;
    Selector* selector;
    OBJECT data1;
    OBJECT data2;
    OBJECT data3;

    int data4;
    size_t hits, misses;
    void* c_data;

    static void init(STATE);
    static SendSite* create(STATE, OBJECT name);
    void set_sender(STATE, OBJECT cm);
    void initialize(STATE);
    bool basic_p(STATE);
  };
};

#endif
