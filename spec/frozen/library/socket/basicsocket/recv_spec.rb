require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#recv" do

  before :each do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
  end

  after :each do
    @server.closed?.should be_false
    @server.close
  end

  it "receives a specified number of bytes of a message from another socket"  do
    data = ""
    t = Thread.new do
      client = @server.accept
      data = client.recv(10)
      client.close
    end
    Thread.pass while t.status and t.status != "sleep"
    t.status.should_not be_nil

    socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    socket.send('hello', 0)
    socket.close

    t.join
    data.should == 'hello'
  end

  it "accepts flags to specify unusual receiving behaviour" do
    data = ""
    t = Thread.new do
      client = @server.accept
      data = client.recv(10)    # in-band data (TCP), doesn't receive the flag.
      client.close
    end
    Thread.pass while t.status and t.status != "sleep"
    t.status.should_not be_nil
    
    socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    socket.send('helloU', Socket::MSG_OOB)
    t.join
    data.should == 'hello'
  end
end
