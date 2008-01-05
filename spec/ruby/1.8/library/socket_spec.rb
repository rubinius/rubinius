require File.dirname(__FILE__) + '/../spec_helper'
require 'socket'

module SocketSpecs
  def self.port
    40000
  end
end

describe "Socket" do
  it "inherits from BasicSocket and IO" do
    Socket.superclass.should == BasicSocket
    BasicSocket.superclass.should == IO
  end
end

describe "The socket class hierarchy" do
  it "has an IPSocket in parallel to Socket" do
    Socket.ancestors.include?(IPSocket).should == false
    IPSocket.ancestors.include?(Socket).should == false
    IPSocket.superclass.should == BasicSocket
  end

  it "has TCPSocket and UDPSocket subclasses of IPSocket" do
    TCPSocket.superclass.should == IPSocket
    UDPSocket.superclass.should == IPSocket
  end

  it "has a UNIXSocket in parallel to Socket" do
    Socket.ancestors.include?(UNIXSocket).should == false
    UNIXSocket.ancestors.include?(Socket).should == false
    UNIXSocket.superclass.should == BasicSocket
  end
end

describe "Server class hierarchy" do
  it "contains UNIXServer" do
    UNIXServer.superclass.should == UNIXSocket
  end
end

describe "TCPServer.new" do
  after(:each) do
    @server.close if @server
  end
  it "binds to a host and a port" do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
  end
end

describe "TCPServer#accept" do
  before(:each) do
    @data = []
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    @read = false
    @thread = Thread.new do
      begin
        client = @server.accept_nonblock
      rescue Errno::EAGAIN, Errno::ECONNABORTED, Errno::EPROTO, Errno::EINTR
        @listening = true
        IO.select([@server])
        retry
      end
      @data << client.read(5)
      @read = true
      client.close
    end
    Thread.pass until @listening
  end

  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "accepts what is written by the client" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.write('hello')
    Thread.pass until @read
    @thread.join
    @data.should == ['hello']
  end
end

describe "IPSocket#peeraddr" do
  before(:each) do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
  end
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "returns an array of values" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.peeraddr.should == ["AF_INET", SocketSpecs.port, "localhost", "127.0.0.1"]
  end
end

describe "BasicSocket#do_not_reverse_lookup" do
  before(:each) do
    BasicSocket.do_not_reverse_lookup = true
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
  end
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "causes 'peeraddr' to avoid name lookups" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.peeraddr.should == ["AF_INET", SocketSpecs.port, "127.0.0.1", "127.0.0.1"]
  end
end

describe "TCPServer#<<" do
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end
end

describe "TCPServer#close" do
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end
end

describe "TCPServer#write" do
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end
end

describe "TCPServer#setsockopt" do
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end
end

describe "TCPServer#readpartial" do
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end
end

describe "UDPSocket.open" do

  after(:each) do
    @socket.close if @socket
    @server.close if @server
  end

  it "returns a socket that can be written to and read from" do
    @ready = false
    server_thread = Thread.new do
      @server = UDPSocket.open
      @server.bind(nil,SocketSpecs.port)
      @ready = true
      msg1 = @server.recvfrom(64)
      msg1[0].should == "ad hoc"
      msg1[1][0].should == "AF_INET"
      (msg1[1][1].kind_of? Fixnum).should == true
      msg1[1][3].should == "127.0.0.1"

      msg2 = @server.recvfrom(64)
      msg2[0].should == "connection-based"
      msg2[1][0].should == "AF_INET"
      (msg2[1][1].kind_of? Fixnum).should == true
      msg2[1][3].should == "127.0.0.1"
    end

    Thread.pass until @ready

    UDPSocket.open.send("ad hoc", 0, 'localhost',SocketSpecs.port)

    @socket = UDPSocket.open
    @socket.connect('localhost',SocketSpecs.port)
    @socket.send("connection-based", 0)

    server_thread.join
  end
end
