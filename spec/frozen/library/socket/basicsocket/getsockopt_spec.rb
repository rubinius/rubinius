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
  
  it "gets a socket option" do
    n = @sock.getsockopt(Socket::SOL_SOCKET, Socket::SO_TYPE)
    n.should == [Socket::SOCK_STREAM].pack("i")
  end

  it "raises a SystemCallError with an invalid socket option" do
    lambda { @sock.getsockopt Socket::SOL_SOCKET, -1 }.should raise_error(Errno::ENOPROTOOPT)
  end
end

