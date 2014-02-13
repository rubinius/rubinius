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

#include "builtin/native_method.hpp"
#include "builtin/io.hpp"
#include "builtin/thread.hpp"

#include "dtrace/dtrace.h"

#include <sstream>
#include <fstream>

namespace rubinius {
  static char tmp_path[PATH_MAX];

  static void remove_tmp_path(void) {
    if(tmp_path[0]) unlink(tmp_path);
    // Ignore any errors, this is happening at shutdown.
  }

  Object* query_agent_tramp(STATE) {
    state->shared().query_agent()->perform(state);
    GCTokenImpl gct;
    state->gc_dependent(gct, 0);
    return cNil;
  }

  QueryAgent::QueryAgent(STATE)
    : AuxiliaryThread()
    , shared_(state->shared())
    , vm_(NULL)
    , thread_(state)
    , port_(0)
    , server_fd_(-1)
    , verbose_(false)
    , exit_(false)
    , max_fd_(0)
    , vars_(0)
    , local_only_(true)
    , tmp_key_(0)
  {
    shared_.auxiliary_threads()->register_thread(this);
    shared_.set_query_agent(this);

    vars_ = new agent::VariableAccess(state);

    initialize(state);
    start_thread(state);
  }

  QueryAgent::~QueryAgent() {
    shared_.auxiliary_threads()->unregister_thread(this);
    delete vars_;
  }

  void QueryAgent::initialize(STATE) {
    FD_ZERO(&fds_);

    tmp_path[0] = 0;

    if(pipe(control_) != 0) {
      perror("pipe");
      rubinius::abort();
    }

    add_fd(read_control());

    loopback_[0] = -1;
    loopback_[1] = -1;

    if(shared_.config.agent_password.set_p()) {
      local_only_ = false;
      use_password_ = true;
      password_ = std::string(shared_.config.agent_password);
    }
  }

  bool QueryAgent::setup_local() {
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, loopback_)) {
      return false;
    }

    add_fd(loopback_[0]);
    add_socket(loopback_[0]);

    return true;
  }

  bool QueryAgent::bind(int port) {
    if(server_fd_ > 0) return false;

    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd_ == -1) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
      std::cerr << "[QA: Unable to create socket: " << err << "]\n";
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
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
      std::cerr << "[QA: Unable to bind socket: " << err << "]\n";
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
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
      std::cerr << "[QA: Unable to listen on socket: " << err << "]\n";
      return false;
    }

    if(verbose_) {
      std::cerr << "[QA: Bound to port " << ntohs(sin.sin_port) << "]\n";
    }

    add_fd(server_fd_);

    make_discoverable();
    wakeup();

    return true;
  }

  namespace {

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

        if(pass && pass->type() == bert::Binary) {
          if(password_ == std::string(pass->string())) {
            delete val;
            return true;
          }
        }
      }
    }

    delete val;
    return false;
  }

  bool QueryAgent::check_file_auth(Client& client) {
    bert::IOReader reader(client.socket);
    bert::Decoder<bert::IOReader> decoder(reader);

    bert::IOWriter writer(client.socket);
    bert::Encoder<bert::IOWriter> encoder(writer);

    std::ostringstream name;
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

    if(!val->equal_atom("ok")) {
      delete val;
      goto auth_error;
    }

    file.open(name.str().c_str());

    char key[PATH_MAX];
    file.getline(key, PATH_MAX);

    if(strcmp(key, "agent start") != 0) {
      delete val;
      goto auth_error;
    }

    unlink(name.str().c_str());
    delete val;
    return true;

auth_error:
    unlink(name.str().c_str());
    return false;
  }

  bool QueryAgent::process_clients(fd_set fds) {
    bool found = false;
    for(std::vector<Client>::iterator i = sockets_.begin();
        i != sockets_.end(); /* nothing */) {
      Client& client = *i;

      if(FD_ISSET(client.socket, &fds)) {
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
      return false;
    }

    return true;
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
              encoder.write_atom("error");
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
              encoder.write_atom("error");
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

  void QueryAgent::start_thread(STATE) {
    SYNC(state);

    if(vm_) return;

    vm_ = state->shared().new_vm();
    exit_ = false;
    thread_.set(Thread::create(state, vm_, G(thread), query_agent_tramp, true));
    run(state);
  }

  void QueryAgent::stop_thread(STATE) {
    SYNC(state);

    if(!vm_) return;

    pthread_t os = vm_->os_thread();

    exit_ = true;
    wakeup();
    void* return_value;
    pthread_join(os, &return_value);
    vm_ = NULL;
  }

  void QueryAgent::wakeup() {
    char buf = '!';
    atomic::memory_barrier();
    if(write(write_control(), &buf, 1) < 0) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
      std::cerr << "[QA: Write error: " << err << "]\n";
    }
  }

  void QueryAgent::shutdown(STATE) {
    cleanup();
    stop_thread(state);
  }

  void QueryAgent::before_exec(STATE) {
    stop_thread(state);
  }

  void QueryAgent::after_exec(STATE) {
    start_thread(state);
  }

  void QueryAgent::before_fork(STATE) {
    stop_thread(state);
  }

  void QueryAgent::after_fork_parent(STATE) {
    start_thread(state);
  }

  void QueryAgent::after_fork_child(STATE) {
    if(vm_) {
      VM::discard(state, vm_);
      vm_ = NULL;
    }
    initialize(state);
    start_thread(state);
  }

  void QueryAgent::run(STATE) {
    int error = thread_.get()->fork_attached(state);
    if(error) rubinius::bug("Unable to start agent thread");
  }

  void QueryAgent::cleanup() {
    remove_tmp_path();
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

      stream << shared_.env()->executable_name() << "\n";

      stream.close();

      atexit(remove_tmp_path);
    }
  }

  void QueryAgent::perform(STATE) {
    GCTokenImpl gct;
    const char* thread_name = "rbx.agent";
    vm_->set_name(thread_name);

    RUBINIUS_THREAD_START(thread_name, state->vm()->thread_id(), 1);

    state->vm()->thread->hard_unlock(state, gct, 0);

    while(1) {
      fd_set read_fds = fds();

      state->gc_independent(gct, 0);
      int ret = select(max_fd() + 1, &read_fds, 0, 0, 0);
      if(exit_) return;

      if(ret < 0) {
        if(errno == EINTR || errno == EAGAIN) continue;
        char buf[RBX_STRERROR_BUFSIZE];
        char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
        std::cerr << "[QA: Select error: " << err << "]\n";
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
          char buf[RBX_STRERROR_BUFSIZE];
          char* err = RBX_STRERROR(errno, buf, RBX_STRERROR_BUFSIZE);
          std::cerr << "[QA: Read error: " << err << "]\n";
        }
      } else if(server_fd() > 0 && FD_ISSET(server_fd(), &read_fds)) {
        // now accept an incoming connection
        struct sockaddr_in sin;
        socklen_t addr_size = sizeof(sin);
        int client = accept(server_fd(), (struct sockaddr *)&sin, &addr_size);

        if(local_only()) {
          if(sin.sin_addr.s_addr != inet_addr("127.0.0.1")) {
            std::cerr << "[QA: Reject connection from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
            close(client);
            continue;
          }

          Client cl(client);

          cl.begin_auth(incr_tmp_key());

          std::ostringstream name;
          name << "/tmp/agent-auth." << getuid() << "-" << getpid() << "." << cl.auth_key;

          int file = open(name.str().c_str(), O_CREAT | O_WRONLY | O_EXCL, 0600);
          if(file == -1) {
            std::cerr << "[QA: Unable to open '" << name.str() << "' to begin local auth]\n";
            close(client);
            continue;
          }

          close(file);

          if(verbose()) {
            std::cerr << "[QA: Requesting file auth from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
          }

          request_auth(client, name.str());

          add_fd(client);

          add_socket(cl);

          continue;
        } else if(use_password()) {
          Client cl(client);
          cl.begin_auth(0);

          if(verbose()) {
            std::cerr << "[QA: Requesting password auth from " << inet_ntoa(sin.sin_addr)
                      << ":" << ntohs(sin.sin_port) << "]\n";
          }

          request_password(client);

          add_fd(client);

          add_socket(cl);

          continue;
        }

        if(verbose()) {
          std::cerr << "[QA: Connection from " << inet_ntoa(sin.sin_addr)
                    << ":" << ntohs(sin.sin_port) << "]\n";
        }

        write_welcome(client);

        add_fd(client);

        Client cl(client);
        cl.set_running();

        add_socket(cl);
      } else {
        if(!process_clients(read_fds)) {
          // TODO: cleanup
          break;
        }
      }
    }
    RUBINIUS_THREAD_STOP(thread_name, state->vm()->thread_id(), 1);
  }
}
