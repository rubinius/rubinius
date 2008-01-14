require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPSocket.new" do
  it "requires a hostname and a port as arguments" do
    lambda { TCPSocket.new }.should raise_error(ArgumentError)
  end
  
  it "refuses the connection when there is no server to connect to" do
    lambda { TCPSocket.new('127.0.0.1', SocketSpecs.port) }.should raise_error(Errno::ECONNREFUSED)
  end
  
# This isn't passing on MRI on at least OS X for me (headius)
quarantine! do
  it "connects to a listening server" do
    @server = TCPServer.new(SocketSpecs.port)
    @server.accept_nonblock
    lambda { TCPSocket.new('127.0.0.1', SocketSpecs.port) }.should_not raise_error(Errno::ECONNREFUSED)
  end
end
end
