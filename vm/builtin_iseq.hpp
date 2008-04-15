#ifndef RBX_BUILTIN_ISEQ_HPP
#define RBX_BUILTIN_ISEQ_HPP

#include "objects.hpp"

namespace rubinius {
  class ISeq : public ByteArray {
  public:

    void post_marshal(STATE);
    static ISeq* create(STATE, size_t bytes);
  };
}

#endif
