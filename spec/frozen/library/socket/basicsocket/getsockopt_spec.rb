require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#getsockopt" do
  before(:each) do
    @sock = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
  end

  after :each do
    @sock.closed?.should be_false
    @sock.close
  end

  it "gets a socket option Socket::SO_TYPE" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_TYPE)
    n.should == [Socket::SOCK_STREAM].pack("i")
  end

  it "gets a socket option Socket::SO_OOBINLINE" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_OOBINLINE)
    n.should == [0].pack("i")
  end

  it "gets a socket option Socket::SO_LINGER" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_LINGER)
    if (n.size == 8) # linger struct on some platforms, not just a value
      n.should == [0, 0].pack("ii")
    else
      n.should == [0].pack("i")
    end
  end

  it "gets a socket option Socket::SO_SNDBUF" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_SNDBUF)
    n.unpack('i')[0].should > 0
  end

  it "raises a SystemCallError with an invalid socket option" do
    lambda { @sock.getsockopt Socket::SOL_SOCKET, -1 }.should raise_error(Errno::ENOPROTOOPT)
  end
end
