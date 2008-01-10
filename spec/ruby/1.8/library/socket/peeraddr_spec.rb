require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
