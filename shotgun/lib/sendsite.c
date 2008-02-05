#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/sendsite.h"
#include "shotgun/lib/selector.h"

void send_site_cleanup(STATE, OBJECT obj) {

}

void send_site_init(STATE) {
  struct type_info info = {
    .object_fields = SEND_SITE_OBJECT_FIELDS,
    .cleanup = send_site_cleanup
  };
  
  BASIC_CLASS(send_site) = rbs_class_new(state, "SendSite", 0, BASIC_CLASS(object));
  class_set_object_type(BASIC_CLASS(send_site), I2N(SendSiteType));

  state_setup_type(state, SendSiteType, &info);
}

OBJECT send_site_create(STATE, OBJECT name, send_site_lookup func) {
  OBJECT ss_obj;
  struct send_site *ss;

  NEW_STRUCT(ss_obj, ss, BASIC_CLASS(send_site), struct send_site);
  ss->name = name;
  ss->selector = selector_lookup(state, name);
  ss->lookup = func;

  selector_associate(state, ss->selector, ss_obj);

  return ss_obj;
}
