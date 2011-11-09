#ifndef RBX_CONFIGURATION_HPP
#define RBX_CONFIGURATION_HPP

#include "util/configuration.hpp"
#include "vm/config.h"

namespace rubinius {
  class Configuration : public config::Configuration {
  public:

#ifdef RBX_ENABLED_18
#define LANGUAGE_18_ENABLED(state)  ((state)->vm()->shared.config.version == 18)
#else
#define LANGUAGE_18_ENABLED(state)  (false)
#endif

#ifdef RBX_ENABLED_19
#define LANGUAGE_19_ENABLED(state)  ((state)->vm()->shared.config.version == 19)
#else
#define LANGUAGE_19_ENABLED(state)  (false)
#endif

#ifdef RBX_ENABLED_20
#define LANGUAGE_20_ENABLED(state)  ((state)->vm()->shared.config.version == 20)
#else
#define LANGUAGE_20_ENABLED(state)  (false)
#endif

#include "vm/gen/config_variables.h"
    void finalize() { }
  };
}

#endif
