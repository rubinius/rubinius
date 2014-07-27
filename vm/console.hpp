#ifndef RBX_CONSOLE_HPP
#define RBX_CONSOLE_HPP

#include "lock.hpp"
#include "auxiliary_threads.hpp"

#include "gc/root.hpp"

#include <list>
#include <string>

namespace rubinius {
  class VM;
  class State;
  class Object;
  class Thread;

  namespace console {
    typedef std::list<char*> RequestList;

    class Console : public AuxiliaryThread, public Lockable {
      SharedState& shared_;
      VM* request_vm_;
      VM* response_vm_;

      TypedRoot<Thread*> request_;
      TypedRoot<Thread*> response_;
      TypedRoot<Object*> console_;

      int request_fd_;

      bool request_exit_;
      bool response_exit_;

      RequestList* request_list_;

      utilities::thread::Mutex list_lock_;
      utilities::thread::Mutex response_lock_;
      utilities::thread::Condition response_cond_;

    public:
      Console(STATE);
      virtual ~Console();

      void cleanup();
      void wakeup();

      void start(STATE);
      void initialize(STATE);
      void run(STATE);

      void start_threads(STATE);
      void stop_threads(STATE);

      void shutdown(STATE);
      void before_exec(STATE);
      void after_exec(STATE);
      void before_fork(STATE);
      void after_fork_parent(STATE);
      void after_fork_child(STATE);

      void process_requests(STATE);
      void process_responses(STATE);

      char* read_request(STATE);
    };
  }
}

#endif
