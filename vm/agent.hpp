#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif
#include <vector>

#include "util/thread.hpp"

#include "windows_compat.h"

namespace rubinius {
  class SharedState;

  namespace agent {
    class VariableAccess;
    class Output;
  }

  class Thread;

  class QueryAgent : public thread::Thread {
    SharedState& shared_;
    VM* state_;
    bool running_;
    int port_;
    int server_fd_;
    bool verbose_;
    fd_set fds_;
    int max_fd_;
    bool exit_;

    int control_[2];
    int loopback_[2];

    int a2r_[2];
    int r2a_[2];

    std::vector<int> sockets_;

    agent::VariableAccess* vars_;

    const static int cBackLog = 10;

  public:
    QueryAgent(SharedState& shared, VM* state);
    ~QueryAgent();

    void set_verbose() {
      verbose_ = true;
    }

    bool running() {
      return running_;
    }

    int port() {
      return port_;
    }

    void add_fd(int fd) {
      FD_SET((int_fd_t)fd, &fds_);
      if(fd > max_fd_) max_fd_ = fd;
    }

    void remove_fd(int fd) {
      FD_CLR((int_fd_t)fd, &fds_);
    }

    int loopback_socket() {
      return loopback_[1];
    }

    int read_control() {
      return control_[0];
    }

    int write_control() {
      return control_[1];
    }

    int a2r_agent() {
      return a2r_[1];
    }

    int a2r_ruby() {
      return a2r_[0];
    }

    int r2a_agent() {
      return r2a_[0];
    }

    int r2a_ruby() {
      return r2a_[1];
    }

    void wakeup();

    bool setup_local();
    bool bind(int port);

    void make_discoverable();

    virtual void perform();
    bool process_commands(int client);

    void on_fork();
    void cleanup();

    static void shutdown(STATE);
    void shutdown_i();
  };
}
