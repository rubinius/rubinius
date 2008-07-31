#include "builtin.hpp"

namespace rubinius {
  StaticScope* StaticScope::create(STATE) {
    return (StaticScope*)state->new_object(G(staticscope));
  }
}
