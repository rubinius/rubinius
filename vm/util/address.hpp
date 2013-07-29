#ifndef RBX_ADDRESS_H
#define RBX_ADDRESS_H

#if defined(i386) || defined(__i386) || defined(__i386__)
#define IS_X86
#endif

#ifdef IS_X86
#define RBX_MEMORY_ALIGN(var) (var)
#else
/*
 * On non X86, we always align on 64 bit boundaries
 * because we sue 64 bit CAS operations, even on
 * 32 bit systems. X86 can do this, but for example ARM
 * can't.
 */
#define RBX_MEMORY_ALIGNMENT sizeof(int64_t)
#define RBX_MEMORY_ALIGN(var) ((var + (RBX_MEMORY_ALIGNMENT - 1)) & ~(RBX_MEMORY_ALIGNMENT - 1))
#endif

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
      : address_(reinterpret_cast<uintptr_t>(addr))
    {}

    /**
     * Type conversion operator; converts this Address to a void*, allowing
     * the implicit type conversion rules to handle comparisons etc.
     */
    operator void*() const {
      return reinterpret_cast<void*>(address_);
    }

    Address operator+(int change) const {
      change = RBX_MEMORY_ALIGN(change);
      return Address(reinterpret_cast<void*>(address_ + change));
    }

    Address operator+=(int change) {
      change = RBX_MEMORY_ALIGN(change);
      address_ += change;
      return *this;
    }

    size_t operator-(Address change) const {
      return address_ - change.address_;
    }

    Address operator-(int change) const {
      change = RBX_MEMORY_ALIGN(change);
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

    uintptr_t as_int() const {
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
