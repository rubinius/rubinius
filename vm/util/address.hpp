#ifndef RBX_ADDRESS_H
#define RBX_ADDRESS_H


namespace memory {

  /**
   * A wonderful little class for working with memory addresses that is used
   * always as a value, never a reference or pointer. It basically allows us
   * the ability to implement our own pointer logic.
   */

  struct Address {

    /// The pointer this Address struct encapsulates
    uintptr_t address_;

    Address()
      : address_(0)
    {}

    Address(void* addr)
      : address_(reinterpret_cast<intptr_t>(addr))
    {}

    /**
     * Type conversion operator; converts this Address to a void*, allowing
     * the implicit type conversion rules to handle comparisons etc.
     */
    operator void*() const {
      return reinterpret_cast<void*>(address_);
    }

    Address operator+(int change) const {
      return Address(reinterpret_cast<void*>(address_ + change));
    }

    Address operator+=(int change) {
      address_ += change;
      return *this;
    }

    size_t operator-(Address change) const {
      return address_ - change.address_;
    }

    Address operator-(int change) const {
      return Address(reinterpret_cast<void*>(address_ - change));
    }

    Address operator&(uintptr_t mask) const {
      return Address(reinterpret_cast<void*>(address_ & mask));
    }

    bool is_null() const {
      return address_ == 0;
    }

    static Address null() {
      return Address(0);
    }

    intptr_t as_int() const {
      return address_;
    }

    /**
     * Converts the memory address represented by this instance to a pointer
     * of the specified type.
     */
    template <typename T>
      T* as() {
        return reinterpret_cast<T*>(reinterpret_cast<void*>(address_));
      }
  };

}

#endif
