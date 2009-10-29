#include "util/thread.hpp"

namespace rubinius {
  class QueryAgent : public thread::Thread {
    int port_;
    int server_fd_;
    bool verbose_;

    const static int cBackLog = 10;

  public:
    QueryAgent(int port)
      : Thread()
      , port_(port)
      , server_fd_(0)
      , verbose_(false)
    {}

    void set_verbose() {
      verbose_ = true;
    }

    void bind();
    virtual void perform();
  };
}
