require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#getpeername" do

  before :each do
    @server = TCPServer.new("127.0.0.1", SocketSpecs.port)
    @client = TCPSocket.new("127.0.0.1", SocketSpecs.port)
  end

  after :each do
    @server.close unless @server.closed?
    @client.close unless @client.closed?
  end

  it "returns the sockaddr of the other end of the connection" do
    server_sockaddr = Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1")
    @client.getpeername.should == server_sockaddr
  end

  # Catch general exceptions to prevent NotImplementedError
  it "raises an error if socket's not connected" do
    lambda { @server.getpeername }.should raise_error(Exception)
  end
end
