#include <sys/select.h>
#include <vector>

#include "util/thread.hpp"

namespace rubinius {
  class SharedState;

  class QueryAgent : public thread::Thread {
    SharedState& shared_;
    int port_;
    int server_fd_;
    bool verbose_;
    fd_set fds_;
    int max_fd_;

    std::vector<int> sockets_;

    const static int cBackLog = 10;

  public:
    QueryAgent(SharedState& shared, int port)
      : Thread()
      , shared_(shared)
      , port_(port)
      , server_fd_(0)
      , verbose_(false)
      , max_fd_(0)
    {
      FD_ZERO(&fds_);
    }

    void set_verbose() {
      verbose_ = true;
    }

    bool bind();
    virtual void perform();
    bool process_commands(int client);
  };
}
