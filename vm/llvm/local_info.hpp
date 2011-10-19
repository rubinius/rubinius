#include "llvm/types.hpp"

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

    void set_known_type(type::KnownType kt) {
      known_type_ = kt;
    }

    // Indicate if the argument is never changed in the body
    // and the type is already known.
    bool static_type_argument_p() {
      return valid_p() && argument_ && sets_ == 0 && known_type_.known_p();
    }
  };
}
