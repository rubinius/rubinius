#include "jit/llvm/types.hpp"

namespace rubinius {
  class LocalInfo {
    int index_;
    bool argument_;
    int pushes_;
    int sets_;
    type::KnownType known_type_;

  public:
    LocalInfo(int i=-1)
      : index_(i)
      , argument_(false)
      , pushes_(0)
      , sets_(0)
    {}

    bool valid_p() {
      return index_ >= 0;
    }

    int index() {
      return index_;
    }

    void make_argument() {
      argument_ = true;
    }

    bool argument_p() {
      return argument_;
    }

    int pushes() {
      return pushes_;
    }

    int sets() {
      return sets_;
    }

    void inc_push() {
      pushes_++;
    }

    void inc_set() {
      sets_++;
    }

    type::KnownType known_type() {
      return known_type_;
    }

    // Ok, some explanation here. Only if it's the first
    // time we set the local, we can uncondionally set the type.
    // If we hit this again, we have to keep the type if it's stil
    // the same, but have to mark it as unknown when we see different
    // types. This makes sure type optimizations see the proper type
    // attributes for locals.
    void set_known_type(type::KnownType kt) {
      if(sets_ <= 1) {
        known_type_ = kt;
      } else {
        if(known_type_.kind() != kt.kind() ||
           known_type_.value() != kt.value()) {
          known_type_ = type::KnownType::unknown();
        }
      }
    }
  };
}
