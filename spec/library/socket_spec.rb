require File.dirname(__FILE__) + '/../spec_helper'
require 'socket'
@port = 40000

describe "Socket" do
  it "should inherit from BasicSocket and IO" do
    Socket.superclass.should == BasicSocket
    BasicSocket.superclass.should == IO
  end
end

describe "The socket class hierarchy" do
  it "should have an IPSocket in parallel to Socket" do
    Socket.ancestors.include?(IPSocket).should == false
    IPSocket.ancestors.include?(Socket).should == false
    IPSocket.superclass.should == BasicSocket
  end

  it "should have TCPSocket and UDPSocket subclasses of IPSocket" do
    TCPSocket.superclass.should == IPSocket
    UDPSocket.superclass.should == IPSocket
  end

  it "should have a UNIXSocket in parallel to Socket" do
    Socket.ancestors.include?(UNIXSocket).should == false
    UNIXSocket.ancestors.include?(Socket).should == false
    UNIXSocket.superclass.should == BasicSocket
  end
end

describe "Server class hierarchy" do
  it "should contain UNIXServer" do
    UNIXServer.superclass.should == UNIXSocket
  end
end

describe "TCPServer.new" do
  after(:each) do
    @server.close if @server
  end
  it "should bind to a host and a port" do
    @server = TCPServer.new('127.0.0.1', @port)
  end
end

describe "TCPServer#accept" do
  before(:each) do
    @data = []
    @server = TCPServer.new('127.0.0.1', @port)
    @read = false
    @thread = Thread.new do
      client = @server.accept
      @data << client.read(5)
      @read = true
      client.close
    end
  end

  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "should accept what is written by the client" do
    @socket = TCPSocket.new('127.0.0.1', @port)
    @socket.write('hello')
    until @read do
      "waiting"
    end
    @thread.join
    @data.should == ['hello']
  end
end

describe "IPSocket#peeraddr" do
  before(:each) do
    @server = TCPServer.new('127.0.0.1', @port)
  end
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "should return an array of values" do
    @socket = TCPSocket.new('127.0.0.1', @port)
    @socket.peeraddr.should == ["AF_INET", @port, "localhost", "127.0.0.1"]
  end
end

describe "BasicSocket#do_not_reverse_lookup" do
  before(:each) do
    BasicSocket.do_not_reverse_lookup = true
    @server = TCPServer.new('127.0.0.1', @port)
  end
  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "should cause 'peeraddr' to avoid name lookups" do
    @socket = TCPSocket.new('127.0.0.1', @port)
    @socket.peeraddr.should == ["AF_INET", @port, "127.0.0.1", "127.0.0.1"]
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
