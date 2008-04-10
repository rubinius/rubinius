#ifndef RBX_BUILTIN_SELECTOR_HPP
#define RBX_BUILTIN_SELECTOR_HPP

#include "prelude.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "objects.hpp"

#include "builtin_sendsite.hpp"

namespace rubinius {
  class SendSite;

  class Selector : public BuiltinType {
    public:

    static const size_t fields = 2;

    OBJECT name;
    Array* send_sites;

    static void init(STATE);
    static Selector* create(STATE, OBJECT name);
    static Selector* lookup(STATE, OBJECT name);
    static void      clear_by_name(STATE,  OBJECT name);

    OBJECT associate(STATE, SendSite* ss);
    void   clear(STATE);
    bool   includes_p(STATE, SendSite* ss);

  };
};

#endif
