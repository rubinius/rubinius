require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

require 'socket'

describe "Socket#connect_nonblock" do
  before :each do
    @addr = Socket.sockaddr_in(SocketSpecs.port, "127.0.0.1")
    @socket = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
  end

  after :each do
    @socket.close
  end

  it "takes an encoded socket address and starts the connection to it" do
    lambda {
      @socket.connect_nonblock(@addr)
    }.should raise_error(Errno::EINPROGRESS)
  end

  it "raises Errno::ISCONN when the socket has connected" do
    ready = false
    thread = Thread.new do
      server = TCPServer.new(SocketSpecs.port)
      ready = true
      conn = server.accept
      conn << "hello!"
      conn.close
      server.close
    end

    Thread.pass while (thread.status and thread.status != 'sleep') or !ready

    begin
      @socket.connect_nonblock(@addr)
    rescue Errno::EINPROGRESS
    end

    IO.select nil, [@socket]

    lambda {
      @socket.connect_nonblock(@addr)
    }.should raise_error(Errno::EISCONN)

    @socket.read(6).should == "hello!"
  end
end
