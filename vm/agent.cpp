#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "util/bert.hpp"

#include "vm.hpp"
#include "configuration.hpp"
#include "agent.hpp"
#include "exception.hpp"
#include "call_frame.hpp"

#include "config.h"

#include "agent_components.hpp"
#include "environment.hpp"

#include <ostream>
#include <sstream>
#include <fstream>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif


namespace rubinius {
  QueryAgent::QueryAgent(SharedState& shared, VM* state)
    : Thread()
    , shared_(shared)
    , state_(state)
    , running_(false)
    , port_(0)
    , server_fd_(-1)
    , verbose_(false)
    , max_fd_(0)
    , vars_(0)
  {
    FD_ZERO(&fds_);
    vars_ = new agent::VariableAccess(state, shared);

    if(pipe(control_) != 0) {
      perror("pipe");
      rubinius::abort();
    }

    add_fd(read_control());

    loopback_[0] = -1;
    loopback_[1] = -1;
  }

  bool QueryAgent::setup_local() {
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, loopback_)) {
      return false;
    }

    add_fd(loopback_[0]);
    sockets_.push_back(loopback_[0]);

    return true;
  }

  bool QueryAgent::bind(int port) {
    if(server_fd_ > 0) return false;

    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd_ == -1) {
      std::cerr << "[QA: Unable to create socket: " << strerror(errno) << "]\n";
      return false;
    }

    // To avoid TIME_WAIT / EADDRINUSE
    int on = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    if(::bind(server_fd_, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
      std::cerr << "[QA: Unable to bind socket: " << strerror(errno) << "]\n";
      return false;
    }

    if(port == 0) {
      socklen_t len = sizeof(sin);
      if(getsockname(server_fd_, (struct sockaddr*)&sin, &len) == -1) {
        std::cerr << "[QA: Unable to resolve random local port]\n";
        return false;
      }
      port_ = ntohs(sin.sin_port);
    } else {
      port_ = port;
    }

    if(::listen(server_fd_, cBackLog) == -1) {
      std::cerr << "[QA: Unable to listen on socket: " << strerror(errno) << "]\n";
      return false;
    }

    if(verbose_) {
      std::cerr << "[QA: Bound to port " << ntohs(sin.sin_port) << "]\n";
    }

    add_fd(server_fd_);

    make_discoverable();

    return true;
  }

  bool QueryAgent::process_commands(int client) {
    bert::IOReader reader(client);
    bert::Decoder<bert::IOReader> decoder(reader);

    bert::IOWriter writer(client);
    bert::Encoder<bert::IOWriter> encoder(writer);

    int ver = decoder.read_version();
    if(ver != 131) return false;

    bert::Value* val = decoder.next_value();
    if(!val || reader.eof_p()) return false;

    encoder.write_version();

    if(val->type() == bert::Tuple) {
      bert::Value* cmd = val->get_element(0);
      if(cmd->equal_atom("set_config")) {
        if(val->elements()->size() == 3) {
          bert::Value* key = val->get_element(1);
          bert::Value* value = val->get_element(2);

          if(key->type() == bert::Binary) {
            agent::Output output(writer);
            vars_->set_path(output, key->string(), value);
            return true;
          }
        }

        encoder.write_atom("error");
        return true;
      } else if(cmd->equal_atom("get_config")) {
        if(val->elements()->size() == 2) {
          bert::Value* key = val->get_element(1);
          if(key->type() == bert::Binary) {
            agent::Output output(writer);

            vars_->read_path(output, key->string());

            return true;
          }
        }

        encoder.write_atom("error");
        return true;

      // If the client ask is the loopback client, give it some extra
      // commands.
      } else if(client == loopback_[0]) {
        if(cmd->equal_atom("bind")) {
          if(val->elements()->size() == 2) {
            bert::Value* port = val->get_element(1);

            if(port->integer_p()) {
              if(bind(port->integer())) {
                encoder.write_atom("ok");
                return true;
              }
            }
          }
          encoder.write_atom("error");
          return true;
        }
      }
    } else if(val->equal_atom("close")) {
      encoder.write_atom("bye");
      return false;
    }

    encoder.write_atom("unknown");

    return true;
  }

  void QueryAgent::wakeup() {
    char buf = '!';
    if(write(write_control(), &buf, 1) < 0) {
      std::cerr << "[QA: Write error: " << strerror(errno) << "]\n";
    }
  }

  void QueryAgent::perform() {
    running_ = true;

    while(1) {
      fd_set read_fds = fds_;

      int ret = select(max_fd_ + 1, &read_fds, 0, 0, 0);

      if(ret < 0) {
        if(errno == EINTR || errno == EAGAIN) continue;
        std::cerr << "[QA: Select error: " << strerror(errno) << "]\n";
        running_ = false;
        return;
      } else if(ret == 0) {
        continue;
      }

      if(FD_ISSET(read_control(), &read_fds)) {
        // Noop, just a wake up!
        // Read the one byte written though so we don't clog up the
        // pipe and have to use the ponies later.
        char buf;
        if(read(read_control(), &buf, 1) < 0) {
          std::cerr << "[QA: Read error: " << strerror(errno) << "]\n";
        }
      } else if(server_fd_ > 0 && FD_ISSET(server_fd_, &read_fds)) {
        // now accept an incoming connection
        struct sockaddr_in sin;
        socklen_t addr_size = sizeof(sin);
        int client = accept(server_fd_, (struct sockaddr *)&sin, &addr_size);

        if(verbose_) {
          std::cerr << "[QA: Connection from " << inet_ntoa(sin.sin_addr)
                    << ":" << ntohs(sin.sin_port) << "]\n";
        }

        bert::IOWriter writer(client);
        bert::Encoder<bert::IOWriter> encoder(writer);
        encoder.write_version();

        encoder.write_tuple(2);
        encoder.write_atom("hello_query_agent");
        encoder.write_binary(RBX_HOST);

        add_fd(client);

        sockets_.push_back(client);
      } else {
        bool found = false;
        for(std::vector<int>::iterator i = sockets_.begin();
            i != sockets_.end(); /* nothing */) {
          int client = *i;
          if(FD_ISSET(client, &read_fds)) {
            found = true;
            if(!process_commands(client)) {
              if(verbose_) {
                struct sockaddr_in sin;
                socklen_t len = sizeof(sin);

                getpeername(*i, (struct sockaddr*)&sin, &len);
                std::cerr << "[QA: Disconnected " << inet_ntoa(sin.sin_addr)
                          << ":" << ntohs(sin.sin_port) << "]\n";
              }
              remove_fd(client);
              close(client);

              i = sockets_.erase(i);
              continue;
            }
          }
          i++;
        }

        if(!found) {
          std::cerr << "[QA: Corruption in select set detected.]\n";
          running_ = false;
          return;
        }
      }
    }
  }

  static char tmp_path[PATH_MAX];

  static void remove_tmp_path(void) {
    if(tmp_path[0]) unlink(tmp_path);
    // Ignore any errors, this is happening at shutdown.
  }

  void QueryAgent::on_fork() {
    // "clear" tmp_path
    tmp_path[0] = 0;
  }

  void QueryAgent::cleanup() {
    remove_tmp_path();
  }

  void QueryAgent::make_discoverable() {
    // Create a tmp file containing the information to be used
    // by console to find us.

    const char* tmpdir = 0;

    if(shared_.config.qa_tmpdir.value.size() > 0) {
      tmpdir = shared_.config.qa_tmpdir.value.c_str();
    } else {
      tmpdir = getenv("TMPDIR");
    }

    if(tmpdir) {
      std::ostringstream ss;
      pid_t pid = getpid();
      ss << tmpdir << "/rubinius-agent." << pid;

      strcpy(tmp_path, ss.str().c_str());
      std::ofstream stream(tmp_path);

      // Couldn't open the path, for whatever reason.
      if(!stream) return;

      int argc = shared_.env()->argc();
      char** argv = shared_.env()->argv();

      if(stream) {
        stream << pid << "\n";
        stream << port() << "\n";

        for(int i = 0; i < argc; i++) {
          stream << argv[i] << " ";
        }
        stream << "\n";

        char buf[PATH_MAX];
#ifdef __APPLE__
        uint32_t size = PATH_MAX;
        if(_NSGetExecutablePath(buf, &size) == 0) {
          stream << buf << "\n";
        } else if(realpath(argv[0], buf)) {
          stream << buf << "\n";
        } else {
          stream << argv[0] << "\n";
        }
#elif defined(__linux__)
        {
          std::ifstream exe("/proc/self/exe");
          if(exe) {
            char buf[PATH_MAX];
            exe.get(buf, PATH_MAX);

            stream << buf << "\n";
          } else if(realpath(argv[0], buf)) {
            stream << buf << "\n";
          } else {
            stream << argv[0] << "\n";
          }
        }
#else
        if(realpath(argv[0], buf)) {
          stream << buf << "\n";
        } else {
          stream << argv[0] << "\n";
        }
#endif

        stream.close();

        atexit(remove_tmp_path);
      }
    }
  }
}
