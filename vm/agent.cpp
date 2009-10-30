#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/utsname.h>

#include "util/bert.hpp"

#include "vm.hpp"
#include "configuration.hpp"
#include "agent.hpp"
#include "exception.hpp"

namespace rubinius {
  bool QueryAgent::bind() {
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
    sin.sin_port = htons(port_);

    if(::bind(server_fd_, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
      std::cerr << "[QA: Unable to bind socket: " << strerror(errno) << "]\n";
      return false;
    }

    if(::listen(server_fd_, cBackLog) == -1) {
      std::cerr << "[QA: Unable to listen on socket: " << strerror(errno) << "]\n";
      return false;
    }

    if(verbose_) {
      std::cerr << "[QA: Bound to port " << ntohs(sin.sin_port) << "]\n";
    }

    FD_SET(server_fd_, &fds_);
    if(server_fd_ > max_fd_) max_fd_ = server_fd_;

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
      if(cmd->equal_atom("uname")) {
        struct utsname name;
        if(uname(&name)) {
          encoder.write_atom("error");
          return true;
        } else {
          encoder.write_tuple(5);
          encoder.write_binary(name.sysname);
          encoder.write_binary(name.nodename);
          encoder.write_binary(name.release);
          encoder.write_binary(name.version);
          encoder.write_binary(name.machine);
          return true;
        }
      } else if(cmd->equal_atom("set_config")) {
        if(val->elements()->size() == 3) {
          bert::Value* key = val->get_element(1);
          bert::Value* value = val->get_element(2);

          if(key->type() == bert::Binary ||
              value->type() == bert::Binary) {
            if(shared_.config.import(key->string(), value->string())) {
              encoder.write_atom("ok");
            } else {
              encoder.write_atom("unknown_key");
            }
            return true;
          }
        }

        encoder.write_atom("error");
        return true;
      } else if(cmd->equal_atom("get_config")) {
        if(val->elements()->size() == 2) {
          bert::Value* key = val->get_element(1);
          if(key->type() == bert::Binary) {
            if(config::ConfigItem* item = shared_.config.find(key->string())) {
              std::stringstream ss;
              item->print_value(ss);

              encoder.write_tuple(2);
              encoder.write_atom("ok");
              if(config::Integer* i = dynamic_cast<config::Integer*>(item)) {
                encoder.write_integer(i->value);
              } else {
                encoder.write_binary(ss.str().c_str());
              }
            } else {
              encoder.write_atom("unknown_key");
            }
            return true;
          }
        }

        encoder.write_atom("error");
        return true;
      }
    } else if(val->equal_atom("close")) {
      encoder.write_atom("bye");
      return false;
    }

    encoder.write_atom("unknown");

    return true;
  }

  void QueryAgent::perform() {
    if(!bind()) return;

    while(1) {
      fd_set read_fds;
      FD_COPY(&fds_, &read_fds);

      int ret = select(max_fd_ + 1, &read_fds, 0, 0, 0);

      if(ret < 0) {
        if(errno == EINTR || errno == EAGAIN) continue;
        std::cerr << "[QA: Select error: " << strerror(errno) << "]\n";
        return;
      } else if(ret == 0) {
        continue;
      }

      if(FD_ISSET(server_fd_, &read_fds)) {
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

        FD_SET(client, &fds_);
        if(max_fd_ < client) max_fd_ = client;

        sockets_.push_back(client);
      } else {
        for(std::vector<int>::iterator i = sockets_.begin();
            i != sockets_.end(); /* nothing */) {
          int client = *i;
          if(FD_ISSET(client, &read_fds)) {
            if(!process_commands(client)) {
              if(verbose_) {
                struct sockaddr_in sin;
                socklen_t len = sizeof(sin);

                getpeername(*i, (struct sockaddr*)&sin, &len);
                std::cerr << "[QA: Disconnected " << inet_ntoa(sin.sin_addr)
                          << ":" << ntohs(sin.sin_port) << "]\n";
              }
              FD_CLR(client, &fds_);
              close(client);

              i = sockets_.erase(i);
              continue;
            }
          }
          i++;
        }
      }
    }
  }
}
