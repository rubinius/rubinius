#ifndef RBX_ON_STACK_HPP
#define RBX_ON_STACK_HPP

#include "gc/variable_buffer.hpp"

namespace rubinius {

  class Object;

  template <int size>
  class OnStack {
    VariableRootBuffer buffer_;
    Object** objects_[size];

  public:

    template <typename T>
    OnStack(STATE, T& o1)
      : buffer_(state->vm()->current_root_buffers(), objects_, size)
    {
      // Ok, let me explain.
      // We want to get the address of o1 to put into the buffer, so that
      // the GC can properly update it. We only want this to be used on
      // Objects.
      //
      // Well, we can't declare o1 as Object*& because g++ won't let use use
      // subclasses (ie, Channel*). So we use a template to bring in the data
      // as ANY type.
      //
      // Well, we don't really want any type, we only want Objects! So
      // what we do is validate the type of o1, as expanded by the template,
      // is an Object* or subclass.
      //
      // Thats what the static_cast<> here does. It will only compiled iff the
      // type of o1 is Object* or a subclass.
      //
      // When compiled though, because it's completely unused, it disappears, thus
      // we've added a compile time type check.
      //
      (void)static_cast<Object*>(o1);

      // We can't use a static_cast<> here to validate the type of o1 because
      // C++ won't let use do a static cast from, say a Channel** to an Object**.
      // Why? I don't know, probably something to with too much type degradation.
      // Anyway, we work around this using the above type check. If that works out,
      // we're free to use reinterpret_cast<> safely.
      objects_[0] = reinterpret_cast<Object**>(&o1);
    }

    // All the above notes apply to the following overloads as well.

    template <typename T1, typename T2>
    OnStack(STATE, T1& o1, T2& o2)
      : buffer_(state->vm()->current_root_buffers(), objects_, size)
    {
      (void)static_cast<Object*>(o1);
      (void)static_cast<Object*>(o2);

      objects_[0] = reinterpret_cast<Object**>(&o1);
      objects_[1] = reinterpret_cast<Object**>(&o2);
    }

    template <typename T1, typename T2, typename T3>
    OnStack(STATE, T1& o1, T2& o2, T3& o3)
      : buffer_(state->vm()->current_root_buffers(), objects_, size)
    {
      (void)static_cast<Object*>(o1);
      (void)static_cast<Object*>(o2);
      (void)static_cast<Object*>(o3);

      objects_[0] = reinterpret_cast<Object**>(&o1);
      objects_[1] = reinterpret_cast<Object**>(&o2);
      objects_[2] = reinterpret_cast<Object**>(&o3);
    }

    template <typename T1, typename T2, typename T3, typename T4>
    OnStack(STATE, T1& o1, T2& o2, T3& o3, T4& o4)
      : buffer_(state->vm()->current_root_buffers(), objects_, size)
    {
      (void)static_cast<Object*>(o1);
      (void)static_cast<Object*>(o2);
      (void)static_cast<Object*>(o3);
      (void)static_cast<Object*>(o4);

      objects_[0] = reinterpret_cast<Object**>(&o1);
      objects_[1] = reinterpret_cast<Object**>(&o2);
      objects_[2] = reinterpret_cast<Object**>(&o3);
      objects_[3] = reinterpret_cast<Object**>(&o4);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    OnStack(STATE, T1& o1, T2& o2, T3& o3, T4& o4, T5& o5)
      : buffer_(state->vm()->current_root_buffers(), objects_, size)
    {
      (void)static_cast<Object*>(o1);
      (void)static_cast<Object*>(o2);
      (void)static_cast<Object*>(o3);
      (void)static_cast<Object*>(o4);
      (void)static_cast<Object*>(o5);

      objects_[0] = reinterpret_cast<Object**>(&o1);
      objects_[1] = reinterpret_cast<Object**>(&o2);
      objects_[2] = reinterpret_cast<Object**>(&o3);
      objects_[3] = reinterpret_cast<Object**>(&o4);
      objects_[4] = reinterpret_cast<Object**>(&o4);
    }

  };
}

#endif
