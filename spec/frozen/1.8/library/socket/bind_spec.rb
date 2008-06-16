require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

include Socket::Constants

describe "Socket#bind" do
  it "verifies we can bind to a port" do
    sock = Socket.new(AF_INET, SOCK_STREAM, 0);
    # Port number taken from random.org, has no meaning
    sockaddr = Socket.pack_sockaddr_in(23986, "127.0.0.1");
    sock.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true);
    sock.bind(sockaddr);
    sock.close();
  end
  
  it "verifies that we cannot bind to a port that's already bound" do
    begin
      sock1 = Socket.new(AF_INET, SOCK_STREAM, 0);
      # Port number taken from random.org, has no meaning
      sockaddr = Socket.pack_sockaddr_in(23986, "127.0.0.1");
      sock1.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true);
      sock1.bind(sockaddr);
      sock2 = Socket.new(AF_INET, SOCK_STREAM, 0);
      sock2.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true);
      lambda { sock2.bind(sockaddr) }.should raise_error;
    ensure
      sock1.close();
    end
  end
end
