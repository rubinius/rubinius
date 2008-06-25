require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#getsockname" do
  
  after :each do
    @socket.close unless @socket.closed?
  end

  it "returns the sockaddr associacted with the socket" do
    @socket = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    @socket.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true)
    @socket.bind(Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1"))
    sockaddr = Socket.unpack_sockaddr_in(@socket.getsockname)
    sockaddr.should == [SocketSpecs.port, "127.0.0.1"]
  end

  it "works on server sockets" do
    @server = TCPServer.new(SocketSpecs.port)
    sockaddr = Socket.unpack_sockaddr_in(@server.getsockname)
    sockaddr.should == [SocketSpecs.port, "0.0.0.0"]
    @server.close
  end

  it "returns empty sockaddr for unbinded sockets" do
    @socket = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
    sockaddr = Socket.unpack_sockaddr_in(@socket.getsockname)
    sockaddr.should == [0, "0.0.0.0"]    
  end
end
