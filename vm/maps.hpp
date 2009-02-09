#ifndef RBX_MAPS_HPP
#define RBX_MAPS_HPP

#include <map>

namespace rubinius {
  class SharedState;
  class VM;

  typedef std::map<int, SharedState*> ShareMap;
  typedef std::map<int, VM*> VMMap;
}

#endif
