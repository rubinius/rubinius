#ifndef RBX_UTIL_REFCOUNT_HPP
#define RBX_UTIL_REFCOUNT_HPP

namespace rubinius {

  class RefCount {
    int ref_count_;

  public:

    RefCount() : ref_count_(0) {}

    void ref() {
      ref_count_++;
    }

    bool deref() {
      return --ref_count_ <= 0;
    }

    int ref_count() const {
      return ref_count_;
    }
  };

}

#endif
