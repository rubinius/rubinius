#ifndef RBX_JIT_H
#define RBX_JIT_H

#include "machine_threads.hpp"

#include "util/thread.hpp"

#include <list>

namespace rubinius {
  namespace jit {
    class CompileRequest;

    typedef std::list<CompileRequest*> CompileList;

    class JIT : public MachineThread {
      CompileList list_;

      utilities::thread::Mutex jit_lock_;
      utilities::thread::Condition jit_condition_;

    public:
      JIT(STATE);
      virtual ~JIT() { }

      void initialize(STATE);
      void after_fork_child(STATE);
      void wakeup(STATE);
      void run(STATE);
    };
  }
}

#endif
