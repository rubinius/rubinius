require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#send" do
  before :each do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
  end

  after :each do
    @server.closed?.should be_false
    @socket.closed?.should be_false

    @server.close
    @socket.close
  end

   it "sends a message to another socket and returns the number of bytes sent" do
     data = nil
     t = Thread.new do
       client = @server.accept
       data = client.recv(5)
       client.close
     end
     Thread.pass while t.status and t.status != "sleep"
     t.status.should_not be_nil

     @socket.send('hello', 0).should == 5

     t.join
     data.should == 'hello'
   end

   it "accepts flags to specify unusual sending behaviour" do
     data = nil
     peek_data = nil
     t = Thread.new do
       client = @server.accept
       peek_data = client.recv(6, Socket::MSG_PEEK)
       data = client.recv(6)
       client.close
     end
     Thread.pass while t.status and t.status != "sleep"
     t.status.should_not be_nil

     @socket.send('helloU', Socket::MSG_PEEK | Socket::MSG_OOB).should == 6

     t.join
     peek_data.should == "hello"
     data.should == 'hello'
   end

  it "accepts a sockaddr as recipient address" do
     data = nil
     t = Thread.new do
       client = @server.accept
       data = client.recv(5)
       client.close
     end
     Thread.pass while t.status and t.status != "sleep"
     t.status.should_not be_nil

     sockaddr = Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1")
     @socket.send('hello', 0, sockaddr).should == 5

     t.join
     data.should == 'hello'
  end
end
