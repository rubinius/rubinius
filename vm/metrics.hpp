#ifndef RBX_METRICS_HPP
#define RBX_METRICS_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include "util/timer.hpp"

namespace rubinius {
  class VM;
  class State;

  namespace metrics {
    class Metrics : public AuxiliaryThread, public Lockable {
      SharedState& shared_;
      VM* vm_;
      bool thread_exit_;

      TypedRoot<Thread*> thread_;

      int interval_;
      utilities::timer::Timer timer_;

    public:
      Metrics(STATE);
      virtual ~Metrics();

      void cleanup();
      void wakeup();

      void start(STATE);

      void start_thread(STATE);
      void stop_thread(STATE);

      void shutdown(STATE);
      void before_exec(STATE);
      void after_exec(STATE);
      void before_fork(STATE);
      void after_fork_parent(STATE);
      void after_fork_child(STATE);

      void process_metrics(STATE);
    };
  }
}

#endif
