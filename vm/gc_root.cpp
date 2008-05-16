#include "builtin.hpp"

namespace rubinius {
  Root::Root(STATE) : object(NULL), roots(&state->globals.roots) { }
  Root::Root(STATE, OBJECT obj) : object(obj), roots(&state->globals.roots) { }
}
