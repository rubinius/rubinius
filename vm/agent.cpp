#include "config.h"

#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef RBX_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "windows_compat.h"
#include "util/bert.hpp"

#include "vm.hpp"
#include "configuration.hpp"
#include "agent.hpp"
#include "exception.hpp"
#include "call_frame.hpp"

#include "agent_components.hpp"
#include "environment.hpp"

#include "builtin/nativemethod.hpp"
#include "builtin/io.hpp"

#include <ostream>
#include <sstream>
#include <fstream>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif

namespace rubinius {
  QueryAgent::QueryAgent(SharedState& shared, STATE)
    : Thread()
    , shared_(shared)
    , state_(shared.new_vm())
    , running_(false)
    , port_(0)
    , server_fd_(-1)
    , verbose_(false)
    , max_fd_(0)
    , exit_(false)
    , vars_(0)
    , local_only_(true)
    , tmp_key_(0)
  {
    FD_ZERO(&fds_);

    vars_ = new agent::VariableAccess(this->state(), shared);

    if(pipe(control_) != 0) {
      perror("pipe");
      rubinius::abort();
    }

    add_fd(read_control());

    loopback_[0] = -1;
    loopback_[1] = -1;

    if(pipe(a2r_) != 0) {
      perror("pipe");
      rubinius::abort();
    }

    if(pipe(r2a_) != 0) {
      perror("pipe");
      rubinius::abort();
    }

    add_fd(r2a_agent());

    // This class is always created with the GIL locked, so it's ok
    // to access ruby stuff here.
    IO* from = IO::create(state, a2r_ruby());
    from->sync(state, Qtrue);

    IO* to = IO::create(state, r2a_ruby());
    to->sync(state, Qtrue);

    shared_.globals.rubinius.get()->set_const(state, "FROM_AGENT", from);
    shared_.globals.rubinius.get()->set_const(state, "TO_AGENT", to);

    if(shared_.config.agent_password.set_p()) {
      local_only_ = false;
      use_password_ = true;
      password_ = std::string(shared_.config.agent_password);
    }
  }

  QueryAgent::~QueryAgent() {
    VM::discard(&state_, state_.vm());
    delete vars_;
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
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    struct sockaddr_in sin = {};
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

  namespace {
    bool get_ruby(QueryAgent* agent, int client, const char* k) {
      bert::IOWriter writer(agent->a2r_agent());
      bert::Encoder<bert::IOWriter> e(writer);

      e.write_tuple(3);
      e.write_integer(client);
      e.write_atom("get");
      e.write_binary(k);

      return true;
    }

    bool set_ruby(QueryAgent* agent, int client, const char* k,
        bert::Value* val)
    {
      bert::IOWriter writer(agent->a2r_agent());
      bert::Encoder<bert::IOWriter> e(writer);

      e.write_tuple(4);
      e.write_integer(client);
      e.write_atom("set");
      e.write_binary(k);
      e.write_value(val);

      return true;
    }

    void respond_from_ruby(QueryAgent* agent) {
      bert::IOReader reader(agent->r2a_agent());
      bert::Decoder<bert::IOReader> d(reader);

      bert::Value* val = d.next_value();

      if(val->type() == bert::Tuple && val->total_elements() == 3) {
        bert::Value* who = val->get_element(0);
        bert::Value* code = val->get_element(1);

        if(code->type() == bert::Atom && who->integer_p()) {
          int client = who->integer();

          bert::IOWriter writer(client);
          bert::Encoder<bert::IOWriter> e(writer);

          if(code->equal_atom("ok")) {
            e.write_tuple(3);
            e.write_atom("ok");
            e.write_atom("value");
            e.write_value(val->get_element(2));
          } else {
            e.write_tuple(2);
            e.write_value(code);
            e.write_value(val->get_element(2));
          }
        }
      }

      delete val;
    }

    void write_welcome(int client) {
      bert::IOWriter writer(client);
      bert::Encoder<bert::IOWriter> encoder(writer);
      encoder.write_version();

      encoder.write_tuple(2);
      encoder.write_atom("hello_query_agent");
      encoder.write_binary(RBX_HOST);
    }

    void request_auth(int client, std::string name) {
      bert::IOWriter writer(client);
      bert::Encoder<bert::IOWriter> encoder(writer);
      encoder.write_version();

      encoder.write_tuple(2);
      encoder.write_atom("file_auth");
      encoder.write_binary(name.c_str());
    }

    void request_password(int client) {
      bert::IOWriter writer(client);
      bert::Encoder<bert::IOWriter> encoder(writer);
      encoder.write_version();

      encoder.write_tuple(1);
      encoder.write_atom("password_auth");
    }
  }

  bool QueryAgent::check_password(Client& client) {
    bert::IOReader reader(client.socket);
    bert::Decoder<bert::IOReader> decoder(reader);

    bert::IOWriter writer(client.socket);
    bert::Encoder<bert::IOWriter> encoder(writer);
    bert::Value* val = 0;

    int ver = decoder.read_version();
    if(ver != 131) return false;

    val = decoder.next_value();
    if(!val) return false;

    if(reader.eof_p()) {
      delete val;
      return false;
    }

    if(val->type() == bert::Tuple) {
      bert::Value* cmd = val->get_element(0);
      if(cmd->equal_atom("password")) {
        bert::Value* pass = val->get_element(1);

        if(pass->type() == bert::Binary) {
          if(password_ == std::string(pass->string())) return true;
        }
      }
    }

    return false;
  }

  bool QueryAgent::check_file_auth(Client& client) {
    bert::IOReader reader(client.socket);
    bert::Decoder<bert::IOReader> decoder(reader);

    bert::IOWriter writer(client.socket);
    bert::Encoder<bert::IOWriter> encoder(writer);

    std::stringstream name;
    std::ifstream file;
    bert::Value* val = 0;

    name << "/tmp/agent-auth." << getuid() << "-" << getpid() << "." << client.auth_key;

    int ver = decoder.read_version();
    if(ver != 131) goto auth_error;

    val = decoder.next_value();
    if(!val) goto auth_error;

    if(reader.eof_p()) {
      delete val;
      goto auth_error;
    }

    if(!val->equal_atom("ok")) goto auth_error;

    file.open(name.str().c_str());

    char key[PATH_MAX];
    file.getline(key, PATH_MAX);

    if(strcmp(key, "agent start") != 0) goto auth_error;

    unlink(name.str().c_str());
    return true;

auth_error:
    unlink(name.str().c_str());
    return false;
  }

  bool QueryAgent::process_commands(Client& client) {
    if(client.needs_auth_p()) {
      if(local_only_) {
        if(!check_file_auth(client)) return false;
      } else if(use_password_) {
        if(!check_password(client)) return false;
      } else {
        return false;
      }

      if(verbose_) {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);

        getpeername(client.socket, (struct sockaddr*)&sin, &len);
        std::cerr << "[QA: Authenticated " << inet_ntoa(sin.sin_addr)
          << ":" << ntohs(sin.sin_port) << "]\n";
      }

      client.set_running();
      write_welcome(client.socket);
      return true;
    }

    bert::IOReader reader(client.socket);
    bert::Decoder<bert::IOReader> decoder(reader);

    bert::IOWriter writer(client.socket);
    bert::Encoder<bert::IOWriter> encoder(writer);

    int ver = decoder.read_version();
    if(ver != 131) return false;

    bert::Value* val = decoder.next_value();
    if(!val) return false;

    if(reader.eof_p()) {
      delete val;
      return false;
    }

    encoder.write_version();

    if(val->type() == bert::Tuple) {
      bert::Value* cmd = val->get_element(0);
      if(cmd->equal_atom("set_config")) {
        if(val->elements()->size() == 3) {
          bert::Value* key = val->get_element(1);
          bert::Value* value = val->get_element(2);

          if(key->type() == bert::Binary) {
            agent::Output output(writer);
            if(!vars_->set_path(output, key->string(), value)) {
              set_ruby(this, client.socket, key->string(), value);
            }
            delete val;
            return true;
          }
        }

        encoder.write_atom("error");
        delete val;
        return true;
      } else if(cmd->equal_atom("get_config")) {
        if(val->elements()->size() == 2) {
          bert::Value* key = val->get_element(1);
          if(key->type() == bert::Binary) {
            agent::Output output(writer);

            if(!vars_->read_path(output, key->string())) {
              get_ruby(this, client.socket, key->string());
            }

            delete val;
            return true;
          }
        }

        encoder.write_atom("error");
        delete val;
        return true;

      // If the client ask is the loopback client, give it some extra
      // commands.
      } else if(client.socket == loopback_[0]) {
        if(cmd->equal_atom("bind")) {
          if(val->elements()->size() == 2) {
            bert::Value* port = val->get_element(1);

            if(port->integer_p()) {
              if(bind(port->integer())) {
                encoder.write_atom("ok");
                delete val;
                return true;
              }
            }
          }
          encoder.write_atom("error");
          delete val;
          return true;
        }
      }
    } else if(val->equal_atom("close")) {
      encoder.write_atom("bye");
      delete val;
      return false;
    }

    encoder.write_atom("unknown");
    delete val;

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

    State state_obj(state_), *state = &state_obj;

    // It's possible we call code that wants this to thread
    // to be setup as a fully managed thread, so lets just make it one.
    NativeMethod::init_thread(state);
    set_delete_on_exit();

    while(1) {
      fd_set read_fds = fds_;

      int ret = select(max_fd_ + 1, &read_fds, 0, 0, 0);
      if(exit_) return;

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
      } else if(FD_ISSET(r2a_agent(), &read_fds)) {
        respond_from_ruby(this);
      } else if(server_fd_ > 0 && FD_ISSET(server_fd_, &read_fds)) {
        // now accept an incoming connection
        struct sockaddr_in sin;
        socklen_t addr_size = sizeof(sin);
        int client = accept(server_fd_, (struct sockaddr *)&sin, &addr_size);

        if(local_only_) {
          if(sin.sin_addr.s_addr != inet_addr("127.0.0.1")) {
            std::cerr << "[QA: Reject connection from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
            close(client);
            continue;
          }

          Client cl(client);

          cl.begin_auth(tmp_key_++);

          std::stringstream name;
          name << "/tmp/agent-auth." << getuid() << "-" << getpid() << "." << cl.auth_key;

          int file = open(name.str().c_str(), O_CREAT | O_WRONLY | O_EXCL, 0600);
          if(file == -1) {
            std::cerr << "[QA: Unable to open '" << name.str() << "' to begin local auth]\n";
            close(client);
            continue;
          }

          close(file);

          if(verbose_) {
            std::cerr << "[QA: Requesting file auth from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
          }

          request_auth(client, name.str());

          add_fd(client);

          sockets_.push_back(cl);

          continue;
        } else if(use_password_) {
          Client cl(client);
          cl.begin_auth(0);

          if(verbose_) {
            std::cerr << "[QA: Requesting password auth from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
          }

          request_password(client);

          add_fd(client);

          sockets_.push_back(cl);

          continue;
        }

        if(verbose_) {
          std::cerr << "[QA: Connection from " << inet_ntoa(sin.sin_addr)
                    << ":" << ntohs(sin.sin_port) << "]\n";
        }

        write_welcome(client);

        add_fd(client);

        Client cl(client);
        cl.set_running();

        sockets_.push_back(cl);
      } else {
        bool found = false;
        for(std::vector<Client>::iterator i = sockets_.begin();
            i != sockets_.end(); /* nothing */) {
          Client& client = *i;

          if(FD_ISSET(client.socket, &read_fds)) {
            found = true;
            if(!process_commands(client)) {
              if(verbose_) {
                struct sockaddr_in sin;
                socklen_t len = sizeof(sin);

                getpeername(client.socket, (struct sockaddr*)&sin, &len);
                std::cerr << "[QA: Disconnected " << inet_ntoa(sin.sin_addr)
                          << ":" << ntohs(sin.sin_port) << "]\n";
              }

              remove_fd(client.socket);
              close(client.socket);

              i = sockets_.erase(i);
              continue;
            }
          }
          ++i;
        }

        if(!found) {
          std::cerr << "[QA: Corruption in select set detected.]\n";
          running_ = false;
          return;
        }
      }
    }

    NativeMethod::cleanup_thread(state);
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

  void QueryAgent::shutdown(STATE) {
    if(!state->shared().agent()) return;
    state->shared().agent()->shutdown_i();
  }

  void QueryAgent::shutdown_i() {
    exit_ = true;
    wakeup();
    cleanup();

    if(!in_self_p()) join();
  }

  void QueryAgent::make_discoverable() {
    // Create a tmp file containing the information to be used
    // by console to find us.

    const char* tmpdir = 0;

    if(shared_.config.agent_tmpdir.value.size() > 0) {
      tmpdir = shared_.config.agent_tmpdir.value.c_str();
    } else {
      tmpdir = getenv("TMPDIR");
    }

    if(!tmpdir) {
      struct stat st;

      if(!stat("/tmp", &st) && S_ISDIR(st.st_mode) && (st.st_mode & S_IRWXU) == S_IRWXU) {
        tmpdir = "/tmp";
      } else {
        fputs("agent: unable to write discovery file", stderr);
        return;
      }
    }

    std::ostringstream ss;
    pid_t pid = getpid();
    ss << tmpdir << "/rubinius-agent." << pid;

    strncpy(tmp_path, ss.str().c_str(), ss.str().size());
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
#elif defined(__FreeBSD__)
      int oid[4];
      size_t len;

      oid[0] = CTL_KERN;
      oid[1] = KERN_PROC;
      oid[2] = KERN_PROC_PATHNAME;
      oid[3] = getpid();
      len = PATH_MAX;
      if(sysctl(oid, 4, buf, &len, 0, 0) == 0) {
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
