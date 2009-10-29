#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "util/bert.hpp"

#include "agent.hpp"
#include "exception.hpp"

namespace rubinius {
  void QueryAgent::bind() {
    std::cout << "binding to " << port_ << "\n";

    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port_);

    ::bind(server_fd_, (struct sockaddr*)&sin, sizeof(sin));
    if(verbose_) {
      std::cerr << "[QA: Bound to port " << ntohs(sin.sin_port) << "]\n";
    }
  }

  void QueryAgent::perform() {
    bind();

    while(1) {
      listen(server_fd_, cBackLog);

      // now accept an incoming connection
      struct sockaddr_storage their_addr;
      socklen_t addr_size = sizeof(their_addr);
      int client = accept(server_fd_, (struct sockaddr *)&their_addr, &addr_size);

      struct sockaddr_in* sin = (struct sockaddr_in*)&their_addr;

      if(verbose_) {
        std::cerr << "[QA: Connection from " << inet_ntoa(sin->sin_addr)
                  << ":" << sin->sin_port << "]\n";
      }

      bert::IOWriter writer(client);
      bert::Encoder<bert::IOWriter> encoder(writer);
      encoder.write_version();

      encoder.write_tuple(2);
      encoder.write_atom("hello_query_agent");
      encoder.write_binary(RBX_HOST);

      close(client);
    }
  }
}
