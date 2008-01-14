require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#getsockopt" do
  
  before(:each) do
    @sock = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
  end
  
  it "identifies the SOCK_STREAM socket type" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_TYPE)
    n.should == [Socket::SOCK_STREAM].pack("i")
  end

  it "identifies the SOCK_DGRAM socket type" do
    UDPSocket.new.getsockopt(Socket::SOL_SOCKET, Socket::SO_TYPE).should == [Socket::SOCK_DGRAM].pack("i")
  end
  
  it "identifies the socket error" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_ERROR)
    n.should == [0].pack("i")  
  end 
  
end

