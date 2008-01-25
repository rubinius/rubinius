require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPSocket.new" do
  it "requires a hostname and a port as arguments" do
    lambda { TCPSocket.new }.should raise_error(ArgumentError)
  end
  
  it "refuses the connection when there is no server to connect to" do
    lambda { TCPSocket.new('127.0.0.1', SocketSpecs.port) }.should raise_error(Errno::ECONNREFUSED)
  end

  it "connects to a listening server" do
    thread = Thread.new do
      server = TCPServer.new(SocketSpecs.port)
      server.accept
      server.close
    end
    Thread.pass until thread.status == 'sleep'
    lambda { TCPSocket.new('127.0.0.1', SocketSpecs.port) }.should_not raise_error(Errno::ECONNREFUSED)
    thread.join
  end

  it "has an address once it has connected to a listening server" do
    thread = Thread.new do
      server = TCPServer.new(SocketSpecs.port)
      server.accept
      server.close
    end
    Thread.pass until thread.status == 'sleep'
    sock = TCPSocket.new('127.0.0.1', SocketSpecs.port)    
    sock.addr[0].should == "AF_INET"
    # on some platforms (Mac), MRI
    # returns comma at the end.
    sock.addr[2].should =~ /^localhost,?$/
    sock.addr[3].should == "127.0.0.1"
    sock.addr.be_kind_of Fixnum
    thread.join
  end
end
