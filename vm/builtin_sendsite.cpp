#include "builtin_sendsite.hpp"
#include "builtin_selector.hpp"

namespace rubinius {
  void SendSite::init(STATE) {
    state->globals.send_site = state->new_class("SendSite", 0);
    state->globals.send_site->set_object_type(SendSiteType);
  }

  SendSite* SendSite::create(STATE, OBJECT name) {
    SendSite* ss = (SendSite*)state->new_struct(state->globals.send_site, sizeof(SendSite));
    SET(ss, name, name);
    SET(ss, sender, Qnil);
    SET(ss, selector, Selector::lookup(state, name));
    ss->hits = ss->misses = 0;

    ss->initialize(state);
    ss->selector->associate(state, ss);

    return ss;
  }

  void SendSite::initialize(STATE) {
    data1 = data2 = data3 = Qnil;
  }

  /* Indicates that the object hasn't been specialized in anyway */
  bool SendSite::basic_p(STATE) {
    return data1 == Qnil;
  }
};
