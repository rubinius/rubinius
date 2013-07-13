#ifndef RBX_CONFIGURATION_HPP
#define RBX_CONFIGURATION_HPP

#include "util/configuration.hpp"
#include "config.h"

namespace rubinius {
  class Configuration : public config::Configuration {
  public:

#include "gen/config_variables.h"

    void finalize() { }
  };
}

#endif
