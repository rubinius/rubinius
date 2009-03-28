#ifndef RBX_OBJECT_POSITION_HPP
#define RBX_OBJECT_POSITION_HPP

namespace rubinius {
  enum ObjectPosition {
    cUnknown,
    cValid,
    cInWrongYoungHalf,
    cMatureObject,
    cContextStack,
    cInImmix,
    cInImmixCorruptHeader
  };
}

#endif
