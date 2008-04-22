#include "builtin.hpp"

namespace rubinius {
  Root::Root(STATE) : object(NULL), roots(&state->globals.roots) { }
}
