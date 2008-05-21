require 'socket'

module SocketSpecs
  def self.port
    40001
  end

  def self.sockaddr_in(port, host)
    Socket::SockAddr_In.new(Socket.sockaddr_in(port, host))
  end

  def self.socket_path
    "/tmp/unix_server_spec.socket"
  end
end
