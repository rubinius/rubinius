require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'


describe "TCPServer#accept" do
  before :each do
    @server = TCPServer.new("127.0.0.1", SocketSpecs.port)
  end

  after :each do
    @server.close unless @server.closed?
  end

  it "accepts a connection and returns a TCPSocket" do
    data = nil
    t = Thread.new do
      client = @server.accept
      client.should be_kind_of(TCPSocket)
      data = client.read(5)
      client << "goodbye"
      client.close
    end
    Thread.pass while t.status and t.status != "sleep"
    
    socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    socket.write('hello')
    socket.read.should == 'goodbye'
    t.join
    data.should == 'hello'
    socket.close
  end

  it "can be interrupted by Thread#kill" do
    t = Thread.new { @server.accept }

    Thread.pass while t.status and t.status != "sleep"

    # kill thread, ensure it dies in a reasonable amount of time
    t.kill
    a = 1
    while a < 1000
      break unless t.alive?
      Thread.pass
      a += 1
    end
    a.should < 1000
  end

  it "can be interrupted by Thread#raise" do
    t = Thread.new { @server.accept }

    Thread.pass while t.status and t.status != "sleep"

    # raise in thread, ensure the raise happens
    ex = Exception.new
    t.raise ex
    lambda { t.join }.should raise_error(Exception)
  end
end
