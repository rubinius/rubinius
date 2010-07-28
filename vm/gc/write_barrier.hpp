#ifndef RBX_GC_WRITE_BARRIER_HPP
#define RBX_GC_WRITE_BARRIER_HPP

#include "util/thread.hpp"

namespace rubinius {
namespace gc {
  class WriteBarrier {
    thread::SpinLock lock_;

  protected:
    ObjectArray* remember_set_;

  public:
    ObjectArray* remember_set() {
      return remember_set_;
    }

    WriteBarrier();
    ~WriteBarrier();

    void write_barrier(Object* target, Object* val) {
      if(target->remembered_p()) return;
      if(!REFERENCE_P(val)) return;
      if(target->zone() == YoungObjectZone) return;
      if(val->zone() != YoungObjectZone) return;

      remember_object(target);
    }

    void remember_object(Object* target);
    void unremember_object(Object* target);

    ObjectArray* swap_remember_set();
  };
}}

#endif
