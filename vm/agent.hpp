#ifndef RBX_WINDOWS
#include <sys/select.h>
#endif
#include <vector>

#include "auxiliary_threads.hpp"
#include "gc/managed.hpp"
#include "gc/root.hpp"
#include "util/thread.hpp"
#include "windows_compat.h"

namespace rubinius {
  class SharedState;
  class VM;
  class Thread;

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

      bool needs_auth_p() const {
        return state == eWaitingAuth;
      }

      void begin_auth(int key) {
        auth_key = key;
        state = eWaitingAuth;
      }

    };

  private:
    SharedState& shared_;
    VM* vm_;
    TypedRoot<Thread*> thread_;
    int port_;
    int server_fd_;
    bool verbose_;
    bool exit_;
    fd_set fds_;
    int max_fd_;

    int control_[2];
    int loopback_[2];

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

    int port() const {
      return port_;
    }

    fd_set fds() const {
      return fds_;
    }

    int max_fd() const {
      return max_fd_;
    }

    int server_fd() const {
      return server_fd_;
    }

    bool local_only() const {
      return local_only_;
    }

    bool use_password() const {
      return use_password_;
    }

    bool verbose() const {
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

    int loopback_socket() const {
      return loopback_[1];
    }

    int read_control() const {
      return control_[0];
    }

    int write_control() const {
      return control_[1];
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

    void perform(STATE);
    void run(STATE);
  };
}
