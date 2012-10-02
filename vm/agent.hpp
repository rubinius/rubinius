#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif
#include <vector>

#include "auxiliary_threads.hpp"
#include "gc/managed.hpp"
#include "util/thread.hpp"

#include "windows_compat.h"

namespace rubinius {
  class SharedState;

  namespace agent {
    class VariableAccess;
    class Output;
  }

  class QueryAgent : public AuxiliaryThread, public Lockable {
    struct Client {
      enum State {
        eUnknown,
        eWaitingAuth,
        eRunning
      } state;

      int socket;
      int auth_key;

      Client(int s)
        : state(eUnknown)
        , socket(s)
        , auth_key(0)
      {}

      void set_running() {
        state = eRunning;
      }

      bool needs_auth_p() {
        return state == eWaitingAuth;
      }

      void begin_auth(int key) {
        auth_key = key;
        state = eWaitingAuth;
      }

    };

    class Thread : public utilities::thread::Thread {
    private:
      QueryAgent* agent_;
      bool exit_;

    public:
      Thread(QueryAgent* agent);

      void stop();
      virtual void perform();
    };

  private:
    SharedState& shared_;
    Thread* thread_;
    int port_;
    int server_fd_;
    bool verbose_;
    fd_set fds_;
    int max_fd_;

    int control_[2];
    int loopback_[2];

    int a2r_[2];
    int r2a_[2];

    std::vector<Client> sockets_;

    agent::VariableAccess* vars_;

    bool local_only_;
    bool use_password_;
    std::string password_;
    uint32_t tmp_key_;

    const static int cBackLog = 10;

  public:
    QueryAgent(STATE);
    virtual ~QueryAgent();

    void set_verbose() {
      verbose_ = true;
    }

    int port() {
      return port_;
    }

    fd_set fds() {
      return fds_;
    }

    int max_fd() {
      return max_fd_;
    }

    int server_fd() {
      return server_fd_;
    }

    bool local_only() {
      return local_only_;
    }

    bool use_password() {
      return use_password_;
    }

    bool verbose() {
      return verbose_;
    }

    uint32_t incr_tmp_key() {
      return tmp_key_++;
    }

    void add_socket(Client client) {
      sockets_.push_back(client);
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

    void initialize(STATE);
    void start_thread(STATE);
    void stop_thread(STATE);
    void wakeup();

    bool setup_local();
    bool bind(int port);

    void make_discoverable();

    bool check_password(Client& client);
    bool check_file_auth(Client& client);
    bool process_commands(Client& client);
    bool process_clients(fd_set fds);

    void cleanup();

    void shutdown(STATE);
    void before_exec(STATE);
    void after_exec(STATE);
    void before_fork(STATE);
    void after_fork_parent(STATE);
    void after_fork_child(STATE);
  };
}
