#ifndef RBX_SIGNAL_HPP
#define RBX_SIGNAL_HPP

#include "machine_thread.hpp"
#include "logger.hpp"
#include "thread_state.hpp"

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>

namespace rubinius {
  class Signals {
  public:

    class SignalThread : public MachineThread {
      const static int pending_signal_size_ = 256;

      int pending_signals_[pending_signal_size_];
      int queue_index_;
      int process_index_;

      std::mutex& lock_;
      std::condition_variable& condition_;

    public:

      SignalThread(STATE, std::mutex& lock, std::condition_variable& condition);
      virtual ~SignalThread() { }

      void initialize(STATE);
      void stop(STATE);
      void wakeup(STATE);
      void run(STATE);

      void queue_signal(int signal);
    };

  private:

    SignalThread* signal_thread_;

    std::mutex watch_lock_;
    std::mutex lock_;
    std::condition_variable condition_;

    std::list<int> watched_signals_;


  public:

    static Signals* signal_handler_;

    enum HandlerType {
      eDefault,
      eIgnore,
      eCustom
    };

    Signals();
    virtual ~Signals() { }

    static void signal_handler(int signal);

    SignalThread* const signal_thread() {
      return signal_thread_;
    }

    void start(STATE);
    void stop(STATE);

    void install_default_handlers();
    void after_fork_child(STATE);

    void add_signal_handler(ThreadState*, int signal, HandlerType type = eCustom);

    void print_backtraces();

    void print_machine_info(logger::PrintFunction function);
    void print_process_info(logger::PrintFunction function);
  };
}

#endif
