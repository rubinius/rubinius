#include <sys/select.h>
#include <vector>

#include "util/thread.hpp"

namespace rubinius {
  class SharedState;

  namespace agent {
    class VariableAccess;
  }

  class QueryAgent : public thread::Thread {
    SharedState& shared_;
    VM* state_;
    int port_;
    int server_fd_;
    bool verbose_;
    fd_set fds_;
    int max_fd_;

    std::vector<int> sockets_;

    agent::VariableAccess* vars_;

    const static int cBackLog = 10;

  public:
    QueryAgent(SharedState& shared, VM* state, int port);

    void set_verbose() {
      verbose_ = true;
    }

    int port() {
      return port_;
    }

    bool bind();
    virtual void perform();
    bool process_commands(int client);
  };
}
