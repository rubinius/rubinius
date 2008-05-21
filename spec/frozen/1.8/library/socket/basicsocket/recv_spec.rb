require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#recv" do
  before :each do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
  end

  after :each do
    @socket.close unless @socket.closed?
    @server.close unless @server.closed?
  end

  it "receives a specified number of bytes of a message from another socket" do
    # TCPServer and TCPSocket both inherit from BasicSocket
    data = nil
    t = Thread.new do
      client = @server.accept
      data = client.recv(5)
      client.close
    end
    Thread.pass until t.status == "sleep"

    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.send('hello', 0).should == 5

    t.join
    data.should == 'hello'
  end

  it "accepts flags to specify unusual receiving behaviour" do
    data = nil
    t = Thread.new do
      client = @server.accept
      data = client.recv(5, Socket::MSG_OOB)
      client.close
    end
    Thread.pass until t.status == "sleep"

    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.send('helloU', Socket::MSG_OOB)

    t.join
    data.should == 'U'
  end
end
