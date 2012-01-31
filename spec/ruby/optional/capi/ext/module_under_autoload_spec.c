#include "ruby.h"

void Init_module_under_autoload_spec(void) {
  rb_define_module_under(rb_cObject, "ModuleUnderAutoload");
}
